#include "selfdefined.h"
#include "simMgr.h"
#include <stdio.h>
#include <iomanip>
#include "misc/util/abc_global.h"
#include "base/io/ioAbc.h"
#include "base/main/main.h"
#include "misc/bbl/bblif.h"
/* self defined function to run simulation on AIG */
/* type sim_aig pattern_file_name to toggle this command */
ABC_NAMESPACE_IMPL_START

static Abc_Frame_t *pAbc;
/* std::ofstream fout("incorrect_pat.txt"); */

/* the simulation command read by ABC framework */
int Abc_SimAig(Abc_Ntk_t * p, char* pFileName) {
	pAbc = Abc_FrameGetGlobalFrame();
	SimulationMgr Mgr;
	Mgr.initMgr(p, pFileName);
	int i = 0, unsatisfied = 0, nOutput = Abc_NtkPoNum(p);
	int incorrect[nOutput] = {0};
	Mgr.readPatternFile();
	while(!Mgr.patternFilereadDone()){
		i++;
		Mgr.readPattern();
		Mgr.simulateNode();
		unsatisfied = Mgr.comparePatternwithSimulation(incorrect);
	}
	std::cout << std::endl;
	std::cout << "simulate " << i*64 << " pattern" << std::endl;
	for (int j = 0; j < nOutput; ++j) {
		if (incorrect[j])
			std::cout << "Output " << std::setw(2) << std::right << j << " error rate: " << incorrect[j] / float(i * 64) << '\n';
	}
	std::cout << "Accuracy: " << (i * 64 - unsatisfied) / float(i*64) << '\n';
	/* Abc_Ntk_t * pNtkRes; */
	/* pNtkRes = Abc_NtkToLogic( Mgr.getABCNetwork()); */
	/* Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes ); */
	/* Mgr.replaceABCNetwork(Abc_FrameReadNtk(pAbc));  */
	/* Mgr.addUnsatPattern2Network(); */
	return 0;
}

/* the command that load unsatisfied pattern into network read by ABC framework */
int Abc_AddPatToNetwork(Abc_Ntk_t * p){
	SimulationMgr Mgr;
	if (!Mgr.isSimlated()) {
		std::cout << "Not Simulated yet" << std::endl;
		return 1;
	}
	Mgr.addUnsatPattern2Network();
	return 0;
}



/* *********************************************************** */
             /* define SimulationMgr's function */
/* *********************************************************** */
//initiailize simulation manager object
void SimulationMgr::initMgr(Abc_Ntk_t * p, char* filename){
	pNtk = p;
	nInputs = Abc_NtkPiNum(pNtk);
	nOutputs = Abc_NtkPoNum(pNtk);
	nGates = Abc_NtkNodeNum(pNtk);
	simulatePattern.resize(nInputs+nOutputs+nGates+1);
	patternFilename = filename;
}
//read pattern from pattern file
void SimulationMgr::readPattern(){
	std::vector<std::string> inputPattern;
	std::vector<std::string> outputPattern;
	std::string tempPattern;
	std::string splitPattern;
	std::bitset<64> pattern2size_t;
	/* pattern file format */
	/* inputPattern outputPattern */
	for (size_t i = 0; i < 64; i++){
		tempPattern.clear();
		splitPattern.clear();
		if(fileString.size()!=0) {
			tempPattern = fileString.back();
			fileString.pop_back();
			size_t pos = tempPattern.find(" ");
			splitPattern = tempPattern.substr(0, pos);
			inputPattern.push_back(splitPattern);
			splitPattern = tempPattern.substr(pos+1, tempPattern.size() - pos -1);
			outputPattern.push_back(splitPattern);
		}
		else break;
	}
	/* make sure that the pattern size = 64 */
	while(inputPattern.size() != 64){
		inputPattern.push_back(inputPattern.back());
		outputPattern.push_back(outputPattern.back());
	}
	/* make sure the size of the pattern is matched to the ABC current AIG */
	assert(inputPattern[0].size() == nInputs);
	assert(outputPattern[0].size() == nOutputs);
	Input.clear();
	Output.clear();
	/* convert the 64 pattern into size_t to perform parallel simulation using bitset */
	for (int i = 0; i < nInputs; i++){
		pattern2size_t.reset();
		for (size_t j = 0; j < 64; j++) {
			if(inputPattern[j][i] == '1') pattern2size_t.set(j);			
		}
		Input.push_back(pattern2size_t);
	}
	for (int i = 0; i < nOutputs; i++) {
		pattern2size_t.reset();
		for (size_t j = 0; j < 64; j++) {
			if (outputPattern[j][i] == '1') pattern2size_t.set(j);
		}
		Output.push_back(pattern2size_t);
	}
}

void SimulationMgr::simulateNode(){
	Abc_Obj_t * pNode;
	simulatePattern.clear();
	simulatePattern.resize(nInputs + nOutputs + nGates + 1);
	int i;
	std::bitset<64> const1;
	const1.set();
	simulatePattern[0] = const1;
	//ID of ABC start from 1
	Abc_NtkForEachPi( pNtk, pNode, i ){
		int Id = pNode->Id;
		simulatePattern[Id] = Input[Id-1];
	}
	Abc_NtkForEachNode( pNtk, pNode, i){
		int Id = pNode->Id;
		int fanin0ID = Abc_ObjFaninId0(pNode);
		int fanin1ID = Abc_ObjFaninId1(pNode);
		std::bitset<64> fanin0 = simulatePattern[fanin0ID];
		std::bitset<64> fanin1 = simulatePattern[fanin1ID];
		if (Abc_ObjFaninC0(pNode)) {fanin0.flip();}
		if (Abc_ObjFaninC1(pNode)) {fanin1.flip();}
		/* perform bitwise and  */
		simulatePattern[Id] = (fanin0 &= fanin1);
	}
	Abc_NtkForEachPo( pNtk, pNode, i){
		int Id = pNode->Id;
		int faninID = Abc_ObjFaninId0(pNode);
		std::bitset<64> fanin = simulatePattern[faninID];
		if (Abc_ObjFaninC0(pNode)) fanin.flip();
		simulatePattern[Id] = fanin;
	}
}

int SimulationMgr::comparePatternwithSimulation(int incorrect[]){
	/* bool isDiff[64] = {false}; */
    std::vector< std::vector<int> > isDiff(64);
	for (int i = 0; i < nOutputs; i++){
		/* perform bitwise XOR */
		/* std::bitset<64> comparison = (Output[i] ^= simulatePattern[i+nInputs+1]); */
		std::bitset<64> comparison = (Output[i] ^ simulatePattern[i+nInputs+1]);
		/* if all the bits are 0 => simulation result match the given pattern */
		if (comparison.none()) continue;
		/* else find the index that the bit = 1 => that pattern is not cover by the current network */
		else {
			for (int j = 0; j < 64; j++){
				/* if(comparison[j] == 1) isDiff[j] = true; */
                if (comparison[j] == 1) isDiff[j].push_back(i);
			}
			incorrect[i] += comparison.count();
		}
	}
	std::string input_pattern, output_pattern;
	int number = 0;
	for (int i = 0; i < 64; i++){
		/* if (isDiff[i] == true){ */
        if (!isDiff[i].empty()) {
			number++;
			for (int j = 0; j < nInputs; j++)
                input_pattern += Input[j][i]? '1':'0';
			unSatisfiedInputPattern.push_back(input_pattern);

			for (int j = 0; j < nOutputs; j++)
			    output_pattern += Output[j][i]? '1':'0';
			unSatisfiedOutputPattern.push_back(output_pattern);
/*
            for (size_t j = 0; j < isDiff[i].size(); ++j)
                fout << isDiff[i][j] << ' ';

            fout << input_pattern << ' ';
            for (size_t j = 0; j < isDiff[i].size(); ++j)
                fout << output_pattern[j];
            fout << '\n';
*/
			input_pattern.clear();
			output_pattern.clear();
		}
	}
//	std::cout << '\r' << std::flush;
//	std::cout << "finding " << unSatisfiedInputPattern.size() << " unsatisfied pattern" << std::flush;
	return unSatisfiedOutputPattern.size();
}

void SimulationMgr::addUnsatPattern2Network(){	
	Abc_Obj_t * pNode;
	Abc_Obj_t * outputFanin;
	Abc_Obj_t * newFanin;
	int i;
	std::vector<Abc_Obj_t *> invertFanin;
	size_t patternNum = unSatisfiedInputPattern.size()/nInputs;
	std::cout << "add " << patternNum << " unSatisfied pattern into current network."<< std::endl;
	for (size_t j = 0; j < patternNum; j++ ){
		Vec_Ptr_t * firstFanins = Vec_PtrAlloc(nInputs);
		Vec_Ptr_t * Fanins = Vec_PtrAlloc(2);
		Abc_NtkForEachPi( pNtk, pNode, i ){
			if (unSatisfiedInputPattern[j][i] == '0') invertFanin.push_back(pNode);
			Vec_PtrPush(firstFanins, pNode);
		}
		Abc_Obj_t * newNode = Abc_NtkCreateNodeAnd(pNtk, firstFanins);
		for (size_t k = 0; k < invertFanin.size(); k++)
			Abc_NodeComplementInput(newNode, invertFanin[k]);
		Abc_NtkForEachPo( pNtk, pNode, i ){
			if (unSatisfiedOutputPattern[j][i] == '1'){
				outputFanin = Abc_ObjFanin0( pNode );
				Abc_ObjRemoveFanins( pNode );
				Vec_PtrPushTwo(Fanins, newNode, outputFanin);
				newFanin = Abc_NtkCreateNodeOr( pNtk, Fanins);
				Abc_ObjAddFanin(pNode, newFanin);
			}
		}
		Vec_PtrFree( firstFanins );
		Vec_PtrFree( Fanins );
		invertFanin.clear();
	}
}

ABC_NAMESPACE_IMPL_END
