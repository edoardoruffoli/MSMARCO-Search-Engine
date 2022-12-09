#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <list>


// Inverted Index
struct term_entry {
    int block_id;
    std::string term;
    std::list<std::pair<int, int>> posting_list;
};

// Posting list skip pointer
struct skip_pointer {
    unsigned int max_doc_id;
    unsigned int doc_id_offset;
    unsigned int freqs_offset;
};

// Posting list iterator
const int MIN_POSTING_LEN = 1024;

struct posting_list {
    std::ifstream f_docs;
    std::ifstream f_freqs;

    std::vector<skip_pointer> skip_pointers;
    unsigned int n_skip_pointers;

    unsigned int pl_len;
    
    unsigned int block_size;
    unsigned int count;         // Count the number of posting read
    unsigned int cur_doc_id;    // DocId of the current document
    unsigned int cur_freq;      // Number of term occurences in the current document
    unsigned int doc_freq;      // Number of documents that contains term of the posting list

    // API
    bool openList(unsigned long docs_offset, unsigned long freqs_offset, unsigned int posting_list_len);
    void closeList();
    void next();
    void nextGEQ(unsigned int d);
    unsigned int getDocId();
    unsigned int getFreq();
};

// Lexicon
const int N_HASH_KEYS = 1000000;

struct lexicon_entry {
    unsigned int doc_freq;
    unsigned long docs_offset;
    unsigned long freqs_offset;
    double max_score;
};

// Document Table
struct doc_table_entry {
    char doc_no[10];
    unsigned int doc_len;
};
