#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <algorithm>
#include "selfdefined.h"
#include "simMgr.h"
#include "base/main/main.h"
#include "base/cmd/cmdInt.h"

#define make_or(n0, n1)  Abc_AigOr( (Abc_Aig_t *)pNtk->pManFunc, n0, n1 )
#define make_and(n0, n1) Abc_AigAnd( (Abc_Aig_t *)pNtk->pManFunc, n0, n1 )
#define make_xor(n0, n1) Abc_AigXor( (Abc_Aig_t *)pNtk->pManFunc, n0, n1 )

Abc_Obj_t * getConst0( Abc_Frame_t * pAbc ) {
	Abc_Ntk_t * pNtk = Abc_FrameReadNtk( pAbc );
	Abc_Obj_t * pi0 = Abc_NtkPi( pNtk, 0 );
	return make_and( pi0, Abc_ObjNot(pi0) );
}

Abc_Obj_t * getPi( Abc_Frame_t * pAbc, int i ) {	
	Abc_Ntk_t * pNtk = Abc_FrameReadNtk( pAbc );
	assert( i < Abc_NtkPiNum( pNtk ) );
	return Abc_NtkPi( pNtk, i );
}

int my_Abc_InitCircuit(Abc_Frame_t * pAbc, std::vector<std::string>& PI_name, std::vector<std::string>& PO_name)
{
	printf("Initializing circuit ...\n");
	Abc_Ntk_t * pNtk;
	Abc_Obj_t * pObj, * pConst0;
//	Abc_Aig_t * pMan;

	pNtk = Abc_NtkAlloc( ABC_NTK_STRASH, ABC_FUNC_AIG, 1 );
	pNtk->pName = Extra_UtilStrsav( "top" );
	pNtk->nConstrs = 0;
//	pMan = (Abc_Aig_t *)pNtk->pManFunc;
//	pConst0 = Abc_ObjNot( Abc_AigConst1( pMan->pNtkAig ) );

	for (size_t i = 0; i < PI_name.size(); ++i) {
		pObj = Abc_NtkCreatePi( pNtk );
		Abc_ObjAssignName( pObj, const_cast<char*>(PI_name[i].c_str()), NULL );
		pObj->pCopy = (Abc_Obj_t *)Abc_ObjName(pObj);
	}

	pConst0 = make_and( Abc_NtkPi(pNtk, 0), Abc_ObjNot(Abc_NtkPi(pNtk, 0)) );
	//	printf("pConst0 = %p\n", pConst0);

	for (size_t i = 0; i < PO_name.size(); ++i) {
		pObj = Abc_NtkCreatePo( pNtk );
		// initialize all POs to const0
		Abc_ObjAddFanin( pObj, pConst0 );
//	    Abc_ObjAddFanin( pObj, Abc_AigAnd( (Abc_Aig_t *)pNtk->pManFunc, Abc_NtkPi(pNtk, 0), Abc_NtkPi(pNtk, 1) ) );
		Abc_ObjAssignName( pObj, const_cast<char*>(PO_name[i].c_str()), NULL );
		pObj->pCopy = (Abc_Obj_t *)Abc_ObjName(pObj);
	}


	
	Abc_NtkDoCheck( pNtk );
	Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
	Abc_FrameClearVerifStatus( pAbc );

//	printf("LEV = %d\n", Abc_AigLevel( pNtk ));
	return pNtk == NULL;
}


// Uses two input gates to connect all inputs, return single output
Abc_Obj_t * add_cone( Abc_Ntk_t * pNtk, Abc_Obj_t * (*gateType)(Abc_Aig_t *, Abc_Obj_t *, Abc_Obj_t *), Vec_Ptr_t * fanins )
{	
	assert(gateType != NULL);
	int i, size = 0;
	Abc_Obj_t * pTemp, * pNode0, * pNode1;

	while ((size = Vec_PtrSize( fanins )) != 1) {
		if (size & 1) pTemp = (Abc_Obj_t *)Vec_PtrPop( fanins );

		Vec_PtrForEachEntryDouble( Abc_Obj_t *, Abc_Obj_t *, fanins, pNode0, pNode1, i ) {
			Vec_PtrWriteEntry( fanins, i >> 1, gateType( (Abc_Aig_t *)pNtk->pManFunc, pNode0, pNode1 ) );
		}

		fanins->nSize = i >> 1;
		if (size & 1) 
			Vec_PtrPush( fanins, pTemp );
	}
	return (Abc_Obj_t *)Vec_PtrEntry( fanins, 0 );
}


int my_Abc_AddCubes(Abc_Frame_t * pAbc, std::vector<std::string>& pats, int out )
{
	Abc_Ntk_t * pNtk = Abc_FrameReadNtk( pAbc );
	Abc_Obj_t * Pi, * Po, * newNode, * oldFanin, * newFanin;
	Vec_Ptr_t * depFanins = Vec_PtrAlloc( 25 );
	Vec_Ptr_t * orFanins = Vec_PtrAlloc( pats.size() );
	
	for (size_t k = 0; k < pats.size(); ++k) {
		for (size_t i = 0; i < pats[k].size(); ++i) {
			// neglect don't care inputs
			if (pats[k][i] == '-') continue;
			Pi = Abc_NtkPi( pNtk, i );
			Vec_PtrPush( depFanins, (pats[k][i] == '1'? Pi: Abc_ObjNot(Pi)) );
		}

		newNode = add_cone( pNtk, Abc_AigAnd, depFanins );
		Vec_PtrPush( orFanins, newNode );
		Vec_PtrClear( depFanins );
	}

	Po = Abc_NtkPo( pNtk, out );
	// remember original fanin of Po
	oldFanin = Abc_ObjChild0( Po );
	// remove original fanin
	Abc_ObjRemoveFanins( Po );
	Vec_PtrPush( orFanins, oldFanin );
	// new fanin of Po =  OR(newNode, oldFanin)
	newFanin = add_cone( pNtk, Abc_AigOr, orFanins );
	Abc_ObjAddFanin( Po, newFanin );

	Vec_PtrFree( depFanins );
	Vec_PtrFree( orFanins );

	return 0;
}

int my_Abc_AddCubes(Abc_Frame_t * pAbc, std::vector<Abc_Obj_t *>& groups, std::vector<int>& pis, std::vector<std::string>& pats, int out) {
	assert(pats.size() == 0 || groups.size() + pis.size() == pats[0].size());
	Abc_Ntk_t * pNtk = Abc_FrameReadNtk( pAbc );
	Abc_Obj_t * fanin, * andNode, * Po, * oldFanin, * newFanin;
	Vec_Ptr_t * depFanins = Vec_PtrAlloc( 25 );
	Vec_Ptr_t * orFanins = Vec_PtrAlloc( pats.size() );
	size_t num_groups = groups.size();	

	for (size_t k = 0; k < pats.size(); ++k) {
		for (size_t i = 0; i < pats[k].size(); ++i) {
			// neglect don't care inputs
			if (pats[k][i] == '-') continue;
			fanin = (i < num_groups)? groups[i] : Abc_NtkPi( pNtk, pis[i - num_groups] );
			Vec_PtrPush( depFanins, (pats[k][i] == '1'? fanin: Abc_ObjNot(fanin)) );
		}

		andNode = add_cone( pNtk, Abc_AigAnd, depFanins );
		Vec_PtrPush( orFanins, andNode );
		Vec_PtrClear( depFanins );
	}
	
	Po = Abc_NtkPo( pNtk, out );
	// remember original fanin of Po
	oldFanin = Abc_ObjChild0( Po );
	// remove original fanin
	Abc_ObjRemoveFanins( Po );
	Vec_PtrPush( orFanins, oldFanin );
	// new fanin of Po =  OR(newNode, oldFanin)
	newFanin = add_cone( pNtk, Abc_AigOr, orFanins );
	Abc_ObjAddFanin( Po, newFanin );

	Vec_PtrFree( depFanins );
	Vec_PtrFree( orFanins );

	return 0;
}

int my_Abc_InvertOutput(Abc_Frame_t * pAbc, int out)
{
	Abc_Ntk_t * pNtk = Abc_FrameReadNtk( pAbc );
	Abc_Obj_t * Po, * oldFanin, * invertedFanin;

	Po = Abc_NtkPo( pNtk, out );
	oldFanin = Abc_ObjChild0( Po );
	Abc_ObjRemoveFanins( Po );
	invertedFanin = Abc_ObjNot( oldFanin );
	Abc_ObjAddFanin( Po, invertedFanin );

	return 0;
}

int my_Abc_AddXor(Abc_Frame_t * pAbc, std::string& pat, int out, bool invert) {
	Abc_Ntk_t * pNtk = Abc_FrameReadNtk( pAbc );
	Abc_Obj_t * Pi, * Po, * xorNode, * andNode, * oldFanin, * newFanin;
	Vec_Ptr_t * xorFanins = Vec_PtrAlloc( pat.size() );
	Vec_Ptr_t * andFanins = Vec_PtrAlloc( pat.size() );

	for (size_t i = 0; i < pat.size(); ++i) {
		Pi = Abc_NtkPi( pNtk, i );
		if (pat[i] == '-') Vec_PtrPush( xorFanins, Pi );
		else Vec_PtrPush( andFanins, (pat[i] == '1'? Pi: Abc_ObjNot(Pi)) );
	}

	xorNode = add_cone( pNtk, Abc_AigXor, xorFanins );
	Vec_PtrPush( andFanins, xorNode );
	andNode = Abc_ObjNotCond( add_cone( pNtk, Abc_AigAnd, andFanins ), invert );

	Po = Abc_NtkPo( pNtk, out );
	oldFanin = Abc_ObjChild0( Po );
	Abc_ObjRemoveFanins( Po );
	
	newFanin = make_or( oldFanin, andNode );
	Abc_ObjAddFanin( Po, newFanin );
//	Abc_ObjAddFanin( Po, andNode );

	Vec_PtrFree( xorFanins );
	Vec_PtrFree( andFanins );
	return 0;
}

Abc_Obj_t * my_Abc_BuildXor( Abc_Frame_t * pAbc, const std::vector<int>& g1, const std::vector<int>& g2 ) {
	assert( g1.size() == g2.size() );
	Abc_Ntk_t * pNtk = Abc_FrameReadNtk( pAbc );
	Abc_Obj_t * Pi_1, * Pi_2, * xorNode, * andNode;
	Vec_Ptr_t * andFanins = Vec_PtrAlloc( g1.size() );

	for (size_t i = 0; i < g1.size(); ++i) {
		Pi_1 = Abc_NtkPi( pNtk, g1[i] );
		Pi_2 = Abc_NtkPi( pNtk, g2[i] );
		xorNode = make_xor( Pi_1, Pi_2 );
		Vec_PtrPush( andFanins, xorNode );
	}

	andNode = add_cone( pNtk, Abc_AigAnd, andFanins );
	return andNode;
}

Abc_Obj_t * my_Abc_BuildEquiv( Abc_Frame_t * pAbc, const std::vector<int>& g1, const std::vector<int>& g2 ) {
	assert( g1.size() == g2.size() );
	Abc_Ntk_t * pNtk = Abc_FrameReadNtk( pAbc );
	Abc_Obj_t * Pi_1, * Pi_2, * equivNode, * andNode;
	Vec_Ptr_t * andFanins = Vec_PtrAlloc( g1.size() );

	for (size_t i = 0; i < g1.size(); ++i) {
		Pi_1 = Abc_NtkPi( pNtk, g1[i] );
		Pi_2 = Abc_NtkPi( pNtk, g2[i] );
		equivNode = Abc_ObjNot( make_xor( Pi_1, Pi_2 ) );
		Vec_PtrPush( andFanins, equivNode );
	}

	andNode = add_cone( pNtk, Abc_AigAnd, andFanins );
	return andNode;
}

Abc_Obj_t * my_Abc_BuildLargerThan( Abc_Frame_t * pAbc, const std::vector<int>& g1, const std::vector<int>& g2, int n ) {
	assert( g1.size() == g2.size() );
	Abc_Ntk_t * pNtk = Abc_FrameReadNtk( pAbc );
	Abc_Obj_t * Pi_1, * Pi_2, * largerNode, * equivNode, * equivClause;
	Pi_1 = Abc_NtkPi( pNtk, g1[n] );
	Pi_2 = Abc_NtkPi( pNtk, g2[n] );

	largerNode = make_and( Pi_1, Abc_ObjNot(Pi_2) );
	if (n == g1.size() - 1)
		return largerNode;

	equivNode = Abc_ObjNot( make_xor( Pi_1, Pi_2 ) );
	equivClause =  make_and( equivNode, my_Abc_BuildLargerThan(pAbc, g1, g2, ++n) );

	return make_or( largerNode, equivClause );
}

Abc_Obj_t * my_Abc_BuildSmallerThan( Abc_Frame_t * pAbc, const std::vector<int>& g1, const std::vector<int>& g2, int n ) {
	assert( g1.size() == g2.size() );
	Abc_Ntk_t * pNtk = Abc_FrameReadNtk( pAbc );
	Abc_Obj_t * Pi_1, * Pi_2, * smallerNode, * equivNode, * equivClause;
	Pi_1 = Abc_NtkPi( pNtk, g1[n] );
	Pi_2 = Abc_NtkPi( pNtk, g2[n] );

	smallerNode = make_and( Abc_ObjNot(Pi_1), Pi_2 );
	if (n == g1.size() - 1)
		return smallerNode;

	equivNode = Abc_ObjNot( make_xor( Pi_1, Pi_2 ) );
	equivClause =  make_and( equivNode, my_Abc_BuildSmallerThan(pAbc, g1, g2, ++n) );

	return make_or( smallerNode, equivClause );
}

Abc_Obj_t * my_Abc_AddCone( Abc_Frame_t * pAbc, const std::vector<int>& inVec, int op ) {
	Abc_Ntk_t * pNtk = Abc_FrameReadNtk( pAbc );
	Vec_Ptr_t * fanins = Vec_PtrAlloc( inVec.size() );

	for (size_t i = 0; i < inVec.size(); ++i)
		Vec_PtrPush( fanins, Abc_NtkPi(pNtk, inVec[i]) );

	Abc_Obj_t * (*func)(Abc_Aig_t *, Abc_Obj_t *, Abc_Obj_t *);
	switch (op) {
		case  1: func = Abc_AigAnd; break;
		case  2: func = Abc_AigOr;  break;
		case  3: func = Abc_AigXor; break;
		default: func = NULL;       break;
	}
	return add_cone( pNtk, func, fanins );
}

Abc_Obj_t * my_Abc_AddCone( Abc_Frame_t * pAbc, const std::vector<Abc_Obj_t *>& andVec, int op ) {
	Abc_Ntk_t * pNtk = Abc_FrameReadNtk( pAbc );
	Vec_Ptr_t * fanins = Vec_PtrAlloc( andVec.size() );

	for (size_t i = 0; i < andVec.size(); ++i)
		Vec_PtrPush( fanins, andVec[i] );

	Abc_Obj_t * (*func)(Abc_Aig_t *, Abc_Obj_t *, Abc_Obj_t *);
	switch (op) {
		case  1: func = Abc_AigAnd; break;
		case  2: func = Abc_AigOr;  break;
		case  3: func = Abc_AigXor; break;
		default: func = NULL;       break;
	}
	return add_cone( pNtk, func, fanins );
}

int my_Abc_AssignPo( Abc_Frame_t * pAbc, int out, bool invert, Abc_Obj_t * fanin ) {
	Abc_Ntk_t * pNtk = Abc_FrameReadNtk( pAbc );
	Abc_Obj_t * Po, * oldFanin;

	Po = Abc_NtkPo( pNtk, out );
	oldFanin = Abc_ObjChild0( Po );
	Abc_ObjRemoveFanins( Po );

	Abc_ObjAddFanin( Po, Abc_ObjNotCond( fanin, invert ) );
	return 0;
}

// returned vector length = max( a.size(), b.size() ) + 1
std::vector<Abc_Obj_t *> my_Abc_AddFullAdder( Abc_Frame_t * pAbc, std::vector<Abc_Obj_t *>& a, std::vector<Abc_Obj_t *>& b ) {
	Abc_Ntk_t * pNtk = Abc_FrameReadNtk( pAbc );
	Abc_Obj_t * pConst0 = getConst0( pAbc );
	Abc_Obj_t * sum, * carry = pConst0, * a_xor_b, * a_and_b, * a_xor_b_and_c;
	std::vector<Abc_Obj_t *> ret;

	// a is the longer one, b fills const0 to be same length as a
	if (a.size() < b.size()) a.swap(b);
//	assert( a.size() >= b.size() );
	for (size_t i = 0, b_size = b.size(); i < a.size() - b_size; ++i)
		b.push_back( pConst0 );
	
	// sum = a ^ b ^ c
	// carry = a * b + (a ^ b) * c
	for (size_t i = 0; i < a.size(); ++i) {
		a_xor_b = make_xor( a[i], b[i] );
		sum     = make_xor( a_xor_b, carry );

		a_and_b = make_and( a[i], b[i] );
		a_xor_b_and_c = make_and( a_xor_b, carry );
		carry   = make_or( a_and_b, a_xor_b_and_c );

		ret.push_back(sum);
	}
	ret.push_back(carry);

	std::reverse(ret.begin(), ret.end());
	return ret;
}

// returned vector length = max( a.size(), b.size() ) + 1
std::vector<Abc_Obj_t *> my_Abc_AddFullSubtractor( Abc_Frame_t * pAbc, std::vector<Abc_Obj_t *>& a, std::vector<Abc_Obj_t *>& b ) {
	Abc_Ntk_t * pNtk = Abc_FrameReadNtk( pAbc );
	Abc_Obj_t * pConst0 = getConst0( pAbc );
	Abc_Obj_t * diff, * carry = pConst0, * a_xor_b, * not_a_and_b, * not_a_xor_b_and_c;
	std::vector<Abc_Obj_t *> ret;

	// a is the longer one, b fills const0 to be same length as a
//	if (a.size() < b.size()) a.swap(b);
	for (int i = 0, a_size = a.size(), b_size = b.size(); i < b_size - a_size; ++i)
		a.push_back( pConst0 );
	for (int i = 0, b_size = b.size(), a_size = a.size(); i < a_size - b_size; ++i)
		b.push_back( pConst0 );
	
	// sum = a ^ b ^ c
	// carry = a * b + (a ^ b) * c
	for (size_t i = 0; i < a.size(); ++i) {
		a_xor_b = make_xor( a[i], b[i] );
		diff    = make_xor( a_xor_b, carry );

		not_a_and_b = make_and( Abc_ObjNot(a[i]), b[i] );
		not_a_xor_b_and_c = make_and( Abc_ObjNot(a_xor_b), carry );
		carry   = make_or( not_a_and_b, not_a_xor_b_and_c );

		ret.push_back(diff);
	}
	ret.push_back(carry);

	std::reverse(ret.begin(), ret.end());
	return ret;
}

std::vector<Abc_Obj_t *> my_Abc_AddArithmeticCircuit( Abc_Frame_t * pAbc, const std::vector<Abc_Obj_t *>& num1, const std::vector<Abc_Obj_t *>& num2, int opt) {
	std::vector<Abc_Obj_t *> num1_cp = num1, num2_cp = num2;
	std::reverse(num1_cp.begin(), num1_cp.end());
	std::reverse(num2_cp.begin(), num2_cp.end());

	if (opt == 9 ) return my_Abc_AddFullAdder( pAbc, num1_cp, num2_cp );
	if (opt == 10) return my_Abc_AddFullSubtractor( pAbc, num1_cp, num2_cp );
}

int my_Abc_getGateNum( Abc_Frame_t * pAbc) {
    Abc_Ntk_t * pNtk = Abc_FrameReadNtk( pAbc );
    return Abc_NtkNodeNum( pNtk );
}
/*
// only used once (circuit built) (k is the constant, only contains 0, 1)
std::vector<Abc_Obj_t *> my_Abc_AddArithmeticCircuit( Abc_Frame_t * pAbc, const std::vector<int>& k, const std::vector<int>& pi_idx, int opt ) {
	assert( k.size() >= pi_idx.size() );
	assert( opt == 9 || opt == 10 );
	for (size_t i = 0; i < k.size(); ++i)
		assert(k[i] == 0 || k[i] == 1);
	
	Abc_Ntk_t * pNtk = Abc_FrameReadNtk( pAbc );
	Abc_Obj_t * pConst0 = getConst0( pAbc );
	Abc_Obj_t * pConst1 = Abc_ObjNot( pConst0 );

	std::vector<Abc_Obj_t *> k_obj;
	for (int i = k.size() - 1; i >= 0; --i)
		k_obj.push_back( k[i]? pConst1 : pConst0 );

	std::vector<Abc_Obj_t *> pi_obj;
	for (int i = pi_idx.size() - 1; i >= 0; --i)
		pi_obj.push_back( Abc_NtkPi( pNtk, pi_idx[i] ) );

	// 9 for ADD, 10 for SUBTRACT
	if (opt == 9 ) return my_Abc_AddFullAdder( pAbc, k_obj, pi_obj );
	if (opt == 10) return my_Abc_AddFullSubtractor( pAbc, k_obj, pi_obj );
}

// used except for the first time
std::vector<Abc_Obj_t *> my_Abc_AddArithmeticCircuit( Abc_Frame_t * pAbc, const std::vector<Abc_Obj_t *>& nodes, const std::vector<int>& pi_idx, int opt ) {
	assert( nodes.size() >= pi_idx.size() );
	assert( opt == 9 || opt == 10 );
	Abc_Ntk_t * pNtk = Abc_FrameReadNtk( pAbc );

	std::vector<Abc_Obj_t *> reversed_nodes = nodes;
	std::reverse(reversed_nodes.begin(), reversed_nodes.end());

	std::vector<Abc_Obj_t *> pi_obj;
	for (int i = pi_idx.size() - 1; i >= 0; --i)
		pi_obj.push_back( Abc_NtkPi( pNtk, pi_idx[i] ) );

	// 9 for ADD, 10 for SUBTRACT
	if (opt == 9 ) return my_Abc_AddFullAdder( pAbc, reversed_nodes, pi_obj );
	if (opt == 10) return my_Abc_AddFullSubtractor( pAbc, reversed_nodes, pi_obj );
}
*/



/* the simulation command read by ABC framework */
std::vector<float> my_Abc_SimSomeOutputs(Abc_Frame_t * pAbc, char * pFileName, const std::vector<int>& ids) {
	Abc_Ntk_t * pNtk = Abc_FrameReadNtk( pAbc );
	SimulationMgr Mgr;
	Mgr.initMgr(pNtk, pFileName);
	int i = 0, nOutput = Abc_NtkPoNum( pNtk );
	int incorrect[nOutput] = {};
	Mgr.readPatternFile();
	while(!Mgr.patternFilereadDone()){
		i++;
		Mgr.readPattern();
		Mgr.simulateNode();
		Mgr.comparePatternwithSimulation(incorrect);
	}
	std::cout << '\n';

	std::vector<float> err_rate;
//	for (size_t j = 0; j < ids.size(); ++j)
//		err_rate.push_back( incorrect[ids[j]] / (float)(i * 64) );
	std::cout << "total pat = " << i * 64 << '\n';
	for (size_t j = 0; j < ids.size(); ++j)
		err_rate.push_back( incorrect[ids[j]] );

	return err_rate;
}

