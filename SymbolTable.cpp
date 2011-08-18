#include "SymbolTable.h"


SymbolTable::SymbolTable(void) : staticIdx(0), fieldIdx(0), argIdx(0), varIdx(0)
{
}


SymbolTable::~SymbolTable(void)
{
}


void SymbolTable::startSubroutine()
{
	subroutineSymbolTable.clear();
	argIdx = varIdx = 0;
}

void SymbolTable::define(string name, string type, string kind)
{
	VariableInfo vi;

	if (kind == "STATIC") {
		VariableInfo v = { type, kind, staticIdx++ };
		vi = v;
	} else if(kind ==  "FIELD") {
		VariableInfo v = { type, kind, fieldIdx++ };
		vi = v;
	} else if(kind ==  "ARG") {
		VariableInfo v = { type, kind, argIdx++ };
		vi = v;
	} else if(kind == "VAR") {
		VariableInfo v = { type, kind, varIdx++ };
		vi = v;
	}
	

	if (kind == "STATIC" || kind == "FIELD") {
		classSymbolTable[name] = vi;
	} else {
		subroutineSymbolTable[name] = vi;
	}
}

size_t SymbolTable::varCount(string kind)
{
	size_t variableCount = 0;
	map<string, VariableInfo>::iterator pos;
	if (kind == "STATIC" || kind ==  "FIELD") {
		for (pos = classSymbolTable.begin(); pos != classSymbolTable.end(); ++pos) {
			if (pos->second.kind == kind) {
				variableCount++;
			}
		}
	} else if(kind == "ARG" || kind == "VAR") {
		for (pos = subroutineSymbolTable.begin(); pos != classSymbolTable.end(); ++pos) {
			if (pos->second.kind == kind) {
				variableCount++;
			}
		}
	}
	return variableCount;
}

string SymbolTable::kindOf(string name)
{
	string kind = "NONE";
	map<string, VariableInfo>::iterator pos;
	pos = subroutineSymbolTable.find(name) ;
	if(pos != subroutineSymbolTable.end()) {
		kind = pos->second.kind;
	} else {
		pos = classSymbolTable.find(name);
		if(pos != classSymbolTable.end()) {
			kind = pos->second.kind;
		}
	}
	return kind;
}

string SymbolTable::typeOf(string name)
{
	string str;
	map<string, VariableInfo>::iterator pos;
	pos = subroutineSymbolTable.find(name) ;
	if(pos != subroutineSymbolTable.end()) {
		str = pos->second.type;
	} else {
		pos = classSymbolTable.find(name);
		if(pos != classSymbolTable.end()) {
			str = pos->second.type;
		}
	}
	return str;
}

size_t SymbolTable::indexOf(string name)
{
	size_t index = 0;
	map<string, VariableInfo>::iterator pos;
	pos = subroutineSymbolTable.find(name) ;
	if(pos != subroutineSymbolTable.end()) {
		index = pos->second.index;
	} else {
		pos = classSymbolTable.find(name);
		if(pos != classSymbolTable.end()) {
			index = pos->second.index;
		}
	}
	return index;
}
