#include "MSMARCO-Search-Engine/io.hpp"


void save_lexicon(const std::map<std::string, std::pair<unsigned long, size_t>>& lexicon,
                std::string &filename) {
    std::ofstream filestream(filename, std::ios::binary);     
    if (filestream.fail()) 
        std::cout << "Error: cannot open lexicon\n";
    boost::archive::binary_oarchive archive(filestream);
    archive << lexicon;
}

void load_lexicon(std::map<std::string, std::pair<unsigned long, size_t>>* lexicon,
                std::string &filename) {
    std::ifstream filestream(filename, std::ios::binary);  
    if (filestream.fail()) 
        std::cout << "Error: cannot open lexicon\n";
    boost::archive::binary_iarchive archive(filestream);

    archive >> *lexicon;
}
