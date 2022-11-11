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

// Posting list iterator
struct posting_list {
    std::ifstream f1;
    std::ifstream f2;
    unsigned int cur_doc_id;
    unsigned int cur_freq;

    bool openList(unsigned long offset);
    void closeList();
    void next();
    void nextGEQ(unsigned int d);
    unsigned int getDocId();
    unsigned int getFreq();
};

// Lexicon

// Document Table
struct doc_entry{
    unsigned int doc_id; 
    std::string doc_no;
    unsigned int doc_len;

    doc_entry(const unsigned int docId = 0, const std::string& docNo = "", const unsigned int docLen = 0)
    :   doc_id(docId),
        doc_no(docNo),
        doc_len(docLen) {}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & doc_id;
        ar & doc_no;
        ar & doc_len;
    }

    bool operator<(const doc_entry& de) const noexcept {
        return doc_id < de.doc_id;
    }

    // Needed for the find() by doc_id
    bool operator==(const doc_entry& de) const noexcept {
        return doc_id == de.doc_id;
    }
};