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

static const int NUM_DATA_BITS = 28;
static const int NUM_SELECTOR_BITS = 4;

static const unsigned char NUM_1_28_BITS = 1;
static const unsigned char NUM_2_14_BITS = 2;
static const unsigned char NUM_3_9_9_10_BITS = 3;
static const unsigned char NUM_4_7_BITS = 4;
static const unsigned char NUM_5_5_5_6_6_6_BITS = 5;
static const unsigned char NUM_7_4_BITS = 6;
static const unsigned char NUM_9_3_3_3_3_3_3_3_3_4_BITS = 7;
static const unsigned char NUM_14_2_BITS = 8;
static const unsigned char NUM_28_1_BITS = 9;

static const int BITS_28_MASK = (1 << 28) - 1;
static const int BITS_14_MASK = (1 << 14) - 1;
static const int BITS_10_MASK = (1 << 10) - 1;
static const int BITS_9_MASK = (1 << 9) - 1;
static const int BITS_7_MASK = (1 << 7) - 1;
static const int BITS_6_MASK = (1 << 6) - 1;
static const int BITS_5_MASK = (1 << 5) - 1;
static const int BITS_4_MASK = (1 << 4) - 1;
static const int BITS_3_MASK = (1 << 3) - 1;
static const int BITS_2_MASK = (1 << 2) - 1;
static const int BITS_1_MASK = (1 << 1) - 1;

int writeS9(unsigned int* result, std::vector<unsigned int> from, std::ofstream& ofile);

int readS9(unsigned int* from, unsigned int* val);
