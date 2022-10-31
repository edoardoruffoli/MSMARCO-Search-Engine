#include "MSMARCO-Search-Engine/inverted_index.hpp"
#include "MSMARCO-Search-Engine/parsing.hpp"

int main(int argc, char* argv[])
{
	const int BLOCK_SIZE = 100;
	int current_size = 0;
	int block_num = 1;

	const char* docfile = "../../examples.txt";//argv[1];
	//std::cout << "--->Parameters<---" << std::endl;
	std::cout << "-> docfile: " << docfile << std::endl << std::endl;

	std::ifstream f(docfile);

	std::string loaded_content;
	unsigned long doc_id = 0;
	std::string doc_no;
	std::string text;

    std::ofstream out_doc_table("../../results/doc_table");

    // std::map guarantees lexicographic terms ordering
	std::map<std::string, std::list<std::pair<int, int>>> partial_inv_idx; //list<MSMarco::posting>>
    
	while (getline(f, loaded_content)) {
        std::cout << "Processing " << doc_id << std::endl;

		std::istringstream iss(loaded_content);
		getline(iss, doc_no, '\t');
		getline(iss, text, '\n');

		std::unordered_map<std::string, int> tokens = getTokens(text);
		//current_size += tokens.size();
        current_size++; // ????

        // Compute doc_len 
        unsigned int doc_len = boost::accumulate(tokens | boost::adaptors::map_values, 0);
        write_doc_table_record(out_doc_table, doc_no, doc_len);

		if (current_size < BLOCK_SIZE) {
			invert_index(partial_inv_idx, tokens, doc_id);
		}
		else {
			write_block_to_disk(partial_inv_idx, block_num);
			current_size = 0;
			block_num++;
			partial_inv_idx.clear();
			invert_index(partial_inv_idx, tokens, doc_id);
		}
        doc_id++;
	}

    write_block_to_disk(partial_inv_idx, block_num);

    // Merge blocks
    merge_blocks(block_num);
    
	f.close();
}
