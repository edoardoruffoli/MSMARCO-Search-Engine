#pragma once

#include <fstream>
#include <iostream>
#include <bitset>
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <sstream>

#include "MSMARCO-Search-Engine/util.hpp"

void encode(unsigned int num, std::ofstream& ofile);
std::vector<int> decode(std::vector<char>& vec);
void read_compressed_index(std::string filename);