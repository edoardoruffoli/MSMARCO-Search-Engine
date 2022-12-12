#include "MSMARCO-Search-Engine/query.hpp"

// Doc Table
DocTable doc_table;

// Lexicon
Lexicon lexicon;

bool init_data_structures() {
    std::cout << "Opening Lexicon and Document Table ...\n";

    if(lexicon.open(std::string("../../output/lexicon.bin")) == false) {
        std::cout << "Error: cannot open lexicon.\n";
        return false;
    }
    if(doc_table.open(std::string("../../output/doc_table.bin")) == false) {
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

/*
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
}*/

void conjunctive_query(std::priority_queue<std::pair<unsigned int, double>,
    std::vector<std::pair<unsigned int, double>>, compare>& min_heap,
    std::vector<posting_list*> pls,
    unsigned int k) {

    unsigned int max_doc_id = std::numeric_limits<unsigned int>::max();
    unsigned int cur_doc_id = pls[0]->getDocId();
    unsigned int i = 1;
    while (cur_doc_id != max_doc_id) {

        while (i < pls.size()) {
            pls[i]->nextGEQ(cur_doc_id);
            //std::cout << pls[i]->getDocId() << std::endl;
            if (pls[i]->getDocId() > cur_doc_id) {
                pls[0]->nextGEQ(pls[i]->getDocId());
                if (pls[0]->getDocId() > pls[i]->getDocId()) {
                    cur_doc_id = pls[0]->getDocId();
                    i = 1;
                }
                else {
                    cur_doc_id = pls[i]->getDocId();
                    i = 0;
                }
                break;
            }
            i++;
        }
        if (i == pls.size()) {
            double score = 0.0;
            for (auto& pl : pls) {
                if (pl->getDocId() == cur_doc_id) {
                    doc_table_entry de;
                    doc_table.getEntryByIndex(cur_doc_id, de);
                    unsigned int term_freq = pl->getFreq();
                    unsigned int doc_len = de.doc_len; // ONLY BM25
                    unsigned int doc_freq = pl->doc_freq;
                    unsigned int N = (unsigned int) doc_table.getSize();
                    //score += TFIDF(term_freq, doc_freq, N);
                    //std::cout << doc_len << " " << avg_doc_len << std::endl;
                    score += BM25(term_freq, doc_freq, doc_len, doc_table.getAvgDocLen(), N);
                }
            }
            if (min_heap.size() >= k) {
                if (min_heap.top().second < score) {
                    min_heap.pop();
                    min_heap.push(std::make_pair(cur_doc_id, score));
                }
            }
            else {
                min_heap.push(std::make_pair(cur_doc_id, score));
            }
            pls[0]->next();
            cur_doc_id = pls[0]->getDocId();
            i = 1;
        }
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
                doc_table_entry de;
                doc_table.getEntryByIndex(cur_doc_id, de);
                unsigned int term_freq = pl->getFreq();
                unsigned int doc_len = de.doc_len; // ONLY BM25
                unsigned int doc_freq = pl->doc_freq;
                unsigned int N = (unsigned int) doc_table.getSize();
                //score += TFIDF(term_freq, doc_freq, N);
                //std::cout << doc_len << " " << avg_doc_len << std::endl;
                score += BM25(term_freq, doc_freq, doc_len, doc_table.getAvgDocLen(), N);
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
                doc_table_entry de;
                doc_table.getEntryByIndex(cur_doc_id, de);

                unsigned int term_freq = pls[i]->getFreq();
                unsigned int doc_len = de.doc_len; // ONLY BM25
                unsigned int doc_freq = pls[i]->doc_freq;
                unsigned int N = (unsigned int) doc_table.getSize();
                //score += TFIDF(term_freq, doc_freq, N);
                //std::cout << doc_len << " " << avg_doc_len << std::endl;
                score += BM25(term_freq, doc_freq, doc_len, doc_table.getAvgDocLen(), N);
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
                doc_table_entry de;
                doc_table.getEntryByIndex(cur_doc_id, de);

                unsigned int term_freq = pls[i]->getFreq();
                unsigned int doc_len = de.doc_len; // ONLY BM25
                unsigned int doc_freq = pls[i]->doc_freq;
                unsigned int N = (unsigned int) doc_table.getSize();
                //score += TFIDF(term_freq, doc_freq, N);
                //std::cout << doc_len << " " << avg_doc_len << std::endl;
                score += BM25(term_freq, doc_freq, doc_len, doc_table.getAvgDocLen(), N);
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

auto LRUCache = boost::compute::detail::lru_cache<std::string, lexicon_entry>(1000);

bool execute_query(std::vector<std::string> &terms, unsigned int mode, unsigned int k) {
    std::cout << "Executing query\n";
    boost::chrono::high_resolution_clock::time_point t1 = boost::chrono::high_resolution_clock::now();

    // Vector of max score one per query term
    std::vector<double> max_scores;

    std::vector<posting_list*> pls;
    for (unsigned int i = 0; i < terms.size(); i++) {
        lexicon_entry le;
        bool found = false;

        // Check if current term is in cache
        if (LRUCache.contains(terms[i])) {
            le = LRUCache.get(terms[i]).get();
            found = true;
        }
        else {
            if (found = lexicon.search(terms[i], le))
                LRUCache.insert(terms[i], le);
        }
        if (found) {
            posting_list *pl = new posting_list();
            pl->n_skip_pointers = ceil((double) le.doc_freq/((unsigned int) sqrt(le.doc_freq))); //Rounding up
            pl->openList(le.docs_offset, le.freqs_offset, le.doc_freq);
            pl->doc_freq = le.doc_freq;
            pls.push_back(pl);
            max_scores.push_back(le.max_score);
        }
        else {
            std::cout << terms[i] << " not present in lexicon.\n";
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
            disjunctive_query(min_heap, pls, k);
            break;
        case DISJUNCTIVE_MODE_MAX_SCORE:
            disjunctive_query_max_score(min_heap, pls, max_scores, k); 
            break;
    }

    boost::chrono::high_resolution_clock::time_point t2 = boost::chrono::high_resolution_clock::now();
    std::cout << "The elapsed time was " << boost::chrono::duration_cast<boost::chrono::milliseconds>(t2-t1) << ".\n";
    std::cout << " ," << boost::chrono::duration_cast<boost::chrono::nanoseconds>(t2 - t1) << ".\n";

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

void query_evaluation(std::string& topics, std::string& result, const std::unordered_set<std::string>& stopwords, unsigned int mode, unsigned int k) {
    std::cout << "Executing queries\n";
    std::vector<double> max_scores;

    std::ifstream instream(topics);
    std::ofstream outstream(result);
    std::string loaded_content;

    int total_time = 0;
    int num_queries = 0;
    //Execution of input queries
    while (getline(instream, loaded_content)) {
        std::vector<std::string> query_terms;
        std::unordered_map<std::string, int> tokens;
        std::istringstream iss;
        std::string query_id;
        std::string text;

        iss = std::istringstream(loaded_content);
        getline(iss, query_id, '\t');
        getline(iss, text, '\n');

        tokenize(text, true, stopwords, tokens);
        for (auto kv : tokens) {
            query_terms.push_back(kv.first);
        }

        // Vector of max score one per query term
        std::vector<double> max_scores;

        std::vector<posting_list*> pls;
        for (unsigned int i = 0; i < query_terms.size(); i++) {
            lexicon_entry le;
            bool found = false;

            // Check if current term is in cache
            if (LRUCache.contains(query_terms[i])) {
                le = LRUCache.get(query_terms[i]).get();
                found = true;
            }
            else {
                if (found = lexicon.search(query_terms[i], le))
                    LRUCache.insert(query_terms[i], le);
            }
            if (found) {
                posting_list* pl = new posting_list();
                pl->n_skip_pointers = ceil((double)le.doc_freq / ((unsigned int)sqrt(le.doc_freq))); //Rounding up
                pl->openList(le.docs_offset, le.freqs_offset, le.doc_freq);
                pl->doc_freq = le.doc_freq;
                pls.push_back(pl);
                max_scores.push_back(le.max_score);
            }
            else {
                //std::cout << query_terms[i] << " not present in lexicon.\n";
            }
        }

        std::priority_queue<std::pair<unsigned int, double>,
            std::vector<std::pair<unsigned int, double>>, compare> min_heap;

        if (pls.size() == 0) {
            //std::cout << "No terms found.\n";
            continue;
        }

        num_queries++;
        boost::chrono::high_resolution_clock::time_point t1 = boost::chrono::high_resolution_clock::now();
        switch (mode) {
        case CONJUNCTIVE_MODE:
            conjunctive_query(min_heap, pls, k);
            break;
        case DISJUNCTIVE_MODE:
            disjunctive_query(min_heap, pls, k);
            break;
        case DISJUNCTIVE_MODE_MAX_SCORE:
            disjunctive_query_max_score(min_heap, pls, max_scores, k);
            break;
        }
        boost::chrono::high_resolution_clock::time_point t2 = boost::chrono::high_resolution_clock::now();
        total_time += boost::chrono::duration_cast<boost::chrono::milliseconds>(t2 - t1).count();
        std::vector<std::pair<unsigned int, double>> results;
        while (!min_heap.empty()) {
            std::pair<unsigned int, double> tmp = min_heap.top();
            min_heap.pop();
            results.push_back(tmp);
        }

        //Save results
        for (int i = results.size() - 1; i >= 0; i--) {
            outstream << query_id << " " << "Q0" << " " << results[i].first << " " << results.size() - i << " " << results[i].second << " " << "STANDARD" << std::endl;
        }

        for (auto pl : pls) {
            pl->closeList();
        }
        
    }
    std::cout << "The mean response time is: " <<  total_time/num_queries << " milliseconds.\n";
    instream.close();
    outstream.close();
}