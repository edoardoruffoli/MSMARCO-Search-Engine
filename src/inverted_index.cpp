#include "MSMARCO-Search-Engine/inverted_index.hpp"

void invert_index(  std::map<std::string, std::list<std::pair<int, int>>>& partial_inv_idx,
                    const std::unordered_map<std::string, int>& token_stream, 
                    int docId) 
{
	for (const std::pair<std::string, int> term_doc : token_stream)
		partial_inv_idx[term_doc.first].push_back(std::make_pair(docId, term_doc.second));
}

void write_block_to_disk(std::map<std::string, std::list<std::pair<int, int>>>& dictionary, 
                    int block_num) 
{
	std::stringstream ss;
	ss << block_num;
	std::string out_file;
	ss >> out_file;
	std::ofstream file("../../results" + out_file);

	if (file.fail()) 
        std::cout << "Error\n";

	for (auto& kv : dictionary) {
		file << kv.first;
		for (auto& i : kv.second)
			file << ' ' << i.first << ' ' << i.second;
		file << std::endl;
	}
}

void merge_blocks(int n_blocks) {
    // Utility to sort the priority queue as min heap in lexicographic
    struct compare {
        bool operator()(
            index_record const& a,
            index_record const& b) const {
                if (a.term == b.term)
                    return a.block_id > b.block_id;
                return a.term > b.term;
        }
    };

    std::ofstream out_file("../../final_results");
    std::priority_queue<index_record, std::vector<index_record>, compare> min_heap;

    // Buffer pointers to the block based inverted indexes 
    std::vector<std::ifstream> in_files;
    for (int i = 1; i <= n_blocks; ++i) {
        in_files.push_back(std::ifstream("../../results" + std::to_string(i)));
        index_record tmp;
        tmp.block_id = i;
        read_record(in_files.back(), tmp);
        min_heap.push(tmp);
    }

    while (!min_heap.empty()) {
        index_record cur = min_heap.top();
        min_heap.pop();
        
        // Update min heap by pushing the new top value of the current block
        index_record tmp;
        tmp.block_id = cur.block_id;
        read_record(in_files[tmp.block_id-1], tmp);
        min_heap.push(tmp);

        // Merge the posting lists of the same terms of the other blocks
        while (min_heap.top().term == cur.term) {
            index_record cur2 = min_heap.top();
            min_heap.pop();
            tmp.block_id = cur2.block_id;
            read_record(in_files[cur2.block_id-1], tmp);
            min_heap.push(tmp);     

            cur.posting_list.splice(cur.posting_list.end(), cur2.posting_list);  // O(1)     
        } 

        // Write on the out buffer
        std::cout << "Writing " << cur.term << '\n';
        write_record(out_file, cur);
    }
}

void read_record(std::ifstream &in, index_record &idx_record) {
    if (in.fail()) 
        std::cout << "Error\n";

    std::string loaded_content;

    getline(in, loaded_content); // Controlli
	std::istringstream iss(loaded_content);
	getline(iss, idx_record.term, ' ');

    std::string docid;
	while (getline(iss, docid, ' ')) {
        std::string freq;
        getline(iss, freq, ' ');
        idx_record.posting_list.push_back(std::make_pair(stoi(docid), stoi(freq)));
    }   
}

void write_record(std::ofstream &out, index_record &idx_record) {
    // Check if term is present

    if (out.fail()) 
        std::cout << "Error\n";

    out << idx_record.term << ' ';
	for (auto& entry : idx_record.posting_list) {
		out << entry.first << ' ' << entry.second << ' ';
	}
    out << '\n';
}
