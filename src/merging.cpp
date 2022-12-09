#include "MSMARCO-Search-Engine/merging.hpp"

/* Read a record from the temportary inverted index file stream passed as parameter and put in term_entry.
*/
bool read_record(std::ifstream &in, term_entry &term_entry) {
    if (in.eof())
        return false;

    std::string loaded_content;

    if (!getline(in, loaded_content))
        return false;
    
    // Parse term
	std::istringstream iss(loaded_content);
	getline(iss, term_entry.term, ' ');
    std::string docid;

    // Parse the posting list
	while (getline(iss, docid, ' ')) {
        std::string freq;
        getline(iss, freq, ' ');
        term_entry.posting_list.push_back(std::make_pair(stoi(docid), stoi(freq)));
    }   
    return true;
}

/* Write term_entry in the .docs and .freqs file in variable byte compressed form. Return the number of bytes 
   written in each of the two files.
*/
std::pair<unsigned long, unsigned long> write_inverted_index_record_compressed(std::ofstream& out_docs, 
                    std::ofstream& out_freqs, 
                    term_entry& term_entry) 
{
    // Keeps the count of the number of bytes written on file  
    unsigned long docs_num_bytes_written = 0, freqs_num_bytes_written = 0;

    // Vectors to store the VB representation of the doc_id and freqs
    std::vector<uint8_t> VB_doc_ids, VB_freqs;

    unsigned int prev_doc_id = 0;

    // Compute skip pointers
    if (term_entry.posting_list.size() > 20) {
        // Compute skip pointer block size
        unsigned int block_size = sqrt(term_entry.posting_list.size());

        // Vectors to store the VB representation of skip pointers
        std::vector<std::vector<uint8_t>> VB_block_max_doc_ids, VB_block_offset_doc_ids, VB_block_offset_freqs;

        unsigned int cur_block_count = 0, doc_ids_offset = 0, freqs_offset = 0;

        std::vector<uint8_t> cur_doc_id_bytes, cur_freqs_bytes, offset_bytes;

        for (auto& entry : term_entry.posting_list) {

            // Add encoding of the current doc_id
            VBencode(unsigned(entry.first - prev_doc_id), cur_doc_id_bytes);
            VB_doc_ids.insert(VB_doc_ids.end(), cur_doc_id_bytes.begin(), cur_doc_id_bytes.end());
            prev_doc_id = entry.first;

            // Add encoding of the current freqs
            VBencode(unsigned(entry.second), cur_freqs_bytes);
            VB_freqs.insert(VB_freqs.end(), cur_freqs_bytes.begin(), cur_freqs_bytes.end());
            cur_block_count++;

            if (cur_block_count == block_size) {
                // Store the max doc_id of the current block, that is the prev iteration doc_id, stored in bytes variable
                VB_block_max_doc_ids.push_back(cur_doc_id_bytes);

                // Store the doc_id offset of the current block
                VBencode(doc_ids_offset, offset_bytes);
                VB_block_offset_doc_ids.push_back(offset_bytes);

                // Update doc_id offset, the start offset of the next block is equal to the current size of VB_doc_ids
                doc_ids_offset = VB_doc_ids.size();

                // Store the freqs offset of the current block
                VBencode(freqs_offset, offset_bytes);
                VB_block_offset_freqs.push_back(offset_bytes);

                // Update doc_id offset, the start offset of the next block is equal to the current size of VB_doc_ids
                freqs_offset = VB_freqs.size();

                // Restart the element in block counter
                cur_block_count = 0;
            }
        }

        // The last block may contain a number of elements < BLOCK_SIZE
        if (cur_block_count > 0) {
            VB_block_max_doc_ids.push_back(cur_doc_id_bytes);
            
            // Store the doc_id offset of the current block
            VBencode(doc_ids_offset, offset_bytes);
            VB_block_offset_doc_ids.push_back(offset_bytes);

            // Update doc_id offset, the start offset of the next block is equal to the current size of VB_doc_ids
            doc_ids_offset = VB_doc_ids.size();

            // Store the freqs offset of the current block
            VBencode(freqs_offset, offset_bytes);
            VB_block_offset_freqs.push_back(offset_bytes);
        }

        // Write the skip pointers list in the docs file
        for (unsigned int i=0; i<VB_block_max_doc_ids.size(); i++) {
            // Write max doc_id
            for (std::vector<uint8_t>::iterator it=VB_block_max_doc_ids[i].begin(); it!=VB_block_max_doc_ids[i].end();it++) {
                out_docs.write(reinterpret_cast<const char*>(&(*it)), 1);
                docs_num_bytes_written++;
            }
            // Write block offset doc_ids
            for (std::vector<uint8_t>::iterator it=VB_block_offset_doc_ids[i].begin(); it!=VB_block_offset_doc_ids[i].end();it++) {
                out_docs.write(reinterpret_cast<const char*>(&(*it)), 1);
                docs_num_bytes_written++;
            }
            // Write block offset freqs
            for (std::vector<uint8_t>::iterator it=VB_block_offset_freqs[i].begin(); it!=VB_block_offset_freqs[i].end();it++) {
                out_docs.write(reinterpret_cast<const char*>(&(*it)), 1);
                docs_num_bytes_written++;
            }
        }

        // Write the doc_ids represented in VB previously computed
        for (std::vector<uint8_t>::iterator it = VB_doc_ids.begin(); it != VB_doc_ids.end(); it++) {
            out_docs.write(reinterpret_cast<const char*>(&(*it)), 1);
            docs_num_bytes_written++;
        }

        // Write the freqs represented in VB previously computed
        for (std::vector<uint8_t>::iterator it = VB_freqs.begin(); it != VB_freqs.end(); it++) {
            out_freqs.write(reinterpret_cast<const char*>(&(*it)), 1);
            freqs_num_bytes_written++;
        }
    }
    // Posting list is small do not compute skip pointers
    else {
        std::vector<uint8_t> bytes;

        for (auto& entry : term_entry.posting_list) {
            // Add encoding of the current doc_id
            VBencode(unsigned(entry.first - prev_doc_id), bytes);
            prev_doc_id = entry.first;

            // Write the doc_ids represented in VB previously computed
            for (std::vector<uint8_t>::iterator it = bytes.begin(); it != bytes.end(); it++) {
                out_docs.write(reinterpret_cast<const char*>(&(*it)), 1);
                docs_num_bytes_written++;
            }

            VBencode(unsigned(entry.second), bytes);
            // Write the freqs represented in VB previously computed
            for (std::vector<uint8_t>::iterator it = bytes.begin(); it != bytes.end(); it++) {
                out_freqs.write(reinterpret_cast<const char*>(&(*it)), 1);
                freqs_num_bytes_written++;
            }
        }
    }

    return std::make_pair(docs_num_bytes_written, freqs_num_bytes_written);
}

void merge_blocks(const unsigned int n_blocks) {
    std::cout << "Started Merging Phase: \n\n";
    std::cout << "Number of blocks: " << n_blocks << "\n\n";

    std::ofstream out_inverted_index_docs("../../output/inverted_index_docs.bin", std::ios::binary | std::ios::out);
    std::ofstream out_inverted_index_freqs("../../output/inverted_index_freqs.bin", std::ios::binary | std::ios::out);

    if (out_inverted_index_docs.fail() || out_inverted_index_freqs.fail())
        std::cout << "Error: cannot open inverted_index\n";
    
    std::string lexicon_file("../../output/lexicon.bin");
    std::string doc_table_file("../../output/doc_table.bin");

    // Utility to keep the priority queue sorted as min heap based on lexicographic order
    struct compare {
        bool operator()(term_entry const& a, term_entry const& b) const {
                if (a.term == b.term)
                    return a.block_id > b.block_id;
                return a.term > b.term;
        }
    };

    std::priority_queue<term_entry, std::vector<term_entry>, compare> min_heap;

    // Streams of the intermediate posting lists
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
    Lexicon lexicon;
    lexicon.create(lexicon_file, N_HASH_KEYS);

    // Doc Table data structure
    DocTable doc_table; 
    doc_table.open(doc_table_file);

    // Utility variables
    unsigned long docs_offset = 0, freqs_offset = 0;
    std::pair<unsigned long, unsigned long> len;
    double max_score = 0;
    double bm25 = 0;

    while (!min_heap.empty()) {
        
        // Extract top item in min heap
        term_entry cur = min_heap.top();
        min_heap.pop();

        // Update min heap by pushing the new top value of the current block
        term_entry tmp;
        tmp.block_id = cur.block_id;
        if (read_record(in_files.at(tmp.block_id-1), tmp))
            min_heap.push(tmp);

        // Merge the posting lists of the same terms of the other blocks
        while (!min_heap.empty() && min_heap.top().term == cur.term) {
            term_entry cur2 = min_heap.top();
            min_heap.pop();
            tmp = {};
            tmp.block_id = cur2.block_id;
            tmp.posting_list.clear();
            if(read_record(in_files.at(cur2.block_id-1), tmp))
                min_heap.push(tmp);     

            cur.posting_list.splice(cur.posting_list.end(), cur2.posting_list);  // O(1)     
        } 

        // Compute upper bound score
        doc_table_entry de;
        for (auto entry : cur.posting_list) {

            // Get doc entry
            if (!doc_table.getEntryByIndex(entry.first, de)) {
                std::cout << "Error while reading the doc_table.\n";
                return;
            }

            bm25 = BM25(entry.second, (unsigned int)cur.posting_list.size(), 
                        de.doc_len, doc_table.getAvgDocLen(), 
                        doc_table.getSize());
            if (bm25 > max_score)
                max_score = bm25;
        }
        
        // Writing
        //std::cout << "Writing Inverted Index record -> " << cur.term << '\n';

        // Update file offsets values
        docs_offset += len.first;
        freqs_offset += len.second;

        // Write the current term entry in the final inverted index in compressed form
        len = write_inverted_index_record_compressed(out_inverted_index_docs, out_inverted_index_freqs, cur);
        
        //lexicon entry format : [term, num_docs, offset inverted index, maxscore]
        lexicon_entry le = {(unsigned int) cur.posting_list.size(), docs_offset, freqs_offset, max_score};
        lexicon.insert(cur.term, le);
    }

    // Close Lexicon
    lexicon.close();

    // Close Doc Table
    doc_table.close();

    // Remove intermediate files
/*    for (unsigned int i = 1; i <= n_blocks; ++i) {
        in_files[i-1].close();
        boost::filesystem::remove(boost::filesystem::path{"../tmp/intermediate_" + std::to_string(i)});
    }
    std::cout << "Removed intermediate files.\n";
    */

   // DELETE in_files
}
