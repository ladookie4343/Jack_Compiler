#ifndef VMWRITER_H
#define VMWRITER_H
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

enum Segment { CONST, ARG, LOCAL, STATIC, THIS, THAT, POINTER, TEMP };
enum Command { ADD, SUB, NEG, EQ, GT, LT, AND, OR, NOT };

class VMWriter
{
public:
	VMWriter(ofstream &outfile_);
	~VMWriter(void);

	void writePush(Segment segment, size_t index);
	void writePop(Segment segment, size_t index);
	void writeArithmetic(Command command);
	void writeLabel(string label);
	void writeGoto(string label);
	void writeIf(string label);
	void writeCall(string name, size_t nArgs);
	void writeFunction(string name, size_t nLocals);
	void writeReturn();
	void close() { outfile.close(); }

private:
	ofstream &outfile;
};

#endif