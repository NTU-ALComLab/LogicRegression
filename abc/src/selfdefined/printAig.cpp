#include "selfdefined.h"
#include "aig/aig/aig.h"
#include "misc/util/abc_global.h"
#include "base/io/ioAbc.h"
#include "base/main/main.h"
#include "misc/bbl/bblif.h"
#include "misc/vec/vecPtr.h"
#include <iostream>
#include <vector>

ABC_NAMESPACE_IMPL_START

int Abc_AddNode ( Abc_Ntk_t * pNtk){
	Vec_Ptr_t * Fanins = Vec_PtrAlloc(2);
	Vec_Ptr_t * firstFanins = Vec_PtrAlloc(3);
	std::vector<Abc_Obj_t *> invertFanin;
	int i;
	Abc_Obj_t * pNode;
	Abc_NtkForEachPi( pNtk, pNode, i ){
		invertFanin.push_back(pNode);
		Vec_PtrPush(firstFanins, pNode);
	}
	Abc_Obj_t * newNode = Abc_NtkCreateNodeAnd(pNtk, firstFanins);
	for (size_t i = 0; i < invertFanin.size(); i++)
		Abc_NodeComplementInput(newNode, invertFanin[i]);
	Abc_Obj_t * pFanin;
	Abc_Obj_t * newFanin;
	Abc_NtkForEachPo( pNtk, pNode, i ){
		pFanin = Abc_ObjFanin0( pNode );
		Abc_ObjRemoveFanins( pNode );
		Vec_PtrPushTwo(Fanins, newNode, pFanin);
		newFanin = Abc_NtkCreateNodeOr( pNtk, Fanins);
		Abc_ObjAddFanin(pNode, newFanin);
	}
	return 0;
}

int Abc_printAig( Abc_Ntk_t * pNtk){
	if (pNtk == NULL) return 1;
	Abc_Obj_t * pNode;
	int i;
	/* Abc_NtkForEachPi( pNtk, pNode, i ){ */
		/* std::cout << "PI: " << pNode->Id << std::endl; */
	/* } */
	/* Abc_NtkForEachNode( pNtk, pNode, i ){ */
		/* std::cout << "AND: " << pNode->Id << " "; */
		/* if (Abc_ObjFaninC0(pNode))  */
			/* std::cout << "!"; */
		/* std::cout << Abc_ObjFaninId0(pNode) << " "; */
		/* if (Abc_ObjFaninC1(pNode)) */
			/* std::cout << "!"; */
		/* std::cout << Abc_ObjFaninId1(pNode) << std::endl; */
	/* } */
	/* Abc_NtkForEachPo( pNtk, pNode, i ){ */
		/* std::cout << "PO " << pNode->Id << " "; */
		/* if (Abc_ObjFaninC0(pNode)) */
			/* std::cout << "!"; */
		/* std::cout << Abc_ObjFaninId0(pNode) << std::endl; */
	/* } */
	Abc_NtkForEachPi( pNtk, pNode, i){
		std::cout << Abc_ObjName(pNode) << " " << pNode->Id << std::endl;
	}
	Abc_NtkForEachPo( pNtk, pNode, i){
		std::cout << Abc_ObjName(pNode) << " " << pNode->Id << std::endl;
	}
	return 0;
}




ABC_NAMESPACE_IMPL_END
