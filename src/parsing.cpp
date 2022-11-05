#include "MSMARCO-Search-Engine/parsing.hpp"

std::unordered_map<std::string, int> tokenize(const std::string content, bool flag, const char *stopwords_filename) {
	//How to deal with empty page, malformed lines, malformed characters?
	std::unordered_map<std::string, int> tokens;    // <term, term_freq>
	boost::char_separator<char> sep(" \t\n");
	std::unordered_set<std::string> stopwords;
	//load stop words
	if (flag) {
		std::ifstream is(stopwords_filename);

		std::string word;
		while (std::getline(is, word)) {
			stopwords.insert(word);
		}
	}

	typedef boost::tokenizer< boost::char_separator<char> > t_tokenizer;
	t_tokenizer tok(content, sep);

	for (t_tokenizer::iterator beg = tok.begin(); beg != tok.end(); ++beg)
	{
		std::string token = *beg;
		boost::trim_if(token, boost::is_punct());
		if (token.size() == 0)
			continue;
		boost::algorithm::to_lower(token);
		if(flag)
			if (std::find(stopwords.begin(), stopwords.end(), token) != stopwords.end()) {
				continue;
			}
			token = porter2::Stemmer{}.stem(token);
		tokens[token]++;
	}

	return tokens;
}

void add_to_posting_list(std::map<std::string, std::list<std::pair<int, int>>>& dictonary,
                  const std::unordered_map<std::string, int>& token_stream, int doc_id) {
	for (const std::pair<std::string, int> term_doc : token_stream)
		dictonary[term_doc.first].push_back(std::make_pair(doc_id, term_doc.second));
}

void write_doc_table_record(std::ofstream &out, std::string &doc_no, unsigned int doc_len) {
    out << doc_no << ' ' << doc_len;
    out << '\n';
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

void parse(const char* in, const unsigned int BLOCK_SIZE, bool flag, const char* stopwords_filename) {
    std::cout << "Started Parsing Phase: \n\n";
	/*
    std::ifstream f(in, std::ios_base::in | std::ios_base::binary);
    // Check arguments validity
    if (f.fail()) 
        std::cout << "Error: input fail not valid.\n";
    
    if (BLOCK_SIZE == 0)
        std::cout << "Error: block size not valid.\n";

    boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf;
    inbuf.push(boost::iostreams::gzip_decompressor());
    inbuf.push(f);

    //Convert streambuf to istream
    std::istream instream(&inbuf);
	*/
	std::ifstream instream(in); //read from examples.txt for debugging
    // Document table output
    std::ofstream out_doc_table("../../output/doc_table");

    unsigned int current_size = 0;
	unsigned int block_num = 1;
    unsigned long doc_id = 0;

    // std::map guarantees lexicographic ordering of the terms
	std::map<std::string, std::list<std::pair<int, int>>> dictonary;

	std::string loaded_content;
	std::string doc_no;
	std::string text;

    while (getline(instream, loaded_content)) {
        std::cout << "Processing doc_id: " << doc_id << std::endl;

		std::istringstream iss(loaded_content);
		getline(iss, doc_no, '\t');
		getline(iss, text, '\n');

		std::unordered_map<std::string, int> tokens = tokenize(text, flag, stopwords_filename);
		//current_size += tokens.size();
        current_size++; // ????

        // Compute document length  (UNIRE A tokenize()??)
        unsigned int doc_len = boost::accumulate(tokens | boost::adaptors::map_values, 0);

        // BSBI
		if (current_size < BLOCK_SIZE) {
			add_to_posting_list(dictonary, tokens, doc_id);
		}
		else {
			write_block_to_disk(dictonary, block_num);
			current_size = 0;
			block_num++;
			dictonary.clear();
			add_to_posting_list(dictonary, tokens, doc_id);
		}

        // Update document table with current document
        write_doc_table_record(out_doc_table, doc_no, doc_len);

        doc_id++;
	}

    // Write last block
    write_block_to_disk(dictonary, block_num);

	instream.close();
}