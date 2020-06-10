/* this is the self defined function to perform simulation for AIG  */
#ifndef ABC__selfdefined_h
#define ABC__selfdefined_h


#include "base/abc/abc.h"
#include "misc/util/abc_namespaces.h"

ABC_NAMESPACE_HEADER_START


int Abc_SimAig( Abc_Ntk_t * pNtk, char* pFileName);
int Abc_printAig( Abc_Ntk_t * pNtk );
int Abc_AddNode( Abc_Ntk_t * pNtk );
int Abc_AddPatToNetwork( Abc_Ntk_t * pNtk );


ABC_NAMESPACE_HEADER_END  
#endif

