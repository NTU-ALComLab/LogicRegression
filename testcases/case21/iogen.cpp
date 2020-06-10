#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char** argv) {
	ifstream fin(argv[1]);
	ofstream fout(argv[2]);

	int num_pat, numPI, numPO = 1, in;
	string names;
	fin >> numPI >> num_pat;
	getline(fin, names);
	getline(fin, names);
	fout << numPI << ' ' << numPO << ' ' << num_pat << '\n';
	fout << "a b c d e f g h i j k l m n o p q r s t u v w q y z\n";
	
	for (int i = 0; i < num_pat; ++i) {
		int ones = 0;
		for (int j = 0; j < numPI; ++j) {
			fin >> in;
			fout << in << ' ';
			if (in) ++ones;
		}
		fout << (in? 0:(ones % 2 == 1)) << '\n';
	}
}
