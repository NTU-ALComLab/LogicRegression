#ifndef INPUTS_H
#define INPUTS_H

#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "pattern.h"
#include "io_helper.h"
#include "info.h"

using namespace std;


extern int numPI;
extern int numPO;
extern IO_Helper ioh;

// A class for inputs handling
class Inputs {

public:
	Inputs(int num_inputs = numPI);
	~Inputs () { for (size_t i = 0; i < _changes.size(); ++i) free(_changes[i]); }

	int test_dependency(vector<Pattern>& in_pat, PatHash& ph, int pat_num, int ratio, bool& is_xor, const Cube& cube = Cube(numPI), int out = -1, bool with_group = 0);

	// update dependency of input i with add_dep
	bool update (int in, const Pattern& comp, const Pattern& out_pat, int out = -1);
 
	// compare function used by sort_by_count
//	bool compare (const int x, const int y);

	// sort _sorted in an ascending order (by # of set bits of input dependency)
//	void sort_by_count ();

	// size of inputs
	size_t size(){ return _in_dep.size(); }

	vector<Pattern> get_output_dependency();
	vector<int> get_sig_in(int i);

	vector<bool> get_output_tendency();
	CubeInfo find_critical_input(const Cube& cube, int out, bool with_group = 0);
    void reset();
	// overload [] to get input dependency of i-th input
	Dep_pat& operator [] (size_t i) { return _in_dep[i]; }
	const Dep_pat& operator [] (size_t i) const { return _in_dep[i]; } 

	void print() {
		for (int i = 0; i < numPO; ++i) {
			for (int j = 0; j < numPI; ++j)
				cout << _changes[j][i] << ' ';
			cout << '\n';
		}
	}


private:
	vector<Dep_pat> _in_dep;	// input depedencies, each with length of numPO
//	vector<int> _sorted;		// indexes of input dependencies sorted by # of set bits
	vector<int*> _changes;			// record # of changes of inputs that induce changes of outputs
	vector<int> _1_count;
	int _pattern_tested;

    PatHash _ph;
};

#endif // INPUTS_H
