#include "globals.h"
#include "CodeGenerator.h"
#include "StringTable.h"
#include <vector>
#include <unordered_map>

using namespace std;

enum dataType {TYPE_INT};
struct SymbolProperties{
	bool isFunc; //True if func false if variable
	int argNum; //Number of args in a function
	string address; //Name of functions/ string to refer to value for variables.
};



struct LexicalLevel{
	int numLocals;
	int numParams;
	unordered_map<int, SymbolProperties> table;
};

class SymbolTable{
public:

	SymbolTable();
	~SymbolTable();

	/*
	Creates a new lexical level.
	*/
	void newLexLevel();

	/*
	Exits a lexical level.
	*/
	void leaveLexLevel();

	/*
	Gets the string used to refer to the memory address of the variable on the stack
	*/
	SymbolProperties getSymbolProps(int key);
	
	//Opening holes that don't belong here. Sometimes we staple things together.
	void associateCodeGen(CodeGenerator* generator);
	void associateStringTable(StringTable* stringTable);

	/*
	Inserts a symbol into the table
	@param The key value of the symbol in the string table.
	*/
	void insertSymbol(int key, bool isFunc, bool isParam);

	void setFuncArgNum(int key, int paramNum);

private:
	vector<Token> argList;
	vector<Token> paramList;
	vector<LexicalLevel> table;
	CodeGenerator* codeGen;
	StringTable* strTable;
	int top;

};