#include "MSMARCO-Search-Engine/parsing.hpp"

int main(int argc, char* argv[])
{
	const int BLOCK_SIZE = 100000;
	const char* docfile = "../../collection.tar.gz";
	//const char* docfile = "../../examples.txt";
	const char* stopwords = "../../stopwords.txt";
    parse(docfile, BLOCK_SIZE, false, stopwords);
    //merge_blocks(block_num);
}
