#include "MSMARCO-Search-Engine/io.hpp"

bool save_intermediate_inv_idx(std::map<std::string, std::list<std::pair<int, int>>>& dictionary, 
                std::string &filename) {
    std::ofstream filestream(filename, std::ios::binary);
    if (filestream.fail()) {
        std::cout << "Fail intermediate inverted index write!" << std::endl;
        return false;
    }
    boost::archive::binary_oarchive archive(filestream);
    archive << dictionary;
    std::cout << "Intermediate Inverted Index saved!" << std::endl;
    return true;
}

bool save_lexicon(const std::map<std::string, lexicon_entry>& lexicon,
                std::string &filename) {
    std::ofstream filestream(filename, std::ios::binary);
    if (filestream.fail()) {
        std::cout << "Fail lexicon write!" << std::endl;
        return false;
    }
    boost::archive::binary_oarchive archive(filestream);
    archive << lexicon;
    std::cout << "Lexicon saved!" << std::endl;
    return true;
}

bool load_lexicon(std::map<std::string, lexicon_entry>* lexicon,
                std::string &filename) {
    std::ifstream filestream(filename, std::ios::binary);  
    if (filestream.fail()) {
        std::cout << "Fail lexicon read!" << std::endl;
        return false;
    }
    boost::archive::binary_iarchive archive(filestream);
    archive >> *lexicon;
    std::cout << "Lexicon read!" << std::endl;
    return true;
}

bool save_doc_table(const std::vector<doc_table_entry> &doc_table, std::string &filename) {
    std::ofstream filestream(filename, std::ios::binary);  
    if (filestream.fail()) 
        return false;
    boost::archive::binary_oarchive archive(filestream);
    archive << doc_table;
    return true;
}

bool load_doc_table(std::map<unsigned int, doc_table_entry> *doc_table, std::string &filename) {
    std::ifstream filestream(filename, std::ios::binary);  
    if (filestream.fail()) 
        return false;
    boost::archive::binary_iarchive archive(filestream);
    archive >> *doc_table;
    return true;
}
