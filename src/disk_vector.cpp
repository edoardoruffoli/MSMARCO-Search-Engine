#include "MSMARCO-Search-Engine/disk_vector.hpp"

DiskVector::DiskVector() {}

DiskVector::~DiskVector() {}

bool DiskVector::create(const std::string& filename) {
	this->f.close();
	if (this->f.is_open())
	    return false;
	this->f.open(filename, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);

    this->size = 0;
    this->doc_len_accumulator = 0;
    this->avg_doc_len = 0;

    // Clear first bytes that will be used to store the size and the avg doc len
    this->f.write(reinterpret_cast<const char*>(&this->size), sizeof(unsigned int));
    this->f.write(reinterpret_cast<const char*>(&this->avg_doc_len), sizeof(double));

	return this->f.good();
}

bool DiskVector::open(const std::string& filename) {
    this->f.close();
	if (this->f.is_open())
		return false;
	this->f.open(filename, std::ios::in | std::ios::out | std::ios::binary);

    // Read the size and the avg_doc_len
    this->f.read(reinterpret_cast<char*>(&this->size), sizeof(unsigned int));
    this->f.read(reinterpret_cast<char*>(&this->avg_doc_len), sizeof(double));

	return this->f.good();
}

void DiskVector::close() {
    // Write the avg doc len at the start of the file
    this->f.seekg(0, std::ios::beg);

    this->avg_doc_len = (double) this->doc_len_accumulator / this->size;

    this->f.write(reinterpret_cast<const char*>(&this->size), sizeof(unsigned int));
    this->f.write(reinterpret_cast<const char*>(&this->avg_doc_len), sizeof(double));
    this->f.close();
}

bool DiskVector::insert(std::vector<doc_table_entry> &entries) {
    // Update size
    this->size += entries.size();

    // Update avg_doc_len
    for (auto doc : entries)
        this->doc_len_accumulator += doc.doc_len;

    this->f.write(reinterpret_cast<const char*>(entries.data()), entries.size() * sizeof(doc_table_entry)); 
    return true;
}

bool DiskVector::getEntryByIndex(unsigned int index, doc_table_entry& de) {
	this->f.seekg(sizeof(unsigned int) + sizeof(double) + index * sizeof(doc_table_entry), std::ios::beg);
    this->f.read(reinterpret_cast<char*>(&de), sizeof(doc_table_entry));
    return true;
}