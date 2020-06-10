#ifndef ABC_WRAPPER_H
#define ABC_WRAPPER_H

#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>

#include "pattern.h"
#include "enum.h"

using namespace std;

#if defined(ABC_NAMESPACE)
namespace ABC_NAMESPACE
{
#elif defined(__cplusplus)
extern "C"
{
#endif

// procedures to start and stop the ABC framework
// (should be called before and after the ABC procedures are called)
void   Abc_Start();
void   Abc_Stop();

// procedures to get the ABC framework and execute commands in it
typedef struct Abc_Frame_t_ Abc_Frame_t;
typedef struct Abc_Obj_t_ Abc_Obj_t;

Abc_Frame_t * Abc_FrameGetGlobalFrame();
int    Cmd_CommandExecute( Abc_Frame_t * pAbc, const char * sCommand );

#if defined(ABC_NAMESPACE)
}
using namespace ABC_NAMESPACE;
#elif defined(__cplusplus)
}
#endif

extern int numPI;
extern int numPO;
extern vector<string> PI_name;
extern vector<string> PO_name;
extern chrono::time_point<chrono::high_resolution_clock> program_start_time;

// A class for instruction wrapping
class Abc_Wrapper
{
    
public:
    
    Abc_Wrapper();
    ~Abc_Wrapper();
    
    void start();
    void end();

	void exec();

	void init();
	Abc_Obj_t * get_const(bool b);
	Abc_Obj_t * get_pi(int i);
	void add(vector<string>& pats, int out);
	void add(vector<Abc_Obj_t *>& groups, vector<int>& pis, vector<string>& pats, int out);
    // fanout = {G1 (C==D), G2(E==F)}
    // pi  = {68, 69, 70, 71} // the PIs not in groups
    void add(const vector<Abc_Obj_t *>& fanout , const vector<int>& pi, const vector<string>& pats, int out);
	void add_xor(string& pat, int out_invert);
	void invert(int out);
	Abc_Obj_t * build_xor(const vector<int>& g1, const vector<int>& g2);
	Abc_Obj_t * build_equiv(const vector<int>& g1, const vector<int>& g2);
	Abc_Obj_t * build_larger_than(const vector<int>& g1, const vector<int>& g2);
	Abc_Obj_t * build_smaller_than(const vector<int>& g1, const vector<int>& g2);
	Abc_Obj_t * build_cone(const vector<Abc_Obj_t *>& fanins, int op);
	Abc_Obj_t * build_cone(const vector<int>& fanins, int op);
	vector<Abc_Obj_t *> multiply(const vector<int>& v, int m);
//	vector<Abc_Obj_t *> build_arithmetic_circuit(const vector<int>& k, const vector<int>& v, Relation rel);
//	vector<Abc_Obj_t *> build_arithmetic_circuit(const vector<Abc_Obj_t *>& v_obj, const vector<int>& v, Relation rel);
	vector<Abc_Obj_t *> build_arithmetic_circuit(const vector<Abc_Obj_t *>& v_obj, const vector<Abc_Obj_t *>& v, Relation rel);
	void assign_outputs(const vector<Abc_Obj_t *>& outputs, const vector<int>& po_idx);
	void assign_output(int out, Abc_Obj_t * fanin, bool invert);
    int get_gate_num();

    void lrg(char ** argv);
	void read(char* file);
	void read_lut(char* lut_file);
	void read_lib(char* lib_file);
	void append(char* file);
    void compress2rs();
    void balance();
    void fix_aig();
    void recadd3();
    void resyn2rs();
    void src_rw();
    void src_rs();
    void src_rws();
    void resyn();
    void resyn2();
    void resyn2a();
    void resyn3();
	void dc2();
    void compress();
    void compress2();
    void choice();
    void choice2();
    void rwsat();
    void drwsat2();
    void ANDsw_();
    void ANDfx_();
    void ANDdc3();
    void ANDdc4();
	void map();
	void ifaK(int i);
    void mfs(char* flag);
    void mfs2();
    void mfs3();
    void mfse();
    void cleanup();
    void collapse();
    void multi();
    void refactor();
    void renode();
    void rewrite();
    void rr();
    void strash();
    void sweep();
    void fraig();
    void fraig_trust();
    void fraig_store();
    void fraig_restore();
    void fraig_clean();
    void fraig_sweep();
    void dress();
    void cec();
    void iprove();
    void sat();
    void aig();
    void append();
    void sop();
    void gen();
    void logic();
    void exdc_free();
    void exdc_get();
    void exdc_set();
    void print_stats();
	void write_verilog(char* file); 
	void write_aig(char* file);
    
    void parseOutputPatternToSim() const;
	void AbcSimulation() const;
	bool check_some_outputs_acc(char* file, const vector<int>& ids);

private:
    char Command[1000];
    char *pFileName;
    Abc_Frame_t * pAbc;
};


#endif // ABC_WRAPPER_H
