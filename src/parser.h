#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>

#include "variable.h"


using namespace std;

string trim (string str, const string& del = "_");

class Parser {
public:
	Parser(char* file = NULL);
	void find_vars();

private:

};

#endif // PARSER_H
