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
#include <boost/serialization/set.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/export.hpp>

#include "MSMARCO-Search-Engine/model.hpp"


void save_lexicon(const std::map<std::string, std::pair<unsigned long, size_t>>& lexicon, std::string& filename);

void load_lexicon(std::map<std::string, std::pair<unsigned long, size_t>>* lexicon, std::string& filename);

void save_doc_table(const std::set<doc_entry> &doc_table, std::string &filename);

void load_doc_table(std::set<doc_entry> *doc_table, std::string &filename);