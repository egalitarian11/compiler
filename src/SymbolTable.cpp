#include "SymbolTable.h"
#include <sstream>
#define LexLevel unordered_map<int, SymbolProperties>

SymbolTable::SymbolTable(){
	top = -1;
	newLexLevel();
}

SymbolTable::~SymbolTable(){

}

void SymbolTable::newLexLevel(){
	LexicalLevel newLL; 
	newLL.numLocals = 0;
	newLL.numParams = 0;
	table.push_back(newLL);
	top++;
}

void SymbolTable::leaveLexLevel(){
	table.pop_back();
	top--;
}

SymbolProperties SymbolTable::getSymbolProps(int key){
	LexLevel currentLevel;
	LexLevel::iterator itr;
	int x = 0;
	//Search from the top of the table down for the symbol
	do{
		currentLevel = table[top - x].table;

		itr = currentLevel.find(key);
		x++;
	}while(itr == currentLevel.end() && top >= x);

	if(itr == currentLevel.end()){
		THROW_ERROR("Symbol not found");
	}

	return itr->second;
}

void SymbolTable::associateCodeGen(CodeGenerator* generator){
	codeGen = generator;
}

void SymbolTable::associateStringTable(StringTable* strTbl){
	strTable = strTbl;
}

void SymbolTable::setFuncArgNum(int key, int paramNum){
	LexLevel::iterator itr = table[0].table.find(key);
	itr->second.argNum = paramNum;
}

void SymbolTable::insertSymbol(int key, bool isFunc, bool isParam){
	SymbolProperties props;
	stringstream temp;
	
	//Make sure its not already defined at this lexical level.
	
	LexLevel::iterator itr = table[top].table.find(key);

	if(itr != table[top].table.end()){
		THROW_ERROR("Variable Redefinition detected");
	}

	if(table.size() > 1){
		//Non-Global
		if(isParam){
			table[top].numParams++;
			temp << "[EBP + " << (table[top].numParams + 1) * 4 << "]";
			
		}else{
			codeGen->writeLine("SUB ESP, 4");
			table[top].numLocals++;
			temp << "[EBP - " << (table[top].numLocals) * 4 << "]";
		}
		props.address = temp.str();
		props.isFunc = false;
	}else{
		//Global
		if(!isFunc){
			//Write the variable to the global data segment
			codeGen->writeLine("_TEXT ENDS");
			codeGen->writeLine("_DATA SEGMENT");
			temp << "_" << strTable->lookUp(key) << " DWORD ?";
			codeGen->writeLine(temp.str());
			codeGen->writeLine("_DATA ENDS");
			codeGen->writeLine("_TEXT SEGMENT");
		}

		temp.str("");
		temp << "_" << strTable->lookUp(key);
		props.address = temp.str();
		props.isFunc = isFunc;
	}
	table[top].table.insert(LexLevel::value_type(key, props));
}