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
	outfile << string(indentCount * 2, ' ') << "<symbol> " 
		    << tokenizer->symbol() << " </symbol>\n";
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
		if (!tokenizer->hasMoreTokens()) { 
			return;
		}
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
	outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
		    << " </symbol>\n";
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
			outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
				    << " </symbol>\n";
			break;
		} else {
			assert(tokenizer->symbol() == ',');
			outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
				    << " </symbol>\n";
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
	outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
		    << " </symbol>\n";
	if (!tokenizer->hasMoreTokens()) { 
		return;
	}
	tokenizer->advance();
	
	compileParameterList();

	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == ')');
	outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
		    << " </symbol>\n";
	if (!tokenizer->hasMoreTokens()) { 
		return;
	}
	tokenizer->advance();
	
	//subroutine body
	outfile << string(indentCount * 2, ' ') << "<subroutineBody>\n";
	indentCount++;
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->symbol() == '{');
	outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
		    << " </symbol>\n";
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
	outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
		    << " </symbol>\n";
	
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
			outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
				    << " </symbol>\n";
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
			outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
	        << " </symbol>\n";
			break;
		} else { // ,
			outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
				    << " </symbol>\n";
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
		tokenizer->advance();
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
				outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
				    << " </symbol>\n";
				if (tokenizer->symbol() == '(') {
					compileExpressionList();
					outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
				    << " </symbol>\n";
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
					outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
							<< " </symbol>\n";
					indentCount--;
					outfile << string(indentCount * 2, ' ') << "</doStatement>\n";
					return;
				}
				break;
			case 3:
					assert(tokenizer->tokenType() == SYMBOL);
					assert(tokenizer->symbol() == '(');
					outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
							<< " </symbol>\n";
					compileExpressionList();
					assert(tokenizer->tokenType() == SYMBOL);
					assert(tokenizer->symbol() == ')');
					outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
							<< " </symbol>\n";		
				
				break;
			case 4:
					assert(tokenizer->tokenType() == SYMBOL);
					assert(tokenizer->symbol() == ';');
					outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
						<< " </symbol>\n";				
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
		outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
				<< " </symbol>\n";
		compileExpression();
		outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
				<< " </symbol>\n";
	} else { // '='
		outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol() // =
				<< " </symbol>\n";
		if (!tokenizer->hasMoreTokens()) { 
			return;
		}
		tokenizer->advance();
		compileExpression();
		if (!tokenizer->hasMoreTokens()) return;
		tokenizer->advance();
		outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
				<< " </symbol>\n";
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
	if (!tokenizer->hasMoreTokens()) { 
		return;
	}
	tokenizer->advance();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->keyWord() == "(");
	outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
			<< " </symbol>\n";
	compileExpression();
	if (!tokenizer->hasMoreTokens()) { 
		return;
	}
	tokenizer->advance();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->keyWord() == ")");
	outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
			<< " </symbol>\n";
	if (!tokenizer->hasMoreTokens()) { 
		return;
	}
	tokenizer->advance();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->keyWord() == "{");
	outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
			<< " </symbol>\n";
	compileStatements();
	if (!tokenizer->hasMoreTokens()) { 
		return;
	}
	tokenizer->advance();
	assert(tokenizer->tokenType() == SYMBOL);
	assert(tokenizer->keyWord() == "}");
	outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
			<< " </symbol>\n";
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
		outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
				<< " </symbol>\n";
		indentCount--;
		outfile << string(indentCount * 2, ' ') << "</returnStatement>\n";
		return;
	} else {
		compileExpression();
		if (!tokenizer->hasMoreTokens()) return;
		tokenizer->advance();
		assert(tokenizer->tokenType() == SYMBOL);
		assert(tokenizer->symbol() == ';');
		outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
				<< " </symbol>\n";
				
	}				

	indentCount--;
	outfile << string(indentCount * 2, ' ') << "</returnStatement>\n";	

}

void CompilationEngine::compileIf()
{

}

void CompilationEngine::compileExpression()
{
	outfile << string(indentCount * 2, ' ') << "<expression>\n";
	indentCount++;
	compileTerm();
	while(true) {
		break;
	}


	indentCount--;
	outfile << string(indentCount * 2, ' ') << "</expression>\n";
}
void CompilationEngine::compileTerm()
{
	outfile << string(indentCount * 2, ' ') << "<term>\n";
	indentCount++;

	assert(tokenizer->tokenType() == IDENTIFIER);
	outfile << string(indentCount * 2, ' ') << "<identifier> " << tokenizer->identifier()
			<< " </identifier>\n";


	indentCount--;
	outfile << string(indentCount * 2, ' ') << "</term>\n";
}

void CompilationEngine::compileExpressionList()
{
	outfile << string(indentCount * 2, ' ') << "<expressionList>\n";
	indentCount++;

	if (!tokenizer->hasMoreTokens()) { 
		return;
	}
	tokenizer->advance();

	if (tokenizer->tokenType() == SYMBOL) {
		assert(tokenizer->symbol() == ')');
		indentCount--;
		outfile << string(indentCount * 2, ' ') << "</expressionList>\n";
		return;
	}

	compileExpression();

	while(true) {
		if (!tokenizer->hasMoreTokens()) return;
		tokenizer->advance();
		assert(tokenizer->tokenType() == SYMBOL) ;
		if (tokenizer->symbol() == ')'){
			indentCount--;
			outfile << string(indentCount * 2, ' ') << "</expressionList>\n";
			return;
		} else { // ','
			assert(tokenizer->symbol() == ',');
			outfile << string(indentCount * 2, ' ') << "<symbol> " << tokenizer->symbol()
					<< " </symbol>\n";
			if (!tokenizer->hasMoreTokens()) return;
			tokenizer->advance();
			compileExpression();
		}
	}

	indentCount--;
	outfile << string(indentCount * 2, ' ') << "</expressionList>\n";
}