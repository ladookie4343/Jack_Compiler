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

void SymbolTable::define(string name, string type, Kind kind)
{
	VariableInfo vi;

	switch(kind) {
		case STATIC:
		{
			VariableInfo v = { type, kind, staticIdx++ };
			vi = v;
			break;
		}
		case FIELD:
		{
			VariableInfo v = { type, kind, fieldIdx++ };
			vi = v;
			break;
		}
		case ARG:
		{
			VariableInfo v = { type, kind, argIdx++ };
			vi = v;
			break;
		}
		case VAR:
		{
			VariableInfo v = { type, kind, varIdx++ };
			vi = v;
			break;
		}
	}

	if (kind == STATIC || kind == FIELD) {
		classSymbolTable[name] = vi;
	} else {
		subroutineSymbolTable[name] = vi;
	}
}

size_t SymbolTable::varCount(Kind kind)
{
	size_t variableCount = 0;
	map<string, VariableInfo>::iterator pos;
	if (kind == STATIC || kind ==  FIELD) {
		for (pos = classSymbolTable.begin(); pos != classSymbolTable.end(); ++pos) {
			if (pos->second.kind == kind) {
				variableCount++;
			}
		}
	} else if(kind == ARG || kind == VAR) {
		for (pos = subroutineSymbolTable.begin(); pos != classSymbolTable.end(); ++pos) {
			if (pos->second.kind == kind) {
				variableCount++;
			}
		}
	}
	return variableCount;
}

Kind SymbolTable::kindOf(string name)
{
	Kind kind = NONE;
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
