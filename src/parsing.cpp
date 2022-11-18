#include "MSMARCO-Search-Engine/parsing.hpp"

std::unordered_map<std::string, int> tokenize(const std::string &content, bool flag, 
                                              std::set<std::string> &stopwords) {
	//How to deal with empty page, malformed lines, malformed characters?
	std::unordered_map<std::string, int> tokens;    // <term, term_freq>
	boost::char_separator<char> sep(" \t\n");

	typedef boost::tokenizer< boost::char_separator<char> > t_tokenizer;
	t_tokenizer tok(content, sep);

	for (t_tokenizer::iterator beg = tok.begin(); beg != tok.end(); ++beg) {
		std::string token = *beg;

        // Remove punctuation
        token.erase(
            std::remove_if(token.begin(), token.end(),[] (unsigned char c) { 
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

		if(flag) {
			if (stopwords.find(token) != stopwords.end()) { //O(logN) using std::set
				continue;
			}
			token = porter2::Stemmer{}.stem(token);
        }
		tokens[token]++;
	}

	return tokens;
}

void add_to_posting_list(std::map<std::string, std::list<std::pair<int, int>>>& dictonary,
                  const std::unordered_map<std::string, int>& token_stream, int doc_id, unsigned int &doc_len) {
	for (const std::pair<std::string, int> term_doc : token_stream) {
		dictonary[term_doc.first].push_back(std::make_pair(doc_id, term_doc.second));
        doc_len += term_doc.second;
    }
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
	std::map<std::string, std::list<std::pair<int, int>>> dictonary;
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

	while (getline(instream, loaded_content)) {
		//#pragma omp task firstprivate(loaded_content)
		//std::cout << omp_get_thread_num() << std::endl;
		//std::cout << "Processing doc_id: " << doc_id << std::endl;

		std::istringstream iss(loaded_content);
		getline(iss, doc_no, '\t');
		getline(iss, text, '\n');
		std::cout << "Processing doc_id: " << doc_id << std::endl;
				
		std::unordered_map<std::string, int> tokens = tokenize(text, flag, stopwords);
		//current_size += tokens.size();
		current_size++; // ????

		// Compute document length
		unsigned int doc_len;

		// BSBI
		if (current_size < BLOCK_SIZE) {
			add_to_posting_list(dictonary, tokens, doc_id, doc_len);
		}
		else {
			write_block_to_disk(dictonary, block_num);
			current_size = 0;
			block_num++;
			dictonary.clear();
			add_to_posting_list(dictonary, tokens, doc_id, doc_len);
		}

		// Update document table with current document information
		doc_table[doc_id] = doc_table_entry{doc_no, doc_len};

		// Next doc_id
		doc_id++;
				
	}

    // Write last block
    write_block_to_disk(dictonary, block_num);

    // Write document table
    save_doc_table(doc_table, doc_table_filename);

	//inbuf.close();
}