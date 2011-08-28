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

	if (kind == "static") {
		VariableInfo v = { type, kind, staticIdx++ };
		vi = v;
	} else if(kind ==  "field") {
		VariableInfo v = { type, kind, fieldIdx++ };
		vi = v;
	} else if(kind ==  "arg") {
		VariableInfo v = { type, kind, argIdx++ };
		vi = v;
	} else if(kind == "var") {
		VariableInfo v = { type, kind, varIdx++ };
		vi = v;
	}
	

	if (kind == "static" || kind == "field") {
		classSymbolTable[name] = vi;
	} else {
		subroutineSymbolTable[name] = vi;
	}
}

size_t SymbolTable::varCount(string kind)
{
	size_t variableCount = 0;
	map<string, VariableInfo>::iterator pos;
	if (kind == "static" || kind ==  "field") {
		for (pos = classSymbolTable.begin(); pos != classSymbolTable.end(); ++pos) {
			if (pos->second.kind == kind) {
				variableCount++;
			}
		}
	} else if(kind == "arg" || kind == "var") {
		for (pos = subroutineSymbolTable.begin(); pos != subroutineSymbolTable.end(); ++pos) {
			if (pos->second.kind == kind) {
				variableCount++;
			}
		}
	}
	return variableCount;
}

string SymbolTable::kindOf(string name)
{
	string kind = "none";
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
	string str = "none";
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

void SymbolTable::display()
{
	map<string, VariableInfo>::iterator pos;

	cout << "class scope symbol table\n";
	cout << setw(20) << "Name" << setw(10) << "Type" << setw(10) << "Kind" << setw(5) << "#\n";
	for (pos = classSymbolTable.begin(); pos != classSymbolTable.end(); ++pos) {
		cout << setw(20) << pos->first << setw(10) << pos->second.type << setw(10) 
			 << pos->second.kind << setw(5) << pos->second.index << endl;
	}

	cout << "\nsubroutine scope symbol table\n";
	cout << setw(20) << "Name" << setw(10) << "Type" << setw(10) << "Kind" << setw(5) << "#\n";
	for (pos = subroutineSymbolTable.begin(); pos != subroutineSymbolTable.end(); ++pos) {
		cout << setw(20) << pos->first << setw(10) << pos->second.type << setw(10) 
			 << pos->second.kind << setw(5) << pos->second.index << endl;		
	}

	cout << "\n\n";
}