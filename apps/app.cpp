#include "MSMARCO-Search-Engine/inverted_index.hpp"

int main(int argc, char* argv[])
{
	const int BLOCK_SIZE = 10000000;
	int current_size = 0;
	int block_num = 1;

	const char* docfile = argv[1];
	//std::cout << "--->Parameters<---" << std::endl;
	std::cout << "-> docfile: " << docfile << std::endl << std::endl;

	std::ifstream f(docfile);

	std::string loaded_content;
	int pid;
	std::string id;
	std::string text;
	std::unordered_map<std::string, std::list<int>> dictionary;

	while (getline(f, loaded_content)) {
		std::istringstream iss(loaded_content);
		getline(iss, id, '\t');
		pid = stoi(id);
		getline(iss, text, '\t');
		current_size += getFileSize(text);
		if (current_size < BLOCK_SIZE) {
			std::unordered_map<std::string, int> tokens = getTokens(text, pid);
			invert_index(tokens, dictionary);
		}
		else {
			write_block_to_disk(dictionary, block_num);
			current_size = 0;
			block_num++;
			dictionary.clear();
		}
	}

	write_block_to_disk(dictionary, block_num);

	for (auto& s : dictionary)
	{
		std::cout << s.first << " ";
		for (auto innerItr : s.second)
			std::cout << innerItr << " ";
		std::cout << std::endl;
	}

	f.close();
}
