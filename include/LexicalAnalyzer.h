#ifndef LEXICALANALYZER_H
#define LEXICALANALYZER_H

using namespace std;
#include "FSM.h"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include "globals.h"

class LexicalAnalyzer{
public:
	
	LexicalAnalyzer();
	
	/*
	Sets up the LA to begin reading filename. Must be called before getNextToken.
	*/
	void setUp(char* fileName, StringTable* stringTable);

	/*
	Gets the next token type in the file.

	@return The type of the next token.
	*/
	Token getNextToken();

	int getValue();
private:
	//The character we are currently worried about including in a token.
	char currentChar;
	FSM fsm;
	ifstream fileStream;
	int value;

	/*
	Reads the next character from the file.
	*/
	void readNextChar();
};
#endif