#include "MSMARCO-Search-Engine/parsing.hpp"

int main(int argc, char* argv[])
{
	unsigned int n_threads = 4;
	const int BLOCK_SIZE = 500000;
	const char* docfile = "../../collection.tar.gz";
	//const char* docfile = "../../examples.txt";
	const char* stopwords = "../../stopwords.txt";
    parse(docfile, BLOCK_SIZE, true, stopwords, n_threads);
    //merge_blocks(block_num);
}
