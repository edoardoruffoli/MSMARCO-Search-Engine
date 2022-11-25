#include "MSMARCO-Search-Engine/compressing.hpp"
#include "MSMARCO-Search-Engine/model.hpp"


bool posting_list::openList(unsigned long offset) {

    this->f1.open("../../output/inverted_index.bin", std::ios::binary);

    unsigned int num_bytes_skip_pointers_list = 0;
    this->f1.seekg(offset);
    
    // Decode array skip pointers
    for (unsigned int i=0; i<this->skip_pointers_list_size; i++) {
        skip_pointer cur_skip_pointer;

        // Decode the current block max doc_id
        cur_skip_pointer.max_doc_id = VBdecode(this->f1, num_bytes_skip_pointers_list);
    
        // Decode the current block doc_id offset
        cur_skip_pointer.doc_id_offset = VBdecode(this->f1, num_bytes_skip_pointers_list);

        // Decode the current block freqs offset
        cur_skip_pointer.freqs_offset = VBdecode(this->f1, num_bytes_skip_pointers_list);

        this->skip_pointers.push_back(cur_skip_pointer);
    }
    this->base_offset = offset;
    this->doc_ids_offset = offset + num_bytes_skip_pointers_list;
    this->freqs_offset = offset + num_bytes_skip_pointers_list + this->skip_pointers[0].freqs_offset;
    this->stop_offset = this->freqs_offset;

    // Init
    next();

    return true;
}

void posting_list::closeList() {
    this->f1.close();
}

void posting_list::next() {
    if (this->doc_ids_offset == this->stop_offset) {
        this->cur_doc_id = std::numeric_limits<unsigned int>::max();
        this->cur_freq = std::numeric_limits<unsigned int>::max();
        return;
    }
    unsigned int bytes = 0;
    this->cur_doc_id = VBdecode(this->f1, bytes);
    this->doc_ids_offset += bytes;

    // Clean
    bytes = 0;
    this->f1.seekg(this->freqs_offset);
    this->cur_freq = VBdecode(this->f1, bytes);
    this->freqs_offset += bytes;

    f1.seekg(this->doc_ids_offset);
}

/*
Advances the iterator forward to the next posting with a docID
greater than or equal to d.
*/
void posting_list::nextGEQ(unsigned int d) {
    // Find the block
    unsigned block_idx;
    for (block_idx = 0; block_idx < this->skip_pointers.size(); block_idx++) {
        if (this->skip_pointers[block_idx].max_doc_id >= d)
            break;
    }

    if (block_idx == this->skip_pointers.size()) {
        this->cur_doc_id = std::numeric_limits<unsigned int>::max();
        this->cur_freq = std::numeric_limits<unsigned int>::max();
        return;
    }

    this->doc_ids_offset = this->base_offset + skip_pointers[block_idx].doc_id_offset;
    this->freqs_offset = this->base_offset + skip_pointers[block_idx].freqs_offset;

    this->f1.seekg(this->doc_ids_offset);
    // Scan the list until I find the doc_id greater or equal
    do {
        unsigned int bytes = 0;
        this->cur_doc_id = VBdecode(this->f1, bytes);
        this->doc_ids_offset += bytes;

        // Clean
        bytes = 0;
        this->f1.seekg(this->freqs_offset);
        this->cur_freq = VBdecode(this->f1, bytes);
        this->freqs_offset += bytes;

        this->f1.seekg(this->doc_ids_offset);
    } while (this->cur_doc_id < d);
}

unsigned int posting_list::getDocId() {
    // Check if end of file
    return this->cur_doc_id;
}

unsigned int posting_list::getFreq() {
    return this->cur_freq;
}