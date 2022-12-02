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
    
    // We have skip pointers only if size > 20
    if (this->pl_len > 20) {

        // Compute skip pointers number
        this->n_skip_pointers = ceil((double)posting_list_len / ((unsigned int)sqrt(posting_list_len)));

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

    // Init
    next();

    return true;
}

void posting_list::closeList() {
    this->f_docs.close();
    this->f_freqs.close();
}

void posting_list::next() {
    if (this->count == this->pl_len) 
        return;
        
    unsigned int bytes = 0;
    this->cur_doc_id = VBdecode(this->f_docs, bytes);
    this->cur_freq = VBdecode(this->f_freqs, bytes);
    
    this->count++;
}

/*
Advances the iterator forward to the next posting with a docID
greater than or equal to d.
*/
void posting_list::nextGEQ(unsigned int d) {
    /*
    if (this->cur_doc_id >= d)
        return;

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

    this->doc_ids_offset = this->base_offset + this->skip_pointers_list_size + skip_pointers[block_idx].doc_id_offset;
    this->freqs_offset = this->base_offset + this->skip_pointers_list_size + skip_pointers[block_idx].freqs_offset;

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
    */
}

unsigned int posting_list::getDocId() {
    // Check if end of file
    return this->cur_doc_id;
}

unsigned int posting_list::getFreq() {
    return this->cur_freq;
}