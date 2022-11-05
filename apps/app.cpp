#include "MSMARCO-Search-Engine/io.hpp"

int main(int argc, char* argv[]) {
    std::cout << "Reading" << std::endl;
    std::map<std::string, std::pair<unsigned long, size_t>> lexicon;
    load_lexicon(&lexicon, std::string("../../output/lexicon.bin"));

    std::cout << lexicon["being"].first << " " << lexicon["being"].second << std::endl;
}