#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <queue>
#include <math.h> 

#include "MSMARCO-Search-Engine/model.hpp"
#include "MSMARCO-Search-Engine/io.hpp"
#include "MSMARCO-Search-Engine/compressing.hpp"

#define CONJUNCTIVE_MODE 0
#define DISJUNCTIVE_MODE 1

bool init_data_structures();

bool execute_query(std::vector<std::string> &terms, unsigned int type, unsigned int k);

void conjunctive_query(std::priority_queue<std::pair<unsigned int, unsigned int>> &min_heap,
                       std::vector<posting_list*> pls, unsigned int k);

void disjunctive_query(std::priority_queue<std::pair<unsigned int, unsigned int>> &min_heap,
                       std::vector<posting_list*> pls, unsigned int k);

double TFIDF(unsigned int tf, unsigned int df, unsigned int N);

double BM25(unsigned int tf, unsigned int df, unsigned int doc_len, unsigned avg_doc_len, unsigned int N);