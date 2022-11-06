#include "MSMARCO-Search-Engine/compressing.hpp"

void encode(unsigned int num, std::ofstream& ofile) {
	std::vector<uint8_t> result;
	uint8_t b;
	while (num >= 128) {
		int a = num % 128;
		std::bitset<8> byte(a);
		byte.flip(7);
		num = (num - a) / 128;
		b = byte.to_ulong();
		std::cout << byte << std::endl;
		result.push_back(b);
	}
	int a = num % 128;
	std::bitset<8> byte(a);
	std::cout << byte << std::endl;
	b = byte.to_ulong();
	result.push_back(b);

	for (std::vector<uint8_t>::iterator it = result.begin(); it != result.end(); it++) {
		ofile.write(reinterpret_cast<const char*>(&(*it)), 1);
	}
}

std::vector<int> decode(std::vector<char> &vec) {
	char c;
	int num;
	int p;
	std::vector<int> result;
	for (std::vector<char>::iterator it = vec.begin(); it != vec.end(); it++) {
		c = *it;
		std::bitset<8> byte(c);
		num = 0;
		p = 0;
		while (byte[7] == 1) {
			byte.flip(7);
			num += byte.to_ulong() * pow(128, p);
			std::cout << "num " << num << std::endl;
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

	std::ofstream out("../tmp/uncompressed_inverted_index_test");
	std::string loaded_content;
	std::string doc_ids;

	char c;
	int num;
	int p;
	std::vector<int> result;

	while (getline(ifile, loaded_content)) {
		//std::istringstream iss(loaded_content);
		//getline(iss, doc_ids);
		std::cout << loaded_content << std::endl;
		std::vector<char> vec(loaded_content.begin(), loaded_content.end());


		for (std::vector<char>::iterator it = vec.begin(); it != vec.end(); it++) {
			c = *it;
			std::bitset<8> byte(c);
			num = 0;
			p = 0;
			while (byte[7] == 1) {
				byte.flip(7);
				num += byte.to_ulong() * pow(128, p);
				//std::cout << "num " << num << std::endl;
				p++;
				it++;
				c = *it;
				byte = std::bitset<8>(c);
			}
			num += (byte.to_ulong()) * pow(128, p);

			result.push_back(num);
		}
		for (std::vector<int>::iterator it = result.begin(); it != result.end(); it++) {
			int t = *it;
			out << t << '\n';
		}
	
	}

	//return result;
	out.close();
}
