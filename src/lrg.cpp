#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <unordered_set>
#include <queue>
#include <cmath>
#include <iomanip>
#include <utility>

#include "abc_wrapper.h"
#include "parser.h"
#include "pattern.h"
#include "io_helper.h"
#include "inputs.h"
#include "info.h"
#include "enum.h"
#include "group.h"
#include "variable.h"
#include "simulator.h"
#include "util.h"

#define CUBES_PER_ITER 50
#define BRUTE_FORCE_DEPTH 18
#define MAX_EXPLORATION_DEPTH 25

#define entropy_testing_pattern_num_upper_bound_pow 12

#define TIME_LIMIT 2700
#define DISCOUNT   0.9
#define MAX_DEP    25

#define TEST_NUM 500000

#pragma GCC diagnostic ignored "-Wunused-result"
#pragma GCC diagnostic ignored "-Wwrite-strings"

using namespace std;

chrono::time_point<chrono::high_resolution_clock> program_start_time;
double time_limit_for_each_output;
double denom = 0;
double deadline = 0;
bool dead = false;

int c = 0;
int numPI;
int numPO;

Abc_Wrapper abc;
IO_Helper   ioh;

vector<string> PI_name;
vector<string> PO_name;

vector< vector<Variable> > vars_with_same_len;
vector< Variable > output_vars;
vector<vector<string>> 	cubes_and_comcubes_2abc; 			// First index: output_id, Second index: sequence of string
vector<bool>	cube_or_comcube;							// First index: input_id
vector< pair< vector<Group>, vector<int> > > comcube_info;	// First index: in order of input with Groups
vector< pair< vector<Group_v2>, Pattern > > math_info;

vector< Variable > all_input_vars;
vector< Variable > all_output_vars;

vector< pair< string, int > > xor_cubes;


void build_circuit_with_group () {
	
	// Build the compact tree of each output
	int comcube_info_index = 0;
	for(int i = 0; i < numPO; ++i) {
		if( cube_or_comcube[i] ) {
			vector<Abc_Obj_t *> fanout;
			for(size_t j = 0; j < comcube_info[comcube_info_index].first.size(); ++j) {
				fanout.push_back(comcube_info[comcube_info_index].first[j].get_fanout());
			}
			abc.add( fanout, comcube_info[comcube_info_index].second, cubes_and_comcubes_2abc[i], i );
			++comcube_info_index;
		}
	}
}


void build_arithmetic_circuit(const vector<Group_v2>& groups, Pattern k_pat) {
	vector<Abc_Obj_t *> v_obj;
	for (size_t j = 0; j < k_pat.size(); ++j)
		v_obj.push_back( abc.get_const(k_pat[j]) );
	
	vector<Abc_Obj_t *> multiplied;
	for (size_t j = 0; j < groups.size(); ++j) {
		multiplied = abc.multiply(groups[j].get_var()->get_pis(), groups[j].get_ratio());
		v_obj = abc.build_arithmetic_circuit(v_obj, multiplied, groups[j].get_relation());
	}

	abc.assign_outputs(v_obj, groups[0].get_output()->get_pis());
}

bool build_circuit_with_group_v2 () {
	for (size_t i = 0; i < math_info.size(); ++i) {
		assert(!math_info[i].first.empty());
		
		build_arithmetic_circuit(math_info[i].first, math_info[i].second);
	}
    return !math_info.empty();
}

int calc_gate(char * file) {
	ifstream fin(file);
	string line, gate;
	int count = 0;
	
	while (getline(fin, line)) {
		istringstream ss(line);
		ss >> gate;
		if (gate == "and"  ||
			gate == "nand" ||
			gate == "or"   ||
			gate == "nor"  ||
			gate == "xor"  ||
			gate == "xnor") ++count;
	}
	return count;
}


void sample_brute_force(Dep_pat out_dep, vector<int>& sig_in, vector<Pattern>& in_pat) {
	//	in_pat_init(in_pat);
	int dep_in_count = out_dep.count() < BRUTE_FORCE_DEPTH? out_dep.count(): BRUTE_FORCE_DEPTH; 
	int sample_size = 1 << dep_in_count;
	unordered_set<Pattern, PatternHasher> sampled_set;

	Pattern p(numPI);
	for (int i = 0; i < sample_size; ++i) {
		p.random();
		int mask = 1;
		for (int j = 0; j < dep_in_count; ++j) {
			if (i & mask) p.set(sig_in[j]);
			else p.reset(sig_in[j]);
			mask <<= 1;
		}		

		if (sampled_set.find(p & out_dep) == sampled_set.end()) {
			in_pat.push_back(p);
			sampled_set.insert(p & out_dep);
		}
	}
}

string pat2str(const Pattern& pat, const Dep_pat& dep) {
	string ret;
	for (size_t i = 0; i < pat.size(); ++i) {
		if (!dep[i]) ret += "-";
		else ret += pat[i]? "1" : "0";
	}
	return ret;
}

void add_cubes(int out, bool tendency, const Dep_pat& out_dep, const PatHash& io_rel) {
	vector<string> cubes;
	for (auto it : io_rel) {
		if (it.second[out] != tendency) {
			cubes.push_back(pat2str(it.first, out_dep));
			if (cubes.size() > CUBES_PER_ITER) {
				abc.add(cubes, out);
				cubes.clear();
			}
		}
	}
	if (cubes.size()) abc.add(cubes, out);
	if (tendency) abc.invert(out);
}

/*************** Group method ***************/

void find_all_groups( int ouput_id, vector<Group>& groups, bool isGrouped[] )
{
	for( int i=0; i<numPI; ++i ) isGrouped[i] = false;
	groups.clear();

	for( size_t i=0; i<vars_with_same_len.size(); ++i ){
		if( vars_with_same_len[i].size() == 1 ) continue;
		for( size_t j=0; j<vars_with_same_len[i].size(); ++j ){
			if( isGrouped[ vars_with_same_len[i][j][0] ] ) continue;
			for( size_t k=j+1; k<vars_with_same_len[i].size(); ++k ){
				if( isGrouped[ vars_with_same_len[i][k][0] ] ) continue;
				Group g( &(vars_with_same_len[i][j]), &(vars_with_same_len[i][k]), ouput_id );
				if( !g.find_enabler() ) continue;
				g.find_relation();
				if( g.valid() ){
					string rel;
					switch ( g.get_relation() ) {
						case EQUIVALENCE : rel = "EQUIVALENCE";  break;
						case LARGER_THAN : rel = "LARGER_THAN";  break;
						case SMALLER_THAN: rel = "SMALLER_THAN"; break;
						default: rel = "NO_RELATION"; break;
					}
                    LogWithTime("Relation \"%s\" is found\n", rel.c_str());

					for( size_t n=0; n<vars_with_same_len[i][j].size(); ++n )
						isGrouped[ vars_with_same_len[i][j][n] ] = true;
					for( size_t n=0; n<vars_with_same_len[i][k].size(); ++n )
						isGrouped[ vars_with_same_len[i][k][n] ] = true;
					groups.push_back( g );
				}
			}
		}
	}
}

bool find_all_groups_v2( int inputs_longest, int output_var_id )
{
	Pattern k = Group_v2::find_k( &( output_vars[ output_var_id ] ) );

	math_info.emplace_back();
	math_info.back().second = k;
	for( size_t i=0; i<vars_with_same_len[inputs_longest].size(); ++i ){
		Group_v2 g( &(vars_with_same_len[inputs_longest][i]), &( output_vars[ output_var_id ] ) );
		g.find_relation();
		if( g.valid() ){
			string rel;
			switch( g.get_relation() ) {
				case ADD      : rel = "ADD";     break;
				case SUBTRACT: rel = "SUBTRACT"; break;
                default: break;
			}
            LogWithTime("Relation \"%s\" is found ( %d )\n", rel.c_str(), g.get_ratio());

			math_info.back().first.push_back( g );
		}
	}
	if( math_info.back().first.empty() ){
		math_info.pop_back();
		return false;
	}

	return true;
}

/*************** Group method ***************/

string minterm2pla(Pattern& p, Dep_pat& dep) {
	string s(numPI, '-');
	for (int i = 0; i < numPI; ++i) {
		if (dep[i]) s[i] = p[i]? '1':'0';
	}
	return s;
}

void construct_cubes_FBDT( int output_id, vector<int>& dependency_rank_overall, bool output_tendency )
{
    LogWithTime("Construct network of output no. %2d with FBDT\n", output_id);

	queue< Cube > cube_queue;
	vector< string > cubes_pla; 
	int depth = 0;
    Inputs inputs;
    CubeInfo info;
	Cube cube( numPI );
    cube_queue.push( cube );

	while( !cube_queue.empty() ){
		cube = cube_queue.front();
		cube_queue.pop();

		if( cube.size() > depth )
            LogWithTime("    ..Exploring depth: %2d\n", (depth = cube.size()) );

        if (dead && cube.getTendency() != output_tendency) {
            cubes_and_comcubes_2abc.back().push_back( cube.to_string() );
            continue;
        }

        inputs.reset();
		info = inputs.find_critical_input( cube, output_id );
		
		// comment this line to disable time limit for each output
		dead = dead || (chrono::high_resolution_clock::now() - program_start_time).count() * 1e-9 - deadline > 0.0; 

		if( info.isXor ) {
			// criticalInput is fake. It indicates invert or not
			xor_cubes.push_back( make_pair(cube.to_string(), output_id * 2 + info.criticalInput) );
		}
		else if( info.isConverged ) {
			if ( info.cubeTendency != output_tendency ){
				cubes_and_comcubes_2abc.back().push_back( cube.to_string() );
			}
		}
		else if( cube.size() >= MAX_EXPLORATION_DEPTH || dead ) {
			if( info.cubeTendency != output_tendency ){
				cubes_and_comcubes_2abc.back().push_back( cube.to_string() );
			}
		}
		else{
			cube.assign( info.criticalInput, '0' );
            cube.setTendency( info.negCofacTendency );
			cube_queue.push( cube );
			cube.assign( info.criticalInput, '1' );
            cube.setTendency( info.posCofacTendency );
			cube_queue.push( cube );
		}
	}
}

bool construct_cubes_template( int output_id, vector<int>& dependency_rank_overall, int dependency_count, bool output_tendency )
{
	// Examine the dependency
	if( dependency_count == 0 ) return false;
    LogWithTime("Construct network of output no. %2d with template\n", output_id);

	vector<Group> relation_groups;
	bool input_has_grouped[numPI];
	find_all_groups( output_id, relation_groups, input_has_grouped );
	
	if( relation_groups.empty() ) {
        LogWithTime("    No groups are found in output no. %2d\n", output_id);
		return false;
	}
	

    LogWithTime("%2lu groups are found in output no. %2d\n", relation_groups.size(), output_id);

	// compact_numPI represents for 
	vector<int> input_id_alone;
	for(int i = 0; i < numPI; ++i) 
		if( !input_has_grouped[i] ) input_id_alone.push_back(i);
	int compact_numPI = input_id_alone.size() + relation_groups.size();

	// Global variable 
	comcube_info.push_back(make_pair(relation_groups, input_id_alone));
	cube_or_comcube.back() = true;

	// Start building
	queue< Compact_Cube > com_cube_queue;
	int depth = 0;
    CubeInfo info;
	Compact_Cube com_cube( compact_numPI );
	com_cube_queue.push( com_cube );
    Inputs inputs(com_cube.capacity());

	while( !com_cube_queue.empty() ) {
		com_cube = com_cube_queue.front();
		com_cube_queue.pop();
		
		if( com_cube.size() > depth )
            LogWithTime("    ..Exploring depth: %2d\n", (depth = com_cube.size()) );

        inputs.reset();
		info = inputs.find_critical_input(com_cube, output_id, 1);
	

		if (info.isConverged) {
			if (info.cubeTendency != output_tendency) {
				cubes_and_comcubes_2abc.back().push_back( com_cube.to_string() );
			}
		}
		else if( com_cube.size() >= MAX_EXPLORATION_DEPTH ) {
			if( info.cubeTendency != output_tendency ) {
				cubes_and_comcubes_2abc.back().push_back( com_cube.to_string() );
			}
		}
		else {
			com_cube.assign( info.criticalInput, '0' );
            com_cube.setTendency( info.negCofacTendency );
			com_cube_queue.push( com_cube );
			com_cube.assign( info.criticalInput, '1' );
            com_cube.setTendency( info.posCofacTendency );
			com_cube_queue.push( com_cube );
		}
	}

    return true;
}

void calc_time_for_output(vector<Dep_pat>& out_dep, int out) {
    float elapsed_time = getElapsedTime( program_start_time );
	float time_left = TIME_LIMIT * 0.9 - elapsed_time;

	deadline = elapsed_time + time_left * pow(2, out_dep[out].count() / 64) / denom * DISCOUNT;
	denom -= pow(2, min(out_dep[out].count(), MAX_DEP) / 64);
}

void optimize(int iter) {
	abc.strash();
	for (int i = 0; i < iter; ++i) {
		int r = rand() % 10;
		if (r < 8) {
			abc.dc2();
			abc.resyn3();
			abc.rewrite();
		}
		else abc.compress2rs();
		abc.print_stats();
	}
}

int main(int argc, char** argv) {
//	srand(time(0));
	program_start_time = chrono::high_resolution_clock::now();

	// check input correctness
	if (argc != 4 ) {
		cout << "Usage: lrg io_info.txt iogen circuit.v" << endl;
		return 0;
	}

	ioh.init(argv[1], argv[2], argv[3]);
	bool po_done[numPO] = {};

	Inputs inputs;
	// Parse groups
	Parser pi_parser;
	pi_parser.find_vars();
	Simulator simulator;


	vector<Pattern> in_pat;
	PatHash io_rel;

	if (!output_vars.empty() && !vars_with_same_len.empty()) {
		abc.init();
		// find variables with largest length
		size_t max_len = vars_with_same_len[0][0].size(), idx = 0;
		for (size_t i = 1; i < vars_with_same_len.size(); ++i) {
			if (max_len < vars_with_same_len[i][0].size()) {
				max_len = vars_with_same_len[i][0].size();
				idx = i;
			}
		}

		for (size_t i = 0; i < output_vars.size(); ++i) {
			// test groups
			if ( !find_all_groups_v2( idx, i ) ) continue;

			// build circuit
			vector<Group_v2>& groups = math_info.back().first;
			Pattern k_pat = math_info.back().second;
			assert(!groups.empty());

			build_arithmetic_circuit(groups, k_pat);	

			// simulation
			if ( simulator.simulate_some_outputs(groups[0].get_output()->get_pis(), 100000, 50, in_pat, io_rel) ) {
				const vector<int> pins = groups[0].get_output()->get_pis();
				for (size_t i = 0; i < pins.size(); ++i)
					po_done[pins[i]] = true;
				continue;
			}

			math_info.pop_back();
		}
	}


	int sample_iter = 40;
	int rand_sample_num = 50;
	bool dummy;
	// test depedency
	for (int i = 0; i < sample_iter * 3; ++i) {
		cout << "Sampling... Iteration: " << i+1 << flush  << '\r';
		int rand1 = 50;
		if (i < sample_iter) rand1 = 10;
		else if (i < 2 * sample_iter) rand1 = 90;
		
		inputs.test_dependency(in_pat, io_rel, rand_sample_num, rand1, dummy);

		in_pat.clear();
		io_rel.clear();
	}

	cout << '\n';

	vector<Dep_pat> out_dep = inputs.get_output_dependency();

	// out_dep log
/*	
	for (size_t i = 0; i < out_dep.size(); ++i) {
		cout << out_dep[i] << ' ' << out_dep[i].count() << endl;
	}
*/
    sort( all_input_vars.begin(), all_input_vars.end() );
    sort( all_output_vars.begin(), all_output_vars.end() );
    for (size_t i = 0; i < all_input_vars.size(); ++i)
        cout << all_input_vars[i].get_name() << ' ';
    cout << '\n';
    for (size_t i = 0; i < all_output_vars.size(); ++i) {
        for (size_t j = 0; j < all_output_vars[i].size(); ++j) {
            cout << setw(16) << trim(PO_name[ all_output_vars[i][j] ], "PO_") << ": ";
            string s = out_dep[ all_output_vars[i][j] ].to_string();
            for (size_t k = 0; k < all_input_vars.size(); ++k) {
                for (size_t l = 0; l < all_input_vars[k].size(); ++l) {
                    cout << s[ all_input_vars[k][l] ];
                }
                cout << ' ';
            }
            cout << out_dep[ all_output_vars[i][j] ].count() << '\n';
        }
    }

	// calculate denominator for time factor
	for (int i = 0; i < numPO; ++i) {
		if (!po_done[i] && out_dep[i].count() > BRUTE_FORCE_DEPTH)
			denom += pow(2, min(out_dep[i].count(), MAX_DEP) / 64);
	}




	vector<bool> output_tendency = inputs.get_output_tendency();
//	cout << "output tendency:"; for (auto i : output_tendency) cout << ' ' << i; cout << endl;

	for( int i = 0; i < numPO; ++i ) {
		cube_or_comcube.push_back(false);
		cubes_and_comcubes_2abc.push_back(vector<string>());

		if (po_done[i]) continue;

		auto start = chrono::high_resolution_clock::now();

		vector<int> dependency_rank = inputs.get_sig_in(i);
		int dependency_count = out_dep[i].count();
		if (dependency_count <= BRUTE_FORCE_DEPTH) {
            LogWithTime("Construct network of output no. %2d with brute force (dep count = %2d)\n", i, dependency_count);
			vector<int> sig_in = inputs.get_sig_in(i);
	 		out_dep[i].reset_all();
 			for (int k = 0; k < dependency_count; ++k)
 				out_dep[i].set(sig_in[k]);

			sample_brute_force(out_dep[i], sig_in, in_pat);
			ioh.get_io_rel(in_pat, io_rel);


			for (auto pat : in_pat) 
				if (io_rel[pat][i] != output_tendency[i]) 
					cubes_and_comcubes_2abc.back().push_back( minterm2pla(pat, out_dep[i]) );

			io_rel.clear();
			in_pat.clear();
		}
		else {
			dead = false;
			calc_time_for_output(out_dep, i);
            bool fTemplate = true;
            bool success = false;
            if ( fTemplate ) success = construct_cubes_template( i, dependency_rank, dependency_count, output_tendency[i] );
            if ( !success  ) construct_cubes_FBDT( i, dependency_rank, output_tendency[i] );
			if( cube_or_comcube.back() ) {
			/* SIMULATE SINGLE OUTPUT CIRCUIT */ 
				vector<Abc_Obj_t *> fanout;
				for(size_t j = 0; j < comcube_info.back().first.size(); ++j)
					fanout.push_back(comcube_info.back().first[j].get_fanout());
				
				abc.add( fanout, comcube_info.back().second, cubes_and_comcubes_2abc[i], i);
				if (output_tendency[i]) abc.invert(i);
				
				vector<int> testing_output({i});
				if( simulator.simulate_some_outputs(testing_output, 50000, 50, in_pat, io_rel) ) continue;
				cubes_and_comcubes_2abc.back().clear();
				cube_or_comcube.back() = false;
				comcube_info.pop_back();
				construct_cubes_FBDT( i, dependency_rank, output_tendency[i] );
			}
			if (dead) {
				LogWithTime("timeout!!!!!!!!!!!! @ output %d\n", i);
			}
		}
        LogWithTime("Construction of output no. %2d took %.3f sec\n", i, getElapsedTime(start));
		
	}


    /****** Build Circuit ******/

	char lut_file[32] = "my.lut";
	char lib_file[32] = "my.genlib";
	char pla_file[32] = "top.pla";
    char pre_opt_file[32] = "pre_opt.v";
    char inter_opt_file[32] = "inter_opt.v";
	int iter = 8;
	abc.read_lut(lut_file);
	abc.read_lib(lib_file);
	abc.init();

	ioh.write_pla( cubes_and_comcubes_2abc, cube_or_comcube );
	abc.read( pla_file );
	abc.strash();
	build_circuit_with_group();

	for (size_t i = 0; i < xor_cubes.size(); ++i) {
		pair< string, int > cube = xor_cubes[i];
		cout << xor_cubes[i].first << ' ' << xor_cubes[i].second << '\n';
		abc.add_xor(cube.first, cube.second ^ output_tendency[cube.second >> 1]);
	}
	for (int i = 0; i < numPO; ++i)
		if (output_tendency[i]) abc.invert(i);

	bool use_group_method = build_circuit_with_group_v2();

	abc.ifaK(2);
	abc.mfs("-rdM 500");

	for (int i = 0; i < numPO; ++i)
		if (cube_or_comcube[i])
			use_group_method = true;

    abc.map();
    abc.write_verilog(pre_opt_file);
    if (!use_group_method) {
        abc.collapse();
        abc.strash();
        abc.map();
        abc.write_verilog(inter_opt_file);
    }
    abc.strash();

    int curr_gate_cnt = 0, last_gate_cnt = abc.get_gate_num();
    auto start_opt_time = chrono::high_resolution_clock::now();
    while ( getElapsedTime( start_opt_time ) < 60 )  {
        optimize(iter);
        optimize(iter);
        optimize(iter);
        abc.strash();
        curr_gate_cnt = abc.get_gate_num();
        if (curr_gate_cnt >= last_gate_cnt)
            break;
        last_gate_cnt = curr_gate_cnt;
    }
	abc.map();
	abc.write_verilog(argv[3]);

	int gate_count = calc_gate(argv[3]);

    int pre_opt_count = calc_gate(pre_opt_file);
    if (pre_opt_count < gate_count) {
        string cmd;
        cmd = "mv " + string(pre_opt_file) + " " + string(argv[3]);
        system(cmd.c_str());
        gate_count = pre_opt_count;
    }
    if (!use_group_method) {
        int inter_opt_count = calc_gate(inter_opt_file);
        if (inter_opt_count < gate_count) {
            string cmd = "mv " + string(inter_opt_file) + " " + string(argv[3]);
            system(cmd.c_str());
            gate_count = inter_opt_count;
        }
    }

	cout << "# of gates: " << gate_count << '\n';
	cout << "Time taken: " << getElapsedTime( program_start_time ) << '\n';
	
	simulator.simulate_circuit(TEST_NUM, 50, in_pat, io_rel);
//	simulator.simulate_circuit(TEST_NUM, 10, in_pat, io_rel);
//	simulator.simulate_circuit(TEST_NUM, 90, in_pat, io_rel);

	return 0;
}

