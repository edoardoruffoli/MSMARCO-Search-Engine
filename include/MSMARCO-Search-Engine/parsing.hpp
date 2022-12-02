#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <sstream>
#include <bitset>
#include <regex>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/locale.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <boost/range/numeric.hpp>
#include <boost/range/adaptor/map.hpp>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/algorithm/string/classification.hpp >

#include "BS_thread_pool.hpp"
#include <Porter2.hpp>
#include <stxxl/vector>
#include <stxxl/io>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#include "MSMARCO-Search-Engine/model.hpp"
#include "MSMARCO-Search-Engine/io.hpp"
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

void write_doc_table_record(std::ofstream &out, std::string &doc_no, unsigned int doc_len);

void write_block_to_disk(std::map<std::string, std::list<std::pair<int, int>>>& dictionary, int block_num);

void parse(const char* in, const unsigned int BLOCK_SIZE, bool flag, const char* stopwords_filename, unsigned int n_threads);

/*
int getMemoryUsed();
*/