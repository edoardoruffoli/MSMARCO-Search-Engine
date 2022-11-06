/*
#include <fstream>
#include <iostream>
#include <bitset>
#include <vector>
#include <cmath>

void write(std::vector<uint8_t> num, std::ofstream& ofile) {
	for (std::vector<uint8_t>::iterator it = num.begin(); it != num.end(); it++) {
		ofile.write(reinterpret_cast<const char*>(&(*it)), 1);
	}
	ofile.close();
}

std::vector<char> read_com(std::ifstream& infile) {
	char c;
	std::vector<char> result;
	while (infile.get(c)) {
		result.push_back(c);
	}
	return result;
}

void VBEncode(unsigned int num) {
	std::ofstream ofile;
	ofile.open("../../test.bin", std::ios::binary);
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
	write(result, ofile);
}

std::vector<int> VBDecode(std::string filename) {
	std::ifstream ifile;
	ifile.open(filename, std::ios::binary);
	char c;
	int num;
	int p;
	std::vector<int> result;
	std::vector<char> vec = read_com(ifile);

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
*/
int main() {
	/*
	uint8_t a = 255;
	std::vector<char> c;
	std::vector<int> i;
	//int in;
	//std::cin >> in;
	VBEncode(122);
	VBEncode(10);
	VBEncode(10);
	VBEncode(11);
	VBEncode(10);
	i = VBDecode("../../test.bin");
	for (std::vector<int>::iterator it = i.begin(); it != i.end(); it++) {
		std::cout << "result " << *it << std::endl;
	}
	*/
	return 0;
}