#include "CompilationEngine.h"
#include <cassert>
#include <iostream>
using namespace std;
/* potential optimization: (instead of writing "if(!tokenizer->hasMoreTokens()) return; tokenizer->advance();" everywhere)

int i = 0;
while (i < 3) {
    if (!tokenizer->hasMoreTokens()) return;
	tokenizer->advance();
	switch (i) {
		case 0:
			//do the first thing
			break;
		case 1:
			// do the second thing
			break;
		case 2:
			// do the third thing
			break;
	}
	i++;
}

*/

CompilationEngine::CompilationEngine(JackTokenizer *tokenizer_, ofstream& outfile_) :
					tokenizer(tokenizer_), outfile(outfile_), indentCount(0)
{
}


CompilationEngine::~CompilationEngine(void)
{
}

void CompilationEngine::compileClass()
{
	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();
	assert(tokenizer->tokenType() == KEYWORD);
	assert(tokenizer->keyWord() == "class");
	outfile << "<class>\n";
	indentCount++;
	outfile << string(indentCount * 2, ' ') << "<keyword> "
		    << tokenizer->keyWord() << " </keyword>\n";
	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();
	assert(tokenizer->tokenType() == IDENTIFIER);
	outfile << string(indentCount * 2, ' ') << "<identifier> "
		    << tokenizer->identifier() << " </identifier>\n";
	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '{');
	outputSymbol();
	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();

	// classVarDec*
	while (true) {
		if (!tokenizer->hasMoreTokens()) {
			return;
		}
		assert(tokenizer->tokenType() == KEYWORD ||
			   tokenizer->tokenType() == SYMBOL);
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

//precondition: tokenizer has already been advanced.
void CompilationEngine::compileClassVarDec()
{
	outfile << string(indentCount * 2, ' ') << "<classVarDec>\n";
	indentCount++;
	outfile << string(indentCount * 2, ' ') << "<keyword> " << tokenizer->keyWord()
		    << " </keyword>\n";
	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();
	TokenType t_type = tokenizer->tokenType();
	assert(t_type == KEYWORD || t_type == IDENTIFIER);
	if (t_type == KEYWORD) {
		string keyword = tokenizer->keyWord();
		assert(keyword == "int" || keyword == "char" || keyword == "boolean");
		outfile << string(indentCount * 2, ' ') << "<keyword> " << keyword << " </keyword>\n";
	} else {
		outfile << string(indentCount * 2, ' ') << "<identifier> " << tokenizer->identifier()
			    << " </identifier>\n";
	}
	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();
	assert(tokenizer->tokenType() == IDENTIFIER);
	outfile << string(indentCount * 2, ' ') << "<identifier> " << tokenizer->identifier()
		    << " </identifier>\n";
	while (true) {
		if (!tokenizer->hasMoreTokens()) {
			return;
		}
		tokenizer->advance();
		assert(tokenizer->tokenType() == SYMBOL);
		if (tokenizer->symbol() == ';') {
			outputSymbol();
			break;
		} else {
			assert(tokenizer->symbol() == ',');
			outputSymbol();
		}
		if (!tokenizer->hasMoreTokens()) {
			return;
		}
		tokenizer->advance();
		assert(tokenizer->tokenType() == IDENTIFIER);
		outfile << string(indentCount * 2, ' ') << "<identifier> " << tokenizer->identifier()
			    << " </identifier>\n";
	}
	indentCount--;
	outfile << string(indentCount * 2, ' ') << "</classVarDec>\n";

	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();
}

//precondition: tokenizer has already been advanced.
void CompilationEngine::compileSubroutine()
{
	outfile << string(indentCount * 2, ' ') << "<subroutineDec>\n";
	indentCount++;
	outfile << string(indentCount * 2, ' ') << "<keyword> " << tokenizer->keyWord()
		    << " </keyword>\n";
	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();
	assert(tokenizer->tokenType() == KEYWORD || tokenizer->tokenType() == IDENTIFIER);
	if (tokenizer->tokenType() == KEYWORD) {
		assert(tokenizer->keyWord() == "void");
		outfile << string(indentCount * 2, ' ') << "<keyword> " << tokenizer->keyWord()
		        << " </keyword>\n";
	} else {
		outfile << string(indentCount * 2, ' ') << "<identifier> " << tokenizer->identifier()
		        << " </identifier>\n";
	}
	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();
	assert(tokenizer->tokenType() == IDENTIFIER);
	outfile << string(indentCount * 2, ' ') << "<identifier> " << tokenizer->identifier()
	        << " </identifier>\n";
	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '(');
	outputSymbol();
	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();

	compileParameterList();

	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == ')');
	outputSymbol();    
	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();

	//subroutine body
	outfile << string(indentCount * 2, ' ') << "<subroutineBody>\n";
	indentCount++;
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '{');
	outputSymbol();
	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();

	// varDec*
	while (true) {
		assert(tokenizer->tokenType() == KEYWORD || tokenizer->tokenType() == SYMBOL);
		if (tokenizer->keyWord() == "var") {
			compileVarDec();
		} else {
			break;
		}
	}

	//statements
	compileStatements();

	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '}');
	outputSymbol();

	indentCount--;
	outfile << string(indentCount * 2, ' ') << "</subroutineBody>\n";
	indentCount--;
	outfile << string(indentCount * 2, ' ') << "</subroutineDec>\n";

	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();
}

void CompilationEngine::compileParameterList()
{
	TokenType t_type = tokenizer->tokenType();
	assert(t_type == SYMBOL || t_type == KEYWORD || t_type == IDENTIFIER);
	outfile << string(indentCount * 2, ' ') << "<parameterList>\n";
	indentCount++;

	if (t_type == SYMBOL) {
		indentCount--;
		outfile << string(indentCount * 2, ' ') << "</parameterList>\n";
		return;
	}
	if (t_type == KEYWORD) {
		assert(tokenizer->keyWord() == "int" || tokenizer->keyWord() == "char" ||
			   tokenizer->keyWord() == "boolean");
		outfile << string(indentCount * 2, ' ') << "<keyword> " << tokenizer->keyWord()
		        << " </keyword>\n";
	} else {
		outfile << string(indentCount * 2, ' ') << "<identifier> " << tokenizer->identifier()
		        << " </identifier>\n";
	}
	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();
	assert(tokenizer->tokenType() == IDENTIFIER);
	outfile << string(indentCount * 2, ' ') << "<identifier> " << tokenizer->identifier()
	        << " </identifier>\n";
	while (true) {
		if (!tokenizer->hasMoreTokens()) {
			return;
		}
		tokenizer->advance();
		assert(tokenizer->tokenType() == SYMBOL);
		assert(tokenizer->symbol() == ',' || tokenizer->symbol() == ')');
		if (tokenizer->symbol() == ')') {
			break;
		} else {
			outputSymbol(); 
			if (!tokenizer->hasMoreTokens()) {
				return;
			}
			tokenizer->advance();
			assert(tokenizer->tokenType() == KEYWORD || tokenizer->tokenType() == IDENTIFIER);
			if (t_type == KEYWORD) {
				assert(tokenizer->keyWord() == "int" || tokenizer->keyWord() == "char" ||
					   tokenizer->keyWord() == "boolean");
				outfile << string(indentCount * 2, ' ') << "<keyword> " << tokenizer->keyWord()
						<< " </keyword>\n";
			} else {
				outfile << string(indentCount * 2, ' ') << "<identifier> " << tokenizer->identifier()
						<< " </identifier>\n";
			}
			if (!tokenizer->hasMoreTokens()) {
				return;
			}
			tokenizer->advance();
			assert(tokenizer->tokenType() == IDENTIFIER);
			outfile << string(indentCount * 2, ' ') << "<identifier> " << tokenizer->identifier()
			        << " </identifier>\n";
		}
	}
	indentCount--;
	outfile << string(indentCount * 2, ' ') << "</parameterList>\n";
}

void CompilationEngine::compileVarDec()
{
	assert(tokenizer->tokenType() == KEYWORD);
	assert(tokenizer->keyWord() == "var");
	outfile << string(indentCount * 2, ' ') << "<varDec>\n";
	indentCount++;
	outfile << string(indentCount * 2, ' ') << "<keyword> " << tokenizer->keyWord()
			<< " </keyword>\n";
	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();
	assert(tokenizer->tokenType() == KEYWORD || tokenizer->tokenType() == IDENTIFIER);
	if (tokenizer->tokenType() == KEYWORD) {
		assert(tokenizer->keyWord() == "int" || tokenizer->keyWord() == "char" ||
			   tokenizer->keyWord() == "boolean");
		outfile << string(indentCount * 2, ' ') << "<keyword> " << tokenizer->keyWord()
		        << " </keyword>\n";
	} else {
		outfile << string(indentCount * 2, ' ') << "<identifier> " << tokenizer->identifier()
		        << " </identifier>\n";
	}
	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();
	assert(tokenizer->tokenType() == IDENTIFIER);
	outfile << string(indentCount * 2, ' ') << "<identifier> " << tokenizer->identifier()
	        << " </identifier>\n";
	while (true) {
		if (!tokenizer->hasMoreTokens()) {
			return;
		}
		tokenizer->advance();
		assert(tokenizer->tokenType() == SYMBOL);
		assert(tokenizer->symbol() == ',' || tokenizer->symbol() == ';');
		if (tokenizer->symbol() == ';') {
			outputSymbol();
			break;
		} else { // ,
			outputSymbol();
			if (!tokenizer->hasMoreTokens()) {
				return;
			}
			tokenizer->advance();
			assert(tokenizer->tokenType() == IDENTIFIER);
			outfile << string(indentCount * 2, ' ') << "<identifier> " << tokenizer->identifier()
					<< " </identifier>\n";
		}
	}
	indentCount--;
	outfile << string(indentCount * 2, ' ') << "</varDec>\n";
	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();
}

void CompilationEngine::compileStatements()
{
	outfile << string(indentCount * 2, ' ') << "<statements>\n";
	indentCount++;
	while (true) {
		if (tokenizer->tokenType() == SYMBOL) {
			assert(tokenizer->symbol() == '}');
			indentCount--;
			outfile << string(indentCount * 2, ' ') << "</statements>\n";
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
		if (!tokenizer->hasMoreTokens()) {
			return;
		}
		if (keyword != "if") {
			tokenizer->advance();
		}
	}
}

//currentToken should be "do" at this point.
void CompilationEngine::compileDo()
{
	assert(tokenizer->tokenType() == KEYWORD);
	assert(tokenizer->keyWord() == "do");
	outfile << string(indentCount * 2, ' ') << "<doStatement>\n";
	indentCount++;
	outfile << string(indentCount * 2, ' ') << "<keyword> " << tokenizer->keyWord()
		    << " </keyword>\n";
	char previousSymbol;

	int i = 0;
	while (i < 5) {
			if (!tokenizer->hasMoreTokens()) return;
			tokenizer->advance();
		switch (i) {
			case 0:
				assert(tokenizer->tokenType() == IDENTIFIER);
				outfile << string(indentCount * 2, ' ') << "<identifier> " << tokenizer->identifier()
						<< " </identifier>\n";
				break;
			case 1:
				assert(tokenizer->tokenType() == SYMBOL);
				assert(tokenizer->symbol() == '(' || tokenizer->symbol() == '.');
				previousSymbol = tokenizer->symbol();
				outputSymbol();
				if (tokenizer->symbol() == '(') {
					compileExpressionList();
					outputSymbol();
				}
				break;
			case 2:
				if (previousSymbol == '.') {
					assert(tokenizer->tokenType() == IDENTIFIER);
					outfile << string(indentCount * 2, ' ') << "<identifier> " << tokenizer->identifier()
							<< " </identifier>\n";
				} else {
					assert(tokenizer->tokenType() == SYMBOL);
					assert(tokenizer->symbol() == ';');
					outputSymbol();
					indentCount--;
					outfile << string(indentCount * 2, ' ') << "</doStatement>\n";
					return;
				}
				break;
			case 3:
					assert(tokenizer->tokenType() == SYMBOL);
					assert(tokenizer->symbol() == '(');
					outputSymbol();
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

	indentCount--;
	outfile << string(indentCount * 2, ' ') << "</doStatement>\n";
}

// currentToken should be "let" at this point.
void CompilationEngine::compileLet()
{
	assert(tokenizer->tokenType() == KEYWORD);
	assert(tokenizer->keyWord() == "let");
	outfile << string(indentCount * 2, ' ') << "<letStatement>\n";
	indentCount++;
	outfile << string(indentCount * 2, ' ') << "<keyword> " << tokenizer->keyWord()
		    << " </keyword>\n";
	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();
	assert(tokenizer->tokenType() == IDENTIFIER);
	outfile << string(indentCount * 2, ' ') << "<identifier> " << tokenizer->identifier()
			<< " </identifier>\n";
	if (!tokenizer->hasMoreTokens()) {
		return;
	}
	tokenizer->advance();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '[' || tokenizer->symbol() == '=');
	if (tokenizer->symbol() == '[') {
		outputSymbol();				
		compileExpression();
		outputSymbol();
				
	} else { // '='
		outputSymbol(); // =				
		if (!tokenizer->hasMoreTokens()) {
			return;
		}
		tokenizer->advance();
		compileExpression();
		outputSymbol();				
	}
	indentCount--;
	outfile << string(indentCount * 2, ' ') << "</letStatement>\n";
}

//currentToken should be "while" at this point.
void CompilationEngine::compileWhile()
{
	assert(tokenizer->tokenType() == KEYWORD);
	assert(tokenizer->keyWord() == "while");
	outfile << string(indentCount * 2, ' ') << "<whileStatement>\n";
	indentCount++;
	outfile << string(indentCount * 2, ' ') << "<keyword> " << tokenizer->keyWord()
		    << " </keyword>\n";


	int i = 0;
	while (i < 5) {
			if (!tokenizer->hasMoreTokens()) return;
			tokenizer->advance();
		switch (i) {
			case 0: // read a '('
				assert(tokenizer->tokenType() == SYMBOL);
				assert(tokenizer->symbol() == '(');
				outputSymbol();						
				break;
			case 1: // read an expression
				compileExpression();
				break;
			case 2: // read a ')'
				assert(tokenizer->tokenType() == SYMBOL);
				assert(tokenizer->symbol() == ')');
				outputSymbol();						
				break;
			case 3: // read a '{'
				assert(tokenizer->tokenType() == SYMBOL);
				assert(tokenizer->symbol() == '{');
				outputSymbol();						
				break;
			case 4: // read statements and a '}'.
				compileStatements();
				assert(tokenizer->tokenType() == SYMBOL);
				assert(tokenizer->symbol() == '}');
				outputSymbol();						
				break;
		}
		i++;
	}

	indentCount++;
	outfile << string(indentCount * 2, ' ') << "</whileStatement>\n";

}

void CompilationEngine::compileReturn()
{
	assert(tokenizer->tokenType() == KEYWORD);
	assert(tokenizer->keyWord() == "return");
	outfile << string(indentCount * 2, ' ') << "<returnStatement>\n";
	indentCount++;
	outfile << string(indentCount * 2, ' ') << "<keyword> " << tokenizer->keyWord()
		    << " </keyword>\n";

	if (!tokenizer->hasMoreTokens()) return;
	tokenizer->advance();

	if(tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ';') {
		outputSymbol();				
		indentCount--;
		outfile << string(indentCount * 2, ' ') << "</returnStatement>\n";
		return;
	} else {
		compileExpression();
		assert(tokenizer->tokenType() == SYMBOL);
		assert(tokenizer->symbol() == ';');
		outputSymbol();				
	}

	indentCount--;
	outfile << string(indentCount * 2, ' ') << "</returnStatement>\n";

}

// current token is "if" at this point
// condition on exit is that the token after the if-else is the current token:
// for example, given: "if(x) { y; } else { z; } let x = y;" the current token
// upon exiting this function will be 'let'
void CompilationEngine::compileIf()
{
	assert(tokenizer->tokenType() == KEYWORD);
	assert(tokenizer->keyWord() == "if");
	outfile << string(indentCount * 2, ' ') << "<ifStatement>\n";
	indentCount++;
	outfile << string(indentCount * 2, ' ') << "<keyword> " << tokenizer->keyWord()
		    << " </keyword>\n";

	int i = 0;
	while (i < 7) {
			if (!tokenizer->hasMoreTokens()) return;
			tokenizer->advance();
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
				if (tokenizer->tokenType() == KEYWORD && tokenizer->keyWord() != "else") {
					indentCount--;
					outfile << string(indentCount * 2, ' ') << "</ifStatement>\n";
					return;
				} else {
					outfile << string(indentCount * 2, ' ') << "<keyword> " << tokenizer->keyWord()
							<< " </keyword>\n";
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

	indentCount--;
	outfile << string(indentCount * 2, ' ') << "</ifStatement>\n";
}

// first token in expression is current token at this point (at least for a return)
void CompilationEngine::compileExpression()
{
	outfile << string(indentCount * 2, ' ') << "<expression>\n";
	indentCount++;

	compileTerm();
	while (true) {
		if (tokenizer->tokenType() != SYMBOL) {
			if (!tokenizer->hasMoreTokens()) return;
			tokenizer->advance();
		}
		if (tokenizer->tokenType() != SYMBOL) {
			break;
		}
		if (tokenizer->tokenType() == SYMBOL) {
			char c = tokenizer->symbol();
			if (c != '+' && c != '-' && c != '*' && c != '/' && c != '&' &&
				c != '|' && c != '<' && c != '>' && c != '=') {
					break;
			}
		}
		assert(tokenizer->tokenType() == SYMBOL);
		outputSymbol();				
		if (!tokenizer->hasMoreTokens()) return;
		tokenizer->advance();
		compileTerm();
	}


	indentCount--;
	outfile << string(indentCount * 2, ' ') << "</expression>\n";
}
void CompilationEngine::compileTerm()
{
	outfile << string(indentCount * 2, ' ') << "<term>\n";
	indentCount++;

	if (tokenizer->tokenType() == INT_CONST) {
		outfile << string(indentCount * 2, ' ') << "<integerConstant> " << tokenizer->intVal()
				<< " </integerConstant>\n";
	} else if (tokenizer->tokenType() == STRING_CONST) {
		outfile << string(indentCount * 2, ' ') << "<stringConstant> " << tokenizer->stringVal()
				<< " </stringConstant>\n";
	} else if (tokenizer->tokenType() == KEYWORD) {
		assert(tokenizer->keyWord() == "true" || tokenizer->keyWord() == "false" ||
			   tokenizer->keyWord() == "null" || tokenizer->keyWord() == "this");
		outfile << string(indentCount * 2, ' ') << "<keyword> " << tokenizer->keyWord()
				<< " </keyword>\n";
	} else if (tokenizer->tokenType() == IDENTIFIER) {
		string ident = tokenizer->identifier();
		if (!tokenizer->hasMoreTokens()) return;
		tokenizer->advance();
		assert(tokenizer->tokenType() == SYMBOL);
		if (tokenizer->symbol() == '[') {
			outfile << string(indentCount * 2, ' ') << "<identifier> " << ident << " </identifier>\n";
			outputSymbol();					
			compileExpression();
			outputSymbol();
					
		} else if (tokenizer->symbol() == '(') {
			outfile << string(indentCount * 2, ' ') << "<identifier> " << ident << " </identifier>\n";
			outputSymbol();					
			compileExpressionList();
			outputSymbol();					
		} else if (tokenizer->symbol() == '.') {
			outfile << string(indentCount * 2, ' ') << "<identifier> " << ident << " </identifier>\n";
			outputSymbol();					
			if (!tokenizer->hasMoreTokens()) return;
			tokenizer->advance();
			assert(tokenizer->tokenType() == IDENTIFIER);
			outfile << string(indentCount * 2, ' ') << "<identifier> " << tokenizer->identifier()
				    << " </identifier>\n";
			if (!tokenizer->hasMoreTokens()) return;
			tokenizer->advance();
			assert(tokenizer->tokenType() == SYMBOL);
			outputSymbol();					
			compileExpressionList();
			outputSymbol();					
		} else {
			outfile << string(indentCount * 2, ' ') << "<identifier> " << ident << " </identifier>\n";
		}
	} else if (tokenizer->tokenType() == SYMBOL) {
		if (tokenizer->symbol() == '(') {
			outputSymbol();					
			if (!tokenizer->hasMoreTokens()) return;
			tokenizer->advance();
			compileExpression();
			outputSymbol();					
			if (!tokenizer->hasMoreTokens()) return;
			tokenizer->advance();

		} else {
			assert(tokenizer->symbol() == '-' || tokenizer->symbol() == '~');
			outputSymbol();					
			if (!tokenizer->hasMoreTokens()) return;
			tokenizer->advance();
			compileTerm();
		}
	}

	indentCount--;
	outfile << string(indentCount * 2, ' ') << "</term>\n";
}

void CompilationEngine::compileExpressionList()
{
	outfile << string(indentCount * 2, ' ') << "<expressionList>\n";
	indentCount++;

	if (!tokenizer->hasMoreTokens()) return;
	tokenizer->advance();

	if (tokenizer->tokenType() == SYMBOL) {
		assert(tokenizer->symbol() == ')');
		indentCount--;
		outfile << string(indentCount * 2, ' ') << "</expressionList>\n";
		return;
	}

	compileExpression();

	while(true) {
		assert(tokenizer->tokenType() == SYMBOL) ;
		if (tokenizer->symbol() == ')'){
			indentCount--;
			outfile << string(indentCount * 2, ' ') << "</expressionList>\n";
			return;
		} else { // ','
			assert(tokenizer->symbol() == ',');
			outputSymbol();
			if (!tokenizer->hasMoreTokens()) return;
			tokenizer->advance();
			compileExpression();
		}
	}

	indentCount--;
	outfile << string(indentCount * 2, ' ') << "</expressionList>\n";
}

void CompilationEngine::outputSymbol()
{
	outfile << string(indentCount * 2, ' ') << "<symbol> ";
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