#include "MSMARCO-Search-Engine/doc_table.hpp"

DocTable::DocTable() {}

DocTable::~DocTable() {}

bool DocTable::create(const std::string& filename) {
    this->f_write.open(filename, std::ios::binary | std::ios::out | std::ios::trunc);

    this->size = 0;
    this->doc_len_accumulator = 0;
    this->avg_doc_len = 0;

    // Clear first bytes that will be used to store the size and the avg doc len
    this->f_write.write(reinterpret_cast<const char*>(&this->size), sizeof(unsigned int));
    this->f_write.write(reinterpret_cast<const char*>(&this->avg_doc_len), sizeof(double));

	return this->f_write.good();
}

bool DocTable::open(const std::string& filename) {
    boost::iostreams::mapped_file_params params;
    params.path = filename;
    params.flags = boost::iostreams::mapped_file_source::mapmode::readonly;

    boost::iostreams::mapped_file_source mmap(params);
    this->f_read.open(mmap);

    // Read the size and the avg_doc_len
    this->f_read.read(reinterpret_cast<char*>(&this->size), sizeof(unsigned int));
    this->f_read.read(reinterpret_cast<char*>(&this->avg_doc_len), sizeof(double));

	return this->f_read.good();
}

void DocTable::close() {
    // If the Doc Table was open in write mode
    if (f_write.is_open()) {
        // Write the avg doc len at the start of the file
        this->f_write.seekp(0, std::ios::beg);

        this->avg_doc_len = (double) this->doc_len_accumulator / this->size;

        this->f_write.write(reinterpret_cast<const char*>(&this->size), sizeof(unsigned int));
        this->f_write.write(reinterpret_cast<const char*>(&this->avg_doc_len), sizeof(double));
        this->f_write.close();
    }

    // If the Doc Table is opened in read mode
    if (f_read.is_open()) {
        this->f_read.close();
    }
}

bool DocTable::insert(std::vector<doc_table_entry> &entries) {
    // Update size
    this->size += entries.size();

    // Update avg_doc_len
    for (auto doc : entries)
        this->doc_len_accumulator += doc.doc_len;

    this->f_write.write(reinterpret_cast<const char*>(entries.data()), entries.size() * sizeof(doc_table_entry)); 
    return true;
}

bool DocTable::getEntryByIndex(unsigned int index, doc_table_entry& de) {
	this->f_read.seekg(sizeof(unsigned int) + sizeof(double) + index * sizeof(doc_table_entry), std::ios::beg);
    this->f_read.read(reinterpret_cast<char*>(&de), sizeof(doc_table_entry));
    return true;
}

unsigned int DocTable::getSize() {
    return this->size;
}

double DocTable::getAvgDocLen() {
    return this->avg_doc_len;
}