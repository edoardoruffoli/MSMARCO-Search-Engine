#include "MSMARCO-Search-Engine/query.hpp"

void init_data_structures() {
    std::cout << "Loading Lexicon and Document Table ...\n";

    if(load_lexicon(&lexicon, std::string("../../output/lexicon.bin")) == false ||
        load_doc_table(&doc_table, std::string("../../output/doc_table.bin")) == false)
        return false;
    printf("Done.\n");
    return true;
}

bool execute_query(std::vector<string> &terms, unsigned int type) {

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

    if (pl.size() == 0) {
        std::cout << "No terms found.\n";
        return;
    }

    switch(MODE){
        case CONJUNCTIVE_MODE:
            //conjunctive_query(hp,lps,cnt,limit); 
            break;
        case DISJUNCTIVE_MODE:
            //disjunctive_query(hp,lps,cnt,limit); 
            break;
    }

    // Showing results


    for (auto pl : pls)
        closeList(pl);

    return true;
}