#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include <string>
#include <map>
using namespace std;

enum Kind { STATIC, FIELD, ARG, VAR, NONE };

class SymbolTable
{
public:
	SymbolTable(void);
	~SymbolTable(void);

	void startSubroutine();
	void define(string name, string type, Kind kind);
	size_t varCount(Kind kind);
	Kind kindOf(string name);
	string typeOf(string name);
	size_t indexOf(string name);

private:
	struct VariableInfo {
		string type;
		Kind kind;
		size_t index;
	};

	map<string, VariableInfo> classSymbolTable;
	map<string, VariableInfo> subroutineSymbolTable;
	size_t staticIdx, fieldIdx, argIdx, varIdx;

};

#endif