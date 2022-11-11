#include "MSMARCO-Search-Engine/query.hpp"

// Doc table
std::set<doc_entry> _doc_table;

std::map<std::string, unsigned long> _lexicon;

bool init_data_structures() {
    std::cout << "Loading Lexicon and Document Table ...\n";

    if(load_lexicon(&_lexicon, std::string("../../output/lexicon.bin")) == false) {
        std::cout << "Error: cannot open lexicon.\n";
        return false;
    }
    if(load_doc_table(&_doc_table, std::string("../../output/doc_table.bin")) == false) {
        std::cout << "Error: cannot open document table.\n";
        return false;
    }
    printf("Done.\n");
    return true;
}

bool execute_query(std::vector<std::string> &terms, unsigned int mode) {
    std::vector<posting_list*> pls;
    for (auto term : terms) {

        auto it = _lexicon.find(term);
        if (it != _lexicon.end()) {
            posting_list *pl = new posting_list();
            pl->openList(it->second);
            pls.push_back(pl);

            // Test
            pl->next();
            std::cout <<"TEST: " << pl->cur_doc_id << " " << pl->cur_freq << std::endl;;
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


    for (auto pl : pls) {
        pl->closeList();
        delete pl;
    }

    return true;
}
