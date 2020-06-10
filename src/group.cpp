#include <iostream>

#include "enum.h"
#include "group.h"
#include "abc_wrapper.h"


extern Abc_Wrapper abc;

extern int numPI;
extern int numPO;

extern IO_Helper ioh;

/********** static data member definition **********/

PatHash Group::_pat_hash;

Variable Group_v2:: *_output;
Pattern Group_v2::_k;
PatHash Group_v2::_pat_hash;

/***************************************************/


Group::Group (Variable* var0, Variable* var1, int output_id) {
	_vars[0] = var0; _vars[1] = var1;
	_relation = NO_RELATION;
	_fanout = NULL;
	_output_id = output_id;

	_enabler = Pattern( numPI );
}

void Group::build_circuit() {
	bool invert = 0;
	Abc_Obj_t * (Abc_Wrapper::*fptr)(const vector<int>&, const vector<int>&);
	switch (_relation) {
		case XOR            : fptr = &Abc_Wrapper::build_xor;          break;
		case EQUIVALENCE    : fptr = &Abc_Wrapper::build_equiv;        break;
		case LARGER_THAN    : fptr = &Abc_Wrapper::build_larger_than;  break;
		case SMALLER_THAN   : fptr = &Abc_Wrapper::build_smaller_than; break;
		case LARGER_EQ_THAN : fptr = &Abc_Wrapper::build_smaller_than; invert = 1; break;
		case SMALLER_EQ_THAN: fptr = &Abc_Wrapper::build_larger_than;  invert = 1; break;
		default: fptr = NULL;
	}

	_fanout = fptr? (abc.*fptr)(_vars[0]->get_pis(), _vars[1]->get_pis()) : NULL;
	if (invert && _fanout)
		_fanout = (Abc_Obj_t *)( (size_t)_fanout ^ (size_t)01 );
}

Abc_Obj_t * Group::get_fanout() {
	build_circuit();
	return _fanout;
}

void Group::gen_pat(Pattern& p, bool b) {
	gen_pat(p, *_vars[0], *_vars[1], _relation, b);
}

void Group::gen_pat(Pattern& pat, const Variable& v0, const Variable& v1, Relation rel, bool correct) {
	size_t size;
	assert((size = v0.size()) == v1.size());
	Pattern p[2] = { Pattern(size), Pattern(size) };
	bool undefined_relation = false;

	if (correct) {
		switch (rel) {
			case XOR            : p[1] = ~(p[0].random());  break;
			case EQUIVALENCE    : p[1] =   p[0].random();   break;
			case LARGER_THAN    : while (p[0].random() <= p[1].random()); break;
			case SMALLER_THAN   : while (p[0].random() >= p[1].random()); break;
			case LARGER_EQ_THAN : while (p[0].random() <  p[1].random()); break;
			case SMALLER_EQ_THAN: while (p[0].random() >  p[1].random()); break;
			case NO_RELATION : p[0].random(); p[1].random(); break;
			default: undefined_relation = true; break;
		}
	}
	else {
		switch (rel) {
			case XOR            : while (p[0].random() == ~(p[1].random())); break;
			case EQUIVALENCE    : while (p[0].random() ==   p[1].random());  break;
			case LARGER_THAN    : while (p[0].random() >  p[1].random()); break;
			case SMALLER_THAN   : while (p[0].random() <  p[1].random()); break;
			case LARGER_EQ_THAN : while (p[0].random() >= p[1].random()); break;
			case SMALLER_EQ_THAN: while (p[0].random() <= p[1].random()); break;
			case NO_RELATION : p[0].random(); p[1].random(); break;
			default: undefined_relation = true; break;
		}
	}

	// assign values to p
	if (!undefined_relation) {
		for (size_t i = 0; i < size; ++i) {
			if (p[0][size - i - 1]) pat.set(v0[i]);
			else pat.reset(v0[i]);
		}

		for (size_t i = 0; i < size; ++i) {
			if (p[1][size - i - 1]) pat.set(v1[i]);
			else pat.reset(v1[i]);
		}
	}
}


bool Group::find_enabler()
{
	bool base;
	int counter = 0;
	while( counter < 20 ){
		if( counter < 6 ) _enabler.random();
		else if( counter < 12 ) _enabler.random( 20 );
		else if( counter < 18 ) _enabler.random( 80 );
		else if( counter == 18 ) _enabler.set_all();
		else _enabler.reset_all();

		_pat_hash.clear();
		Pattern temp0( _vars[0]->size() ), temp1( _vars[0]->size() );
		vector< Pattern > in_pat;
		in_pat.reserve( 64 );

		for( int i=0; i<8; ++i ){
			gen_pat( _enabler, *_vars[0], *_vars[1], EQUIVALENCE, true );
			in_pat.push_back( _enabler );
		}
		for( int i=0; i<56; ++i ){
			gen_pat( _enabler, *_vars[0], *_vars[1], NO_RELATION, true );
			in_pat.push_back( _enabler );
		}
		
		ioh.get_io_rel( in_pat, _pat_hash );

		base = _pat_hash[ in_pat[0] ][ _output_id ];
		for( int i=1; i<64; ++i ){
			if( base != _pat_hash[ in_pat[i] ][ _output_id ] ) return true;
		}
		++counter;
	}
	return false;
}

void Group::find_relation()
{
	vector<Pattern> in_pat;
	in_pat.reserve( 128 );
	_pat_hash.clear();
	for( int i=0; i<8; ++i ){
		gen_pat( _enabler, *_vars[0], *_vars[1], EQUIVALENCE, true );
		in_pat.push_back( _enabler );
	}
	for( int i=0; i<60; ++i ){
		gen_pat( _enabler, *_vars[0], *_vars[1], LARGER_THAN, true );
		in_pat.push_back( _enabler );
	}
	for( int i=0; i<60; ++i ){
		gen_pat( _enabler, *_vars[0], *_vars[1], SMALLER_THAN, true );
		in_pat.push_back( _enabler );
	}

	ioh.get_io_rel( in_pat, _pat_hash );

	bool base0 = _pat_hash[ in_pat[0] ][ _output_id ];
	bool base1 = _pat_hash[ in_pat[8] ][ _output_id ];
	bool base2 = _pat_hash[ in_pat[68] ][ _output_id ];

	for( int i=1; i<8; ++i ){
		if( _pat_hash[ in_pat[i] ][ _output_id ] != base0 ) return;
	}
	for( int i=9; i<68; ++i ){
		if( _pat_hash[ in_pat[i] ][ _output_id ] != base1 ) return;
	}
	for( int i=69; i<128; ++i ){
		if( _pat_hash[ in_pat[i] ][ _output_id ] != base2 ) return;
	}

	if( ( base1 == base2 ) && ( base1 != base0 ) ) _relation = EQUIVALENCE;
	else if( ( base0 == base1 ) && ( base0 != base2 ) ) _relation = SMALLER_THAN;
	else if( ( base0 == base2 ) && ( base0 != base1 ) ) _relation = LARGER_THAN;
}

Group_v2::Group_v2( Variable* var, Variable* output )
{
	_var = var;
	_output = output;
	_relation = NO_RELATION;
}

Pattern Group_v2::find_k( Variable* output )
{
	vector<Pattern> v;
	v.push_back( Pattern( numPI ) );
	ioh.get_io_rel( v, _pat_hash );
	return ( _k = ( _pat_hash[ v[0] ].sub_pat( output->get_pis() ) ) );
}

void Group_v2::find_relation()
{
	Pattern p( numPI );
	vector<Pattern> in_pat;
	in_pat.reserve( 64 );
	_pat_hash.clear();
	p.reset_all();
	p.set( _var->get_pis().back() );
	in_pat.push_back( p );
	for( int i=1; i<64; ++i ){
		gen_pat( p, _var );
		in_pat.push_back( p );
	}

	ioh.get_io_rel( in_pat, _pat_hash );

	Pattern input;
	Pattern output;

	size_t num;

	output = _pat_hash[ in_pat[0] ].sub_pat( _output->get_pis() ).reverse();
	_ratio = ( output - _k.reverse() ).to_size_t();
	for( int i=1; i<64; ++i ){
		input = in_pat[i].sub_pat( _var->get_pis() ).reverse().resize( _output->size() );
		output = _pat_hash[ in_pat[i] ].sub_pat( _output->get_pis() ).reverse();
		num = ( input.to_size_t() * _ratio ) & ( ( 1 << output.size() ) -1 );
		if( ( output - _k.reverse() ).to_size_t() != num ){
			return;
		}
	}

	if( _ratio == 0 ){
		_relation = NO_RELATION;
	}
	else if( _ratio & ( 1 << ( output.size() - 1 ) ) ){
		_relation = SUBTRACT;
		_ratio = ( 1 << output.size() ) - _ratio;
	}
	else{
		_relation = ADD;
	}
}

void Group_v2::gen_pat( Pattern& p, Variable* v )
{
	Pattern temp( v->size() );
	temp.random();
	p.reset_all();
	for( size_t i=0; i<v->size(); ++i ){
		temp[i] ? p.set( (*v)[i] ) : p.reset( (*v)[i] );
	}
}
