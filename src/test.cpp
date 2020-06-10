#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <unordered_map>

#include "pattern.h"
#include "io_helper.h"
#include "abc_wrapper.h"
#include "simulator.h"

#define TEST_NUM 500000

using namespace std;

chrono::time_point<chrono::high_resolution_clock> program_start_time;

int numPI;
int numPO;
Abc_Wrapper abc;
IO_Helper   ioh;

vector<string> PI_name;
vector<string> PO_name;

int main(int argc, char ** argv) {
   ioh.init( argv[1], argv[2], argv[3] );
   abc.read( argv[3] );
   abc.strash();

   char cmd[64];
   sprintf(cmd, "sh count.sh %s", argv[3]);
   if (system( cmd )) exit(1);

   vector<Pattern> in_pat;
   PatHash io_rel;
   Simulator simulator;
   
   simulator.simulate_circuit(TEST_NUM, 50, in_pat, io_rel);
   simulator.simulate_circuit(TEST_NUM, 10, in_pat, io_rel);
   simulator.simulate_circuit(TEST_NUM, 90, in_pat, io_rel);
}
