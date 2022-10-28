// MSMARCO-Search-Engine.cpp : Defines the entry point for the application.
//

#include "MSMARCO-Search-Engine/inverted_index.h"

std::string clearToken(const std::string& token) {
	std::string cleared_token;
	for (char ch : token) {
		ch = char(tolower(ch));
		if (int(ch) >= 97 && int(ch) <= 122)
			cleared_token += ch;
	}
	return cleared_token;
}

std::unordered_map<std::string, int> getTokens(const std::string &content, const int& doc_id) {
	std::unordered_map<std::string, int> tokens; //pair<int, int>
	std::string token;
	for (char itr : content) {
		if (itr == ' ' || itr == '\n') {
			if (token.length() > 1) {
				token = clearToken(token);
				//if token is present count++
				tokens[token] = doc_id;
			}
			token.clear();
			continue;
		}
		token += char(tolower(itr));
	}
	return tokens;
}

void invert_index(const std::unordered_map<std::string, int>& token_stream, std::unordered_map<std::string, std::list<int>>& dictionary) {
	for (const std::pair<std::string, int> term_doc : token_stream)
		dictionary[term_doc.first].push_back(term_doc.second);
}

void write_block_to_disk(std::unordered_map<std::string, std::list<int>>& dictionary, int block_num) {
	std::stringstream ss;
	ss << block_num;
	std::string out_file;
	ss >> out_file;
	std::ofstream file("results" + out_file);
	if (file.fail()) std::cout << "Error\n";
	for (auto& kv : dictionary) {
		file << kv.first;
		for (auto& i : kv.second)
			file << " " << i;
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
