#include "MSMARCO-Search-Engine/query.hpp"

bool init_data_structures() {
    std::cout << "Loading Lexicon and Document Table ...\n";

    if(load_lexicon(&lexicon, std::string("../../output/lexicon.bin")) == false) {
        std::cout << "Error: cannot open lexicon.\n";
        return false;
    }
    if(load_doc_table(&doc_table, std::string("../../output/doc_table.bin")) == false) {
        std::cout << "Error: cannot open document table.\n";
        return false;
    }
    printf("Done.\n");
    return true;
}

bool execute_query(std::vector<std::string> &terms, unsigned int mode) {

    std::vector<posting_list> pls;
    for (auto term : terms) {

        auto it = lexicon.find(term);
        if (it != lexicon.end()) {
            //pl.push_back(openList(it->second));
        }
        else {
            std::cout << term << " not present in lexicon.\n";
        }
    }

    // Min heap

    if (pls.size() == 0) {
        std::cout << "No terms found.\n";
        return false;
    }

    switch(mode){
        case CONJUNCTIVE_MODE:
            //conjunctive_query(hp,lps,cnt,limit); 
            break;
        case DISJUNCTIVE_MODE:
            //disjunctive_query(hp,lps,cnt,limit); 
            break;
    }

    // Showing results


    //for (auto pl : pls)
        //closeList(pl);

    return true;
}
/*
posting_list openList(std::string term, const std::map<std::string, std::pair<unsigned long, size_t>>& lexicon) {
    std::vector<char> cur;
    std::vector<int> postingL;
    char c;
    std::ifstream ifile;
    ifile.open("../tmp/uncompressed_inverted_index.bin", std::ios::binary);
    unsigned long offset = lexicon.find(term)->second.first;
    size_t p_len = lexicon.find(term)->second.second;

    ifile.seekg(offset);

    while (p_len) {
        ifile.get(c);
        cur.push_back(c);
        offset++;
        p_len--;
    }

    postingL = VBdecode(cur);

}
*/