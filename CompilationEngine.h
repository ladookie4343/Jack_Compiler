#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H
#include "JackTokenizer.h"


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
	
	JackTokenizer *tokenizer;
	ofstream &outfile;
	size_t indentCount;
};

#endif