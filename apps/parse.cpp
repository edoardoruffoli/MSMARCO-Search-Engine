#include "MSMARCO-Search-Engine/parsing.hpp"

int main(int argc, char* argv[])
{
	const int BLOCK_SIZE = 100;
	const char* docfile = "../../collection.tar.gz";

    parse(docfile, BLOCK_SIZE);
    //merge_blocks(block_num);
}
