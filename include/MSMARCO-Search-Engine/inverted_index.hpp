#pragma once

#include <iostream>
#include <cmath>
#include <fstream>
#include <random>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <queue>
#include <sstream>
#include <filesystem>
#include <boost/filesystem.hpp>
// includes for vector and unique_ptr.
#include <vector>
#include <memory>

struct index_record{
    int block_id;
    std::string term;
    std::list<std::pair<int, int>> posting_list;
};

void invert_index(std::map<std::string, std::list<std::pair<int, int>>>& dictionary, const std::unordered_map<std::string, int>& token_stream, int docId);

void write_block_to_disk(std::map<std::string, std::list<std::pair<int, int>>>& dictionary, int block_num);

void merge_blocks(int n_blocks);

bool read_record(std::ifstream &in, index_record &idx_record);

void write_record(std::ofstream &out, index_record &idx_record);