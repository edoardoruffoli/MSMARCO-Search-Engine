#pragma once

#include <iostream>
#include <vector>

// Posting lists
struct posting_list {
    std::vector<unsigned int> doc_ids;
    std::vector<unsigned int> freqs;
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