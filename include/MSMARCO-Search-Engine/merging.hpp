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

struct term_entry{
    int block_id;
    std::string term;
    std::list<std::pair<int, int>> posting_list;
};
bool read_record(std::ifstream &in, term_entry &term_entry);

void write_inverted_index_record(std::ofstream &out, term_entry &term_entry);

void write_lexicon_record(std::ofstream &out, term_entry &term_entry, unsigned long offset);

void merge_blocks(const unsigned int n_blocks);
