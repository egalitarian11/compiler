#include "LexicalAnalyzer.h"


LexicalAnalyzer::LexicalAnalyzer(){
	
}

void LexicalAnalyzer::setUp(char* fileName, StringTable* stringTable){
	//Open the filestream
	fileStream.open(fileName);
	fsm.associateStringTable(stringTable);
	//Populate the first character
	readNextChar();
}

void LexicalAnalyzer::readNextChar(){
	if(fileStream.read(&this->currentChar, 1)){
		printf("%c", currentChar);
	}else{
		currentChar = EOF;
	}
}

int LexicalAnalyzer::getValue(){
	return value;
}

Token LexicalAnalyzer::getNextToken(){
	Token rv;
	fsm.reset();

	//Load the next character untill the FSM blocks.
	while(fsm.transition(this->currentChar) != FSM_BLOCK){
		readNextChar();
	}
	
	rv.type = fsm.getTokenType();
	rv.info = -1;

	//Error out if token is invalid.
	if(rv.type == TOKEN_INVALID_TOKEN_TYPE){
		THROW_ERROR("Bad token type");
	}

	//Handle string or integer values.
	if(rv.type == TOKEN_STRING || rv.type == TOKEN_IDENTIFIER){
		rv.info = fsm.writeString();
	}

	if(rv.type == TOKEN_INT){
		rv.info = fsm.getIntValue();
	}

	
	return rv;
}