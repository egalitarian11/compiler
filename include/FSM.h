#ifndef FSM_H
#define FSM_H

#include "globals.h"
#include "StringTable.h"
#include <sstream>
//Enum

enum FSM_BlockState {FSM_HALT, FSM_BLOCK, FSM_NONE};

using namespace std;

//Represents the finite state machine to be used within the lexical analyzer.
class FSM {
public:
	FSM();
	
	~FSM();
	/*
	Causes the FSM to transition state based on input. 
	IMPORTANT: If the input would cause the FSM to block, BLOCK is returned but the machine does not transition except in the case where an error is detected. 

	@param c The input to the FSM
	@return The block state the machine would be in after transitioning.
	*/
	FSM_BlockState transition(char c);

	/*
	Gets the token type corresponding to the current state of the FSM. 

	@return The token type corresponding to the current state of the FSM.
	*/
	TokenType getTokenType();

	/*
	Writes to the associated string table;

	@return Key within the string table.
	*/
	int writeString();

	/*
	Gets the value for an integer token.

	@return Current integer value.
	*/
	int getIntValue();

	void associateStringTable(StringTable* table);
	void reset();
private:
	stringstream currentString;
	StringTable* strTable;

	int intVal;
	//States for the finite state machine. STATE_HALT_* indicates a halting state.
	static enum FSM_State {
		STATE_START,  //Starting state
		STATE_HALT_IDENTIFIER, //Reading an identifier

		STATE_HALT_INTEGER, //Reading integer.

		STATE_STRING_READ, STATE_HALT_STRING_FINISH, //Reading/read a string
		STATE_STRING_ESCAPE, //Encountered an escape while reading a string.

		STATE_COMMENT, STATE_COMMENT_BLOCK, //Comments
		STATE_COMMENT_BLOCK_STAR, // Found a "*" in a comment block

		STATE_HALT_SEMICOLON, STATE_HALT_COMMA, STATE_HALT_IN_PAREN, STATE_HALT_OUT_PAREN, STATE_HALT_IN_BRACE, STATE_HALT_OUT_BRACE, //Single character tokens

		STATE_HALT_EQUALS, //Assignment Op
		STATE_HALT_GT, STATE_HALT_LT, STATE_HALT_GTE, STATE_HALT_LTE, STATE_HALT_EQUIVALENCE, STATE_HALT_NE,//Equivilence ops
		
		STATE_HALT_PLUS, STATE_HALT_MINUS, STATE_HALT_MULT, STATE_HALT_DIVIDE,//Mathematical ops
		STATE_HALT_BANG,  //Unary Ops
		STATE_OR, STATE_AND, //First char of logical ops read
		STATE_HALT_AND, STATE_HALT_OR, //Logical Ops	
		STATE_HALT_EOF, //EOF
		STATE_FAIL}; //Failure state.


	FSM_State state;
	FSM_BlockState haltState;

	bool isInHaltState();
	string convertEscapeChar(char c);
};
#endif