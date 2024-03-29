#include "MSMARCO-Search-Engine/compressing.hpp"
#include "MSMARCO-Search-Engine/model.hpp"


bool posting_list::openList(unsigned long docs_offset, unsigned long freqs_offset, unsigned int posting_list_len) {

    this->f_docs.open("../../output/inverted_index_docs.bin", std::ios::binary | std::ios::in);
    this->f_freqs.open("../../output/inverted_index_freqs.bin", std::ios::binary | std::ios::in);

    unsigned int num_bytes_skip_pointers_list = 0;

    // Position file pointers
    this->f_docs.seekg(docs_offset);
    this->f_freqs.seekg(freqs_offset);

    // Save posting_list_len
    this->pl_len = posting_list_len;
    
    // We have skip pointers only if size > MIN_POSTING_LEN
    if (this->pl_len > MIN_POSTING_LEN) {

        // Compute skip pointers number
        this->block_size = (unsigned int)sqrt(posting_list_len);
        this->n_skip_pointers = ceil((double) posting_list_len / this->block_size);

        for (unsigned int i=0; i<this->n_skip_pointers; i++) {
            skip_pointer cur_skip_pointer;

            // Decode the current block max doc_id
            cur_skip_pointer.max_doc_id = VBdecode(this->f_docs, num_bytes_skip_pointers_list);
        
            // Decode the current block doc_id offset
            cur_skip_pointer.doc_id_offset = VBdecode(this->f_docs, num_bytes_skip_pointers_list);

            // Decode the current block freqs offset
            cur_skip_pointer.freqs_offset = VBdecode(this->f_docs, num_bytes_skip_pointers_list);

            this->skip_pointers.push_back(cur_skip_pointer);
        }

    }

    //Used in nextGEQ
    this->start_docs_offset = docs_offset + num_bytes_skip_pointers_list;
    this->start_freq_offset = freqs_offset;
    
    this->cur_doc_id = 0;
    // Init
    next();

    return true;
}

void posting_list::closeList() {
    this->f_docs.close();
    this->f_freqs.close();
}

void posting_list::next() {
    if (this->count == this->pl_len) {
        this->cur_doc_id = std::numeric_limits<unsigned int>::max();
        return;
    }
        
    unsigned int bytes = 0;
    this->cur_doc_id = this->cur_doc_id + VBdecode(this->f_docs, bytes);
    this->cur_freq = VBdecode(this->f_freqs, bytes);
    
    this->count++;
}

/*
Advances the iterator forward to the next posting with a docID
greater than or equal to d.
*/
void posting_list::nextGEQ(unsigned int d) {
    // Check if skip pointers are present
    if (this->cur_doc_id >= d) {
        return;
    }
    if (this->pl_len > MIN_POSTING_LEN) {

        // Check if there is a block that contains doc_id greater or equal than d (FROM THE CURRENT POINT)
        // Get cur_doc_id block

        unsigned block_idx;
        for (block_idx = 0; block_idx < this->skip_pointers.size(); block_idx++) {
            if (this->skip_pointers[block_idx].max_doc_id >= d)
                break;
        }

        // No blocks contain doc_id greater or equal than d
        if (block_idx == this->skip_pointers.size()) {
            this->cur_doc_id = std::numeric_limits<unsigned int>::max();
            return;
        }

        // Position the file pointers to the start of the block that contains doc_id greater or equal than d
        this->f_docs.seekg(start_docs_offset + skip_pointers[block_idx].doc_id_offset);
        this->f_freqs.seekg(start_freq_offset + skip_pointers[block_idx].freqs_offset);

        // Update the counter
        this->count = block_idx * this->block_size;

        // Dgaps
        if (block_idx > 0)
            this->cur_doc_id = skip_pointers[block_idx-1].max_doc_id;
        else 
            this->cur_doc_id = 0;
        // Init cur_doc_id and cur_freq
        next();
    }
    // Scan the list until I find the doc_id greater or equal
    while (this->cur_doc_id < d) {
        next();
    } 
}

unsigned int posting_list::getDocId() {
    return this->cur_doc_id;
}

unsigned int posting_list::getFreq() {
    return this->cur_freq;
}