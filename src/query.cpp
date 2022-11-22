#include "MSMARCO-Search-Engine/query.hpp"

// Doc table
std::vector<doc_table_entry> doc_table;

std::map<std::string, lexicon_entry> lexicon;

double avg_doc_len;

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

    // IF BM25 compute avg doc len
    unsigned int sum = 0;
    for (auto doc : doc_table) {
        sum += doc.doc_len;
    }
    avg_doc_len = (double)sum/doc_table.size();

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

bool get_conjunctive_doc_id(std::vector<posting_list*> pls) {
    unsigned int min_doc_id = std::numeric_limits<unsigned int>::max();
    bool find = false;
    while (!find) {
        auto prev = pls.begin();   
        for (auto pl = pls.begin()+1; pl != pls.end(); pl++) {
            if (((*prev)->cur_doc_id == min_doc_id) || ((*pl)->cur_doc_id == min_doc_id)) {
                return false;
            }
            else if ((*prev)->cur_doc_id < (*pl)->cur_doc_id) {
                (*prev)->next();
                break;
            }
            else if ((*prev)->cur_doc_id > (*pl)->cur_doc_id) {
                (*pl)->next();
                break;
            }
            else {
                prev = pl;
                if (pl == pls.end() - 1) {
                    find = true;
                }
            }
        }
    }
    return true;
}

void conjunctive_query(std::priority_queue<std::pair<unsigned int, double>, 
                       std::vector<std::pair<unsigned int, double>>, compare> &min_heap,
                       std::vector<posting_list*> pls,
                       unsigned int k) {

    bool find = get_conjunctive_doc_id(pls);

    while (find) {
        double score = 0.0;
        unsigned int cur_doc_id;
        for (auto& pl : pls) {
            cur_doc_id = pl->getDocId();
            unsigned int term_freq = pl->getFreq();
            unsigned int doc_len = doc_table[cur_doc_id].doc_len; // ONLY BM25
            unsigned int doc_freq = pl->doc_freq;
            unsigned int N = (unsigned int)doc_table.size();   // O(1)
            //score += TFIDF(term_freq, doc_freq, N);
            score += BM25(term_freq, doc_freq, doc_len, avg_doc_len, N);
            pl->next();
        }
        // SCORE OF A DOCUMENT
        if (min_heap.size() >= k) {
            if (min_heap.top().second < score) {
                min_heap.pop();
                min_heap.push(std::make_pair(cur_doc_id, score));
            }
        }
        else {
            min_heap.push(std::make_pair(cur_doc_id, score));
        }
        find = get_conjunctive_doc_id(pls);
    }
}


void disjunctive_query(std::priority_queue<std::pair<unsigned int, double>, 
                       std::vector<std::pair<unsigned int, double>>, compare> &min_heap,
                       std::vector<posting_list*> pls,
                       unsigned int k) {

    unsigned int cur_doc_id = get_min_doc_id(pls);
    unsigned int max_doc_id = std::numeric_limits<unsigned int>::max();

    while(cur_doc_id != max_doc_id){
        double score = 0.0;
        for (auto &pl : pls) {
            if (pl->getDocId() == cur_doc_id) {
                unsigned int term_freq = pl->getFreq();
                unsigned int doc_len = doc_table[cur_doc_id].doc_len; // ONLY BM25
                unsigned int doc_freq = pl->doc_freq;
                unsigned int N = (unsigned int) doc_table.size();   // O(1)
                //score += TFIDF(term_freq, doc_freq, N);
                //std::cout << doc_len << " " << avg_doc_len << std::endl;
                score += BM25(term_freq, doc_freq, doc_len, avg_doc_len, N);
                pl->next();
            }
        }
        // SCORE OF A DOCUMENT
        if (min_heap.size() >= k) {
            if (min_heap.top().second < score) {
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

void disjunctive_query_max_score(std::priority_queue<std::pair<unsigned int, double>, 
                                std::vector<std::pair<unsigned int, double>>, compare> &min_heap,
                                std::vector<posting_list*> &pls,
                                std::vector<double> &max_scores,
                                unsigned int k) {

    std::vector<double> upper_bounds(max_scores.size(), 0);
    upper_bounds[0] = max_scores[0];
    for (unsigned int i = 1; i < upper_bounds.size(); i++) {
        upper_bounds[i] = upper_bounds[i-1] + max_scores[i];
    }

    double teta = 0;
    unsigned int pivot = 0;
    unsigned int cur_doc_id = get_min_doc_id(pls);
    unsigned int max_doc_id = std::numeric_limits<unsigned int>::max();

    while (pivot < pls.size() && cur_doc_id != max_doc_id) {
        double score = 0;
        unsigned int next_doc_id = std::numeric_limits<unsigned int>::max();

        // Essential lists
        for (unsigned int i = pivot; i < pls.size(); i++) {
            if (pls[i]->getDocId() == cur_doc_id) {
                unsigned int term_freq = pls[i]->getFreq();
                unsigned int doc_len = doc_table[cur_doc_id].doc_len; // ONLY BM25
                unsigned int doc_freq = pls[i]->doc_freq;
                unsigned int N = (unsigned int) doc_table.size();   // O(1)
                //score += TFIDF(term_freq, doc_freq, N);
                //std::cout << doc_len << " " << avg_doc_len << std::endl;
                score += BM25(term_freq, doc_freq, doc_len, avg_doc_len, N);
                pls[i]->next();
            }
            if (pls[i]->getDocId() < next_doc_id) {
                next_doc_id = pls[i]->getDocId();
            }
        }

        // Non essential lists
        for (int i = pivot-1; i >= 0; i--) {
            if (score + upper_bounds[i] <= teta)
                break;
            pls[i]->nextGEQ(cur_doc_id);
            if (pls[i]->getDocId() == cur_doc_id) {
                unsigned int term_freq = pls[i]->getFreq();
                unsigned int doc_len = doc_table[cur_doc_id].doc_len; // ONLY BM25
                unsigned int doc_freq = pls[i]->doc_freq;
                unsigned int N = (unsigned int) doc_table.size();   // O(1)
                //score += TFIDF(term_freq, doc_freq, N);
                //std::cout << doc_len << " " << avg_doc_len << std::endl;
                score += BM25(term_freq, doc_freq, doc_len, avg_doc_len, N);
            }
        }

        if (min_heap.size() >= k) {
            if (min_heap.top().second < score) {
                min_heap.pop();
                min_heap.push(std::make_pair(cur_doc_id, score));

                teta = min_heap.top().second;
                while (pivot < pls.size() && upper_bounds[pivot] <= teta)
                    pivot++;
            }
        }
        else {
            min_heap.push(std::make_pair(cur_doc_id, score));
            teta = min_heap.top().second;

            while (pivot < pls.size() && upper_bounds[pivot] <= teta)
                pivot++;
        }
        cur_doc_id = next_doc_id;
    }
}

bool execute_query(std::vector<std::string> &terms, unsigned int mode, unsigned int k) {
    std::cout << "Executing query\n";
    boost::chrono::high_resolution_clock::time_point t1 = boost::chrono::high_resolution_clock::now();

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

    // Vector of max score one per query term
    std::vector<double> max_scores;
    for (auto &term : terms) {
        max_scores.push_back(lexicon[term].max_score);
    }

    switch(mode){
        case CONJUNCTIVE_MODE:
            if(pls.size() != 1)
                conjunctive_query(min_heap, pls, k); 
            else
                disjunctive_query(min_heap, pls, k);
            break;
        case DISJUNCTIVE_MODE:
            disjunctive_query(min_heap, pls, k);
            break;
        case DISJUNCTIVE_MODE_MAX_SCORE:
            disjunctive_query_max_score(min_heap, pls, max_scores, k); 
            break;
    }

    boost::chrono::high_resolution_clock::time_point t2 = boost::chrono::high_resolution_clock::now();
    std::cout << "The elapsed time was " << boost::chrono::duration_cast<boost::chrono::nanoseconds>(t2-t1) << " ns.\n";

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
