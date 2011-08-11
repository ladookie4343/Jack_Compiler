#include "JackAnalyzer.h"


JackAnalyzer::JackAnalyzer(const string& filenameOrDirectory)
{
	path p(filenameOrDirectory);
	
	if (exists(p)) {
		if (is_regular_file(p)) {
			jackFilePaths.push_back(p); 
		} else if (is_directory(p)) {
			vector<path> paths;
			copy(directory_iterator(p), directory_iterator(), back_inserter(paths));

			for (vector<path>::const_iterator it(paths.begin()); it != paths.end(); ++it) {
				string str((*it).string());
				size_t index = str.find_first_of(".");
				string extension(str, index);
				if (extension == ".jack") {
					jackFilePaths.push_back(*it);
				}
			}
		} else {
			cout << p << " exists but is neither a regular file or a directory.\n";
		}
	} else {
		cout << p << " does not exist.\n";
	}
}


void JackAnalyzer::compile()
{	
	for (size_t i = 0; i < jackFilePaths.size(); ++i) {
		infile.open(jackFilePaths[i].string().c_str());
		if(infile.fail()) {
			cout << jackFilePaths[i].filename().string() << " no such file or directory\n";
		}

		string outputfilename(jackFilePaths[i].string());	
		outfile.open(stringReplace(outputfilename, ".xml"));
		cout << "Compiling " << jackFilePaths[i].string() << endl;
		
		tokenizer = new JackTokenizer(infile);
		engine = new CompilationEngine(tokenizer, outfile);
		engine->compileClass();

		cout << "Compilation successful\n\n";
		infile.close();
		outfile.close();		
	}
}


string JackAnalyzer::stringReplace(string filename_, string replace)
{
	size_t idx = filename_.find(".");
	if (idx != string::npos) {
		filename_.replace(idx, string::npos, replace);
	}
	return filename_;
}