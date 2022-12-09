#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <memory>
#include <sstream>

#include <boost/filesystem.hpp>

#include "MSMARCO-Search-Engine/compressing.hpp"
#include "MSMARCO-Search-Engine/scoring_functions.hpp"
#include "MSMARCO-Search-Engine/doc_table.hpp"
#include "MSMARCO-Search-Engine/lexicon.hpp"

bool read_record(std::ifstream &in, term_entry &term_entry);

void write_inverted_index_record(std::ofstream &out, term_entry &term_entry);

std::pair<unsigned long, unsigned long> write_inverted_index_record_compressed(std::ofstream& out_docs, std::ofstream& out_freqs, term_entry& term_entry);

void write_lexicon_record(std::ofstream &out, term_entry &term_entry, unsigned long offset);

void merge_blocks(const unsigned int n_blocks);
