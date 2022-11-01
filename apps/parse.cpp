#include "MSMARCO-Search-Engine/parsing.hpp"

int main(int argc, char* argv[])
{
	const int BLOCK_SIZE = 100;
	const char* docfile = "../../examples.txt";

    parse(docfile, BLOCK_SIZE);
    //merge_blocks(block_num);
}
