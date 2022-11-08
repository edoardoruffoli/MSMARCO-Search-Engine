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

void openList(std::string term, posting_list *result) {
    std::vector<char> cur;
    std::vector<unsigned int> decompressed_list;
    char c;

    std::ifstream ifile;
    
    ifile.open("../tmp/uncompressed_inverted_index.bin", std::ios::binary);
    auto it = lexicon.find(term);
    unsigned long offset = it->second.first;
    size_t p_len = it->second.second;

    ifile.seekg(offset);
    
    while (p_len) {
        ifile.get(c);
        cur.push_back(c);
        offset++;
        p_len--;
    }
    
    decompressed_list = VBdecode(cur);
    int size_pl = decompressed_list.size() / 2;

    for (std::vector<unsigned int>::iterator it = decompressed_list.begin(); it != decompressed_list.end(); ++it) {
        if (size_pl) {
            result->doc_ids.push_back(*it);
            size_pl--;
        }
        else {
            result->freqs.push_back(*it);
        }
    }
    ifile.close();
}
