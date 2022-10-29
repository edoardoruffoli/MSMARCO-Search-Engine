#include "MSMARCO-Search-Engine/parsing.hpp"

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
				tokens[token]++;
			}
			token.clear();
			continue;
		}
		token += char(tolower(itr));
	}
	return tokens;
}