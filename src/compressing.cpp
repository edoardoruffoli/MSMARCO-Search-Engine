#include "MSMARCO-Search-Engine/compressing.hpp"


void VBencode(unsigned int num, std::vector<uint8_t> &result) {
    result.clear();
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
}

unsigned int VBdecode(std::istream& ifile, unsigned int &n_bytes) {
    char c;
	int num = 0;
	int p = 0;

    ifile.get(c);
    n_bytes++;
	std::bitset<8> byte(c);
	while (byte[7] == 1) {
		byte.flip(7);
		num += byte.to_ulong() * pow(128, p);
		p++;
        ifile.get(c);
        n_bytes++;
		byte = std::bitset<8>(c);
	}
	num += (byte.to_ulong()) * pow(128, p);
    return num;
}
