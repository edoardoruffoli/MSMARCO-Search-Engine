#include "MSMARCO-Search-Engine/compressing.hpp"
#include "MSMARCO-Search-Engine/model.hpp"
    
bool posting_list::openList(unsigned long offset) {

    this->f1.open("../../output/inverted_index.bin", std::ios::binary);
    this->f2.open("../../output/inverted_index.bin", std::ios::binary); // this->f2 = f1;
    this->f1.seekg(offset);

    // Decode freqs offset
    size_t freqs_offset = 0;
    this->f1.read(reinterpret_cast<char*>(&freqs_offset), sizeof(size_t));

    // Postion freqs stream
    this->f2.seekg(freqs_offset);

    return true;
}

void posting_list::closeList() {
    this->f1.close();
    this->f2.close();
}

void posting_list::next() {
    this->cur_doc_id = VBdecode(this->f1);
    this->cur_freq = VBdecode(this->f2);
}

void nextGEQ(unsigned int d) {

}