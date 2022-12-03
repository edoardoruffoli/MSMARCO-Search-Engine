#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "MSMARCO-Search-Engine/model.hpp"

const unsigned int MAX_KEY_LEN = 20;

class DiskHashMap {
    public:
        struct HashMapEntry {
            char key[MAX_KEY_LEN];
            lexicon_entry le;
            unsigned int next;  // Offset of the next entry with the same hash

            bool isEmpty() {
                return !strcmp(key, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
            }
        };
        DiskHashMap();
        ~DiskHashMap();
        bool create(const std::string& filename, unsigned int numBuckets);
        bool open(const std::string& filename);
        void close();
        bool insert(const std::string& key, /*template*/const lexicon_entry& le);
        bool search(const std::string& key, lexicon_entry& le);
        HashMapEntry getEntryByOffset(unsigned long offset);

    private:
        bool updateEntry(HashMapEntry entry, unsigned long offset);

        std::fstream f;
        unsigned int N;
        unsigned int n_collisions;  // Used to retrieve the next address to write at
};