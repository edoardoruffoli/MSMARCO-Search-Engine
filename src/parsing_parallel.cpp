#include "MSMARCO-Search-Engine/parsing.hpp"
#include "BS_thread_pool.hpp"

// https://tristanbrindle.com/posts/a-quicker-study-on-tokenising/
template <class InputIt, class ForwardIt, class BinOp>
void for_each_token(InputIt first, InputIt last,
                    ForwardIt s_first, ForwardIt s_last,
                    BinOp binary_op)
{
    while (first != last) {
        const auto pos = find_first_of(first, last, s_first, s_last);
        binary_op(first, pos);
        if (pos == last) break;
            first = next(pos);
    }
}

void tokenize(const std::string &content, const bool flag, const std::unordered_set<std::string> &stopwords, 
                                              std::unordered_map<std::string, int> &tokens) {
	// How to deal with empty page, malformed lines, malformed characters?
    std::string delims = "\t ";
    for_each_token(cbegin(content), cend(content), cbegin(delims), cend(delims), 
                    [&tokens, flag, &stopwords] (auto first, auto second) 
                    {
                        if (first != second) {
                            std::string token = std::string(first, second);

                            // Remove punctuation
                            token.erase(
                                std::remove_if(token.begin(), token.end(),[] (unsigned char c) { 
                                    return ispunct(c); 
                                }),
                                token.end()
                            );

                            // To lower
                            std::transform(token.begin(), token.end(), token.begin(), [](unsigned char c) {
                                return std::tolower(c);
                            });

                            // Check if stopword and stem
                            if (flag && stopwords.find(token) == stopwords.end()) 
                                token = porter2::Stemmer{}.stem(token);

                            tokens[token]++;
                        }
                    });
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

void BSBI_Invert(std::vector<std::string> &documents, unsigned int start_doc_id, unsigned int block_num, 
                       BS::thread_pool &pool, std::vector<doc_table_entry> &doc_table,
                       std::unordered_set<std::string> &stopwords, bool flag) {
    BS::timer tmr;
    tmr.start();
    std::map<std::string, std::list<std::pair<int, int>>> dictionary;
    std::cout << "Starting doc_id: " << start_doc_id << " block_num:" << block_num << " num_docs: " << documents.size() << '\n';

    // Add typedef <std::map<std::string, std::list<std::pair<int, int>>>

    auto process_block = [documents, start_doc_id, doc_table, flag, stopwords](const unsigned start, const unsigned end) 
    {
        std::cout << "Launched Worker Thread, Interval: [" << start << "," << end << "]\n";
        std::string doc_no;
        std::string text;
        std::istringstream iss;
        std::map<std::string, std::list<std::pair<int, int>>> dict;
        std::unordered_map<std::string, int> tokens;
        unsigned int doc_len;
                                
        for (unsigned int i = start; i < end; i++) {
            iss = std::istringstream(documents[i]);
            getline(iss, doc_no, '\t');
            getline(iss, text, '\n');
            tokenize(text, flag, stopwords, tokens);
            add_to_posting_list(dict, tokens, start_doc_id + i, doc_len);
            tokens.clear();
            doc_table[start_doc_id + i].doc_len;
            doc_table[start_doc_id + i].doc_no;
        }
        return dict;
    };

    // Parallel Processing of the docs
    BS::multi_future<std::map<std::string, std::list<std::pair<int, int>>>> mf = pool.parallelize_loop(0, documents.size(),
                            process_block);

    std::vector<std::map<std::string, std::list<std::pair<int, int>>>> totals = mf.get();
    std::cout << "Ended Paralleling Processing: \n"; 

    // Merge maps
    for (auto output_dict : totals) {   // The output map are ordered
        for (auto entry : output_dict) {
            // If term does not exist create it otherwise concate the posting list
            if (dictionary.find(entry.first) == dictionary.end()) {
                dictionary[entry.first] = entry.second;
            } else {
                dictionary[entry.first].splice(dictionary[entry.first].end(), entry.second);
            }
        }
   }

    write_block_to_disk(dictionary, block_num);
    tmr.stop();
    std::cout << "The elapsed time was " << tmr.ms() << " ms.\n";
}

void parse(const char* in, const unsigned int BLOCK_SIZE, bool flag, const char* stopwords_filename, unsigned int n_threads) {
	std::cout << "Started Parsing Phase: \n\n";

	if (BLOCK_SIZE == 0)
		std::cout << "Error: block size not valid.\n";

	boost::iostreams::stream<boost::iostreams::mapped_file_source> file;
	boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf;
	file.open(boost::iostreams::mapped_file_source(in));
    if (file.fail())
		std::cout << "Error: input fail not valid.\n";
    
    // Compressed reading
	inbuf.push(boost::iostreams::gzip_decompressor());
	inbuf.push(file);

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
    BS::thread_pool pool;

    // Stores a block of documents
    std::vector<std::string> block;

    // Init
    unsigned int current_size = 0;
	unsigned int block_num = 1;
	unsigned int doc_id = 0;
    std::string loaded_content;

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

	//inbuf.close();
}