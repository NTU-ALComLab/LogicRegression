#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>
#include <unordered_set>

#include "pattern.h"
#include "abc_wrapper.h"
#include "io_helper.h"

#define SIM_FILE "sim_pat.txt"

#pragma GCC diagnostic ignored "-Wwrite-strings"

extern int numPI;
extern IO_Helper ioh;
extern Abc_Wrapper abc;

class Simulator {
public:
	void simulate_circuit(int num_pat, int ratio, vector<Pattern>& in_pat, PatHash& ph);
	bool simulate_some_outputs(const vector<int>& ids, int num_pat, int ratio, vector<Pattern>& in_pat, PatHash& ph);

private:
};


void Simulator::simulate_circuit(int num_pat, int ratio, vector<Pattern>& in_pat, PatHash& ph) {
	if      (ratio > 80) cout << "\n=========== HIGH 1 ============\n";
	else if (ratio < 20) cout << "\n=========== HIGH 0 ============\n";
	else                 cout << "\n=========== RANDOM ============\n";

	Pattern sim_pat(numPI);
	in_pat.clear();
	ph.clear();
	
	for (int i = 0; i < num_pat; ++i)
		in_pat.push_back(sim_pat.random(ratio));

	ioh.get_io_rel(in_pat, ph);	
	abc.parseOutputPatternToSim();
	abc.AbcSimulation();

	cout << "===============================\n";

	in_pat.clear();
	ph.clear();
}

bool Simulator::simulate_some_outputs(const vector<int>& ids, int num_pat, int ratio, vector<Pattern>& in_pat, PatHash& ph) {
	Pattern sim_pat(numPI);
	in_pat.clear();
	ph.clear();

	for (int i = 0; i < num_pat; ++i)
		in_pat.push_back(sim_pat.random(ratio));

	ioh.get_io_rel(in_pat, ph);	
	abc.parseOutputPatternToSim();
	
	in_pat.clear();
	ph.clear();
	
	return abc.check_some_outputs_acc(SIM_FILE, ids);
}


#endif // SIMULATOR_H
