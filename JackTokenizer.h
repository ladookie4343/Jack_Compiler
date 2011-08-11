#ifndef JACK_TOKENIZER_H
#define JACK_TOKENIZER_H

#include <fstream>
#include <string>
#include <vector>
using namespace std;

enum TokenType { KEYWORD, SYMBOL, IDENTIFIER, INT_CONST, STRING_CONST };

/*
enum KeywordType { CLASS, METHOD, FUNCTION, CONSTRUCTOR, INT, BOOLEAN, 
	               CHAR, VOID, VAR, STATIC, FIELD, LET, DO, IF, ELSE, 
				   WHILE, RETURN, TRUE, FALSE, NULL, THIS };
*/

class JackTokenizer
{
public:
	JackTokenizer(ifstream& infile_);
	~JackTokenizer(void) {};

	bool hasMoreTokens() { return hasMoreTokens_; };
	void advance();
	TokenType tokenType() { return tokenType_; };
	string keyWord();
	char symbol();
	string identifier();
	int intVal();
	string stringVal();


private:
	void getAllTokens();
	void stripComments(string& line);
	void eraseOpenCloseComments(string& line); // helper function for stripComments()
	void splitIntoTokens(string& line);
	void parseQuote(string& word);
	int numberOfSpacesBeforeFirstQuotationMark(string& line, int index);

	vector<string> tokens;
	int currentTokenIndex;
	int nextTokenIndex;
	TokenType tokenType_;
	ifstream& infile;
	bool hasMoreTokens_;
	bool inComment;
	bool inQuote;
};

#endif