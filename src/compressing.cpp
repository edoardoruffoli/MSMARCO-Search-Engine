#include "MSMARCO-Search-Engine/compressing.hpp"


unsigned long VBencode(unsigned int num, std::ofstream& ofile) {
	std::vector<uint8_t> result;
	uint8_t b;
	while (num >= 128) {
		int a = num % 128;
		std::bitset<8> byte(a);
		byte.flip(7);
		num = (num - a) / 128;
		b = byte.to_ulong();
		result.push_back(b);
	}
	int a = num % 128;
	std::bitset<8> byte(a);
	b = byte.to_ulong();
	result.push_back(b);

	for (std::vector<uint8_t>::iterator it = result.begin(); it != result.end(); it++) {
		ofile.write(reinterpret_cast<const char*>(&(*it)), 1);
	}

	return result.size();
}

std::vector<unsigned int> VBdecode(std::vector<char> &vec) {
	char c;
	int num;
	int p;
	std::vector<unsigned int> result;
	for (std::vector<char>::iterator it = vec.begin(); it != vec.end(); it++) {
		c = *it;
		std::bitset<8> byte(c);
		num = 0;
		p = 0;
		while (byte[7] == 1) {
			byte.flip(7);
			num += byte.to_ulong() * pow(128, p);
			p++;
			it++;
			c = *it;
			byte = std::bitset<8>(c);
		}
		num += (byte.to_ulong()) * pow(128, p);

		result.push_back(num);
	}

	return result;
}

void read_compressed_index(std::string filename) {
	std::ifstream ifile;
	ifile.open(filename, std::ios::binary);
	std::map<std::string, unsigned long> lexicon;
	std::string lexicon_file("../../output/lexicon.bin");

	load_lexicon(&lexicon, lexicon_file);

	std::ofstream out("../tmp/uncompressed_inverted_index_test");
	std::string loaded_content;
	std::string doc_ids;

	char c;
	int num;
	int p;
	std::vector<char> cur;
	//bool control = true;
	for (std::map<std::string, unsigned long>::iterator it = lexicon.begin(); it != lexicon.end(); it++) {
		//if (control) {
		//	control = false;
		//	continue;
		//}
		std::string term = it->first;
		unsigned long offset = it->second;
		size_t p_len;
        ifile.read(reinterpret_cast<char*>(&p_len), sizeof(size_t));

		//std::cout << "Term : " << it->first << std::endl;
		//std::cout << "Offset --> " << offset << std::endl;
		//std::cout << "Len --> " << p_len << std::endl;
		ifile.seekg(offset);
		
		#pragma omp simd 
		while (p_len) {
			ifile.get(c);
			cur.push_back(c);
			offset++;
			p_len--;
		}

		std::vector<unsigned int> cur_posting_list;
		for (std::vector<char>::iterator it = cur.begin(); it != cur.end(); it++) {
			c = *it;
			std::bitset<8> byte(c);
			num = 0;
			p = 0;
			while (byte[7] == 1) {
				byte.flip(7);
				num += byte.to_ulong() * pow(128, p);
				p++;
				it++;
				c = *it;
				byte = std::bitset<8>(c);
			}
			num += (byte.to_ulong()) * pow(128, p);

			cur_posting_list.push_back(num);
		}
		out << term << ' ';
		for (std::vector<unsigned int>::iterator it = cur_posting_list.begin(); it != cur_posting_list.end(); it++) {
			int t = *it;
			out << t << ',';
		}
		out << '\n';
		cur.clear();
	}
	out.close();
}
