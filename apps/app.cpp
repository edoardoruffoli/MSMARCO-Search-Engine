#include "MSMARCO-Search-Engine/io.hpp"
#include "MSMARCO-Search-Engine/compressing.hpp"
//#include "MSMARCO-Search-Engine/query.hpp"

int main(int argc, char* argv[]) {
    std::cout << "Reading" << std::endl;
    std::map<std::string, std::pair<unsigned long, size_t>> lexicon;
    //std::set<doc_entry> doc_table;
    load_lexicon(&lexicon, std::string("../../output/lexicon.bin"));
    //init_data_structures();
    //auto it = doc_table.find(13);
    //std::cout << it->first << " " << it->second.first << " " << it->second.second << std::endl;
    /*
    for (std::map<std::string, std::pair<unsigned long, size_t>>::iterator it = lexicon.begin(); it != lexicon.end(); it++) {
        std::cout << it->first << " " << it->second.first << " " << it->second.second << std::endl;
    }*/

    //openList test
    /*
    std::string term = "applianc";
    posting_list ret;

    std::vector<char> cur;
    std::vector<unsigned int> decompressed_list;
    char c;

    std::ifstream ifile;

    ifile.open("../tmp/uncompressed_inverted_index.bin", std::ios::binary);
    auto it = lexicon.find(term);
    unsigned long offset = it->second.first;
    size_t p_len = it->second.second;

    ifile.seekg(offset);

    while (p_len) {
        ifile.get(c);
        cur.push_back(c);
        offset++;
        p_len--;
    }

    decompressed_list = VBdecode(cur);
    int size_pl = decompressed_list.size() / 2;

    for (std::vector<unsigned int>::iterator it = decompressed_list.begin(); it != decompressed_list.end(); ++it) {
         ret.doc_ids.push_back(*it);
        //else {
        //    ret.freqs.push_back(decompressed_list[i]);
        //}
    }
    ifile.close();

    std::cout << term << " ";
    for (std::vector<unsigned int>::iterator it = ret.doc_ids.begin(); it != ret.doc_ids.end(); ++it) {
        std::cout << *it << ",";
    }
    //for (int i : ret.freqs) {
    //    std::cout << ret.freqs[i] << ",";
    //}
    std::cout << std::endl;
    */
    
}