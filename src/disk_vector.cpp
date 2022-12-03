#include "MSMARCO-Search-Engine/disk_vector.hpp"

bool DiskVector::create(const std::string& filename) {
	this->f.close();
	if (this->f.is_open())
	    return false;
	this->f.open(filename, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    this->size = 0;
	return this->f.good();
}

bool DiskVector::open(const std::string& filename) {
    this->f.close();
	if (this->f.is_open())
		return false;
	this->f.open(filename, std::ios::in | std::ios::out | std::ios::binary);
	return this->f.good();
}

void DiskVector::close() {
    this->f.close();
}

bool DiskVector::insert(std::vector<doc_table_entry> &entries) {
    this->f.write(reinterpret_cast<const char*>(entries.data()), entries.size() * sizeof(doc_table_entry)); 
    return true;
}

bool DiskVector::getEntryByIndex(unsigned int index, doc_table_entry& de) {
	this->f.seekg(index * sizeof(doc_table_entry), std::ios::beg);
    this->f.read(reinterpret_cast<char*>(&de), sizeof(doc_table_entry));
    return true;
}