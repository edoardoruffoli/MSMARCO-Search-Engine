#include "MSMARCO-Search-Engine/parsing.hpp"

/*
#if defined _WIN32
int getMemoryUsed() {
    //Memory usage
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    return statex.dwMemoryLoad;
}
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
int getMemoryUsed() {
    long long totalPhysMem = memInfo.totalram;
    totalPhysMem *= memInfo.mem_unit;
    long long physMemUsed = memInfo.totalram - memInfo.freeram;
    physMemUsed *= memInfo.mem_unit
        return (physMemUsed / totalPhysMem) * 100
}
#endif
*/

/* Tokenize a text and return the result into the tokens variable.
*/
void tokenize(std::string &content, bool flag, const std::unordered_set<std::string> &stopwords, 
                std::unordered_map<std::string, int> &tokens) 
{
    // Replace characters that are not numbers or ASCII letters with spaces
    std::replace_if(content.begin(), content.end(), [] (unsigned char c) { 
        return !(c>='0' && c <= '9') && !isalpha(c) && c != ' ';
    }, ' ');

    // To lower case
    std::transform(content.begin(), content.end(), content.begin(), [](unsigned char c) {
        return std::tolower(c); 
    });

    // Split content using whitespace characters
    std::istringstream iss(content);
    std::string token;
    while (getline(iss, token, ' ')) {
        
        // If the current token is empty continue
		if (!token.size())
			continue;

        if(flag) {
			if (stopwords.find(token) != stopwords.end()) {
				continue;
			}
            token = porter2::Stemmer{}.stem(token);
        }
		tokens[token]++;
    }
}

/* Add a token_stream to the dictionary passed by parameter and compute the document len by counting the token
   in the stream.
*/
void add_to_posting_list(std::map<std::string, std::list<std::pair<int, int>>>& dictionary,
                  const std::unordered_map<std::string, int>& token_stream, 
                  int doc_id, 
                  unsigned int &doc_len) 
{
    doc_len = 0;
	for (const std::pair<std::string, int> term_doc : token_stream) {
		dictionary[term_doc.first].push_back(std::make_pair(doc_id, term_doc.second));
        doc_len += term_doc.second;
    }
}

/* Save the intermediate inverted index passed as parameter on a temporary file.
*/
bool save_intermediate_inv_idx(std::map<std::string, std::list<std::pair<int, int>>>& dictionary, 
                const char *filename) {
    std::ofstream filestream(filename, std::ios::binary);
    if (filestream.fail()) {
        std::cout << "Fail intermediate inverted index write!" << std::endl;
        return false;
    }
	for (auto& kv : dictionary) {
		filestream << kv.first;
		for (auto& i : kv.second)
			filestream << ' ' << i.first << ' ' << i.second;
		filestream << std::endl;
	}
    filestream.close();
    return true;
}

/* Process a block of documents and write the results on file. The processing is done using a pool of threads.
*/
void SPIMI_Invert(std::vector<std::string> &documents, unsigned int start_doc_id, unsigned int block_num,
                   BS::thread_pool &pool, 
                   DocTable &doc_table,
                   std::unordered_set<std::string> &stopwords, bool flag) 
{
    // Data structures that will be used to merge the threads outputs
    std::map<std::string, std::list<std::pair<int, int>>> dictionary;
    std::vector<doc_table_entry> doc_table_entries;

    std::cout << "Starting doc_id: " << start_doc_id << " block_num:" << block_num << " num_docs: " << documents.size() << '\n';
    
    // Start a processing timer
    BS::timer tmr;
    tmr.start();

    // Each thread will output a partial doc_table and a partial inverted index
    typedef std::pair<std::vector<doc_table_entry>,std::map<std::string, std::list<std::pair<int,int>>>> thread_res;

    // Worker thread function: each thread will process a portion of the document in the current block and will
    // output the partial results based on its portion
    auto process_block = [&documents, &doc_table, start_doc_id, flag, &stopwords]
    (const unsigned start, const unsigned end) 
    {
        //std::cout << "Launched Worker Thread, Interval: [" << start << "," << end << "]\n";

        // Data structures that will contain the partial output
        std::map<std::string, std::list<std::pair<int, int>>> dict;
        std::vector<doc_table_entry> partial_doc_table;

        // Utility variables
        std::string doc_no, text;
        std::istringstream iss;
        std::unordered_map<std::string, int> tokens;
        doc_table_entry de;
        unsigned int doc_len = 0;
                                
        for (unsigned int i = start; i < end; i++) {
            // Parse the current doc to obtain doc_no and text
            iss = std::istringstream(documents[i]);
            getline(iss, doc_no, '\t');
            getline(iss, text, '\n');

            // Tokenize the text into tokens
            tokenize(text, flag, stopwords, tokens);

            // Add tokens to posting list of the partial dictionary
            add_to_posting_list(dict, tokens, start_doc_id + i, doc_len);
            tokens.clear();

            // Clear previous values
            memset(de.doc_no, '\0', sizeof(de.doc_no));

            // Add document information to the partial doc_table
            strcpy(de.doc_no, doc_no.c_str());
            de.doc_len = doc_len;
            partial_doc_table.push_back(de);
        }

        return std::make_pair(partial_doc_table, dict);
    };

    // Start the parallel processing of the docs
    BS::multi_future<thread_res> mf = pool.parallelize_loop(0, documents.size(), process_block);

    // Get the results of the processing. Note that the results are ordered by thread id
    std::vector<thread_res> results = mf.get();

    // Merge the results into the final data structures
    for (auto result : results) {

        // Merge the partial doc tables by concatenating them
        doc_table_entries.insert(doc_table_entries.end(), result.first.begin(), result.first.end());

        // Merge the partial dictionaries, since the outputs are ordered, we can simply concate the 
        // posting list to obtain the final dictionary
        for (auto entry : result.second) {

            // If term entry does not exist create it otherwise concatenate the posting list
            if (dictionary.find(entry.first) == dictionary.end()) {
                dictionary[entry.first] = entry.second;
            } else {
                dictionary[entry.first].splice(dictionary[entry.first].end(), entry.second);    // O(1)
            }
        }
    }

    // Update the disk based Doc Table 
    doc_table.insert(doc_table_entries);

    // Save the intermediate dictionary results on file
    char path[40];
    sprintf(path, "../tmp/intermediate_%d", block_num);

    save_intermediate_inv_idx(dictionary, path);
    //save_intermediate_inv_idx(dictionary, std::string("../tmp/intermediate_" + std::to_string(block_num)));

    std::cout << "Ended Paralleling Processing: \n"; 

    // Stop the timer
    tmr.stop();
    std::cout << "The elapsed time was " << tmr.ms() << " ms.\n";
}

void parse(const char* in, const unsigned int BLOCK_SIZE, bool flag, const char* stopwords_filename, 
            unsigned int n_threads) 
{
    std::cout << "Started Parsing Phase: \n\n";

    // Check input arguments
    if (BLOCK_SIZE == 0)
        std::cout << "Error: block size not valid.\n";

    // Use in memory mapping to efficiently read the file
    boost::iostreams::stream<boost::iostreams::mapped_file_source> mapped_file_stream;
	boost::iostreams::mapped_file_source mmap(in);
	boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf;
    mapped_file_stream.open(mmap);
    if (mapped_file_stream.fail())
		std::cout << "Error: input fail not valid.\n";
    
    // Compressed reading using gzip
	inbuf.push(boost::iostreams::gzip_decompressor());
	inbuf.push(mapped_file_stream);

	// Convert streambuf to istream
	std::istream instream(&inbuf);

	// Doc Table file
	std::string doc_table_filename("../../output/doc_table.bin");

    // Clear previous Doc Table
    if (boost::filesystem::exists(doc_table_filename)) {
        boost::filesystem::remove(doc_table_filename);
    }
    
    // Disk based Doc Table data structure
    DocTable doc_table;
    doc_table.create("../../output/doc_table.bin");

	// Load stopwords
	std::unordered_set<std::string> stopwords;
	if (flag) {
		std::ifstream is(stopwords_filename);
		std::string word;
		while (std::getline(is, word))
			stopwords.insert(word);
        std::cout << "Stopwords file loaded.\n";
	}

    // Constructs a worker threads pool 
    BS::thread_pool pool(n_threads);

    // Stores a block of documents to be processed
    std::vector<std::string> block;

    // Init utility variables
    unsigned int current_size = 0;
	unsigned int block_num = 1;
	unsigned int doc_id = 0;
    std::string loaded_content;
    bool first_doc = true;

    // Start processing timer
    BS::timer tmr;
    tmr.start();

    // Blocked Sort Based Indexing BSBI 
	while (getline(instream, loaded_content)) {

        // The first doc includes the header of the gzip file
        if (first_doc) {
            loaded_content.erase(0, 512);
            first_doc = false;
        }

        block.push_back(loaded_content);
        current_size++;

        //if (getMemoryUsed() < 61){
        if (current_size < BLOCK_SIZE) {
            continue;
		}
		else {
            // When a block of BLOCK_SIZE documents has been read, start the processing
            SPIMI_Invert(block, doc_id, block_num, pool, doc_table, stopwords, flag);
            doc_id += current_size;
			block_num++;
            current_size = 0;
            // Clear block for the next iterations
            block.clear();
		}
	}

    // Process last block
    SPIMI_Invert(block, doc_id, block_num, pool, doc_table, stopwords, flag);
    block.clear();

    // Close the Doc Table
    doc_table.close();

    std::cout << "Ended Parsing Phase: \n\n";

    // Stop the processing timer
    tmr.stop();
    std::cout << "The elapsed time was " << tmr.ms() << " ms.\n";
}