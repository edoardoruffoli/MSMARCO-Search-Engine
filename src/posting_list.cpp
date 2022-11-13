#include "MSMARCO-Search-Engine/compressing.hpp"
#include "MSMARCO-Search-Engine/model.hpp"
    
bool posting_list::openList(unsigned long offset) {

    this->f1.open("../../output/inverted_index.bin", std::ios::binary);

    // Decode freqs offset
    unsigned int tmp;
    this->f1.seekg(offset);
    this->f1.read(reinterpret_cast<char*>(&tmp), sizeof(int));

    this->stop_offset = offset + tmp + sizeof(int);
    this->freqs_offset = this->stop_offset;
    this->doc_ids_offset = offset + sizeof(int);

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
Advances the itarator forward to the next posting with a docID
greater than or equal to d.
*/
void posting_list::nextGEQ(unsigned int d) {
    do {
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
    } while (this->cur_doc_id < d);
}

unsigned int posting_list::getDocId() {
    // Check if end of file
    return this->cur_doc_id;
}

unsigned int posting_list::getFreq() {
    return this->cur_freq;
}