#include "MSMARCO-Search-Engine/query.hpp"

// Doc table
std::set<doc_entry> doc_table;

std::map<std::string, unsigned long> lexicon;

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

unsigned int get_min_doc_id(std::vector<posting_list*> pls) {
    unsigned int min_doc_id = std::numeric_limits<unsigned int>::max();
    for (auto &pl : pls) {
        min_doc_id = std::min(min_doc_id, pl->getDocId);
    }
    return min_doc_id;
}

void conjunctive_query(std::priority_queue, std::vector<posting_list*> pls) {
    unsigned int did = 0;
    int d = 0;
    while(did < maxdocID){
        did = nextGEQ(lps[0], did);
        if(did >= maxdocID)
            break;
        for(int i = 1; i < cnt && ((d = nextGEQ(lps[i], did)) == did);i++);
        if(d > did) 
            did = d;
        else {
            double bm25 = 0;
            for(int i = 0; i < cnt; i++){
                int fp = getFreq(lps[i]);
                int ft = lps[i]->length;
                bm25 += calculate_BM25(fp, ft, doc_table[did]->size_of_doc);
            }
            to_rank_heap(hp, did, bm25, limit);
            did++;
        }
    }
}

bool execute_query(std::vector<std::string> &terms, unsigned int mode, int k) {
    std::vector<posting_list*> pls;
    for (auto term : terms) {

        auto it = lexicon.find(term);
        if (it != lexicon.end()) {
            posting_list *pl = new posting_list();
            pl->openList(it->second);
            pls.push_back(pl);
        }
        else {
            std::cout << term << " not present in lexicon.\n";
        }
    }

    // Min heap <doc_id, score>
    struct compare {
        bool operator()(std::pair<unsigned int, unsigned int> const& a, std::pair<unsigned int, unsigned int> const& b) const {
                if (a.second == b.second)
                    return a.first > b.first;
                return a.second > b.second;
        }
    };
    std::priority_queue<std::pair<unsigned int, unsigned int>, compare> min_heap(k);

    if (pls.size() == 0) {
        std::cout << "No terms found.\n";
        return false;
    }

    switch(mode){
        case CONJUNCTIVE_MODE:
            conjunctive_query(min_heap, lps); 
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
