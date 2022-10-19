#include "searchengine.h";

using namespace std;

string clearToken(const string& token) {
	string cleared_token;
	for (char ch : token) {
		ch = char(tolower(ch));
		if (int(ch) >= 97 && int(ch) <= 122)
			cleared_token += ch;
	}
	return cleared_token;
}

map<string, int> getTokens(const string& content, const int& doc_id) {
	map<string, int> tokens; //pair<int, int>
	string token;
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

void invert_index(const map<string, int>& token_stream, map<string, list<int>>& dictionary) {
	for (const pair<string, int> term_doc : token_stream)
		dictionary[term_doc.first].push_back(term_doc.second);
}

void write_block_to_disk(map<string, list<int>>& dictionary, int block_num) {
	stringstream ss;
	ss << block_num;
	string out_file;
	ss >> out_file;
	ofstream file("C:\\Users\\franc\\vsProjects\\searchengine\\" + out_file);
	if (file.fail()) cout << "Error\n";
	for (auto& kv : dictionary) {
		file << kv.first;
		for (auto& i : kv.second)
			file << " " << i;
		file << endl;
	}
}

//questa operazione viene fatta due volte!!!
int getFileSize(const string& text) {
	int counter = 0;
	for (char itr : text) {
		if (itr == ' ' || itr == '\n') {
			counter++;
			continue;
		}
	}
	return counter;
}

int main(int argc, char* argv[]) {

	const int BLOCK_SIZE = 10000000;
	int current_size = 0;
	int block_num = 1;

	char* docfile = argv[1];
	cout << "--->Parameters<---" << endl;
	cout << "-> docfile: " << docfile << endl << endl;
	
	ifstream f(docfile);

	string loaded_content;
	int pid;
	string id;
	string text;
	map<string, list<int>> dictionary;

	while (getline(f, loaded_content)) {
		istringstream iss(loaded_content);
		getline(iss, id, '\t');
		pid = stoi(id);
		getline(iss, text, '\t');
		current_size += getFileSize(text);
		if (current_size < BLOCK_SIZE) {
			map<string, int> tokens = getTokens(text, pid);
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
		cout << s.first << " ";
		for (auto innerItr : s.second)
			cout << innerItr << " ";
		cout << endl;
	}

	f.close();

}
