#pragma once

#include <iostream>
#include <cmath>
#include <fstream>
#include <random>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <sstream>
#include <filesystem>
#include <boost/filesystem.hpp>

void invert_index(const std::unordered_map<std::string, int>& token_stream, std::unordered_map<std::string, std::list<std::pair<int, int>>>& dictionary, int docId);

void write_block_to_disk(std::unordered_map<std::string, std::list<std::pair<int, int>>>& dictionary, int block_num);