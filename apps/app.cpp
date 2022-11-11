#include "MSMARCO-Search-Engine/io.hpp"
#include "MSMARCO-Search-Engine/compressing.hpp"
//#include "MSMARCO-Search-Engine/query.hpp"
//#include "MSMARCO-Search-Engine/simple9.hpp"


int main(int argc, char* argv[]) {
    std::cout << "Reading" << std::endl;
    std::map<std::string, unsigned long> lexicon;
    //std::set<doc_entry> doc_table;
    load_lexicon(&lexicon, std::string("../../output/lexicon.bin"));

    //************
    //test simple9
    //************
    /*
    unsigned int result;
    std::vector<unsigned int> encodeTest0 = { 3, 4, 6, 8, 9, 10, 12};
    std::ofstream out_file("../tmp/simple9.bin", std::ios::binary);
    int len;
    len = writeS9(&result, encodeTest0, out_file);
    unsigned int val[100] = { 0 };
    len = readS9(&result, val);
    for (int i = 0; i < len; i++) {
        std::cout << val[i] << " ";
    }
    */
    //if (result == 0x6fffffff)
    //    std::cout << "encode successfully Len : " << len << std::endl;
    //else
    //    std::cout << "encode error" << std::endl;
    /*
    unsigned int val[100] = { 0 };
    len = readS9(&result, val);
    for (int i = 0; i < len; i++) {
        std::cout << val[i] << " ";
    }
    std::cout << std::endl;

    unsigned int encodeTest1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    len = writeS9(&result, encodeTest1, 10);
    std::cout << " write Len " << len << std::endl;
    len = readS9(&result, val);
    for (int i = 0; i < len; i++) {
        std::cout << val[i] << " ";
    }
    std::cout << std::endl;
    
    return 0;


    //*******************
    //test binary writing
    //*******************


    //init_data_structures();
    //auto it = doc_table.find(13);
    //std::cout << it->first << " " << it->second.first << " " << it->second.second << std::endl;
    /*
    for (std::map<std::string, std::pair<unsigned long, size_t>>::iterator it = lexicon.begin(); it != lexicon.end(); it++) {
        std::cout << it->first << " " << it->second.first << " " << it->second.second << std::endl;
    }*/


    //*************
    //openList test
    //*************

    std::string term = "applianc";
    posting_list ret;

    std::vector<char> cur;
    std::vector<unsigned int> decompressed_list;
    char c;

    std::ifstream ifile;

    ifile.open("../tmp/uncompressed_inverted_index.bin", std::ios::binary);
    auto it = lexicon.find(term);
    unsigned long offset = it->second;

    ifile.seekg(offset);
    size_t p_len = 0;
    ifile.read(reinterpret_cast<char*>(&p_len), sizeof(size_t));

    while (p_len) {
        ifile.get(c);
        cur.push_back(c);
        offset++;
        p_len--;
    }

    decompressed_list = VBdecode(cur);
    size_t size_pl = decompressed_list.size() / 2;

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
    
}