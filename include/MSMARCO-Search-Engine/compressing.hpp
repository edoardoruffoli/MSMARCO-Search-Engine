#pragma once

#include <fstream>
#include <iostream>
#include <bitset>
#include <vector>
#include <cmath>

void VBencode(unsigned int num, std::vector<uint8_t>& result);

unsigned int VBdecode(std::istream& ifile, unsigned int &n_bytes);