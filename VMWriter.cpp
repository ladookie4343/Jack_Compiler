#include "VMWriter.h"


VMWriter::VMWriter(ofstream &outfile_) : outfile(outfile_)
{
}


VMWriter::~VMWriter(void)
{
}

void VMWriter::writePush(Segment segment, size_t index)
{
	switch(segment) {
		case CONST:
			outfile << "push constant " << index << endl;
			break;
		case ARG:
			outfile << "push argument " << index << endl;
			break;
		case LOCAL: 
			outfile << "push local " << index << endl;
			break; 
		case STATIC: 
			outfile << "push static " << index << endl;
			break;
		case THIS:  
			outfile << "push this " << index << endl;
			break;
		case THAT: 
			outfile << "push that " << index << endl;
			break;
		case POINTER: 
			outfile << "push pointer " << index << endl;
			break;
		case TEMP: 
			outfile << "push temp " << index << endl;
			break;
	}
}

void VMWriter::writePop(Segment segment, size_t index)
{
	switch(segment) {
		case CONST:
			outfile << "pop constant " << index << endl;
			break;
		case ARG:
			outfile << "pop argument " << index << endl;
			break;
		case LOCAL: 
			outfile << "pop local " << index << endl;
			break; 
		case STATIC: 
			outfile << "pop static " << index << endl;
			break;
		case THIS:  
			outfile << "pop this " << index << endl;
			break;
		case THAT: 
			outfile << "pop that " << index << endl;
			break;
		case POINTER: 
			outfile << "pop pointer " << index << endl;
			break;
		case TEMP: 
			outfile << "pop temp " << index << endl;
			break;
	}
}

void VMWriter::writeArithmetic(Command command)
{
	switch(command) {
		case ADD:
			outfile << "add\n";
			break;
		case SUB:
			outfile << "sub\n";
			break;
		case NEG: 
			outfile << "neg\n";
			break; 
		case EQ: 
			outfile << "eq\n";
			break;
		case GT:  
			outfile << "gt\n";
			break;
		case LT: 
			outfile << "lt\n";
			break;
		case AND: 
			outfile << "and\n";
			break;
		case OR: 
			outfile << "or\n";
			break;
		case NOT:
			outfile << "not\n";
			break;
	}
}

void VMWriter::writeLabel(string label)
{
	outfile << "label " << label << endl;
}

void VMWriter::writeGoto(string label)
{
	outfile << "goto " << label << endl;
}

void VMWriter::writeIf(string label)
{
	outfile << "if-goto " << label << endl;
}

void VMWriter::writeCall(string name, size_t nArgs)
{
	outfile << "call " << name << " " << nArgs << endl;
}

void VMWriter::writeFunction(string name, size_t nLocals)
{
	outfile << "function " << name << " " << nLocals << endl;
}

void VMWriter::writeReturn()
{
	outfile << "return\n";
}
