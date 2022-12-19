#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#include "BS_thread_pool.hpp"
#include <Porter2.hpp>

#include "MSMARCO-Search-Engine/doc_table.hpp"
#include "MSMARCO-Search-Engine/model.hpp"

/*
#if defined _WIN32
#include <windows.h>
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
#include <sys/sysinfo.h>
#endif
*/

void tokenize(std::string &content, bool flag, const std::unordered_set<std::string> &stopwords,
                std::unordered_map<std::string, int> &tokens);

void add_to_posting_list(std::map<std::string, std::list<std::pair<int, int>>>& dictionary,
                  const std::unordered_map<std::string, int>& token_stream, int doc_id, unsigned int &doc_len);

bool save_intermediate_inv_idx(std::map<std::string, std::list<std::pair<int, int>>>& dictionary, std::string &filename);

void SPIMI_Invert(std::vector<std::string> &documents, unsigned int start_doc_id, unsigned int block_num,
                   BS::thread_pool &pool, 
                   DocTable &doc_table,
                   std::unordered_set<std::string> &stopwords, bool flag);

void parse(const char* in, const unsigned int BLOCK_SIZE, bool flag, const char* stopwords_filename, unsigned int n_threads);

/*
int getMemoryUsed();
*/