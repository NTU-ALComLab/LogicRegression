#ifndef VARIABLE_H
#define VARIABLE_H

#include <vector>
#include <string>
#include <utility>
#include <algorithm>

using namespace std;


class Variable {
public:
	Variable() {}
	Variable(vector< pair<string, int> >& v, string name): _name(name) {
		sort(v.begin(), v.end(), compare);
		for (auto it : v) _pis.push_back(it.second);
	}

	const vector<int>& get_pis () const { return _pis; }
	size_t size () const { return _pis.size(); }
    string get_name() const { return _name; }

	// gets the i-th bit of the variable (i = 0 is MSB)
	// e.g. A is a 15 bit variable --> A_15 -> i = 0, A_12 -> i = 3
	int operator [] (int i) { return _pis[i]; }
	const int operator [] (int i) const { return _pis[i]; }
    bool operator < (const Variable& v) const {
        if (_name.size() != v._name.size())
            return _name.size() > v._name.size();
        for (size_t i = 0; i < _name.size(); ++i)
            if (_name[i] != v._name[i])
                return _name[i] > v._name[i];
        return false;
    }


private:
	vector<int> _pis; // ids of pis sorted by names of pis
    string _name;
	
	static bool compare (const pair<string, int>& p1, const pair<string, int>& p2) {
		string a = p1.first, b = p2.first;
		if (a.size() != b.size())
			return a.size() > b.size();

		for (size_t i = 0; i < a.size(); ++i) {
			if (a[i] != b[i])
				return a[i] > b[i];
		}
		return false;
	}


};


#endif // VARIABLE_H
