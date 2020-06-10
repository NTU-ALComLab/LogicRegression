#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "pattern.h"

using namespace std;


Pattern::Pattern (int size, size_t init) {
	_size = size;
	_block.resize(size2bnum(size), 0);
	_block[0] = init;
}

Pattern::Pattern (int size, vector<size_t>& block) {
	_size = size;
	_block.swap(block);
}


int Pattern::count () const {
	int ret = 0;
	for (int i = 0; i < _size; ++i)
		if (block_num(i) & bit_on(i)) ++ret;
	return ret;
}

string Pattern::to_string () const {
	string s;
	for (int i = 0; i < _size; ++i) 
		s += (block_num(i) & bit_on(i))? "1":"0";
	return s;
}

size_t Pattern::to_size_t() const{
	assert( _block.size() == 1 );
	return ( _block[0] & ( ( 1 << _size ) -1 ) );
}

size_t Pattern::operator () () const {
	size_t ret = 0;
	for (unsigned i = 0; i < _block.size(); ++i)
		ret ^= _block[i];
	return ret;
}

Pattern Pattern::operator ~ () const {
	vector<size_t> block(_block.size());
	for (unsigned i = 0; i < _block.size(); ++i)
		block[i] = _block[i] ^ ALL1;

	return Pattern(_size, block);
}

bool Pattern::operator == (const Pattern& p) const {
	if (p._size != _size) return false;
	for (unsigned i = 0; i < _block.size() - 1; ++i)
		if (p._block[i] != _block[i]) return false;

	size_t mod = 1 << (_size % 64);
	return p._block.back() % mod == _block.back() % mod;
}

bool Pattern::operator != (const Pattern& p) const {
	return !(*this == p);
}

Pattern Pattern::operator ^ (const Pattern& p) const {
	assert(p._size == _size);
	vector<size_t> block(_block.size());
	for (unsigned i = 0; i < _block.size(); ++i)
		block[i] = p._block[i] ^ _block[i];

	return Pattern(_size, block);
}

Pattern Pattern::operator | (const Pattern& p) const {
	assert(p._size == _size);
	vector<size_t> block(_block.size());
	for (unsigned i = 0; i < _block.size(); ++i)
		block[i] = p._block[i] | _block[i];

	return Pattern(_size, block);
}

Pattern Pattern::operator & (const Pattern& p) const {
	assert(p._size == _size);
	vector<size_t> block(_block.size());
	for (unsigned i = 0; i < _block.size(); ++i)
		block[i] = p._block[i] & _block[i];

	return Pattern(_size, block);
}

Pattern& Pattern::operator |= (const Pattern& p) {
	assert(p._size == _size);
	for (unsigned i = 0; i < _block.size(); ++i)
		_block[i] |= p._block[i];
	return *this;
}

bool Pattern::operator < (const Pattern& p) const {
/*	int last_block_valid_bits = _size % 64;
	size_t mask = ALL1 >> (64 - last_block_valid_bits);

	if (_block.back() & mask != p._block.back() & mask)
		return (_block.back() & mask) < (p._block.back() & mask);

	for (int i = (int)_block.size() - 2; i >= 0; --i)
		if (_block[i] != p._block[i])
			return _block[i] < p._block[i];
	return false;
	*/

	for (int i = _size - 1; i >= 0; --i) {
		if ((*this)[i] != p[i])
			return p[i];
	}
	return false;
}

bool Pattern::operator > (const Pattern& p) const {
/*	int last_block_valid_bits = _size % 64;
	size_t mask = ALL1 >> (64 - last_block_valid_bits);

	if (_block.back() & mask != p._block.back() & mask)
		return (_block.back() & mask) > (p._block.back() & mask);

	for (int i = (int)_block.size() - 2; i >= 0; --i)
		if (_block[i] != p._block[i])
			return _block[i] > p._block[i];
	return false;
*/
	for (int i = _size - 1; i >= 0; --i) {
		if ((*this)[i] != p[i])
			return !p[i];
	}
	return false;

}

bool Pattern::operator <= (const Pattern& p) const {
	return *this < p || *this == p;
}

bool Pattern::operator >= (const Pattern& p) const {
	return *this > p || *this == p;
}

Pattern& Pattern::operator ++ () {
	assert( _block.size() == 1 );
	++_block[0];
	return *this;
}

Pattern Pattern::operator + (const Pattern& p) const {
	assert( _size == p._size );
	Pattern ret(_size);
	size_t carry = 0;
	for (size_t i = 0; i < _block.size(); ++i) {
		ret._block[i] = _block[i] + p._block[i] + carry;
		carry = ret._block[i] < _block[i];
	}
	return ret;
}

Pattern Pattern::operator - (const Pattern& p) const {
	assert( _size == p._size );
	Pattern ret(_size);
	size_t carry = 0;
	for (size_t i = 0; i < _block.size(); ++i) {
		ret._block[i] = _block[i] - p._block[i] - carry;
		carry = ret._block[i] > _block[i];
	}
	return ret;
}

void Pattern::append ( bool b ) {
	if (_size % 64 == 0)
		_block.push_back(0);
	if( b )	set( _size++ );
	else reset( _size++ );
}

Pattern& Pattern::concat(const Pattern& p) {
	int size = p._size;
	for (int i = 0; i < size; ++i)
		append(p[i]);
	return *this;
}

Pattern Pattern::duplicate() {
	Pattern p = *this;
	p.concat(p);
	return p;
}

Pattern Pattern::reverse() {
	Pattern p = *this;
	for (int i = 0; i < _size / 2; ++i) {
		if (p[i] != p[_size - i - 1]) {
			p.flip(i);
			p.flip(_size - i -1);
		}
	}
	return p;
}

bool Pattern::back () {
	return block_num(_size-1) & bit_on(_size-1);
}

Pattern Pattern::sub_pat (const vector<int>& idx) const {
	Pattern p(idx.size());
	for (size_t i = 0; i < idx.size(); ++i) {
		if ( (*this)[ idx[i] ] ) p.set(i);
		else p.reset(i);
	}
	return p;
}
