#ifndef JACK_ANALYZER_H
#define JACK_ANALYZER_H

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include "JackTokenizer.h"
#include "CompilationEngine.h"
using namespace boost::filesystem;
using namespace std;

class JackAnalyzer
{
public:
	JackAnalyzer(const string& filenameOrDirectory);
	~JackAnalyzer(void) {};

	void compile();

private:
	string stringReplace(string filename_, string replace);

	JackTokenizer *tokenizer;
	CompilationEngine *engine;
	vector<path> jackFilePaths;
	ifstream infile;
	ofstream outfile;
};

#endif