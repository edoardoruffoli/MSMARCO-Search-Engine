#include "MSMARCO-Search-Engine/parsing.hpp"
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/locale.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>

std::unordered_map<std::string, int> getTokens(const std::string content) {
	//How to deal with empty page, malformed lines, malformed characters?
	std::unordered_map<std::string, int> tokens;

	boost::char_separator<char> sep(" ");
	typedef boost::tokenizer< boost::char_separator<char> > t_tokenizer;
	t_tokenizer tok(content, sep);

	for (t_tokenizer::iterator beg = tok.begin(); beg != tok.end(); ++beg)
	{
		std::string token = *beg;
		boost::trim_if(token, boost::is_punct());
		if (token.size() == 0)
			continue;
		//std::cout << token << std::endl;
		boost::algorithm::to_lower(token);
		tokens[token]++;
	}

	return tokens;
}