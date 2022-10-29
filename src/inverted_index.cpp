// MSMARCO-Search-Engine.cpp : Defines the entry point for the application.
//

#include "MSMARCO-Search-Engine/inverted_index.hpp"

std::string clearToken(const std::string& token) {
	std::string cleared_token;
	for (char ch : token) {
		ch = char(tolower(ch));
		if (int(ch) >= 97 && int(ch) <= 122)
		cleared_token += ch;
	}
	return cleared_token;
}

std::unordered_map<std::string, int> getTokens(const std::string &content) {
	std::unordered_map<std::string, int> tokens;
	std::string token;
	for (char itr : content) {
		if (itr == ' ' || itr == '\n') {
			if (token.length() > 1) {
				token = clearToken(token);
				//if (tokens.find(token) == tokens.end())
				//	tokens[token].first = doc_id;
				tokens[token]++;

			}
			token.clear();
			continue;
		}
		token += char(tolower(itr));
	}
	return tokens;
}

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

//questa operazione viene fatta due volte!!!
int getFileSize(const std::string& text) {
	int counter = 0;
	for (char itr : text) {
		if (itr == ' ' || itr == '\n') {
			counter++;
			continue;
		}
	}
	return counter;
}