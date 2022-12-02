#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>

#include <boost/filesystem.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/export.hpp>

#include <stxxl/vector>
#include <stxxl/io>

#include "MSMARCO-Search-Engine/model.hpp"

bool save_intermediate_inv_idx(std::map<std::string, std::list<std::pair<int, int>>>& dictionary, std::string &filename);

bool save_lexicon(const std::map<std::string, lexicon_entry>& lexicon, std::string& filename);

bool load_lexicon(std::map<std::string, lexicon_entry>* lexicon, std::string& filename);

bool save_doc_table(const std::vector<doc_table_entry> &doc_table, std::string &filename);

bool load_doc_table(std::vector<doc_table_entry>* doc_table, std::string &filename);