#include "MSMARCO-Search-Engine/inverted_index.hpp"
#include "MSMARCO-Search-Engine/parsing.hpp"

int main(int argc, char* argv[])
{
	const int BLOCK_SIZE = 10000;
	int current_size = 0;
	int block_num = 1;

	const char* docfile = "../../examples.txt";//argv[1];
	//std::cout << "--->Parameters<---" << std::endl;
	std::cout << "-> docfile: " << docfile << std::endl << std::endl;

	std::ifstream f(docfile);

	std::string loaded_content;
	int docId;
	std::string id;
	std::string text;
    /*
    // std::map guarantees lexicographic terms ordering
	std::map<std::string, std::list<std::pair<int, int>>> partial_inv_idx; //list<MSMarco::posting>>
    
	while (getline(f, loaded_content)) {
		std::istringstream iss(loaded_content);
		getline(iss, id, '\t');
		docId = stoi(id);
        std::cout << "Processing " << docId << std::endl;
		getline(iss, text, '\n');

		std::unordered_map<std::string, int> tokens = getTokens(text);
		//current_size += tokens.size();
        current_size++;

		if (current_size < BLOCK_SIZE) {
			invert_index(partial_inv_idx, tokens, docId);
		}
		else {
			write_block_to_disk(partial_inv_idx, block_num);
			current_size = 0;
			block_num++;
			partial_inv_idx.clear();
			invert_index(partial_inv_idx, tokens, docId);
		}
	}

    write_block_to_disk(partial_inv_idx, block_num);
    */
    // Merge blocks
    merge_blocks(11);
    
	f.close();
}
