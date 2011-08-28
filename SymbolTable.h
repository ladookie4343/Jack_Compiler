#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include <string>
#include <map>
#include <iostream>
#include <iomanip>
using namespace std;

class SymbolTable
{
public:
	SymbolTable(void);
	~SymbolTable(void);

	void startSubroutine();
	void define(string name, string type, string kind);
	size_t varCount(string kind);
	string kindOf(string name);
	string typeOf(string name);
	size_t indexOf(string name);
	void display();

private:
	struct VariableInfo {
		string type;
		string kind;
		size_t index;
	};

	map<string, VariableInfo> classSymbolTable;
	map<string, VariableInfo> subroutineSymbolTable;
	size_t staticIdx, fieldIdx, argIdx, varIdx;
};

#endif