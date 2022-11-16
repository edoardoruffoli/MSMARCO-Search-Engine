#include "MSMARCO-Search-Engine/parsing.hpp"
#include "BS_thread_pool.hpp"

void tokenize(const std::string &content, bool flag, std::set<std::string> &stopwords, 
                                              std::unordered_map<std::string, int> &tokens) {
	//How to deal with empty page, malformed lines, malformed characters?
    
    typedef std::string::const_iterator iter;
    std::string token;

    for(std::string::const_iterator it = content.begin(), end = content.end(); it != end; ++it ) {
        if (ispunct((unsigned char)*it)) {
            continue;
        }
        if(*it == '\t' || *it == ' ') {
            if(flag) {
                if (stopwords.find(token) != stopwords.end()) { //O(logN) using std::set
                    token.clear();
                    continue;
                }
                token = porter2::Stemmer{}.stem(token);
            }
            tokens[token]++;
            token.clear();
        }
        std::tolower(*it);
        token.push_back(*it);
    }
    
    /*
    std::vector<std::string> strs;
    boost::split(strs, content, boost::is_any_of("\t "));

    for (auto token : strs) {
        // Remove punctuation
        token.erase(
            std::remove_if(token.begin(), token.end(), [](unsigned char c) {
                return ispunct(c);
                }),
            token.end()
                    );

        // To lower case
        std::transform(token.begin(), token.end(), token.begin(), [](unsigned char c) {
            return std::tolower(c);
            });

        if (token.size() == 0)
            continue;

        if (flag) {
            if (stopwords.find(token) != stopwords.end()) { //O(logN) using std::set
                continue;
            }
            token = porter2::Stemmer{}.stem(token);
        }
        tokens[token]++;
    }
    */
}



void add_to_posting_list(std::map<std::string, std::list<std::pair<int, int>>>& dictonary,
                  const std::unordered_map<std::string, int>& token_stream, int doc_id, unsigned int &doc_len) {
	for (const std::pair<std::string, int> term_doc : token_stream) {
		dictonary[term_doc.first].push_back(std::make_pair(doc_id, term_doc.second));
        doc_len += term_doc.second;
    }
}

void write_block_to_disk(std::map<std::string, std::list<std::pair<int, int>>>& dictionary, int block_num) {    
    std::ofstream f("../tmp/intermediate_" + std::to_string(block_num));

	if (f.fail()) 
        std::cout << "Error: not found intermediate file.\n";

	for (auto& kv : dictionary) {
		f << kv.first;
		for (auto& i : kv.second)
			f << ' ' << i.first << ' ' << i.second;
		f << std::endl;
	}
    f.close();
}

void process_document(std::vector<std::string> &documents, unsigned start_doc_id, unsigned int block_num, 
                       std::set<std::string> &stopwords, bool flag) {
    unsigned int doc_id = start_doc_id;
	std::string loaded_content;
	std::string doc_no;
	std::string text;
    std::map<std::string, std::list<std::pair<int, int>>> dictonary;
    std::cout << doc_id << " block_num:" << block_num << '\n';
    for (auto document : documents) {
        std::istringstream iss(document);
		getline(iss, doc_no, '\t');
		getline(iss, text, '\n');

        //if (block_num == 1)
		//    std::cout << "Processing doc_id: " << doc_id << std::endl;

        unsigned int doc_len;
        std::unordered_map<std::string, int> tokens;
        tokenize(text, flag, stopwords, tokens);
        add_to_posting_list(dictonary, tokens, doc_id, doc_len);

        doc_id++;
        // Update document table with current document information
		//doc_table[doc_id] = doc_table_entry{doc_no, doc_len};
    }
    write_block_to_disk(dictonary, block_num);
    dictonary.clear();
    documents[block_num - 1].clear();

}

void parse(const char* in, const unsigned int BLOCK_SIZE, bool flag, const char* stopwords_filename, unsigned int n_threads) {
	std::cout << "Started Parsing Phase: \n\n";

	//std::ifstream f(in, std::ios_base::in | std::ios_base::binary);
	// Check arguments validity
	/*
	if (f.fail())
		std::cout << "Error: input fail not valid.\n";

	if (BLOCK_SIZE == 0)
		std::cout << "Error: block size not valid.\n";
	*/

	boost::iostreams::stream<boost::iostreams::mapped_file_source> file;
	boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf;
	file.open(boost::iostreams::mapped_file_source(in));
	inbuf.push(boost::iostreams::gzip_decompressor());
	inbuf.push(file);

	//Convert streambuf to istream
	std::istream instream(&inbuf);

	//std::ifstream instream(in); //read from examples.txt for debugging
	// Document table output
	std::string doc_table_filename("../../output/doc_table.bin");

	unsigned int current_size = 0;
	unsigned int block_num = 1;
	unsigned int doc_id = 0;

	// std::map guarantees lexicographic ordering of the terms
	std::map<unsigned int, doc_table_entry> doc_table;

	std::string loaded_content;
	std::string doc_no;
	std::string text;

	// Load stopwords
	std::set<std::string> stopwords;

	if (flag) {
		std::ifstream is(stopwords_filename);
		std::string word;
		while (std::getline(is, word)) {
			stopwords.insert(word);
		}
	}

    // Constructs a thread pool with as many threads as available in the hardware.
    BS::thread_pool pool(n_threads-1);
    std::vector<std::vector<std::string>> docs;
    std::vector<std::string> row;
    docs.push_back(row);

	while (getline(instream, loaded_content)) {
		//std::cout << "Processing doc_id: " << doc_id << std::endl;

		current_size++; // ????

		// BSBI
		if (current_size < BLOCK_SIZE) {
            docs[block_num-1].push_back(loaded_content);
            continue;
		}
		else {
            pool.push_task(process_document, docs[block_num-1], doc_id, block_num, stopwords, flag);
            doc_id += BLOCK_SIZE;
			block_num++;
            current_size = 0;
            std::vector<std::string> row;
            docs.push_back(row);
            if (block_num == 6)
                break;
		}
	}

    // Write last block

    // Write document table
    //save_doc_table(doc_table, doc_table_filename);

	//inbuf.close();
}