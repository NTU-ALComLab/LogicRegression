#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <numeric>
#include "pattern.h"
#include "inputs.h"
#include <cmath>
#include "group.h"

using namespace std;

#define FIND_CRITICAL_PAT_NUM 20


extern vector< pair< vector<Group>, vector<int> > > comcube_info;
/* extern int total_nodes; */
/* extern int node_bf; */

/* ofstream ff("record.txt"); */

Inputs::Inputs (int num_inputs) {
	// resize _in_dep, _sorted to be the size of numPI
	_in_dep.resize(num_inputs);
//	_sorted.resize(numPI);
	_1_count.resize(numPO);

	_pattern_tested = 0;

	// initialize dependencies with all bits unset
	for (int i = 0; i < num_inputs; ++i)
		_in_dep[i] = Pattern(numPO);

	// initialize _sorted i.e. _sorted[i] = i
//	iota(_sorted.begin(), _sorted.end(), 0);

	for (int i = 0; i < num_inputs; ++i)
		_changes.push_back((int*)calloc(numPO, sizeof(int)));

}

void Inputs::reset() {
    fill(_in_dep.begin(), _in_dep.end(), Pattern(numPO));
    fill(_1_count.begin(), _1_count.end(), 0);
    for (auto entry : _changes) 
        memset(entry, 0, numPO * sizeof(int));
    _pattern_tested = 0;
    _ph.clear();
}

void transform_pattern(vector<Pattern>& in_pat) {
	vector<Group>& groups = comcube_info.back().first;
	vector<int>& pis = comcube_info.back().second;
	int gsize = groups.size();

	Pattern new_pat(numPI);
	for (size_t i = 0; i < in_pat.size(); ++i) {
		for (size_t j = 0; j < groups.size(); ++j)
			groups[j].gen_pat(new_pat, in_pat[i][j]);

		for (size_t j = 0; j < pis.size(); ++j) {
			if (in_pat[i][gsize + j]) new_pat.set(pis[j]);
			else new_pat.reset(pis[j]);
		}

		in_pat[i] = new_pat;	
	}
}

int Inputs::test_dependency(vector<Pattern>& in_pat, PatHash& ph, int pat_num, int ratio, bool& is_xor, const Cube& cube, int out, bool with_group) {
	int capacity = cube.capacity();
	int changeable = capacity - cube.size();
	if (changeable == 0){
		Pattern p(capacity);
		for( int i = 0; i < capacity; ++i ){
			if( cube[i] == '1' ) p.set(i);
			else p.reset(i);
		}
		in_pat.push_back(p);

		if (with_group)
			transform_pattern(in_pat);

		ioh.get_io_rel( in_pat, ph );

		return 0;
	}
	Pattern p(capacity);

	for (int i = 0; i < pat_num; ++i) {
		p.random(ratio);
		if (i == pat_num - 1) {
			if (ratio < 20) p.set_all();
			if (ratio > 80) p.reset_all();
		}

		// create golden pattern
		if (changeable != capacity) {
			for (int j = 0; j < capacity; ++j) {
				if (cube[j] == '1') p.set(j);
				else if (cube[j] == '0') p.reset(j);
			}
		}
		in_pat.push_back(p);

		// flip each changeable bit
		for (int j = 0; j < capacity; ++j) {
			if (cube[j] == '-') {
				p.flip(j);
				in_pat.push_back(p);
				p.flip(j);
			}
		}
	}
	
	if (with_group)
		transform_pattern(in_pat);

	ioh.get_io_rel(in_pat, ph);

	// compare with golden pattern
	for (int i = 0; i < pat_num; ++i) {
		Pattern comp = in_pat[i * (changeable + 1)];
		int idx = 0;
		for (int j = 0; j < capacity; ++j) {
			if (cube[j] == '-') {
				is_xor &= update(j, ph[comp], ph[in_pat[i * (changeable + 1) + (++idx)]], out);
			}
			assert(idx <= changeable);
		}
	}
	return 0;
}

bool Inputs::update(int in, const Pattern& comp, const Pattern& out_pat, int out) {
	Dep_pat add_dep = comp ^ out_pat;
	_in_dep[in] |= add_dep;
	if (out == -1) {
		for (int k = 0; k < numPO; ++k) {
			if (add_dep[k]) ++_changes[in][k];
			if (out_pat[k]) ++_1_count[k];
		}
	}
	else {
		if (add_dep[out]) ++_changes[in][out];
		if (out_pat[out]) ++_1_count[out];
	}
	++_pattern_tested;
	return out != -1? add_dep[out] : 0;
}


vector<Pattern> Inputs::get_output_dependency () {
	vector<Pattern> out_dep;

	for (int i = 0; i < numPO; ++i) {
		out_dep.push_back( Dep_pat(numPI) );
		for (int j = 0; j < numPI; ++j) {
			if (_in_dep[j][i]) out_dep[i].set(j);
		}
	}

	return out_dep;
}

vector<int> Inputs::get_sig_in (int i) {
	vector<int> input_changes(numPI);
	for (int j = 0; j < numPI; ++j)
		input_changes[j] = _changes[j][i];

	vector<int> sig_in(numPI);
	iota(sig_in.begin(), sig_in.end(), 0);
	
	sort(sig_in.begin(), sig_in.end(),
		[=](const int x, const int y) { return input_changes[x] > input_changes[y]; });

	return sig_in;
}

vector<bool> Inputs::get_output_tendency() {
	vector<bool> tendency;
	for (auto count : _1_count) {
		tendency.push_back(count > _pattern_tested / 2);
//		cout << count / float(_pattern_tested) << ' ';
	}
	cout << '\n';
	return tendency;
}


CubeInfo Inputs::find_critical_input(const Cube& cube, int out, bool with_group) {
	vector<Pattern> in_pat;
	int capacity = cube.capacity();
	bool is_xor = true;
    CubeInfo info = {0,0,0,0,0,0};

	test_dependency(in_pat, _ph, FIND_CRITICAL_PAT_NUM, 50, is_xor, cube, out, with_group);
	in_pat.clear();
	test_dependency(in_pat, _ph, FIND_CRITICAL_PAT_NUM, 10, is_xor, cube, out, with_group);
	in_pat.clear();
	test_dependency(in_pat, _ph, FIND_CRITICAL_PAT_NUM, 90, is_xor, cube, out, with_group);
	in_pat.clear();

	if (is_xor && !with_group) {
		// create pattern with '-' changed to '0'
		Pattern p(capacity);
		for (int i = 0; i < capacity; ++i) {
			if (cube[i] == '1') p.set(i);
			else p.reset(i);
		}
		info.criticalInput = _ph[p][out];
		cout << "XOR pattern is found!!\n";
        info.isXor = 1;
		return info;
	}

	// find critical input
	for (int i = 0; i < capacity; ++i) {
		if (_changes[i][out] > _changes[info.criticalInput][out])
			info.criticalInput = i;
	}

	int output_count[2] = {0};
    int cofac_output_count[2][2] = {{}};
	// state
	for (auto out_pat : _ph) {
        ++cofac_output_count[ out_pat.first[info.criticalInput] ][ out_pat.second[out] ];
		++output_count[out_pat.second[out]];
    }
    info.cubeTendency = output_count[1] > output_count[0];
    info.negCofacTendency = cofac_output_count[0][1] > cofac_output_count[0][0];
    info.posCofacTendency = cofac_output_count[1][1] > cofac_output_count[1][0];

	int threshold, depth = cube.size();
	if( depth < 16 ) threshold = 0;
	else if( depth > 20 ) threshold = _ph.size() * 0.2;
	else threshold = ( _ph.size() * pow( 2, (depth - 16) ) / 100 );

	if (output_count[0] <= threshold || output_count[1] <= threshold)
        info.isConverged = 1;
		/* return output_count[0] <= threshold; */
	/* return output_count[0] > output_count[1]? 10 : 11; */
    return info;
}

