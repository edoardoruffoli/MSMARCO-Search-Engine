#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "MSMARCO-Search-Engine/io.hpp"
#include "MSMARCO-Search-Engine/compressing.hpp"

#define CONJUNCTIVE_MODE 0
#define DISJUNCTIVE_MODE 1

// Doc table
std::set<doc_entry> doc_table;

std::map<std::string, unsigned long> lexicon;

bool init_data_structures();

bool execute_query(std::vector<std::string> &terms, unsigned int type);

void openList(std::string term, posting_list *result);
