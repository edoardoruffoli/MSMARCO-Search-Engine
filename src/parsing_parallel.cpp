#include "MSMARCO-Search-Engine/parsing.hpp"
#include "BS_thread_pool.hpp"

void tokenize(const std::string &content, const bool flag, const std::unordered_set<std::string> &stopwords, 
                                              std::unordered_map<std::string, int> &tokens) {
	// How to deal with empty page, malformed lines, malformed characters?
    auto iss = std::istringstream(content);
    auto str = std::string{};

    while (iss >> str) {
        tokens[str]++;
        // replace spces
        // replace punct
        // lowecase
        // stopwords
    }
}

void add_to_posting_list(std::map<std::string, std::list<std::pair<int, int>>>& dictionary,
                  const std::unordered_map<std::string, int>& token_stream, int doc_id, unsigned int &doc_len) {
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

    //boost::shared_mutex mutex;

    // Add typedef <std::map<std::string, std::list<std::pair<int, int>>>
    auto process_block = [&documents, &doc_table, &dictionary, start_doc_id, flag, &stopwords]
    (const unsigned start, const unsigned end) 
    {
        std::cout << "Launched Worker Thread, Interval: [" << start << "," << end << "]\n";
        std::string doc_no;
        std::string text;
        std::istringstream iss;
        std::unordered_map<std::string, int> tokens;
        unsigned int doc_len;
                                
        for (unsigned int i = start; i < end; i++) {
            iss = std::istringstream(documents[i]);
            getline(iss, doc_no, '\t');
            getline(iss, text, '\n');
            tokenize(text, flag, stopwords, tokens);
            add_to_posting_list(dictionary, tokens, start_doc_id + i, doc_len);
            tokens.clear();
            doc_table[start_doc_id + i].doc_len;
            doc_table[start_doc_id + i].doc_no;
        }
    };

    // Parallel Processing of the docs
    pool.parallelize_loop(0, documents.size(), process_block).wait();

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
	//inbuf.push(boost::iostreams::gzip_decompressor());
	inbuf.push(mapped_file_stream);

	// Convert streambuf to istream
	std::istream instream(&inbuf);

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
	}

    // Constructs a thread pool with as many threads as available in the hardware.
    BS::thread_pool pool(n_threads);

    // Stores a block of documents
    std::vector<std::string> block;

    // Init
    unsigned int current_size = 0;
	unsigned int block_num = 1;
	unsigned int doc_id = 0;
    std::string loaded_content;

    // Blocked Sort Based Indexing
    BS::timer tmr;
    tmr.start();
    
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
    tmr.stop();
    std::cout << "The elapsed time was " << tmr.ms() << " ms.\n";

    // Write last block
    doc_table.resize(BLOCK_SIZE*block_num);
    BSBI_Invert(block, doc_id, block_num, pool, doc_table, stopwords, flag);
    block.clear();

    // Write document table
    save_doc_table(doc_table, doc_table_filename);

	//inbuf.close();
}