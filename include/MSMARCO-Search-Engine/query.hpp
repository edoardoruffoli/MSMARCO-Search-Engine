#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <queue>
#include <math.h> 
#include <boost/chrono.hpp>

#include "MSMARCO-Search-Engine/model.hpp"
#include "MSMARCO-Search-Engine/io.hpp"
#include "MSMARCO-Search-Engine/compressing.hpp"
#include "MSMARCO-Search-Engine/scoring_functions.hpp"
#include "MSMARCO-Search-Engine/parsing.hpp"

#define CONJUNCTIVE_MODE 0  // DAAT
#define DISJUNCTIVE_MODE 1
#define CONJUNCTIVE_MODE_MAX_SCORE 2
#define DISJUNCTIVE_MODE_MAX_SCORE 3

// Min heap <doc_id, score>
struct compare {
    bool operator()(std::pair<unsigned int, double> const& a, std::pair<unsigned int, double> const& b) const {
        if (a.second == b.second)
            return a.first < b.first;
        return a.second > b.second;
    }
};

bool init_data_structures();

bool execute_query(std::vector<std::string> &terms, unsigned int type, unsigned int k);

void conjunctive_query(std::priority_queue<std::pair<unsigned int, unsigned int>> &min_heap,
                       std::vector<posting_list*> pls, unsigned int k);

void disjunctive_query(std::priority_queue<std::pair<unsigned int, unsigned int>> &min_heap,
                       std::vector<posting_list*> &pls, unsigned int k);

void query_evaluation(std::string& topics, std::string& result, const std::unordered_set<std::string>& stopwords, unsigned int mode, unsigned int k);

