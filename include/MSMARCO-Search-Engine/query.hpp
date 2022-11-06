#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <boost/range/numeric.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/utility.hpp>

#include "MSMARCO-Search-Engine/io.hpp"

#define CONJUNCTIVE_MODE 0
#define DISJUNCTIVE_MODE 1

std::map<std::string, std::pair<unsigned long, size_t>> lexicon;

// Doc table
std::set<doc_entry> doc_table;

bool init_data_structures();

bool execute_query(std::vector<std::string> &terms, unsigned int type);