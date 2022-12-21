#include "MSMARCO-Search-Engine/lexicon.hpp"
#include <list>
#include <unordered_map> // Hash function
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>

Lexicon::Lexicon() {}

Lexicon::~Lexicon() {}

bool Lexicon::create(const std::string& filename, unsigned int N) {
	this->f_create.open(filename, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);

    // Fill file with null values. So that we can know when an entry has been inserted
    char val = '\0';
    std::fill_n(std::ostreambuf_iterator<char>(this->f_create), N * sizeof(HashMapEntry), val);

    // Init Lexicon values
    this->N = N;
    this->n_collisions = 0;

    // Clear first bytes that will be used to store the N value of the lexicon
    this->f_create.write(reinterpret_cast<const char*>(&this->N), sizeof(unsigned int));

	return this->f_create.good();
}

bool Lexicon::open(const std::string& filename) {
    boost::iostreams::mapped_file_params params;
    params.path = filename;
    params.flags = boost::iostreams::mapped_file_source::mapmode::readonly;

    boost::iostreams::mapped_file_source mmap(filename);
    this->f_read.open(mmap);

    // Read the N value
    this->f_read.read(reinterpret_cast<char*>(&this->N), sizeof(unsigned int));
    
	return this->f_read.good();
}

void Lexicon::close() {
    // If the Lexicon was opened in write mode
    if (f_create.is_open()) {
        // Write the N value at the start of the file
        this->f_create.seekp(0, std::ios::beg);
        this->f_create.write(reinterpret_cast<const char*>(&this->N), sizeof(unsigned int));
        this->f_create.close();
    }

    // If the Lexicon was opened in read mode
    if (f_read.is_open()) {
        this->f_read.close();
    }
}

bool Lexicon::insert(const std::string& key, const lexicon_entry& le) {
    // Truncate key to be in 20 bytes
    std::string hash_key = key.substr(0, MAX_KEY_LEN-1);

     // Offset where the entry will be written
    unsigned int target_offset;   

    // Compute the index using hash 
    std::size_t h1 = std::hash<std::string>{}(hash_key);
    unsigned int index = h1 % this->N;

    // Index of the first member of the collision list if present
    unsigned start_offset = sizeof(unsigned int) + index * sizeof(HashMapEntry);   

    // Check if there is already an entry at index
    Lexicon::HashMapEntry e = Lexicon::getEntryByOffset(this->f_create, start_offset);
    if (e.isEmpty()) {
        target_offset = start_offset;
    } 
    else {
        // Scan all the collision list
        unsigned int prev = start_offset;
        while (e.next != 0) {
            prev = e.next;
            e = getEntryByOffset(this->f_create, e.next);
        }

        // Find the first free offset using the number of collisions
        target_offset = sizeof(unsigned int) + (N + n_collisions) * sizeof(Lexicon::HashMapEntry);
        n_collisions++;

        // Update the previous member of the collision list
        e.next = target_offset;
        updateEntry(e, prev);
    }

    // Create the current entry struct
    HashMapEntry data;
    strcpy(data.key, hash_key.c_str());
    data.le = le;
    data.next = 0;

    updateEntry(data, target_offset);
    return true;
}

bool Lexicon::search(const std::string& key, lexicon_entry& le) {
    // Compute the index using hash 
    std::size_t h1 = std::hash<std::string>{}(key);
    unsigned int index = h1 % this->N;

    unsigned int prev;

    // Index of the first member of the collision list if present
    unsigned start_offset = sizeof(unsigned int) + index * sizeof(HashMapEntry);   

    Lexicon::HashMapEntry e = Lexicon::getEntryByOffset(this->f_read, start_offset);
    if (e.isEmpty()) {
        return false;
    } 
    else {
        // Scan all the collision list
        prev = start_offset;

        // Loop until we reach the last element or we have found the matching key
        while (e.next != 0 && strcmp(e.key, key.c_str())) {
            prev = e.next;
            e = getEntryByOffset(this->f_read, e.next);
        }
        // Found elem
        if (!strcmp(e.key, key.c_str())) {
            le = e.le;
            return true;
        }
        return false;
    }
}

Lexicon::HashMapEntry Lexicon::getEntryByOffset(std::istream &f, unsigned long offset) {
	HashMapEntry entry;
    f.seekg(offset, std::ios::beg);
    f.read(reinterpret_cast<char*>(&entry), sizeof(entry));
	return entry;
}

bool Lexicon::updateEntry(Lexicon::HashMapEntry entry, unsigned long offset) { 
    return this->f_create.seekg(offset, std::ios::beg) && 
            this->f_create.write(reinterpret_cast<const char*>(&entry), sizeof(entry)); 
}
