#include "LexicalAnalyzer.h"
#include "Parser.h"

//REMOVE
#include <unordered_map>
#include <sstream>
#include <iostream>
using namespace std;

typedef std::unordered_map<string, int> Dictionary; 
typedef std::unordered_map<int, string> ReverseDictionary; 


int main(int argc, char* argv[]) {

	try{
		Parser parser(argv[1], argv[2]);		
		parser.parse();
		printf("\n\nCOMPILATION SUCCESSFUL");
	}
	catch(...){
		printf("\n\nCOMPILATION FAILED");
		getchar();
	}
	return 0;
}
