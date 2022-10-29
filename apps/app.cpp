#include "MSMARCO-Search-Engine/inverted_index.hpp"
#include <iostream>
#include <fstream>

#include <codecvt>
#include <fcntl.h>
#include <io.h>

int main(int argc, char* argv[])
{
	const int BLOCK_SIZE = 10000000;
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
	std::unordered_map<std::string, std::list<std::pair<int, int>>> partial_inv_idx; //list<MSMarco::posting>>

	while (getline(f, loaded_content)) {
		std::istringstream iss(loaded_content);
		getline(iss, id, '\t');
		docId = stoi(id);
		getline(iss, text, '\n');

		std::unordered_map<std::string, int> tokens = getTokens(text);
		current_size += tokens.size();

		//current_size += getFileSize(text);
		if (current_size < BLOCK_SIZE) {
			invert_index(tokens, partial_inv_idx, docId);
		}
		else {
			write_block_to_disk(partial_inv_idx, block_num);
			current_size = 0;
			block_num++;
			partial_inv_idx.clear();
			invert_index(tokens, partial_inv_idx, docId);
		}
	}

	write_block_to_disk(partial_inv_idx, block_num);

	for (auto& s : partial_inv_idx)
	{
		std::cout << s.first << " ";
		for (auto innerItr : s.second)
			std::cout << innerItr.first << " " << innerItr.second;
		std::cout << std::endl;
	}

	f.close();
}
