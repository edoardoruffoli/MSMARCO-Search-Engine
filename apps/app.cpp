#include "MSMARCO-Search-Engine/io.hpp"

int main(int argc, char* argv[]) {
    std::cout << "Reading" << std::endl;
    std::map<std::string, std::pair<unsigned long, size_t>> lexicon;
    std::set<doc_entry> doc_table;
    load_doc_table(&doc_table, std::string("../../output/doc_table.bin"));

    auto it = doc_table.find(13);
    std::cout << it->doc_id << " " << it->doc_no << " " << it->doc_len << std::endl;
}