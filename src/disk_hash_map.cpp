#include "MSMARCO-Search-Engine/disk_hash_map.hpp"
#include <list>
#include <unordered_map> // Hash function
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>

DiskHashMap::DiskHashMap() {}
DiskHashMap::~DiskHashMap() {}

bool DiskHashMap::create(const std::string& filename, unsigned int N) {
	this->f.close();
	if (this->f.is_open())
	    return false;
	this->f.open(filename, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);

    // Fill file with null values 
    char val = '\0';
    std::fill_n(std::ostreambuf_iterator<char>(this->f), N * sizeof(HashMapEntry), val);
    this->N = N;
    this->n_collisions = 0;
	return this->f.good();
}

bool DiskHashMap::open(const std::string& filename) {
    boost::iostreams::mapped_file mmap(filename);
    this->f.open(mmap);
	return this->f.good();
}

void DiskHashMap::close() {
    this->f.close();
}

bool DiskHashMap::insert(const std::string& key, /*template*/const lexicon_entry& le) {
    // Truncate key to be in 20 bytes
    std::string hash_key = key.substr(0, MAX_KEY_LEN);
    unsigned int target_offset;    // Offset where the entry will be written

    // Compute the index using hash 
    std::size_t h1 = std::hash<std::string>{}(hash_key);
    unsigned int index = h1 % 1000000;

    // Index of the first member of the collision list if present
    unsigned start_offset = index * sizeof(HashMapEntry);   

    // Check if there is already an entry at index
    DiskHashMap::HashMapEntry e = DiskHashMap::getEntryByOffset(start_offset);
    if (e.isEmpty()) {
        target_offset = start_offset;
    } 
    else {
        // Scan all the collision list
        unsigned int prev = start_offset;
        while (e.next != 0) {
            prev = e.next;
            e = getEntryByOffset(e.next);
        }

        // Find the first free offset using the number of collisions
        target_offset = (N + n_collisions) * sizeof(DiskHashMap::HashMapEntry);
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

bool DiskHashMap::search(const std::string& key, lexicon_entry& le) {
    // Compute the index using hash 
    std::size_t h1 = std::hash<std::string>{}(key);
    unsigned int index = h1 % 1000000;

    unsigned int prev;

    // Index of the first member of the collision list if present
    unsigned start_offset = index * sizeof(HashMapEntry);   

    DiskHashMap::HashMapEntry e = DiskHashMap::getEntryByOffset(start_offset);
    if (e.isEmpty()) {
        return false;
    } 
    else {
        // Scan all the collision list
        prev = start_offset;

        // Loop until we reach the last element or we have found the matching key
        while (e.next != 0 && strcmp(e.key, key.c_str())) {
            prev = e.next;
            e = getEntryByOffset(e.next);
        }
        // Found elem
        if (!strcmp(e.key, key.c_str())) {
            le = e.le;
            return true;
        }
        return false;
    }
}

DiskHashMap::HashMapEntry DiskHashMap::getEntryByOffset(unsigned long offset) {
	HashMapEntry entry;
    this->f.seekg(offset, std::ios::beg);
    this->f.read(reinterpret_cast<char*>(&entry), sizeof(entry));
	return entry;
}

bool DiskHashMap::updateEntry(DiskHashMap::HashMapEntry entry, unsigned long offset) { 
    return this->f.seekg(offset, std::ios::beg) && 
            this->f.write(reinterpret_cast<const char*>(&entry), sizeof(entry)); 
}

