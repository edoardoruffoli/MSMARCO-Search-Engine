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

void write_inverted_index_record(std::ofstream &out, term_entry &term_entry) {
    out << term_entry.term << ' ';
	for (auto& entry : term_entry.posting_list) {
		out << entry.first << ',';
	}
    out << ' ';
    for (auto& entry : term_entry.posting_list) {
		out << entry.second << ',';
	}
    out << '\n';
}

unsigned long write_inverted_index_record_compressed(std::ofstream& out, term_entry& term_entry) {
    //encode DocID
    unsigned long num_bytes = 0;
    for (auto& entry : term_entry.posting_list)
        num_bytes += VBencode(unsigned(entry.first), out);

    //encode frenquncy
    for (auto& entry : term_entry.posting_list)
        num_bytes += VBencode(unsigned(entry.second), out);

    return num_bytes;
}

void write_lexicon_record(std::ofstream &out, term_entry &term_entry, unsigned long offset) {
    out << term_entry.term << ' ' << offset << ' ' << term_entry.posting_list.size();
    out << '\n';
}

void merge_blocks(const unsigned int n_blocks) {
    std::cout << "Started Merging Phase: \n\n";
    std::cout << "Number of blocks: " << n_blocks << "\n\n";

    //std::ofstream out_inverted_index("../tmp/uncompressed_inverted_index");
    std::ofstream out_inverted_index("../tmp/uncompressed_inverted_index.bin", std::ios::binary);
    
    if (out_inverted_index.fail()) 
        std::cout << "Error: cannot open uncompressed_inverted_index\n";
    
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
        if (in_files.back().fail()) {
            std::cout << "Error: intermediate file of block " << std::to_string(i) << " not found.\n";
            continue;
        }

        term_entry tmp;
        tmp.block_id = i;
        read_record(in_files.back(), tmp);
        min_heap.push(tmp);
    }

    // Lexicon data structure
    std::map<std::string, std::pair<unsigned long, size_t>> lexicon;

    // Pointer to the posting list in the inverted index file
    unsigned long offset = 0;
    unsigned long len = 0;


    while (!min_heap.empty()) {
        term_entry cur = min_heap.top();
        min_heap.pop();
        
        // Update min heap by pushing the new top value of the current block
        term_entry tmp;
        tmp.block_id = cur.block_id;
        read_record(in_files[tmp.block_id-1], tmp);
        min_heap.push(tmp);

        // Merge the posting lists of the same terms of the other blocks
        while (!min_heap.empty() && min_heap.top().term == cur.term) {
            term_entry cur2 = min_heap.top();
            min_heap.pop();
            tmp.block_id = cur2.block_id;
            if(read_record(in_files[cur2.block_id-1], tmp))
                min_heap.push(tmp);     

            cur.posting_list.splice(cur.posting_list.end(), cur2.posting_list);  // O(1)     
        } 

        // Writing
        //std::cout << "Writing Inverted Index record -> " << cur.term << '\n';
        //write_inverted_index_record(out_inverted_index, cur);
        //offset, length
        offset += len;
        len = write_inverted_index_record_compressed(out_inverted_index, cur);
        lexicon[cur.term] = std::make_pair(offset, len);
        //write_lexicon_record(out_lexicon, cur, offset);

    }

    // Write Lexicon on file
    save_lexicon(lexicon, lexicon_file);

    // Remove intermediate files
    for (unsigned int i = 1; i <= n_blocks; ++i) {
        in_files[i-1].close();
        boost::filesystem::remove(boost::filesystem::path{"../tmp/intermediate_" + std::to_string(i)});
    }
    std::cout << "Removed intermediate files.\n";
}
