#include "MSMARCO-Search-Engine/inverted_index.hpp"

void invert_index(const std::unordered_map<std::string, int>& token_stream, std::unordered_map<std::string, std::list<std::pair<int, int>>>& partial_inv_idx, int docId) {
	for (const std::pair<std::string, int> term_doc : token_stream)
		partial_inv_idx[term_doc.first].push_back(std::make_pair(docId, term_doc.second));
}

void write_block_to_disk(std::unordered_map<std::string, std::list<std::pair<int, int>>>& dictionary, int block_num) {
	std::stringstream ss;
	ss << block_num;
	std::string out_file;
	ss >> out_file;
	std::ofstream file("../../results" + out_file);
	if (file.fail()) std::cout << "Error\n";
	for (auto& kv : dictionary) {
		file << kv.first;
		for (auto& i : kv.second)
			file << ' ' << i.first << ' ' << i.second;
		file << std::endl;
	}
}
