#include "CompilationEngine.h"
#include <cassert>
#include <iostream>
using namespace std;

CompilationEngine::CompilationEngine(JackTokenizer *tokenizer_, ofstream& outfile_) :
					tokenizer(tokenizer_), outfile(outfile_)
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

	outputSymbol();
	outfile << "</class>\n";

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

	string subroutineType = tokenizer->keyWord();

	advanceTokenizer(); // type 
	assert(tokenizer->tokenType() == KEYWORD || tokenizer->tokenType() == IDENTIFIER);
	
	advanceTokenizer(); //name of subroutine
	assert(tokenizer->tokenType() == IDENTIFIER);
	subroutineName = tokenizer->identifier();
	
	advanceTokenizer(); // '('
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '(');
	
	advanceTokenizer();
	if (subroutineType == "method") {
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
	if (subroutineType == "method") {
		vmWriter->writePush(ARG, 0);
		vmWriter->writePop(POINTER, 0);
	} else if (subroutineType == "constructor") {
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
	string ident;

	int i = 0;
	while (i < 5) {
		advanceTokenizer();
		switch (i) {
			case 0:
				assert(tokenizer->tokenType() == IDENTIFIER);
				ident = tokenizer->identifier();
				break;
			case 1:
				assert(tokenizer->tokenType() == SYMBOL);
				assert(tokenizer->symbol() == '(' || tokenizer->symbol() == '.');
				previousSymbol = tokenizer->symbol();
				if (tokenizer->symbol() == '(') {
					vmWriter->writePush(POINTER, 0);
					advanceTokenizer();
					compileExpressionList();
				}
				break;
			case 2:
				if (previousSymbol == '.') {
					assert(tokenizer->tokenType() == IDENTIFIER);
					outfile << "<identifier> " << tokenizer->identifier()
							<< " </identifier>\n";
				} else {
					assert(tokenizer->tokenType() == SYMBOL);
					assert(tokenizer->symbol() == ';');
					vmWriter->writeCall(className + "." + ident, nArgs);
					vmWriter->writePop(TEMP, 0);
					return;
				}
				break;
			case 3:
					assert(tokenizer->tokenType() == SYMBOL);
					assert(tokenizer->symbol() == '(');
					outputSymbol();
					advanceTokenizer();
					compileExpressionList();
					assert(tokenizer->tokenType() == SYMBOL);
					assert(tokenizer->symbol() == ')');
					outputSymbol();
				break;
			case 4:
					assert(tokenizer->tokenType() == SYMBOL);
					assert(tokenizer->symbol() == ';');
					outputSymbol();						
				break;
		}
		i++;
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
		advanceTokenizer(); // beginning of expression
		compileExpression(); // expression and ']'
		advanceTokenizer(); // '='
		assert(tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == '=');
		advanceTokenizer();
		compileExpression(); // expression and ';'
	} else { // '='			
		advanceTokenizer();
		compileExpression(); // expression and ';' or ')'
		if(tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ')') {
			advanceTokenizer();
		}			
	}

	vmWriter->writePop(segment(symbolTable->kindOf(assignedVariable)), 
					   symbolTable->indexOf(assignedVariable));
}

//currentToken should be "while" at this point.
void CompilationEngine::compileWhile()
{
	assert(tokenizer->tokenType() == KEYWORD);
	assert(tokenizer->keyWord() == "while");
	outfile << "<whileStatement>\n";
	
	outfile << "<keyword> " << tokenizer->keyWord()
		    << " </keyword>\n";


	int i = 0;
	while (i < 4) {
		advanceTokenizer();
		switch (i) {
			case 0: // read a '('
				assert(tokenizer->tokenType() == SYMBOL);
				assert(tokenizer->symbol() == '(');
				outputSymbol();						
				break;
			case 1: // read an expression
				compileExpression();
				assert(tokenizer->tokenType() == SYMBOL);
				assert(tokenizer->symbol() == ')');
				outputSymbol();	
				break;
			case 2: // read a '{'
				assert(tokenizer->tokenType() == SYMBOL);
				assert(tokenizer->symbol() == '{');
				outputSymbol();						
				break;
			case 3: // read statements and a '}'.
				compileStatements();
				assert(tokenizer->tokenType() == SYMBOL);
				assert(tokenizer->symbol() == '}');
				outputSymbol();						
				break;
		}
		i++;
	}

	
	outfile << "</whileStatement>\n";

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
{
	assert(tokenizer->tokenType() == KEYWORD);
	assert(tokenizer->keyWord() == "if");
	outfile << "<ifStatement>\n";
	
	outfile << "<keyword> " << tokenizer->keyWord()
		    << " </keyword>\n";

	int i = 0;
	while (i < 7) {
		advanceTokenizer();
		switch (i) {
			case 0: // read a '('
				assert(tokenizer->tokenType() == SYMBOL);
				assert(tokenizer->symbol() == '(');
				outputSymbol();						
				break;
			case 1: // read an expression and ')'
				compileExpression();
				outputSymbol();
				break;
			case 2: // read a '{'
				assert(tokenizer->tokenType() == SYMBOL);
				assert(tokenizer->symbol() == '{');
				outputSymbol();						
				break;
			case 3: // read statements and a closing brace.
				compileStatements();
				assert(tokenizer->tokenType() == SYMBOL);
				assert(tokenizer->symbol() == '}');
				outputSymbol();
				break;
			case 4: // read a potential 'else' and return if not 'else'
				if (tokenizer->tokenType() == KEYWORD && tokenizer->keyWord() == "else") {
					outfile << "<keyword> " << tokenizer->keyWord()
							<< " </keyword>\n";
				} else {
					
					outfile << "</ifStatement>\n";
					return;
				}
				break;
			case 5: // read '{'
				assert(tokenizer->tokenType() == SYMBOL);
				assert(tokenizer->symbol() == '{');
				outputSymbol();						
				break;
			case 6: // read statements and a closing brace.
				compileStatements();
				assert(tokenizer->tokenType() == SYMBOL);
				assert(tokenizer->symbol() == '}');
				outputSymbol();						
				break;
		}
		i++;
	}

	
	advanceTokenizer();
	
	outfile << "</ifStatement>\n";
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
		vmWriter->writeArithmetic(command(op));
	}
}

void CompilationEngine::compileTerm()
{
	if (tokenizer->tokenType() == INT_CONST) {
		vmWriter->writePush(CONST, tokenizer->intVal());
	} else if (tokenizer->tokenType() == STRING_CONST) {
		outfile << "<stringConstant> " << tokenizer->stringVal()
				<< " </stringConstant>\n";
	} else if (tokenizer->tokenType() == KEYWORD) {
		assert(tokenizer->keyWord() == "true" || tokenizer->keyWord() == "false" ||
			   tokenizer->keyWord() == "null" || tokenizer->keyWord() == "this");
		if (tokenizer->keyWord() == "this") {
			vmWriter->writePush(POINTER, 0);
		}
	} else if (tokenizer->tokenType() == IDENTIFIER) {
		string ident = tokenizer->identifier();
		advanceTokenizer();
		assert(tokenizer->tokenType() == SYMBOL);
		if (tokenizer->symbol() == '[') {
			outfile << "<identifier> " << ident << " </identifier>\n";
			outputSymbol();
			advanceTokenizer();
			compileExpression();
			outputSymbol();
		} else if (tokenizer->symbol() == '(') {
			outfile << "<identifier> " << ident << " </identifier>\n";
			outputSymbol();
			advanceTokenizer();
			compileExpressionList();
			outputSymbol();					
		} else if (tokenizer->symbol() == '.') {
			outfile << "<identifier> " << ident << " </identifier>\n";
			outputSymbol();					
			advanceTokenizer();
			assert(tokenizer->tokenType() == IDENTIFIER);
			outfile << "<identifier> " << tokenizer->identifier()
				    << " </identifier>\n";
			advanceTokenizer();
			assert(tokenizer->tokenType() == SYMBOL);
			outputSymbol();	
			advanceTokenizer();
			compileExpressionList();
			outputSymbol();					
		} else {
			vmWriter->writePush(segment(symbolTable->kindOf(ident)), symbolTable->indexOf(ident));
			return;
		}
	} else if (tokenizer->tokenType() == SYMBOL) {
		if (tokenizer->symbol() == '(') {
			outputSymbol();					
			advanceTokenizer();
			compileExpression();
			outputSymbol();					
		} else {
			assert(tokenizer->symbol() == '-' || tokenizer->symbol() == '~');
			outputSymbol();					
			advanceTokenizer();
			compileTerm();
			
			outfile << "</term>\n";
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

void CompilationEngine::outputSymbol()
{
	outfile << "<symbol> ";
	string str;
	char ch = tokenizer->symbol();
	str = ch;
	if (ch == '<' || ch == '>' || ch == '&') {
		if (ch == '<') {
			str = "&lt;";
		} else if (ch == '>') {
			str = "&gt;";
		} else {
			str = "&amp;";
		}
	}
	outfile << str;
	outfile << " </symbol>\n";
}

void CompilationEngine::printCurrentToken()
{
	cout << "\ncurrent token: ";
	switch(tokenizer->tokenType()) {
		case SYMBOL:
			cout << "SYMBOL " << tokenizer->symbol() << endl;
			break;
		case IDENTIFIER:
			cout << "IDENTIFIER " << tokenizer->identifier() << endl;
			break;
		case KEYWORD:
			cout << "KEYWORD " << tokenizer->keyWord() << endl;
			break;
		case INT_CONST:
			cout << "INT_CONST " << tokenizer->intVal() << endl;
			break;
		case STRING_CONST:
			cout << "STRING_CONST " << tokenizer->stringVal() << endl;
			break;
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
		case '+':
			com = ADD;
			break;
		case '-':
			com = SUB;
			break;
		case '&':
			com = AND;
			break;
		case '|':
			com = OR;
			break;
		case '<':
			com = LT;
			break;
		case '>':
			com = GT;
			break;
		case '=':
			com = EQ;
			break;
	}

	return com;
}