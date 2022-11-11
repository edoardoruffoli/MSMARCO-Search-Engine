#include "MSMARCO-Search-Engine/compressing.hpp"
#include "MSMARCO-Search-Engine/model.hpp"
    
bool posting_list::openList(unsigned long offset) {

    this->f1.open("../../output/inverted_index.bin", std::ios::binary);
    this->f2.open("../../output/inverted_index.bin", std::ios::binary); // Use only one 
    this->f1.seekg(offset);

    // Decode freqs offset
    unsigned int freqs_offset = 0;
    this->f1.read(reinterpret_cast<char*>(&freqs_offset), sizeof(int));

    // Postion freqs stream
    this->f2.seekg(freqs_offset);

    // Init 
    next();
    return true;
}

void posting_list::closeList() {
    this->f1.close();
    this->f2.close();
}

void posting_list::next() {
    // Check if valid
    this->cur_doc_id = VBdecode(this->f1);
    this->cur_freq = VBdecode(this->f2);
}

void nextGEQ(unsigned int d) {

}

unsigned int getDocId() {
    // Check if end of file
    return this->cur_doc_id;
}