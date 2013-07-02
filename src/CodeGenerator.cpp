#include "CodeGenerator.h"

CodeGenerator::CodeGenerator(){
	nextLabel = 1;
}

CodeGenerator::~CodeGenerator(){

}

void CodeGenerator::setup(char* outFile){
	outStream.open(outFile);
}

void CodeGenerator::writeLine(string str){
	outStream << str.c_str() << endl;
}

int CodeGenerator::getNextLabel(){
	nextLabel++;
	return nextLabel - 1;
}

void CodeGenerator::writeLabel(int x){
	outStream << "L" << x << ":" << endl;
}