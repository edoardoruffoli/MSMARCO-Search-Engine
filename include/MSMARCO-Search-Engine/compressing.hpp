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

#include "MSMARCO-Search-Engine/io.hpp"

void VBencode(unsigned int num, std::vector<uint8_t>& result);
std::vector<unsigned int> VBdecode(std::vector<char>& vec);
unsigned int VBdecode(std::istream& ifile, unsigned int &n_bytes);
//void read_compressed_index(std::string filename);