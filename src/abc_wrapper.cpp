#include <cstdio>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <chrono>

#include "abc_wrapper.h"
#include "enum.h"

#define IO_GEN_OUT_FILE "sim_rel.txt"
#define ABC_SIMULATION_FILE "sim_pat.txt"

using namespace std;

int my_Abc_InitCircuit ( Abc_Frame_t * pAbc, vector<string>& PI_name, vector<string>& PO_name );
int my_Abc_AddCubes ( Abc_Frame_t * pAbc, vector<string>& pats, int out );
int my_Abc_AddCubes ( Abc_Frame_t * pAbc, vector<Abc_Obj_t *>& groups, vector<int>& pis, vector<string>& pats, int out );
int my_Abc_InvertOutput ( Abc_Frame_t * pAbc, int out );
int my_Abc_AddXor( Abc_Frame_t * pAbc, string& pat, int out, bool invert );
Abc_Obj_t * my_Abc_BuildXor( Abc_Frame_t * pAbc, const vector<int>& g1, const vector<int>& g2 );
Abc_Obj_t * my_Abc_BuildEquiv( Abc_Frame_t * pAbc, const vector<int>& g1, const vector<int>& g2 );
Abc_Obj_t * my_Abc_BuildLargerThan( Abc_Frame_t * pAbc, const vector<int>& g1, const vector<int>& g2, int n );
Abc_Obj_t * my_Abc_BuildSmallerThan( Abc_Frame_t * pAbc, const vector<int>& g1, const vector<int>& g2, int n );
Abc_Obj_t * my_Abc_AddCone( Abc_Frame_t * pAbc, const vector<Abc_Obj_t *>& andVec, int op );
Abc_Obj_t * my_Abc_AddCone( Abc_Frame_t * pAbc, const vector<int>& inVec, int op );
int my_Abc_AssignPo( Abc_Frame_t * pAbc, int out, bool invert, Abc_Obj_t * fanin );
// vector<Abc_Obj_t *> my_Abc_AddArithmeticCircuit( Abc_Frame_t * pAbc, const vector<int>& k, const vector<int>& pi_idx, int opt );
// vector<Abc_Obj_t *> my_Abc_AddArithmeticCircuit( Abc_Frame_t * pAbc, const vector<Abc_Obj_t *>& nodes, const vector<int>& pi_idx, int opt );
vector<Abc_Obj_t *> my_Abc_AddArithmeticCircuit( Abc_Frame_t * pAbc, const vector<Abc_Obj_t *>& num1, const vector<Abc_Obj_t *>& num2, int opt);
Abc_Obj_t * getConst0( Abc_Frame_t * pAbc );
Abc_Obj_t * getPi( Abc_Frame_t * pAbc, int i );
int my_Abc_getGateNum( Abc_Frame_t * pAbc );

vector<float> my_Abc_SimSomeOutputs( Abc_Frame_t * pAbc, char * pFilename, const vector<int>& ids );

Abc_Wrapper::Abc_Wrapper()
{
    Abc_Start();
    pAbc = Abc_FrameGetGlobalFrame();
	Cmd_CommandExecute( pAbc, "source abc.rc" );
}

Abc_Wrapper::~Abc_Wrapper()
{ Abc_Stop(); }

void Abc_Wrapper::exec() {
	if ( Cmd_CommandExecute( pAbc, Command ) )
		fprintf (stdout, "Cannot execute command \"%s\". \n", Command );
}

void Abc_Wrapper::init()
{
	if ( my_Abc_InitCircuit( pAbc, PI_name, PO_name ) )
		fprintf (stdout, "Init circuit failed\n");
}

Abc_Obj_t * Abc_Wrapper::get_const(bool b)
{
	Abc_Obj_t * const0 = getConst0( pAbc );
	return b? (Abc_Obj_t *)((size_t)const0 ^ 01) : const0;
}

Abc_Obj_t * Abc_Wrapper::get_pi(int i)
{
	return getPi( pAbc, i );
}

int Abc_Wrapper::get_gate_num() {
    return my_Abc_getGateNum( pAbc );
}

void Abc_Wrapper::add(vector<string>& pats, int out)
{
	if ( my_Abc_AddCubes( pAbc, pats, out ) )
		fprintf (stdout, "Failed to add cubes.\n");
	dc2();
	resyn3();
}

void Abc_Wrapper::add(vector<Abc_Obj_t *>& groups, vector<int>& pis, vector<string>& pats, int out)
{
	if ( my_Abc_AddCubes( pAbc, groups, pis, pats, out ) )
		fprintf (stdout, "Failed to add cubes.\n");
	dc2();
	resyn3();
}

void Abc_Wrapper::add_xor(string& pat, int out_invert)
{
	if ( my_Abc_AddXor( pAbc, pat, out_invert >> 1, out_invert & 1) )
		fprintf (stdout, "Failed to add xor.\n");
}

void Abc_Wrapper::invert(int out)
{
	if ( my_Abc_InvertOutput( pAbc, out ) )
		fprintf(stdout, "Failed to invert output\n");
}

Abc_Obj_t * Abc_Wrapper::build_xor(const vector<int>& g1, const vector<int>& g2)
{
	return my_Abc_BuildXor( pAbc, g1, g2 );
}

Abc_Obj_t * Abc_Wrapper::build_equiv(const vector<int>& g1, const vector<int>& g2)
{
	return my_Abc_BuildEquiv( pAbc, g1, g2 );
}

Abc_Obj_t * Abc_Wrapper::build_larger_than(const vector<int>& g1, const vector<int>& g2)
{
	return my_Abc_BuildLargerThan( pAbc, g1, g2, 0 );
}

Abc_Obj_t * Abc_Wrapper::build_smaller_than(const vector<int>& g1, const vector<int>& g2)
{
	return my_Abc_BuildSmallerThan( pAbc, g1, g2, 0 );
}

Abc_Obj_t * Abc_Wrapper::build_cone(const vector<Abc_Obj_t *>& fanins, int op)
{
	return my_Abc_AddCone( pAbc, fanins, op );
}

Abc_Obj_t * Abc_Wrapper::build_cone(const vector<int>& fanins, int op)
{
	return my_Abc_AddCone( pAbc, fanins, op );
}

vector<Abc_Obj_t *> Abc_Wrapper::build_arithmetic_circuit(const vector<Abc_Obj_t *>& v_obj, const vector<Abc_Obj_t *>& v, Relation rel) {
	return my_Abc_AddArithmeticCircuit( pAbc, v_obj, v, rel );
}

vector<Abc_Obj_t *> Abc_Wrapper::multiply(const vector<int>& v, int m) {
	vector<Abc_Obj_t *> result;
	vector<Abc_Obj_t *> v_obj;

	for (size_t i = 0; i < v.size(); ++i)
		v_obj.push_back( get_pi(v[i]) );

	while (m) {
		if (m & 01) {
			result = build_arithmetic_circuit(result, v_obj, ADD);
		}
		v_obj.push_back( get_const(0) );
		m >>= 1;
	}

	return result;
}

void Abc_Wrapper::assign_outputs(const vector<Abc_Obj_t *>& outputs, const vector<int>& po_idx)
{
	assert( outputs.size() >= po_idx.size() );
	vector<Abc_Obj_t *> reversed_outputs = outputs;
	vector<int> reversed_idx = po_idx;
	reverse(reversed_outputs.begin(), reversed_outputs.end());
	reverse(reversed_idx.begin(), reversed_idx.end());

	for (size_t i = 0; i < reversed_idx.size(); ++i)
		assign_output(reversed_idx[i], reversed_outputs[i], 0);
}

void Abc_Wrapper::assign_output(int out, Abc_Obj_t * fanin, bool invert) 
{
	if ( my_Abc_AssignPo( pAbc, out, invert, fanin ) )
		fprintf(stdout, "Failed to assign output.\n");
}

void Abc_Wrapper::lrg(char** argv)
{ sprintf( Command, "lrg %s %s %s", argv[1], argv[2], argv[3]); exec(); }

void Abc_Wrapper::read(char* file)
{ sprintf( Command, "read  %s", file ); exec(); }

void Abc_Wrapper::read_lut(char* lut_file)
{ sprintf( Command, "rlut %s", lut_file ); exec(); }

void Abc_Wrapper::read_lib(char* lib_file)
{ sprintf( Command, "rlib %s", lib_file); exec(); }

void Abc_Wrapper::append(char* file)
{ sprintf( Command, "append %s", file ); exec(); }

void Abc_Wrapper::compress2rs()
{ sprintf( Command, "compress2rs" ); exec(); }

void Abc_Wrapper::balance()
{ sprintf( Command, "balance" ); exec(); }

void Abc_Wrapper::fix_aig()
{ sprintf( Command, "fix_aig" ); exec(); }

void Abc_Wrapper::recadd3()
{ sprintf( Command, "recadd3" ); exec(); }

void Abc_Wrapper::resyn2rs()
{ sprintf( Command, "resyn2rs" ); exec(); }

void Abc_Wrapper::src_rs()
{ sprintf( Command, "src_rs" ); exec(); }

void Abc_Wrapper::src_rw()
{ sprintf( Command, "src_rw" ); exec(); }

void Abc_Wrapper::src_rws()
{ sprintf( Command, "src_rws" ); exec(); }

void Abc_Wrapper::resyn()
{ sprintf( Command, "resyn" ); exec(); }

void Abc_Wrapper::resyn2()
{ sprintf( Command, "resyn2" ); exec(); }

void Abc_Wrapper::resyn2a()
{ sprintf( Command, "resyn2a" ); exec(); }

void Abc_Wrapper::resyn3()
{ sprintf( Command, "resyn3" ); exec(); }

void Abc_Wrapper::dc2()
{ sprintf( Command, "dc2" ); exec(); }

void Abc_Wrapper::compress()
{ sprintf( Command, "compress" ); exec(); }

void Abc_Wrapper::compress2()
{ sprintf( Command, "compress2" ); exec(); }

void Abc_Wrapper::choice()
{ sprintf( Command, "choice" ); exec(); }

void Abc_Wrapper::choice2()
{ sprintf( Command, "choice2" ); exec(); }

void Abc_Wrapper::rwsat()
{ sprintf( Command, "rwsat" ); exec(); }

void Abc_Wrapper::drwsat2()
{ sprintf( Command, "drwsat2" ); exec(); }

void Abc_Wrapper::ANDsw_()
{ sprintf( Command, "&sw_" ); exec(); }

void Abc_Wrapper::ANDfx_()
{ sprintf( Command, "&fx_" ); exec(); }

void Abc_Wrapper::ANDdc3()
{ sprintf( Command, "&dc3" ); exec(); }

void Abc_Wrapper::ANDdc4()
{ sprintf( Command, "&dc4" ); exec(); }

void Abc_Wrapper::ifaK(int i)
{ sprintf( Command, "if -aK %d", i ); exec(); }

void Abc_Wrapper::map()
{ sprintf( Command, "map -as" ); exec(); }

void Abc_Wrapper::mfs(char* flag)
{ sprintf( Command, "mfs %s", flag ); exec(); }

void Abc_Wrapper::mfs2()
{ sprintf( Command, "mfs2" ); exec(); }

void Abc_Wrapper::mfs3()
{ sprintf( Command, "mfs3" ); exec(); }

void Abc_Wrapper::mfse()
{ sprintf( Command, "mfse" ); exec(); }

void Abc_Wrapper::cleanup()
{ sprintf( Command, "cleanup" ); exec(); }

void Abc_Wrapper::collapse()
{ sprintf( Command, "collapse" ); exec(); }

void Abc_Wrapper::multi()
{ sprintf( Command, "multi" ); exec(); }

void Abc_Wrapper::refactor()
{ sprintf( Command, "refactor" ); exec(); }

void Abc_Wrapper::renode()
{ sprintf( Command, "renode" ); exec(); }

void Abc_Wrapper::rewrite()
{ sprintf( Command, "rewrite" ); exec(); }

void Abc_Wrapper::rr()
{ sprintf( Command, "rr" ); exec(); }

void Abc_Wrapper::strash()
{ sprintf( Command, "strash" ); exec(); }

void Abc_Wrapper::sweep()
{ sprintf( Command, "sweep" ); exec(); }

void Abc_Wrapper::fraig()
{ sprintf( Command, "fraig" ); exec(); }

void Abc_Wrapper::fraig_clean()
{ sprintf( Command, "fraig_clean" ); exec(); }

void Abc_Wrapper::fraig_store()
{ sprintf( Command, "fraig_store" ); exec(); }

void Abc_Wrapper::fraig_restore()
{ sprintf( Command, "fraig_restore" ); exec(); }

void Abc_Wrapper::fraig_sweep()
{ sprintf( Command, "fraig_sweep" ); exec(); }

void Abc_Wrapper::fraig_trust()
{ sprintf( Command, "fraig_trust" ); exec(); }

void Abc_Wrapper::dress()
{ sprintf( Command, "dress" ); exec(); }

void Abc_Wrapper::cec()
{ sprintf( Command, "cec" ); exec(); }

void Abc_Wrapper::iprove()
{ sprintf( Command, "iprove" ); exec(); }

void Abc_Wrapper::sat()
{ sprintf( Command, "sat" ); exec(); }

void Abc_Wrapper::aig()
{ sprintf( Command, "aig" ); exec(); }

void Abc_Wrapper::append()
{ sprintf( Command, "append" ); exec(); }

void Abc_Wrapper::sop()
{ sprintf( Command, "sop" ); exec(); }

void Abc_Wrapper::gen()
{ sprintf( Command, "gen" ); exec(); }

void Abc_Wrapper::logic()
{ sprintf( Command, "logic" ); exec(); }

void Abc_Wrapper::exdc_get()
{ sprintf( Command, "exdc_get" ); exec(); }

void Abc_Wrapper::exdc_set()
{ sprintf( Command, "exdc_set" ); exec(); }

void Abc_Wrapper::exdc_free()
{ sprintf( Command, "exdc_free" ); exec(); }

void Abc_Wrapper::print_stats()
{ sprintf( Command, "print_stats" ); exec(); }

void Abc_Wrapper::write_verilog(char* file)
{ sprintf( Command, "write_verilog %s", file ); exec(); }

void Abc_Wrapper::write_aig(char* file)
{ sprintf( Command, "write_aiger %s", file ); exec(); }

void Abc_Wrapper::parseOutputPatternToSim() const
{
	ifstream iogen_out( "io_rel.txt" );
	ofstream simulation( "sim_pat.txt" );
	string line;
	getline( iogen_out, line );
	getline( iogen_out, line );
	size_t pos1;

	while ( getline( iogen_out, line ) ) {
		pos1 = 0;
		for (int i = 0; i < numPI; ++i) {
			simulation << line.c_str()[pos1];
			pos1 += 2;
		}

		simulation << ' ' ;
		for (int i = 0; i < numPO; ++i) {
			simulation << line.c_str()[pos1];
			pos1 += 2;
		}
		simulation << '\n';
	}
	iogen_out.close();
	simulation.close();
	cout << "sim_pat is written out\n";
}

void Abc_Wrapper::AbcSimulation() const
{
	char cmd[128];
	sprintf( cmd, "sim_aig sim_pat.txt");
	cout << "simulation start\n";
	Cmd_CommandExecute( pAbc, "strash" );
	Cmd_CommandExecute( pAbc, cmd );
	cout << "simulation complete\n";
}

bool Abc_Wrapper::check_some_outputs_acc(char * file, const vector<int>& ids)
{
	vector<float> err_rate = my_Abc_SimSomeOutputs( pAbc, file, ids );
	bool valid = 1;
	for (size_t i = 0; i < err_rate.size(); ++i) {
		cout << "Output " << ids[i] << " error rate = " << err_rate[i] << '\n';
		if (err_rate[i] > 1e-4) valid = 0; 
	}
	return valid;
}
