#ifndef DISK_VECTOR_HEADER
#define DISK_VECTOR_HEADER

#include <iostream>
#include <fstream>
#include "MSMARCO-Search-Engine/model.hpp"

class DiskVector {
    public:
        DiskVector();
        ~DiskVector();
        bool create(const std::string& filename);
        bool open(const std::string& filename);
        void close();
        bool insert(std::vector<doc_table_entry> &entries);
        bool getEntryByIndex(unsigned int index, doc_table_entry& de);
    private:
        std::fstream f;
        unsigned int size;
        unsigned int doc_len_accumulator;
        double avg_doc_len;
};

#endif