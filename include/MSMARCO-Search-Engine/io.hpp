#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/export.hpp>

#include "MSMARCO-Search-Engine/model.hpp"


bool save_lexicon(const std::map<std::string, unsigned long>& lexicon, std::string& filename);

bool load_lexicon(std::map<std::string, unsigned long>* lexicon, std::string& filename);

bool save_doc_table(const std::set<doc_entry> &doc_table, std::string &filename);

bool load_doc_table(std::set<doc_entry> *doc_table, std::string &filename);