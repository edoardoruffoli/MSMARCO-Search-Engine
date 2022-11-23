#include "MSMARCO-Search-Engine/parsing.hpp"

void tokenize(std::string &content, bool flag, const std::unordered_set<std::string> &stopwords, 
                                              std::unordered_map<std::string, int> &tokens) {
	// How to deal with empty page, malformed lines, malformed characters?
    std::regex re("[ \t]");
    //the '-1' is what makes the regex split (-1 := what was not matched)
    std::sregex_token_iterator first{content.begin(), content.end(), re, -1}, last;
    std::vector<std::string> v{first, last};

    for (auto token : v) {
		if (!token.size())
			continue;

        // Remove punctuation
        token.erase(std::remove_if(token.begin(), token.end(), ispunct), token.end());
        
		if (!token.size())
			continue;

        // To lower case
        std::transform(token.begin(), token.end(), token.begin(), [](unsigned char c) { 
            return std::tolower(c); 
        });

        if(flag) {
			if (stopwords.find(token) != stopwords.end()) {
				continue;
			}
			token = porter2::Stemmer{}.stem(token);
        }
		tokens[token]++;
    }
}

void add_to_posting_list(std::map<std::string, std::list<std::pair<int, int>>>& dictionary,
                  const std::unordered_map<std::string, int>& token_stream, int doc_id, unsigned int &doc_len) {
    doc_len = 0;
	for (const std::pair<std::string, int> term_doc : token_stream) {
		dictionary[term_doc.first].push_back(std::make_pair(doc_id, term_doc.second));
        doc_len += term_doc.second;
    }
}

void BSBI_Invert(std::vector<std::string> &documents, unsigned int start_doc_id, unsigned int block_num, 
                       BS::thread_pool &pool, std::vector<doc_table_entry> &doc_table,
                       std::unordered_set<std::string> &stopwords, bool flag) {
    BS::timer tmr;
    tmr.start();
    std::map<std::string, std::list<std::pair<int, int>>> dictionary;
    std::cout << "Starting doc_id: " << start_doc_id << " block_num:" << block_num << " num_docs: " << documents.size() << '\n';

    std::mutex doc_table_mutex;

    // Add typedef <std::map<std::string, std::list<std::pair<int, int>>>
    auto process_block = [&documents, &doc_table, &doc_table_mutex, start_doc_id, flag, &stopwords]
    (const unsigned start, const unsigned end) 
    {
        std::cout << "Launched Worker Thread, Interval: [" << start << "," << end << "]\n";
        std::string doc_no;
        std::string text;
        std::istringstream iss;
        std::map<std::string, std::list<std::pair<int, int>>> dict;
        std::unordered_map<std::string, int> tokens;
        unsigned int doc_len = 0;
                                
        for (unsigned int i = start; i < end; i++) {
            iss = std::istringstream(documents[i]);
            getline(iss, doc_no, '\t');
            getline(iss, text, '\n');
            tokenize(text, flag, stopwords, tokens);
            add_to_posting_list(dict, tokens, start_doc_id + i, doc_len);
            tokens.clear();
            doc_table_mutex.lock();
            doc_table[start_doc_id + i].doc_len = doc_len;    // ??? Concurrency
            doc_table[start_doc_id + i].doc_no = doc_no;
            doc_table_mutex.unlock();
        }
        return dict;
    };

    // Parallel Processing of the docs
    BS::multi_future<std::map<std::string, std::list<std::pair<int, int>>>> mf = pool.parallelize_loop(0, documents.size(),
                            process_block);

    std::vector<std::map<std::string, std::list<std::pair<int, int>>>> totals = mf.get();

    // Merge the output of each thread in the global map, since the outputs are ordered, we can simply 
    // concate them to the global dict
    for (auto output_dict : totals) {
        for (auto entry : output_dict) {
            // If term does not exist create it otherwise concate the posting list
            if (dictionary.find(entry.first) == dictionary.end()) {
                dictionary[entry.first] = entry.second;
            } else {
                dictionary[entry.first].splice(dictionary[entry.first].end(), entry.second);    // O(1)
            }
        }
    }

    save_intermediate_inv_idx(dictionary, std::string("../tmp/intermediate_" + std::to_string(block_num)));

    tmr.stop();
    std::cout << "Ended Paralleling Processing: \n"; 
    std::cout << "The elapsed time was " << tmr.ms() << " ms.\n";
}

void parse(const char* in, const unsigned int BLOCK_SIZE, bool flag, const char* stopwords_filename, unsigned int n_threads) {
	std::cout << "Started Parsing Phase: \n\n";
	if (BLOCK_SIZE == 0)
		std::cout << "Error: block size not valid.\n";

    boost::iostreams::stream<boost::iostreams::mapped_file_source> mapped_file_stream;
	boost::iostreams::mapped_file_source mmap(in);
	boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf;
    mapped_file_stream.open(mmap);
    if (mapped_file_stream.fail())
		std::cout << "Error: input fail not valid.\n";
    
    // Compressed reading
	inbuf.push(boost::iostreams::gzip_decompressor());
	inbuf.push(mapped_file_stream);

	// Convert streambuf to istream
	std::istream instream(&inbuf);
    //std::ifstream instream(in);

	// Document table output
	std::string doc_table_filename("../../output/doc_table.bin");

    // Document table
	std::vector<doc_table_entry> doc_table;

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

    // Stores a block of documents
    std::vector<std::string> block;

    // Init
    unsigned int current_size = 0;
	unsigned int block_num = 1;
	unsigned int doc_id = 0;
    std::string loaded_content;

    // Timer
    BS::timer tmr;
    tmr.start();

    // Blocked Sort Based Indexing   
	while (getline(instream, loaded_content)) {
		current_size++;
        block.push_back(loaded_content);

		if (current_size < BLOCK_SIZE) {
            continue;
		}
		else {
            // Init next BLOCK_SIZE values in order to let the threads acces them
            doc_table.resize(BLOCK_SIZE*block_num);
            std::cout << doc_table.size() << "\n";
            BSBI_Invert(block, doc_id, block_num, pool, doc_table, stopwords, flag);
            doc_id += BLOCK_SIZE;
			block_num++;
            current_size = 0;
            block.clear();
		}
	}

    // Write last block
    doc_table.resize(BLOCK_SIZE*block_num);
    BSBI_Invert(block, doc_id, block_num, pool, doc_table, stopwords, flag);
    block.clear();

    // Write document table
    save_doc_table(doc_table, doc_table_filename);

    std::cout << "Ended Parsing Phase: \n\n";
    tmr.stop();
    std::cout << "The elapsed time was " << tmr.ms() << " ms.\n";
}