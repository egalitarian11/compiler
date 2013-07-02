#ifndef PARSER_H
#define PARSER_H

#include "globals.h"
#include "LexicalAnalyzer.h"
#include "SymbolTable.h"
#include "CodeGenerator.h"

class Parser{
public:
	LexicalAnalyzer scanner;
	Parser(char* inputFileName, char* outputFileName);
	~Parser();

	/*
	Parses the token

	@param token The token being parsed.
	*/
	void parse();

private:
	Token nextToken;
	StringTable strTable;
	SymbolTable symbolTable;
	CodeGenerator codeGen;
	//Nonterminals
	void Program();
	void ExDef();
	void FuncDef(int funcName);
	void Desc(int name);
	void Decl();
	/*
	@param paramNum The number of parameters
	*/
	void ParamList(int* paramNum);
	/*
	@param loopTop Destination of continues. -1 if not applicable.
	@param loopTop Destination of breaks. -1 if not applicable.
	@param isFuncDef If false a new lexical level needs to be made.
	*/
	void CS(int loopTop, int loopBottom, bool isFuncDef);
	/*
	@param loopTop Destination of continues. -1 if not applicable.
	@param loopTop Destination of breaks. -1 if not applicable.
	*/
	void State(int loopTop, int loopBottom);
	void DataDef();
	void DecList();
	/*
	@param loopTop Destination of continues. -1 if not applicable.
	@param loopTop Destination of breaks. -1 if not applicable.
	*/
	void IfLoop(int loopTop, int loopBottom);
	void BreakableLoop();
	void Exp();
	void Or(bool possibleLValue);
	void And(bool possibleLValue);
	void RelEq(bool possibleLValue);
	void RelOp(bool possibleLValue);
	void Term(bool possibleLValue);

	/*
	@param possibleLValue True if the first Token in the factor could potentially be an Lvalue
	*/
	void Factor(bool possibleLValue);
	void IdPreExp(bool possibleLValue);

	/*
	@param argNum Number of arguments.
	@param hasStringArgs Is true if a stringArg is found. Is false otherwise.
	*/

	void ExpList(int* argNum, bool* hasStringArgs);

	/*
	Verifies that the next token is the right type. Throws if it isn't.

	@param type The type being checked for.
	*/
	void MatchToken(TokenType type);

	/*
	Gets the next token from the scanner and puts it in nextToken.
	*/
	void NextToken();
};
#endif