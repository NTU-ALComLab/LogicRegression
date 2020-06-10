#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;


int main(int argc, char** argv) {
	ifstream fin(argv[1]);
	ofstream fout(argv[2]);
	int numPI, num_pat;
	int numPO[2] = {17, 19};

	string s;
	fin >> numPI >> num_pat;
	getline(fin, s);
	getline(fin, s);
	
	fout << numPI << ' ' << numPO[0] + numPO[1] << ' ' << num_pat << '\n';
	fout << s;
	for (int k = 0; k < 2; ++k)
		for (int i = numPO[k] - 1; i >= 0; --i)
			fout << " PO_out" << k << "_" << i << "_";
	fout << '\n';


	for (int i = 0; i < num_pat; ++i) {
		getline(fin, s);
		string s_out = s;
		s.erase(remove(s.begin(), s.end(), ' '), s.end());
		int num[3] = {};
		for (size_t j = 0; j < s.size(); ++j) {
			num[j / 16] <<= 1;
			num[j / 16] += s[j] - '0';
		}
		int out[2] = { num[0] - 153781, 34283 - num[0] + num[2] + num[1]  };
		fout << s_out;
		for (int k = 0; k < 2; ++k) {
			size_t mask = 1 << (numPO[k] - 1);
			for (int j = 0; j < numPO[k]; ++j) {
				fout << ' ' << (out[k] & mask? '1': '0');
				mask >>= 1;
			}
		} fout << '\n';
	}
}
