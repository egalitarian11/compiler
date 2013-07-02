#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <iostream>
#include <fstream>

using namespace std;
class CodeGenerator{
public:
	CodeGenerator();
	~CodeGenerator();

	void setup(char* outFile);
	void writeLine(string str);
	void writeLabel(int x);
	int getNextLabel();

private:
	ofstream outStream;
	int nextLabel;
};

#endif