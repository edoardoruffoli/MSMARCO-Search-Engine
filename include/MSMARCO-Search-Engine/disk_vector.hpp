#pragma once
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
#include <iostream>
#include <fstream>
#include "MSMARCO-Search-Engine/model.hpp"

class DiskVector {
    public:
        // Constructor
        DiskVector();

        // Destructor
        ~DiskVector();

        // Create a new Doc Table in filename
        bool create(const std::string& filename);

        // Read the Doc Table stored in filename
        bool open(const std::string& filename);

        // Close the I/O streams
        void close();

        // Insert entries in Doc Table
        bool insert(std::vector<doc_table_entry> &entries);

        // Retrieve entry at index
        bool getEntryByIndex(unsigned int index, doc_table_entry& de);

        // Get Doc Table length
        unsigned int getSize();

        // Get Doc Table average document length (for BM25)
        double getAvgDocLen();
    private:
        // Regular stream for writing the Doc Table
        std::ofstream f_write;  

        // Memory mapped stream for reading the Doc Table
        boost::iostreams::stream<boost::iostreams::mapped_file_source> f_read;

        // Length of the Doc Table
        unsigned int size;

        // Cumulative document length
        unsigned int doc_len_accumulator;

        // Average document length, used to compute BM25
        double avg_doc_len;
};
