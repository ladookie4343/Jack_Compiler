#include "CompilationEngine.h"
#include <cassert>
#include <iostream>
using namespace std;

CompilationEngine::CompilationEngine(JackTokenizer *tokenizer_, ofstream& outfile_) :
					tokenizer(tokenizer_), outfile(outfile_), ifCount(-1), whileCount(-1)
{
	symbolTable = new SymbolTable();
	vmWriter = new VMWriter(outfile_);
}


CompilationEngine::~CompilationEngine(void)
{
	delete vmWriter;
	delete symbolTable;
}

void CompilationEngine::advanceTokenizer()
{
	assert(tokenizer->hasMoreTokens());
	tokenizer->advance();
}

void CompilationEngine::compileClass()
{	
	advanceTokenizer();
	assert(tokenizer->tokenType() == KEYWORD);
	assert(tokenizer->keyWord() == "class");
	
	advanceTokenizer();
	assert(tokenizer->tokenType() == IDENTIFIER);
	className = tokenizer->identifier();
	
	advanceTokenizer();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '{');
	
	advanceTokenizer();

	// classVarDec*
	while (true) {
		assert(tokenizer->tokenType() == KEYWORD || tokenizer->tokenType() == SYMBOL);
		if (tokenizer->tokenType() == KEYWORD) {
			assert(tokenizer->keyWord() == "static" || tokenizer->keyWord() == "field" ||
				   tokenizer->keyWord() == "constructor" || tokenizer->keyWord() == "function" ||
				   tokenizer->keyWord() == "method");
			if (tokenizer->keyWord() == "field" || tokenizer->keyWord() == "static") {
				compileClassVarDec();
			} else { // constructor or function or method
				break;
			}
		} else { // symbol: }
			break;
		}
	}

	// subroutineDec*
	while (true) {
		assert(tokenizer->tokenType() == SYMBOL || tokenizer->tokenType() == KEYWORD);
		if (tokenizer->tokenType() == SYMBOL) {
			assert(tokenizer->symbol() == '}');
			break;
		} else {
			assert(tokenizer->keyWord() == "constructor" || tokenizer->keyWord() == "function" ||
				   tokenizer->keyWord() == "method");
			compileSubroutine();
		}
	}
	symbolTable->display();

	assert(tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == '}');
}

void CompilationEngine::compileClassVarDec()
{
	string name, type, kind;
	kind = tokenizer->keyWord();

	advanceTokenizer();
	TokenType t_type = tokenizer->tokenType();
	assert(t_type == KEYWORD || t_type == IDENTIFIER);
	if (t_type == KEYWORD) {
		string keyword = tokenizer->keyWord();
		assert(keyword == "int" || keyword == "char" || keyword == "boolean");
		type = keyword;
	} else {
		type = tokenizer->identifier();
	}
	
	advanceTokenizer();
	assert(tokenizer->tokenType() == IDENTIFIER);
	name = tokenizer->identifier();
	symbolTable->define(name, type, kind);

	while (true) {
		advanceTokenizer();
		assert(tokenizer->tokenType() == SYMBOL);
		if (tokenizer->symbol() == ';') {
			break;
		} else {
			assert(tokenizer->symbol() == ',');
		}
		advanceTokenizer();
		assert(tokenizer->tokenType() == IDENTIFIER);
		name = tokenizer->identifier();
		symbolTable->define(name, type, kind);
	}

	advanceTokenizer();
}

void CompilationEngine::compileSubroutine()
{   //method or constructor or function
	symbolTable->startSubroutine();

	currentSubroutineType = tokenizer->keyWord();

	advanceTokenizer(); // type 
	assert(tokenizer->tokenType() == KEYWORD || tokenizer->tokenType() == IDENTIFIER);
	
	advanceTokenizer(); //name of subroutine
	assert(tokenizer->tokenType() == IDENTIFIER);
	subroutineName = tokenizer->identifier();
	
	advanceTokenizer(); // '('
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '(');
	
	advanceTokenizer();
	if (currentSubroutineType == "method") {
		symbolTable->define("this", className, "arg");
	}
	compileParameterList();

	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == ')');
	
	advanceTokenizer(); // '{'

	//subroutine body
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '{');

	advanceTokenizer(); // varDec or statement or '}'

	// varDec*
	while (true) {
		assert(tokenizer->tokenType() == KEYWORD || tokenizer->tokenType() == SYMBOL);
		if (tokenizer->keyWord() == "var") {
			compileVarDec();
		} else {
			break;
		}
	}
	
	vmWriter->writeFunction(className + "." + subroutineName, symbolTable->varCount("var"));
	if (currentSubroutineType == "method") {
		vmWriter->writePush(ARG, 0);
		vmWriter->writePop(POINTER, 0);
	} else if (currentSubroutineType == "constructor") {
		vmWriter->writePush(CONST, symbolTable->varCount("field"));
		vmWriter->writeCall("Memory.alloc", 1);
		vmWriter->writePop(POINTER, 0);
	}
	
	compileStatements();

	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '}');

	advanceTokenizer();
}

void CompilationEngine::compileParameterList()
{   // type or ')'
	string name, type, kind = "arg";

	TokenType t_type = tokenizer->tokenType();
	assert(t_type == SYMBOL || t_type == KEYWORD || t_type == IDENTIFIER);

	if (t_type == SYMBOL) {
		return;
	}
	if (t_type == KEYWORD) {
		assert(tokenizer->keyWord() == "int" || tokenizer->keyWord() == "char" ||
			   tokenizer->keyWord() == "boolean");
		type = tokenizer->keyWord();
	} else {
		type = tokenizer->identifier();
	}
	
	advanceTokenizer(); // variableName
	assert(tokenizer->tokenType() == IDENTIFIER);
	name = tokenizer->identifier();

	symbolTable->define(name, type, kind);

	while (true) {
		advanceTokenizer(); // ',' or ')'
		assert(tokenizer->tokenType() == SYMBOL);
		assert(tokenizer->symbol() == ',' || tokenizer->symbol() == ')');
		if (tokenizer->symbol() == ')') {
			break;
		} else {
			advanceTokenizer(); // type
			assert(tokenizer->tokenType() == KEYWORD || tokenizer->tokenType() == IDENTIFIER);
			if (tokenizer->tokenType() == KEYWORD) {
				assert(tokenizer->keyWord() == "int" || tokenizer->keyWord() == "char" ||
					   tokenizer->keyWord() == "boolean");
				type = tokenizer->keyWord();
			} else {
				type = tokenizer->identifier();
			}
			advanceTokenizer(); // variable name
			assert(tokenizer->tokenType() == IDENTIFIER);
			name = tokenizer->identifier();
			symbolTable->define(name, type, kind);
		}
	}
}

void CompilationEngine::compileVarDec()
{   // "var"
	string name, type, kind = "var";
	assert(tokenizer->tokenType() == KEYWORD);
	assert(tokenizer->keyWord() == "var");

	advanceTokenizer(); // type
	assert(tokenizer->tokenType() == KEYWORD || tokenizer->tokenType() == IDENTIFIER);
	if (tokenizer->tokenType() == KEYWORD) {
		assert(tokenizer->keyWord() == "int" || tokenizer->keyWord() == "char" ||
			   tokenizer->keyWord() == "boolean");
		type = tokenizer->keyWord();
	} else {
		type = tokenizer->identifier();
	}

	advanceTokenizer(); // variable name
	assert(tokenizer->tokenType() == IDENTIFIER);
	name = tokenizer->identifier();
	symbolTable->define(name, type, kind);

	while (true) {
		advanceTokenizer(); // ',' or ';'
		assert(tokenizer->tokenType() == SYMBOL);
		assert(tokenizer->symbol() == ',' || tokenizer->symbol() == ';');
		if (tokenizer->symbol() == ';') {
			break;
		} else { // ,
			advanceTokenizer(); // variable name
			assert(tokenizer->tokenType() == IDENTIFIER);
			name = tokenizer->identifier();
			symbolTable->define(name, type, kind);
		}
	}
	
	advanceTokenizer();
}

void CompilationEngine::compileStatements()
{	// statement or '}'

	while (true) {
		if (tokenizer->tokenType() == SYMBOL) {
			assert(tokenizer->symbol() == '}');
			return;
		}
		assert(tokenizer->tokenType() == KEYWORD);
		string keyword = tokenizer->keyWord();
		assert(keyword == "let" || keyword == "if" || keyword == "while" ||
			   keyword == "do"  || keyword == "return");
		if (keyword == "let") {
			compileLet();
		} else if (keyword == "if") {
			compileIf();
		} else if (keyword == "while") {
			compileWhile();
		} else if (keyword == "do") {
			compileDo();
		} else if (keyword == "return") {
			compileReturn();
		}
		if (keyword != "if") {
			advanceTokenizer();
		}
	}
}

//currentToken should be "do" at this point.
void CompilationEngine::compileDo()
{
	assert(tokenizer->tokenType() == KEYWORD);
	assert(tokenizer->keyWord() == "do");
	
	char previousSymbol;
	string identBeforeDot, identAfterDot;

	advanceTokenizer();
	assert(tokenizer->tokenType() == IDENTIFIER);
	identBeforeDot = tokenizer->identifier();
				
	advanceTokenizer();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '(' || tokenizer->symbol() == '.');
	previousSymbol = tokenizer->symbol();
	if (tokenizer->symbol() == '(') {
		if (currentSubroutineType == "constructor") {
			vmWriter->writePush(POINTER, 0);
		}else {
			vmWriter->writePush(ARG, 0);
		}
		advanceTokenizer();
		compileExpressionList();
	}

	advanceTokenizer();
	if (previousSymbol == '.') {
		assert(tokenizer->tokenType() == IDENTIFIER);
		identAfterDot = tokenizer->identifier();
	} else {
		assert(tokenizer->tokenType() == SYMBOL);
		assert(tokenizer->symbol() == ';');
		vmWriter->writeCall(className + "." + identBeforeDot, nArgs + 1);
		vmWriter->writePop(TEMP, 0);
		return;
	}
	
	advanceTokenizer();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '(');
	advanceTokenizer();
	if (symbolTable->kindOf(identBeforeDot) != "none") {
		vmWriter->writePush(segment(symbolTable->kindOf(identBeforeDot)), symbolTable->indexOf(identBeforeDot));
	}
	compileExpressionList();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == ')');

	advanceTokenizer();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == ';');	

	if (symbolTable->kindOf(identBeforeDot) == "none") {
		vmWriter->writeCall(identBeforeDot + "." + identAfterDot, nArgs);
	} else {
		vmWriter->writeCall(symbolTable->typeOf(identBeforeDot) + "." + identAfterDot, nArgs + 1);
	}
	vmWriter->writePop(TEMP, 0);
}

// currentToken should be "let" at this point.
void CompilationEngine::compileLet()
{   // "let"
	assert(tokenizer->tokenType() == KEYWORD);
	assert(tokenizer->keyWord() == "let");
	
	advanceTokenizer(); // variable name
	assert(tokenizer->tokenType() == IDENTIFIER);
	string assignedVariable = tokenizer->identifier();
	
	advanceTokenizer(); // '=' or '['
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '[' || tokenizer->symbol() == '=');
	if (tokenizer->symbol() == '[') {
		vmWriter->writePush(segment(symbolTable->kindOf(assignedVariable)), 
			                symbolTable->indexOf(assignedVariable));
		advanceTokenizer(); // beginning of expression
		compileExpression(); // expression and ']'
		vmWriter->writeArithmetic(ADD);
		vmWriter->writePop(POINTER, 1);
		advanceTokenizer(); // '='
		assert(tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == '=');
		advanceTokenizer();
		compileExpression(); // expression and ';'
		vmWriter->writePop(THAT, 0);
	} else { // '='			
		advanceTokenizer();
		compileExpression(); // expression and ';' or ')'
		if(tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ')') {
			advanceTokenizer();
		}
		vmWriter->writePop(segment(symbolTable->kindOf(assignedVariable)), 
					       symbolTable->indexOf(assignedVariable));
	}
}

//currentToken should be "while" at this point.
void CompilationEngine::compileWhile()
{
	assert(tokenizer->tokenType() == KEYWORD);
	assert(tokenizer->keyWord() == "while");

	vmWriter->writeLabel("WHILE" + iToStr(++whileCount));
	whileStack.push(whileCount);
	
	 // read a '('
	advanceTokenizer();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '(');		
				
	// read an expression and ')'
	advanceTokenizer();
	compileExpression();
	vmWriter->writeArithmetic(NOT);
	vmWriter->writeIf("END_WHILE" + iToStr(whileStack.top()));
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == ')');
				
	// read a '{'
	advanceTokenizer();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '{');		
				
	// read statements and a '}'.
	advanceTokenizer();
	compileStatements();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '}');

	vmWriter->writeGoto("WHILE" + iToStr(whileStack.top()));
	vmWriter->writeLabel("END_WHILE" + iToStr(whileStack.top()));
	whileStack.pop();
}

void CompilationEngine::compileReturn()
{
	assert(tokenizer->tokenType() == KEYWORD);
	assert(tokenizer->keyWord() == "return");
	
	advanceTokenizer();
	if(tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ';') {
		vmWriter->writePush(CONST, 0);
		vmWriter->writeReturn();
		return;
	} else {
		compileExpression();
		assert(tokenizer->tokenType() == SYMBOL);
		assert(tokenizer->symbol() == ';');
		vmWriter->writeReturn();
	}
}

void CompilationEngine::compileIf()
{   // "if"
	assert(tokenizer->tokenType() == KEYWORD);
	assert(tokenizer->keyWord() == "if");

	// read '('
	advanceTokenizer();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '(');

	// read an expression and ')'
	advanceTokenizer();
	compileExpression();
	vmWriter->writeArithmetic(NOT);
	vmWriter->writeIf("IF_FALSE" + iToStr(++ifCount));
	ifStack.push(ifCount);

	// read a '{'
	advanceTokenizer();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '{');

	// read statements and a closing brace.
	advanceTokenizer();
	compileStatements();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '}');
	vmWriter->writeGoto("END_IF" + iToStr(ifStack.top()));
	vmWriter->writeLabel("IF_FALSE" + iToStr(ifStack.top()));

	// read a potential 'else' and return if not 'else'
	advanceTokenizer();
	if (tokenizer->tokenType() != KEYWORD ||
	   (tokenizer->tokenType() == KEYWORD && tokenizer->keyWord() != "else")) {
		vmWriter->writeLabel("END_IF" + iToStr(ifStack.top()));
		ifStack.pop();
		return;
	}

	// read '{'
	advanceTokenizer();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '{');

	// read statements and a closing brace.
	advanceTokenizer();
	compileStatements();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '}');
	vmWriter->writeLabel("END_IF" + iToStr(ifStack.top()));
	ifStack.pop();

	advanceTokenizer();
}

// first token in expression is current token at this point
void CompilationEngine::compileExpression()
{
	char op;

	compileTerm();
	while (true) {
		if (tokenizer->tokenType() != SYMBOL) {
			break;
		}
		if (tokenizer->tokenType() == SYMBOL) {
			op = tokenizer->symbol();
			if (op != '+' && op != '-' && op != '*' && op != '/' && op != '&' &&
				op != '|' && op != '<' && op != '>' && op != '=') {
					break;
			}
		}
		assert(tokenizer->tokenType() == SYMBOL);
		advanceTokenizer();
		compileTerm();
		if (op == '*') { 
			vmWriter->writeCall("Math.multiply", 2);
		} else if(op == '/') {
			vmWriter->writeCall("Math.divide", 2);
		} else {
			vmWriter->writeArithmetic(command(op));
		}
	}
}

void CompilationEngine::compileTerm()
{
	if (tokenizer->tokenType() == INT_CONST) {
		vmWriter->writePush(CONST, tokenizer->intVal());
	} else if (tokenizer->tokenType() == STRING_CONST) {
		vmWriter->writePush(CONST, tokenizer->stringVal().size());
		vmWriter->writeCall("String.new", 1);
		vmWriter->writePop(TEMP, 1);
		for(size_t i = 0; i < tokenizer->stringVal().size(); ++i) {
			vmWriter->writePush(TEMP, 1);
			vmWriter->writePush(CONST, tokenizer->stringVal()[i]);
			vmWriter->writeCall("String.appendChar", 2);
			vmWriter->writePop(TEMP, 1);
		}
		vmWriter->writePush(TEMP, 1);
	} else if (tokenizer->tokenType() == KEYWORD) {
		assert(tokenizer->keyWord() == "true" || tokenizer->keyWord() == "false" ||
			   tokenizer->keyWord() == "null" || tokenizer->keyWord() == "this");
		if (tokenizer->keyWord() == "this") {
			if(currentSubroutineType == "constructor") {
				vmWriter->writePush(POINTER, 0);
			} else {
				vmWriter->writePush(ARG, 0);
			}
		} else if (tokenizer->keyWord() == "true") {
			vmWriter->writePush(CONST, 1);
			vmWriter->writeArithmetic(NEG);
		} else if (tokenizer->keyWord() == "false" || tokenizer->keyWord() == "null") {
			vmWriter->writePush(CONST, 0);
		}
	} else if (tokenizer->tokenType() == IDENTIFIER) {
		string ident = tokenizer->identifier();
		advanceTokenizer();
		assert(tokenizer->tokenType() == SYMBOL);
		if (tokenizer->symbol() == '[') {
			vmWriter->writePush(segment(symbolTable->kindOf(ident)), symbolTable->indexOf(ident));
			advanceTokenizer();
			compileExpression();
			vmWriter->writeArithmetic(ADD);
			vmWriter->writePop(POINTER, 1);
			vmWriter->writePush(THAT, 0);
		} else if (tokenizer->symbol() == '(') {
			advanceTokenizer();
			compileExpressionList();
			vmWriter->writeCall(ident, nArgs);
		} else if (tokenizer->symbol() == '.') {
			advanceTokenizer(); // function name
			assert(tokenizer->tokenType() == IDENTIFIER);
			string functionName = tokenizer->identifier();
			advanceTokenizer(); // '('
			assert(tokenizer->tokenType() == SYMBOL);	
			advanceTokenizer(); 
			compileExpressionList(); // expression list and ')'
			if (symbolTable->kindOf(ident) == "none") {
				vmWriter->writeCall(ident + "." + functionName, nArgs);
			} else {
				vmWriter->writePush(ARG, 0);
				vmWriter->writeCall(symbolTable->typeOf(ident) + "." + functionName, nArgs + 1);
			}
		} else {
			vmWriter->writePush(segment(symbolTable->kindOf(ident)), symbolTable->indexOf(ident));
			return;
		}
	} else if (tokenizer->tokenType() == SYMBOL) {
		if (tokenizer->symbol() == '(') {				
			advanceTokenizer();
			compileExpression();			
		} else {
			assert(tokenizer->symbol() == '-' || tokenizer->symbol() == '~');		
			char binaryOp = tokenizer->symbol();
			advanceTokenizer();
			compileTerm();
			if (binaryOp == '-') {
				vmWriter->writeArithmetic(NEG);
			} else { // assumes '~'
				vmWriter->writeArithmetic(NOT);
			}
			return;
		}
	}
	
	advanceTokenizer();
}

void CompilationEngine::compileExpressionList()
{
	nArgs = 0;

	if (tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ')') {
		return;
	}

	compileExpression();
	while(true) {
		nArgs++;
		assert(tokenizer->tokenType() == SYMBOL) ;
		if (tokenizer->symbol() == ')'){
			return;
		} else { // ','
			assert(tokenizer->symbol() == ',');
			advanceTokenizer();
			compileExpression();
		}
	}
}
Segment CompilationEngine::segment(string seg)
{
	assert(seg == "static" || seg == "field" || seg == "arg" || seg == "var");
	Segment aSegment;
	if (seg == "static") {
		aSegment = STATIC;
	} else if (seg == "field") {
		aSegment = THIS;
	} else if (seg ==  "arg") {
		aSegment = ARG;
	} else if (seg == "var") {
		aSegment = LOCAL;
	}

	return aSegment;		
}

Command CompilationEngine::command(char op)
{
	Command com = ADD;
	
	switch (op) {
		case '+': com = ADD; break;
		case '-': com = SUB; break;
		case '&': com = AND; break;
		case '|': com = OR; break;
		case '<': com = LT;	break;
		case '>': com = GT;	break;
		case '=': com = EQ;	break;
	}

	return com;
}

string CompilationEngine::iToStr(size_t aNumber)
{
    stringstream out;
    string str;

    out << aNumber;
    out >> str;

    return str;
}