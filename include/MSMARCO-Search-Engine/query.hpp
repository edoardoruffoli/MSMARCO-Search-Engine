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

std::map<std::string, std::pair<unsigned long, size_t>> lexicon;

// Doc table
std::set<doc_entry> doc_table;

bool init_data_structures();

bool execute_query(std::vector<std::string> &terms, unsigned int type);
/*
posting_list openList(std::string term, const std::map<std::string, std::pair<unsigned long, size_t>>& lexicon);
*/