#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <stack>
#include <algorithm>
#include "pattern.h"
#include "io_helper.h"

#pragma GCC diagnostic ignored "-Wunused-result"

using namespace std;


string in_pat_file = "in_pat.txt";
string io_rel_file = "io_rel.txt";
int wire_number = 0;

IO_Helper::IO_Helper (char* info_file, char* iogen_exe, char* circuit_file)
	{ if (info_file) init (info_file, iogen_exe, circuit_file); }

void IO_Helper::init (char* info_file, char* iogen_exe, char* circuit_file) {
	ifstream fin(info_file);
	// read # of PI, # of PO
	fin >> numPI >> numPO;

	string name;
	// read PI names
	for (int i = 0; i < numPI; ++i) {
		fin >> name;
		PI_name.push_back(name);
	}
	// read PO names
	for (int i = 0; i < numPO; ++i) {
		fin >> name;
		PO_name.push_back(name);
	}

	_exe = string(iogen_exe);
	_circuit_file = circuit_file;
}


void IO_Helper::get_io_rel (const vector<Pattern>& in_pat, PatHash& io_rel) const {
	// write in_pat to in_pat.txt first
	write_in_pat(in_pat);

	// execute command i.e. "./iogen in_pat.txt io_rel.txt"
	string command = "./" + _exe + " " + in_pat_file + " " + io_rel_file;
	// cout << "Executing command " << command << " ..." << endl;
	system(command.c_str());

	// read from io_rel.txt
	read_out_pat(in_pat, io_rel);
}


void IO_Helper::write_in_pat (const vector<Pattern>& in_pat) const {
	ofstream fout(in_pat_file);

	fout << numPI << ' ' << in_pat.size() << '\n';

	fout << PI_name[0];
	for (unsigned i = 1; i < PI_name.size(); ++i)
		fout << ' ' << PI_name[i];
	fout << '\n';

	string buff;
	for (unsigned i = 0; i < in_pat.size(); ++i) {
		buff = in_pat[i].to_string();
		fout << buff[0];
		for (unsigned j = 1; j < buff.size(); ++j)
			fout << ' ' << buff[j];
		fout << '\n';
	}

	fout.close();
}

Pattern parse_output (const string& s) {
	int bit_count = 0;
	size_t block = 0, add = 1;
	vector<size_t> blocks;
	for (int i = numPI * 2; i < (numPI + numPO) * 2; i+= 2) {

		if (s[i] == '1') block += add;
		add <<= 1;
		++bit_count;

		if (bit_count == SIZE_T) {
			blocks.push_back(block);
			bit_count = 0;
			block = 0;
		}
	}
	if (bit_count != 0) {
		blocks.push_back(block);
	}

	return Pattern(numPO, blocks);
}

void IO_Helper::read_out_pat (const vector<Pattern>& in_pat, PatHash& io_rel) const {
	ifstream fin(io_rel_file);
	string pat;

	// ignore first two lines
	fin.ignore(numeric_limits<streamsize>::max(), '\n');
	fin.ignore(numeric_limits<streamsize>::max(), '\n');

	for (unsigned i = 0; i < in_pat.size(); ++i) {
		// get patterns with spaces from file
		getline(fin, pat);
		// add io relation to hash
		io_rel[in_pat[i]] = parse_output(pat);
	}

	fin.close();
}


void IO_Helper::write_pla( const vector<vector<string>>& pat, const vector<bool>& cube_or_comcube ) const {
	ofstream fout("top.pla");
	
	fout << ".i " << numPI << '\n';
	fout << ".o " << numPO << '\n';
	fout << ".ilb";
	for (int i = 0; i < numPI; ++i) fout << ' ' << PI_name[i];
	fout << "\n.ob";
	for (int i = 0; i < numPO; ++i) fout << ' ' << PO_name[i];

	int total_pat = 0;
	for(int i = 0; i < numPO; ++i)
		if( !cube_or_comcube[i] ) total_pat += pat[i].size();

	fout << "\n.p " << (total_pat == 0? 1 : total_pat) << '\n';
	if( total_pat == 0 ){
		fout << string( numPI, '-' ) << ' ' << string( numPO, '-' ) << "\n.e\n";
		fout.close();
		return;
	}

	for(int i = 0; i < numPO; ++i) {
		if( !cube_or_comcube[i] ) {
			string outpat_s(numPO, '-');
			outpat_s[i] = '1';
			for(size_t j = 0; j < pat[i].size(); ++j) 
				fout << pat[i][j] << ' ' << outpat_s << endl;
		}
	}
	fout << ".e\n";
	fout.close();
}


void IO_Helper::get_pla_input (const vector<Pattern>& in_pat, vector<pair<string, string> >& io_pair) const {
	write_in_pat(in_pat);

	string command = "./" + _exe + " " + in_pat_file + " " + io_rel_file;
	cout << "Executing command " << command << " ..." << endl;
	system(command.c_str());

	string pat;
	ifstream fin(io_rel_file);
	io_pair.reserve(in_pat.size());

	fin.ignore(numeric_limits<streamsize>::max(), '\n');
	fin.ignore(numeric_limits<streamsize>::max(), '\n');

	for (unsigned i = 0; i < in_pat.size(); ++i) {
		// get patterns with spaces from file
		getline(fin, pat);
		// remove all spaces
		pat.erase(std::remove(pat.begin(), pat.end(), ' '), pat.end());
		// add io relation to io_pair
		io_pair.push_back(make_pair(in_pat[i].to_string(), pat.substr(numPI)));
	}

}

