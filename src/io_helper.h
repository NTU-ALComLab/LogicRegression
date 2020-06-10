#ifndef IO_HELPER_H
#define IO_HELPER_H

#include <iostream>
#include <string>
#include <vector>
#include "pattern.h"

using namespace std;


extern int numPI;
extern int numPO;

extern vector<string> PI_name;
extern vector<string> PO_name;

// A class managing I/O
class IO_Helper {

public:
	IO_Helper(char* info_file = NULL, char* iogen_exe = NULL, char* circuit_file = NULL);
	~IO_Helper() {}

	void init(char* info_file, char* iogen_exe, char* circuit_file);

	// parse output patterns, and adds (in_pat -> out_pat) to hash 
	void get_io_rel(const vector<Pattern>& in_pat, PatHash& io_rel) const;
	// write input patterns to file, execute command,
	void write_pla( const vector<vector<string>>& pat, const vector<bool>& cube_or_comcube) const;

	// get input/output relations for pla
	void get_pla_input(const vector<Pattern>& in_pat, vector<pair<string, string> >& io_pair) const;


private:

	char* _circuit_file;		// final circuit file name (.v)
	string _exe;				// executable file name

	// functions called by get_io_rel
	void write_in_pat(const vector<Pattern>& in_pat) const;
	void read_out_pat(const vector<Pattern>& in_pat, PatHash& io_rel) const;
};

#endif
