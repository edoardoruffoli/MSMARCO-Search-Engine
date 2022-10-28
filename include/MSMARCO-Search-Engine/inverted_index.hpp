// MSMARCO-Search-Engine.h : Include file for standard system include files,
// or project specific include files.

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

// TODO: Reference additional headers your program requires here.

// MSMARCO-Search-Engine.cpp : Defines the entry point for the application.
//

#include "MSMARCO-Search-Engine/inverted_index.hpp"

std::string clearToken(const std::string& token);

std::unordered_map<std::string, int> getTokens(const std::string &content, const int& doc_id);

void invert_index(const std::unordered_map<std::string, int>& token_stream, std::unordered_map<std::string, std::list<int>>& dictionary);

void write_block_to_disk(std::unordered_map<std::string, std::list<int>>& dictionary, int block_num);

int getFileSize(const std::string& text);