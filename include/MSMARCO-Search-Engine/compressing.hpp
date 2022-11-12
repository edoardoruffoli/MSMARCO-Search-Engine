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

unsigned long VBencode(unsigned int num, std::ofstream& ofile);
std::vector<unsigned int> VBdecode(std::vector<char>& vec);
unsigned int VBdecode(std::ifstream& ifile, unsigned int &n_bytes);
//void read_compressed_index(std::string filename);