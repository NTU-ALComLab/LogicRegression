#ifndef ABC__selfdefined__simMgr_h
#define ABC__selfdefined__simMgr_h


#include "base/abc/abc.h"
#include "misc/util/abc_namespaces.h"
#include <assert.h>
#include <fstream>
#include <bitset>
#include <string>
#include <vector>
#include <iostream>


ABC_NAMESPACE_HEADER_START
class SimulationMgr;

class SimulationMgr
{
public:
	SimulationMgr() {}
	~SimulationMgr(){}
	void initMgr(Abc_Ntk_t * p, char* filename);
	//read Pattern file and store it to fileString
	inline void readPatternFile() {
		std::ifstream patternFile(patternFilename);
		std::string tempPattern;
		while(getline(patternFile, tempPattern)){
			fileString.push_back(tempPattern);
		}
		patternFile.close();
	}
	inline bool patternFilereadDone(){
		return (fileString.empty());
	}
	inline void setSimulation(int pos, std::bitset<64> pat){
		assert(pos < nInputs + nOutputs + nGates);
		simulatePattern[pos-1] = pat;
	}
	inline Abc_Ntk_t * getABCNetwork(){return pNtk;}
	inline void replaceABCNetwork(Abc_Ntk_t * pNtkRes){pNtk = pNtkRes;}
	inline bool isSimlated(){return !(unSatisfiedInputPattern.empty());}
	//for Simulation
	void simulateNode();
	void readPattern();
	int  comparePatternwithSimulation(int incorrect[]);
	//dumping blif file and includes unsatisfied pattern
	void addUnsatPattern2Network();
private:
	//ABC framework
	Abc_Ntk_t * pNtk;			//current ABC framework
	int nInputs;				//input number of current AIG
	int nOutputs;				//output number of current AIG
	int nGates;					//gate number of current AIG
	//simulation
	std::vector< std::string > 		fileString;
	std::vector< std::bitset<64> > 	simulatePattern;	
	//pattern
	char* patternFilename;
	std::vector< std::bitset<64> > Input;		//the given input pattern
	std::vector< std::bitset<64> > Output;	//the given output pattern
	//output
	std::vector< std::string > unSatisfiedInputPattern;
	std::vector< std::string > unSatisfiedOutputPattern;
};


ABC_NAMESPACE_HEADER_END  
#endif
