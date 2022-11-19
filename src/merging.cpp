#include "MSMARCO-Search-Engine/merging.hpp"

bool read_record(std::istream &in, term_entry &term_entry) {
    if (in.eof())
        return false;

    std::string loaded_content;

    getline(in, loaded_content); // Controlli
    
	std::istringstream iss(loaded_content);
    std::cout << loaded_content << std::endl;
	getline(iss, term_entry.term, ' ');

    std::string docid;
	while (getline(iss, docid, ' ')) {
        std::string freq;
        getline(iss, freq, ' ');
        //if (docid == "64")
        //std::cout << "docid: " << docid << " freq" << freq << "\n";
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

std::ifstream in("../../output/inverted_index_test.bin", std::ios::binary);
unsigned long write_inverted_index_record_compressed(std::ofstream& out, term_entry& term_entry) {
    // Number of bytes written
    unsigned int num_bytes = 0;

    // Save first position that will be used to store the freqs offset
    long first_byte = out.tellp();
    
    // Clean first 4 bytes
    int tmp = 0;
    out.write(reinterpret_cast<const char*>(&tmp), sizeof(int));

    // Encode doc_id
    for (auto& entry : term_entry.posting_list)
        num_bytes += VBencode(unsigned(entry.first), out);

    unsigned int freqs_offset = num_bytes;

    // Encode frequency
    for (auto& entry : term_entry.posting_list)
        num_bytes += VBencode(unsigned(entry.second), out);

    // Write freqs_offset value at first 4 bytes
    out.seekp(first_byte);
    out.write(reinterpret_cast<const char*>(&freqs_offset), sizeof(int));
    num_bytes += sizeof(int);

    // Reposition file stream to the next position
    out.seekp(first_byte + num_bytes);

    return num_bytes;
}

void write_lexicon_record(std::ofstream &out, term_entry &term_entry, unsigned long offset) {
    out << term_entry.term << ' ' << offset << ' ' << term_entry.posting_list.size();
    out << '\n';
}

double BM25(unsigned int tf, unsigned int df, unsigned int doc_len, unsigned int avg_doc_len, unsigned int N) {
    double k1 = 1.2;
    double b = 0.75;
    return tf * log10((double)N / df) / (k1 * ((1 - b) + b * ((double)doc_len / avg_doc_len)) + tf);
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
    std::vector<std::istream*> in_files;
    for (unsigned int i = 1; i <= n_blocks; ++i) {
        boost::iostreams::stream<boost::iostreams::mapped_file_source> mapped_file_stream;
        boost::iostreams::mapped_file_source mmap("../tmp/intermediate_" + std::to_string(i));

        mapped_file_stream.open(mmap);
        if (mapped_file_stream.fail())
            std::cout << "Error: input fail not valid.\n";
        
        boost::iostreams::filtering_streambuf<boost::iostreams::input> *inbuf = new boost::iostreams::filtering_streambuf<boost::iostreams::input>();
        (*inbuf).push(mapped_file_stream);
        
        // Convert streambuf to istream and push
        in_files.push_back(new std::istream(inbuf));
        if ((*in_files.back()).fail()) {
            std::cout << "Error: intermediate file of block " << std::to_string(i) << " not found.\n";
            continue;
        }

        term_entry tmp;
        tmp.block_id = i;
        read_record(*in_files.back(), tmp);
        min_heap.push(tmp);
    }

    // Lexicon data structure
    std::map<std::string, lexicon_entry> lexicon;

   /* std::vector<doc_table_entry> doc_table;
    load_doc_table(&doc_table, std::string("../../output/doc_table.bin"));

    double avg_doc_len;
    // IF BM25 compute avg doc len
    int sum = 0;
    for (auto doc : doc_table) {
        sum += doc.doc_len;
    }
    avg_doc_len = (double)sum / doc_table.size();

    // Free memory
    doc_table.clear();*/

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
        read_record(*in_files.at(tmp.block_id-1), tmp);
        min_heap.push(tmp);

        // Merge the posting lists of the same terms of the other blocks
        while (!min_heap.empty() && min_heap.top().term == cur.term) {
            term_entry cur2 = min_heap.top();
            min_heap.pop();
            tmp.block_id = cur2.block_id;
            if(read_record(*in_files.at(cur2.block_id-1), tmp))
                min_heap.push(tmp);     

            cur.posting_list.splice(cur.posting_list.end(), cur2.posting_list);  // O(1)     
        } 

        /*
        for (auto entry : cur.posting_list) {
            bm25 = BM25(entry.second, (unsigned int)cur.posting_list.size(), doc_table[entry.first].doc_len, avg_doc_len, (unsigned int)doc_table.size());
            if (bm25 > max_score)
                max_score = bm25;
        }
        */

        // Writing
        std::cout << "Writing Inverted Index record -> " << cur.term << '\n';
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
