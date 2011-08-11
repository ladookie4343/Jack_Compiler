#include "JackTokenizer.h"
#include <sstream>
#include <cassert>
#include <boost/algorithm/string.hpp>
#include <algorithm>
using namespace std;
using namespace boost;

JackTokenizer::JackTokenizer(ifstream& infile_) : 
		infile(infile_), inComment(false), currentTokenIndex(-1), nextTokenIndex(0),
		hasMoreTokens_(true), inQuote(false)
{
	getAllTokens();
	//copy(tokens.begin(), tokens.end(), ostream_iterator<string>(cout, "\n"));
	
}


void JackTokenizer::advance()
{
	currentTokenIndex++;
	nextTokenIndex = currentTokenIndex + 1;

	if (nextTokenIndex == tokens.size()) {
		hasMoreTokens_ = false;
		return;
	}

	string str = tokens[currentTokenIndex];
	if (str == "class"  || str == "constructor" || str == "function" || str == "method" || 
		str == "field"  || str == "static"      || str == "var"      || str == "int"    || 
		str == "char"   || str == "boolean"     || str == "void"     || str == "true"   || 
		str == "false"  || str == "null"        || str == "this"     || str == "let"    || 
		str == "do"     || str == "if"          || str == "else"     || str == "while"  || 
		str == "return") {
			tokenType_ = KEYWORD;
	} else if (str.find_first_of("{}()[].,;+-*/&|<>=~") != string::npos) {
		tokenType_ = SYMBOL;
	} else if (isdigit(str[0])) {
		tokenType_ = INT_CONST;
	} else if (str[0] == '\"') {
		tokenType_ = STRING_CONST;
	} else {
		tokenType_ = IDENTIFIER;
	}
}
		
string JackTokenizer::keyWord()
{
	assert(tokenType_ == KEYWORD);
	return tokens[currentTokenIndex];
}
		
char JackTokenizer::symbol()
{
	assert(tokenType_ == SYMBOL);
	return tokens[currentTokenIndex][0];
}
		
string JackTokenizer::identifier()
{
	assert(tokenType_ == IDENTIFIER);
	return tokens[currentTokenIndex];
}

int JackTokenizer::intVal()
{
	assert(tokenType_ == INT_CONST);
	return atoi(tokens[currentTokenIndex].c_str());
}

string JackTokenizer::stringVal()
{
	assert(tokenType_ == STRING_CONST);
	string str(tokens[currentTokenIndex]);
	str.erase(0, 1); str.erase(str.size() - 1);
	return str;
}

void JackTokenizer::getAllTokens() 
{
	string line;

	while (getline(infile, line)) {		
		stripComments(line);
		if (!line.empty()){
			splitIntoTokens(line);
		}
	}

	if (tokens.empty()) {
		hasMoreTokens_ = false;
	}
}

void JackTokenizer::stripComments(string& line)
{
	string comment("//"), openComment("/*"), closeComment("*/");

    if (inComment) {
        size_t  index = line.find(closeComment);
        if (index == string::npos) {
            line.erase();
        } else {
            line.erase(0, index + 2);
            inComment = false;
            eraseOpenCloseComments(line);
        }
    } else {
        size_t index = line.find(comment);
        if (index != string::npos) {
            line.erase(index);
        } else {
            eraseOpenCloseComments(line);
        }
    }

    trim(line);
}

void JackTokenizer::eraseOpenCloseComments(string& line)
{
    string openComment("/*"), closeComment("*/");

    while (true) {
        size_t openIndex, closeIndex;
        openIndex = line.find(openComment);
        if (openIndex == string::npos) break;
        closeIndex = line.find(closeComment);
        if (closeIndex == string::npos) {
            line.erase(openIndex);
            inComment = true;
            break;
        }
        line.erase(openIndex, closeIndex - openIndex + 2);
        inComment = false;
    }
}

void JackTokenizer::splitIntoTokens(string& line)
{
    stringstream tokenizer;
    string symbols("{}()[].,;+-*/&|<>=~");
    string word;
	string quoteString;
	int spaceCount = numberOfSpacesBeforeFirstQuotationMark(line, line.find_first_of("\""));

	size_t begQuoteIndex, endQuoteIndex;
	if ((begQuoteIndex = line.find_first_of("\"")) != string::npos) {
		// find quote and extract it from the line to be inserted later into tokens vector.
		endQuoteIndex = line.find_last_of("\"");
		int length = endQuoteIndex - begQuoteIndex + 1;
		quoteString = line.substr(begQuoteIndex, length);
		line = line.erase(begQuoteIndex, length);
	}
	int currentIndex = spaceCount;

    tokenizer << line;
    while (tokenizer >> word) {
        size_t index;
        while ((index = word.find_first_of(symbols)) != string::npos) {
			if (currentIndex == begQuoteIndex) {
				tokens.push_back(quoteString);
				currentIndex += quoteString.size();
			} 
			if (index == 0) {
                string str;
                str = word[0];
                tokens.push_back(str);
                word.erase(0, 1);
				currentIndex++;
            } else {
                string str1(word, 0, index);
                string str2;
                str2 = word[index];
                tokens.push_back(str1);
                tokens.push_back(str2);
                word.erase(0, index + 1);
				currentIndex += (str1.size() + 1);
            }
        }
        if (word.size() > 0) {
            tokens.push_back(word);
			currentIndex += word.size();
        }
    }
}

int JackTokenizer::numberOfSpacesBeforeFirstQuotationMark(string& line, int index)
{
	int count = 0;
	for(int i = index - 1; i > 0; i--) {
		if (isspace(line[i])) {
			count++;
		}
	}
	return count;
}