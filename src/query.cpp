#include "MSMARCO-Search-Engine/query.hpp"

// Doc table
std::map<unsigned int, doc_table_entry> doc_table;

std::map<std::string, lexicon_entry> lexicon;

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
        min_doc_id = std::min(min_doc_id, pl->getDocId());
    }
    return min_doc_id;
}

double TFIDF(unsigned int tf, unsigned int df, unsigned int N) {
    return (1.0 + log10(tf))*log10((double)N/df);
}


// Min heap <doc_id, score>
struct compare {
    bool operator()(std::pair<unsigned int, double> const& a, std::pair<unsigned int, double> const& b) const {
            if (a.second == b.second)
                return a.first > b.first;
            return a.second > b.second;
    }
};

void conjunctive_query(std::priority_queue<std::pair<unsigned int, double>, 
                       std::vector<std::pair<unsigned int, double>>, compare> &min_heap,
                       std::vector<posting_list*> pls,
                       unsigned int k) {

    unsigned int cur_doc_id = get_min_doc_id(pls);
    unsigned int max_doc_id = std::numeric_limits<unsigned int>::max();
    int d = 0;

    while(cur_doc_id != max_doc_id){
        double score = 0.0;
        for (auto &pl : pls) {
            if (pl->getDocId() == cur_doc_id) {
                unsigned int term_freq = pl->getFreq();
                //unsigned int doc_len = doc_table[cur_doc_id]; // ONLY BM25
                unsigned int doc_freq = pl->doc_freq;
                unsigned int N = (unsigned int) doc_table.size();   // O(1)
                score += TFIDF(term_freq, doc_freq, N);
                pl->next();
            }
        }
        // SCORE OF A DOCUMENT
        if (min_heap.size() >= k) {
            if (min_heap.top().second > score) {
                min_heap.pop();
                min_heap.push(std::make_pair(cur_doc_id, score));
            }
        }
        else {
            min_heap.push(std::make_pair(cur_doc_id, score));
        }

        // Update current doc id
        cur_doc_id = get_min_doc_id(pls);
    }
}

bool execute_query(std::vector<std::string> &terms, unsigned int mode, unsigned int k) {
    std::vector<posting_list*> pls;
    for (auto term : terms) {

        auto it = lexicon.find(term);
        if (it != lexicon.end()) {
            posting_list *pl = new posting_list();
            pl->openList(it->second.offset);
            pl->doc_freq = it->second.doc_freq;
            pls.push_back(pl);
        }
        else {
            std::cout << term << " not present in lexicon.\n";
        }
    }

    std::priority_queue<std::pair<unsigned int, double>, 
                        std::vector<std::pair<unsigned int, double>>, compare> min_heap;

    if (pls.size() == 0) {
        std::cout << "No terms found.\n";
        return false;
    }

    switch(mode){
        case CONJUNCTIVE_MODE:
            conjunctive_query(min_heap, pls, k); 
            break;
        case DISJUNCTIVE_MODE:
            //disjunctive_query(hp,lps,cnt,limit); 
            break;
    }

    // Showing results
    std::vector<std::pair<unsigned int, double>> results;
    while (!min_heap.empty()) {
        std::pair<unsigned int, double> tmp = min_heap.top();
        min_heap.pop();
        results.push_back(tmp);
    }

    std::cout << "RESULTS:\nDoc Id\tScore\n";
    for (int i=results.size()-1; i>=0; i--) {
        std::cout << results[i].first << '\t' << results[i].second << '\n';
    }

    for (auto pl : pls) {
        pl->closeList();
    }

    return true;
}
