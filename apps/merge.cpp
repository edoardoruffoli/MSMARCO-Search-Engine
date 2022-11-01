#include "MSMARCO-Search-Engine/merging.hpp"

int main(int argc, char* argv[]) {

    // Count the blocks
    unsigned int n_blocks = 0;
    while (boost::filesystem::exists("../tmp/intermediate_" + std::to_string(n_blocks+1))) {
        n_blocks++;
    }

    merge_blocks(n_blocks);
}
