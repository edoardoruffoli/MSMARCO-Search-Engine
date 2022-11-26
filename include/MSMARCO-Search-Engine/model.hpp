#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <list>

struct term_entry {
    int block_id;
    std::string term;
    std::list<std::pair<int, int>> posting_list;
};

struct skip_pointer {
    unsigned int max_doc_id;
    unsigned int doc_id_offset;
    unsigned int freqs_offset;
};

// Posting list iterator
struct posting_list {
    std::ifstream f1;

    std::vector<skip_pointer> skip_pointers;
    unsigned int n_skip_pointers;
    
    unsigned long base_offset;
    unsigned long skip_pointers_list_size;

    unsigned long doc_ids_offset;
    unsigned long freqs_offset;
    unsigned long stop_offset;

    unsigned int cur_doc_id;    // DocId of the current document
    unsigned int cur_freq;      // Number of term occurences in the current document
    unsigned int doc_freq;      // Number of documents that contains term of the posting list

    // API
    bool openList(unsigned long offset);
    void closeList();
    void next();
    void nextGEQ(unsigned int d);
    unsigned int getDocId();
    unsigned int getFreq();
};

// Lexicon
struct lexicon_entry {
    unsigned int doc_freq;
    unsigned long offset;
    double max_score;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & doc_freq;
        ar & offset;
        ar & max_score;
    }
};

// Document Table
struct doc_table_entry {
    std::string doc_no;
    unsigned int doc_len;
    // URL
    // Page Rank

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & doc_no;
        ar & doc_len;
    }
};