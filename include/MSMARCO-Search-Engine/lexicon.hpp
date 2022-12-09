#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>

#include "MSMARCO-Search-Engine/model.hpp"

const unsigned int MAX_KEY_LEN = 20;

class Lexicon {
    public:
        // Hash Map Entry struct
        struct HashMapEntry {
            // Hash key (truncated at MAX_KEY_LEN)
            char key[MAX_KEY_LEN];

            lexicon_entry le;

            // File offset of the next entry with the same hash
            unsigned int next; 

            // Check if entry is empty
            bool isEmpty() {
                return !strcmp(key, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
            }
        };

        // Constructor
        Lexicon();

        // Destructor
        ~Lexicon();

        // Create a new Lexicon in filename with N possible hash keys
        bool create(const std::string& filename, unsigned int N);

        // Read the Lexicon stored in filename
        bool open(const std::string& filename);

        // Close the I/O streams
        void close();

        // Insert lexicon entry with key in Lexicon
        bool insert(const std::string& key, const lexicon_entry& le);

        // Search for the lexicon entry with the given key
        bool search(const std::string& key, lexicon_entry& le);

        // Returns the lexicon entry given the offset
        HashMapEntry getEntryByOffset(std::istream &f, unsigned long offset);

    private:

        // Update the lexicon entry at offset
        bool updateEntry(HashMapEntry entry, unsigned long offset);

        // Regular stream for creating the Lexicon
        std::fstream f_create;  

        // Memory mapped stream for reading the Lexicon
        boost::iostreams::stream<boost::iostreams::mapped_file_source> f_read;

        // Number of hash keys
        unsigned int N;

        // Number of total hash keys collisions. Used to retrieve the address at which write the entries 
        // that collided (at the end of file)
        unsigned int n_collisions;
};