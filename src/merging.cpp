#include "MSMARCO-Search-Engine/merging.hpp"

bool read_record(std::ifstream &in, term_entry &term_entry) {
    if (in.eof())
        return false;

    std::string loaded_content;

    getline(in, loaded_content); // Controlli
    
	std::istringstream iss(loaded_content);
	getline(iss, term_entry.term, ' ');
    std::string docid;
	while (getline(iss, docid, ' ')) {
        std::string freq;
        getline(iss, freq, ' ');
        term_entry.posting_list.push_back(std::make_pair(stoi(docid), stoi(freq)));
    }   
    return true;
}


unsigned long write_inverted_index_record_compressed(std::ofstream& out, term_entry& term_entry) {    
    unsigned int num_bytes_written = 0;

    // Compute skip pointer block size
    unsigned int block_size = sqrt(term_entry.posting_list.size());
    unsigned int n_block = 0;

    // Vector to store the VB representation of the doc_id and freqs
    std::vector<uint8_t> VB_doc_ids;    
    std::vector<uint8_t> VB_freqs;

    std::vector<std::vector<uint8_t>> VB_block_max_doc_ids;
    std::vector<std::vector<uint8_t>> VB_block_offset_doc_ids;
    std::vector<std::vector<uint8_t>> VB_block_offset_freqs;

    unsigned int num_skip_pointers;
   
    // Start counting the bytes required to encode the doc_ids
    std::vector<uint8_t> bytes;
    unsigned int cur_block_count = 0;
    unsigned int offset = 0;
    for (auto& entry : term_entry.posting_list) {   
        // Add encoding of the current doc_id
        VBencode(unsigned(entry.first), bytes);
        VB_doc_ids.insert(VB_doc_ids.end(), bytes.begin(), bytes.end());
        cur_block_count++;

        if (cur_block_count == block_size) {
            // Store the max doc_id of the current block, that is the prev iteration doc_id, stored in bytes variable
            VB_block_max_doc_ids.push_back(bytes);

            // Store the doc_id offset of the current block
            VBencode(offset, bytes);
            VB_block_offset_doc_ids.push_back(bytes);

            // Update offset
            offset = VB_doc_ids.size();
            n_block++;
            cur_block_count = 0;
        }
    }

    cur_block_count = 0;
    offset = 0;
    for (auto& entry : term_entry.posting_list) {
        // Add encoding of the current freq
        VBencode(unsigned(entry.second), bytes);
        VB_freqs.insert(VB_freqs.end(), bytes.begin(), bytes.end());
        cur_block_count++;

        if (cur_block_count == block_size) {
            // Write the freqs offset of the current block
            VBencode(VB_doc_ids.size() + offset, bytes);
            VB_block_offset_freqs.push_back(bytes);

            offset = VB_freqs.size();
            cur_block_count = 0;
        }
    }

    num_skip_pointers = n_block;

    // Write the skip pointers size
    VBencode(num_skip_pointers, bytes);
    for (std::vector<uint8_t>::iterator it = bytes.begin(); it != bytes.end(); it++) {
        out.write(reinterpret_cast<const char*>(&(*it)), 1);
        num_bytes_written++;
    }

    // Write the skip pointers list
    for (unsigned int i=0; i<VB_block_max_doc_ids.size(); i++) {

        // Write max doc_id
        for (std::vector<uint8_t>::iterator it=VB_block_max_doc_ids[i].begin(); it!=VB_block_max_doc_ids[i].end();it++) {
            out.write(reinterpret_cast<const char*>(&(*it)), 1);
            num_bytes_written++;
        }
        
        // Write block offset doc_ids
        for (std::vector<uint8_t>::iterator it=VB_block_offset_doc_ids[i].begin(); it!=VB_block_offset_doc_ids[i].end();it++) {
            out.write(reinterpret_cast<const char*>(&(*it)), 1);
            num_bytes_written++;
        }

        // Write block offset freqs
        for (std::vector<uint8_t>::iterator it=VB_block_offset_freqs[i].begin(); it!=VB_block_offset_freqs[i].end();it++) {
            out.write(reinterpret_cast<const char*>(&(*it)), 1);
            num_bytes_written++;
        }
    }

    // Write the doc_ids represented in VB previously computed
    for (std::vector<uint8_t>::iterator it = VB_doc_ids.begin(); it != VB_doc_ids.end(); it++) {
        out.write(reinterpret_cast<const char*>(&(*it)), 1);
        num_bytes_written++;
    }

    // Write the freqs represented in VB previously computed
    for (std::vector<uint8_t>::iterator it = VB_freqs.begin(); it != VB_freqs.end(); it++) {
        out.write(reinterpret_cast<const char*>(&(*it)), 1);
        num_bytes_written++;
    }

    return num_bytes_written;
}

void merge_blocks(const unsigned int n_blocks) {
    std::cout << "Started Merging Phase: \n\n";
    std::cout << "Number of blocks: " << n_blocks << "\n\n";

    //std::ofstream out_inverted_index("../tmp/uncompressed_inverted_index");
    std::ofstream out_inverted_index("../../output/inverted_index.bin", std::ios::binary);
    
    if (out_inverted_index.fail()) 
        std::cout << "Error: cannot open inverted_index\n";
    
    std::string lexicon_file("../../output/lexicon.bin");

    // Utility to sort the priority queue as min heap based on lexicographic order
    struct compare {
        bool operator()(term_entry const& a, term_entry const& b) const {
                if (a.term == b.term)
                    return a.block_id > b.block_id;
                return a.term > b.term;
        }
    };

    std::priority_queue<term_entry, std::vector<term_entry>, compare> min_heap;

    // Buffer pointers to the intermediate posting lists
    std::vector<std::ifstream> in_files;
    for (unsigned int i = 1; i <= n_blocks; ++i) {
        in_files.push_back(std::ifstream("../tmp/intermediate_" + std::to_string(i)));
        if ((in_files.back()).fail()) {
            std::cout << "Error: intermediate file of block " << std::to_string(i) << " not found.\n";
            continue;
        }

        term_entry tmp;
        tmp.block_id = i;
        read_record(in_files.back(), tmp);
        min_heap.push(tmp);
    }

    // Lexicon data structure
    std::map<std::string, lexicon_entry> lexicon;

    std::vector<doc_table_entry> doc_table;
    load_doc_table(&doc_table, std::string("../../output/doc_table.bin"));

    double avg_doc_len;
    // IF BM25 compute avg doc len
    int sum = 0;
    for (auto doc : doc_table) {
        sum += doc.doc_len;
    }
    avg_doc_len = (double)sum / doc_table.size();

    // Pointer to the posting list in the inverted index file
    unsigned long offset = 0;
    unsigned long len = 0;
    double max_score = 0;
    double bm25 = 0;

    while (!min_heap.empty()) {
        term_entry cur = min_heap.top();
        min_heap.pop();
        
        // Update min heap by pushing the new top value of the current block
        term_entry tmp;
        tmp.block_id = cur.block_id;
        read_record(in_files.at(tmp.block_id-1), tmp);
        min_heap.push(tmp);

        // Merge the posting lists of the same terms of the other blocks
        while (!min_heap.empty() && min_heap.top().term == cur.term) {
            term_entry cur2 = min_heap.top();
            min_heap.pop();
            tmp.block_id = cur2.block_id;
            if(read_record(in_files.at(cur2.block_id-1), tmp))
                min_heap.push(tmp);     

            cur.posting_list.splice(cur.posting_list.end(), cur2.posting_list);  // O(1)     
        } 

        // Compute upper bound score
        for (auto entry : cur.posting_list) {
            bm25 = BM25(entry.second, (unsigned int)cur.posting_list.size(), 
                        doc_table[entry.first].doc_len, avg_doc_len, 
                        (unsigned int)doc_table.size());
            if (bm25 > max_score)
                max_score = bm25;
        }

        // Writing
        //std::cout << "Writing Inverted Index record -> " << cur.term << '\n';
        offset += len;
        len = write_inverted_index_record_compressed(out_inverted_index, cur);
        
        //lexicon : [term, num_docs, offset inverted index, maxscore]
        lexicon[cur.term] = {(unsigned int) cur.posting_list.size(), offset, max_score};
        //write_lexicon_record(out_lexicon, cur, offset);
    }

    // Write Lexicon on file
    save_lexicon(lexicon, lexicon_file);

    // Remove intermediate files
/*    for (unsigned int i = 1; i <= n_blocks; ++i) {
        in_files[i-1].close();
        boost::filesystem::remove(boost::filesystem::path{"../tmp/intermediate_" + std::to_string(i)});
    }
    std::cout << "Removed intermediate files.\n";
    */

   // DELETE in_files
}
