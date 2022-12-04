#pragma once
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
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
        bool load(std::vector<doc_table_entry> &in_mem_doc_table);
        bool insert(std::vector<doc_table_entry> &entries);
        bool getEntryByIndex(unsigned int index, doc_table_entry& de);
        unsigned int getSize();
        double getAvgDocLen();
    private:
        boost::iostreams::stream<boost::iostreams::mapped_file> f;
        unsigned int size;
        unsigned int doc_len_accumulator;
        double avg_doc_len;
};
