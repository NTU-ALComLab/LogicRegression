#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>

#include "parser.h"
#include "variable.h"

#define MIN_VARIABLE_LITERAL_NUM 8

using namespace std;

extern int numPI;
extern int numPO;
extern vector<string> PI_name;
extern vector<string> PO_name;

extern vector< vector<Variable> > vars_with_same_len;
extern vector< Variable > output_vars;
string del = "_";

extern vector< Variable > all_input_vars;
extern vector< Variable > all_output_vars;


string& ltrim (string& str, const string& del = "_") {
	size_t pos = 0;
	while ((pos = str.find(del)) != string::npos) {
		str.erase(0, pos + del.length());
	}
	return str;
}

string& rtrim (string& str, const string& del = "_") {
	if (str.back() == '_') str.pop_back();
	return str;
}

string trim (string str, const string& del) {
	return rtrim(ltrim(str, del), "_");
}


Parser::Parser(char* file) {
	if (file == NULL) return;
	ifstream fin(file);
	fin >> numPI >> numPO;
	string name;
	for (int i = 0; i < numPI; ++i) {
		fin >> name;
		PI_name.push_back(name);
	}
	for (int i = 0; i < numPO; ++i) {
		fin >> name;
		PO_name.push_back(name);
	}
	fin.close();
}

void Parser::find_vars() {
	vector< vector<Variable> >& var_grps = vars_with_same_len;
	vector<string> trimmed_names;
	unordered_map<string, vector< pair<string, int> > > vars;

	// parse input names
	string PI_del = "PI_";
	for (int i = 0; i < numPI; ++i) {
		string trimmed = trim(PI_name[i], PI_del);
		size_t pos = trimmed.find_last_of(del);
		string var_name = trimmed.substr(0, pos);
		trimmed.erase(0, pos + del.length());
		vars[var_name].push_back(make_pair(trimmed, i));
	}

	// group the variables by size
	size_t max_grp_size = 0;
	for (auto it : vars) {
		vector< pair<string, int> >& var_pairs = it.second;
        all_input_vars.push_back( Variable(var_pairs, it.first) );
		if (var_pairs.size() >= MIN_VARIABLE_LITERAL_NUM) {
			Variable var(var_pairs, it.first);
			bool added = false;
			for (size_t i = 0; i < var_grps.size(); ++i) {
				if (var_grps[i][0].size() == var.size()) {
					var_grps[i].push_back(var);
					added = true;
					break;
				}
			}
			if (!added) {
				var_grps.push_back(vector<Variable>({ var }));
				if (var.size() > max_grp_size)
					max_grp_size = var.size();
			}
		}
	}

/* 	for (auto it : vars) { */
/* 		cout << it.first << "\t: "; */
/* //		sort(it.second.begin(), it.second.end(), compare); */
/* 		for (size_t i = 0; i < it.second.size(); ++i) */
/* 			cout << it.second[i].first << ' '; */
/* 		cout << '\t' << it.second.size() << '\n'; */
/* 	} */

	// parse output names
	vars.clear();
	string PO_del = "PO_";
	for (int i = 0; i < numPO; ++i) {
		string trimmed = trim(PO_name[i], PO_del);
		size_t pos = trimmed.find_last_of(del);
		string var_name = trimmed.substr(0, pos);
		trimmed.erase(0, pos + del.length());
		vars[var_name].push_back(make_pair(trimmed, i));
	}

	for (auto it : vars) {
		vector< pair<string, int> >& var_pairs = it.second;
        all_output_vars.push_back( Variable(var_pairs, it.first) );
		if (var_pairs.size() >= max_grp_size)
			output_vars.push_back( Variable(var_pairs, it.first) );
	}


/* 	for (auto it : vars) { */
/* 		cout << it.first << "\t: "; */
/* //		sort(it.second.begin(), it.second.end(), compare); */
/* 		for (size_t i = 0; i < it.second.size(); ++i) */
/* 			cout << it.second[i].first << ' '; */
/* 		cout << '\t' << it.second.size() << '\n'; */
/* 	} */


	/* for (size_t i = 0; i < output_vars.size(); ++i) { */
	/* 	for (size_t j = 0; j < output_vars[i].size(); ++j) */
	/* 		cout << output_vars[i][j] << ' '; */
	/* 	cout << '\n'; */
	/* } */

}

