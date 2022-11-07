#include "MSMARCO-Search-Engine/io.hpp"

int main(int argc, char* argv[]) {
    std::cout << "Reading" << std::endl;
    std::map<std::string, std::pair<unsigned long, size_t>> lexicon;
    std::set<doc_entry> doc_table;
    load_lexicon(&lexicon, std::string("../../output/lexicon.bin"));

    //auto it = doc_table.find(13);
    //std::cout << it->first << " " << it->second.first << " " << it->second.second << std::endl;
    for (std::map<std::string, std::pair<unsigned long, size_t>>::iterator it = lexicon.begin(); it != lexicon.end(); it++) {
        std::cout << it->first << " " << it->second.first << " " << it->second.second << std::endl;
    }
}