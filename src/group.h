#ifndef GROUP_H
#define GROUP_H

#include "variable.h"
#include "enum.h"
#include "pattern.h"
#include "abc_wrapper.h"
#include "io_helper.h"


class Group {
public:
	Group ( Variable* var0, Variable* var1, int ouput_id );
	
	void build_circuit();
	
	// bool b: modify pattern to satisfy the relation (or not). b = 1 to satisfy
	void gen_pat(Pattern& p, bool b);

	// return false if it is a fake group
	bool valid() { return _relation != NO_RELATION; }

	Abc_Obj_t * get_fanout();

	bool find_enabler();

	void find_relation();

	Relation get_relation() const { return _relation; }


	// void fill_literals( Pattern&, const Pattern&, const Pattern& ) const;

	// Pattern& p: the pattern required to modify
	// const Variable& v0, v1: variables which we suppose there exists a relation
	// Relation rel: the relation (NO_RELATION produces random patterns for vars)
	// correct: the two patterns satisfy the relation or not
	static void gen_pat(Pattern& p, const Variable& v0, const Variable& v1, Relation rel, bool correct);

private:
	Relation    _relation;
	Variable  * _vars[2];
	Abc_Obj_t * _fanout;

	int _output_id;
	Pattern _enabler;

	static PatHash _pat_hash;
};

class Group_v2
{
public:
	Group_v2 ( Variable* var, Variable* output );

	// return false if it is a fake group
	bool valid() { return _relation != NO_RELATION; }

	static Pattern find_k( Variable* output );

	void find_relation();

	Relation get_relation() const { return _relation; }

	static void gen_pat( Pattern& p, Variable* v );

	Variable* get_output() const { return _output; }
	Variable* get_var() const { return _var; }

	int get_ratio() const { return _ratio; }

private:
	Relation    _relation;
	Variable  * _var;
	Variable * _output;

	int _ratio;

	static Pattern _k;

	static PatHash _pat_hash;
};

#endif // GROUP_H
