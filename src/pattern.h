#ifndef PATTERN_H
#define PATTERN_H

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <unordered_map>
#include <cassert>

using namespace std;

#define SIZE_T 			sizeof(size_t) * 8
#define LOG_SIZE_T 		6
#define block_num(i)	( _block[(i) >> LOG_SIZE_T] )
#define bit_on(i)		( 1UL << (((i) & 63UL)) )
#define size2bnum(s)	( ((s-1) >> LOG_SIZE_T) + 1 )

#define Compact_Cube	Cube

#define Dep_pat			Pattern
#define DR_pat			Pattern
#define Cube_pat		pair<DR_pat, vector<Pattern> * >
#define PatHash 		unordered_map<Pattern, Pattern, PatternHasher>
#define PatHashIter		unordered_map<Pattern, Pattern, PatternHasher>::iterator

// implemented most of the functionalities of dynamic_bitset
class Pattern {
public:
	#define ALL1 0xFFFFFFFFFFFFFFFF
	Pattern () { _size = 0; }
	Pattern (int size, size_t init = 0);
	Pattern (int size, vector<size_t>& block);
	Pattern (const Pattern& p) { _size = p._size; _block = p._block; }
	~Pattern() {}

	size_t  size () const { return _size; }
	Pattern& resize (int s) { _size = s; _block.resize(size2bnum(s), 0); return *this; }
	void flip   (int i) { block_num(i) ^= bit_on(i); }	// flip the i-th bit
	void set    (int i) { block_num(i) |= bit_on(i); }	// set the i-th bit to 1
	void reset  (int i) { block_num(i) &= ~bit_on(i); }	// set the i-th bit to 0
	void set_all	 () { for (size_t i = 0; i < _block.size(); ++i) _block[i] = ALL1; }				// set all bits to 1
	void reset_all   () { for (size_t i = 0; i < _block.size(); ++i) _block[i] = 0; }					// set all bits to 0
	Pattern& random	 () { for (size_t i = 0; i < _block.size(); ++i) _block[i] = rn_gen_size_t(); return *this; }	// randomly set bits
	// randomly set r% of bits to 1
	Pattern& random (int r) {
		random();
		int change_rate = r > 50? r - 50: 50 - r;
		for (int i = 0; i < change_rate * _size / 100; ++i) {
			if (r > 50) set(rand() % _size);
			else reset(rand() % _size);
		} 
		return *this;
	 }

	int count () const;					// return number of set bits
	string to_string () const;			// return string of 0s & 1s

	size_t to_size_t() const;


	size_t   operator () () const; 		// used by hash (unordered_map)
	Pattern  operator ~  () const;
	bool     operator == (const Pattern& p) const;
	bool     operator != (const Pattern& p) const;
	Pattern  operator ^  (const Pattern& p) const;
	Pattern  operator |  (const Pattern& p) const;
	Pattern  operator &  (const Pattern& p) const;
	Pattern& operator |= (const Pattern& p);
	bool 	 operator <  (const Pattern& p) const;
	bool     operator >  (const Pattern& p) const;
	bool 	 operator <= (const Pattern& p) const;
	bool 	 operator >= (const Pattern& p) const;
	Pattern  operator +  (const Pattern& p) const;
	Pattern  operator -  (const Pattern& p) const;
	bool operator [] (int i) { return block_num(i) & bit_on(i); }
	const bool operator [] (int i) const { return block_num(i) & bit_on(i); }

	// for DR_pat
	Pattern& operator ++ ();
	void append( bool b );
	Pattern& concat(const Pattern& p);
	Pattern duplicate();
	Pattern reverse();
	bool back();
	Pattern sub_pat (const vector<int>& idx) const;

	friend ostream& operator << (ostream& os, const Pattern& p) {
		os << p.to_string();
		return os;
	}
	
private:
	int _size;
	vector<size_t> _block;

	size_t rn_gen_size_t() { return (size_t(rand()) << 32) + rand(); }
};

// used by hash (unordered_map)
struct PatternHasher {
	size_t operator () (const Pattern& p) const { return p(); }
};

class Cube
{
public:
	Cube() { _size = 0; _done = 0; }
	Cube( int n, char c = '-' ){
		_size = ( c == '-' ) ? 0 : n;
		_s = string( n, c );
        _done = 0;
	}
	Cube& operator = ( const Cube& c ){
		_s = c._s;
		_size = c._size;
		return *this;
	}
	void assign( int pos, char value ){
		if( _s[pos] == '-' && value != '-' ) ++_size;
		else if( _s[pos] != '-' && value == '-' ) --_size;
		_s[pos] = value;
	}
	char get(int i) const { return _s[i]; }
	const char operator [] (int i) const { return _s[i]; }
	int size() const { return _size; }
	int capacity() const { return _s.size(); }
	bool empty() const { return ( _size == 0 ); }
	string to_string() const { return _s; }

    void setDone() { _done = 1; }
    bool isDone() const { return _done; }
    void setTendency(bool tend) { _tend = tend; }
    bool getTendency() const { return _tend; }

	friend ostream& operator<<( ostream& os, Cube& c ){
		os << c.to_string();
		return os;
	}
private:
	string _s;
	int _size;
    bool _done;
    bool _tend;
};


#endif // PATTERN_H
