#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H
#include <sstream>
#include <stack>
#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"

class CompilationEngine
{
public:
	CompilationEngine(JackTokenizer *tokenizer_, ofstream& outfile_);
	~CompilationEngine(void);

	void compileClass();

private:
	void compileClassVarDec();
	void compileSubroutine();
	void compileParameterList();
	void compileVarDec();
	void compileStatements();
	void compileDo();
	void compileLet();
	void compileWhile();
	void compileReturn();
	void compileIf();
	void compileExpression();
	void compileTerm();
	void compileExpressionList();
	void outputSymbol();
	void printCurrentToken();
	void advanceTokenizer();
	Segment segment(string seg);
	Command command(char op);
	string iToStr(size_t aNumber);
	
	JackTokenizer *tokenizer;
	ofstream &outfile;
	SymbolTable *symbolTable;
	VMWriter *vmWriter;
	string className, subroutineName, currentSubroutineType;
	size_t nArgs;
	size_t ifCount, whileCount;
	stack<size_t> ifStack, whileStack;
};

#endif