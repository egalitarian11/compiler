#include "FSM.h"

//Note that '_' is considered alpha.
bool isAlpha(char c){
	return ((c >='A' && c <= 'Z') || (c >='a' && c <= 'z') || c == '_');
}

bool isNumeric(char c){
	return (c >= '0' && c <= '9');
}

bool isWhiteSpace(char c){
	return (c == ' ' || c == '\t' || c == '\n' || c == 13);
}

string FSM::convertEscapeChar(char c){
	stringstream ret;
	switch(c){
	
	case 'n':
	case '\"':
	case 'b':
		//Dirty string hack #2
		if(!currentString.str().compare("") || currentString.str().back() != '\"'){
			ret << "\",";
		}else{
			//overwrtie the last quote
			currentString.seekp((long)currentString.tellp() - 1);
		}
		//Dirty string hack #1
		switch(c){
		case 'n':
			ret << "10,\"";
			break;
		case '\"':
			ret << "34,\"";
			break;
		case 'b':
			ret << "8,\"";
			break;
		}
		return ret.str();
	case '\\':
		return "\\";
	
	case '\'':
		return "\'";
	case '0':
		return "\0";
	case 'a':
		return "\a";
	
	case 't':
		return "\t";
	default:
		THROW_ERROR("Unsupported escape sequence encountered");
	}
}

//
bool FSM::isInHaltState(){
	//If the FSM is in a halting state then a token will be returned.
	return getTokenType() != TOKEN_INVALID_TOKEN_TYPE;
}

int FSM::writeString(){
	return strTable->insert(currentString.str());
}

int FSM::getIntValue(){
	return intVal;
}

void FSM::reset(){
	currentString.str("");
	intVal = 0;
	state = STATE_START;
	haltState = FSM_NONE;
}

FSM::FSM(){
	state = STATE_START;
	haltState = FSM_NONE;
}

void FSM::associateStringTable(StringTable* table){
	strTable = table;
}

FSM::~FSM(){
}

TokenType FSM::getTokenType(){
	switch(state){
	//List of halting states.
	
	//Identifier states
	case STATE_HALT_IDENTIFIER:
		//Check if its a key word. "Cheating" to keep the FSM simple.
		if(!currentString.str().compare("break")){
			return TOKEN_KEYWORD_BREAK;
		}else if(!currentString.str().compare("continue")){
			return TOKEN_KEYWORD_CONTINUE;
		}else if(!currentString.str().compare("do")){
			return TOKEN_KEYWORD_DO;
		}else if(!currentString.str().compare("else")){
			return TOKEN_KEYWORD_ELSE;
		}else if(!currentString.str().compare("if")){
			return TOKEN_KEYWORD_IF;
		}else if(!currentString.str().compare("int")){
			return TOKEN_KEYWORD_INT;
		}else if(!currentString.str().compare("return")){
			return TOKEN_KEYWORD_RETURN;
		}else if(!currentString.str().compare("while")){
			return TOKEN_KEYWORD_WHILE;
		}else{
			//Must just be a plain old identifier.
			return TOKEN_IDENTIFIER;
		}
	case STATE_HALT_INTEGER:
		return TOKEN_INT;
	case STATE_HALT_STRING_FINISH: 
		return TOKEN_STRING;
	case STATE_HALT_SEMICOLON:
		return TOKEN_SEMICOLON;
	case STATE_HALT_COMMA:
		return TOKEN_COMMA;
	case STATE_HALT_IN_PAREN: 
		return TOKEN_IN_PAREN;
	case STATE_HALT_OUT_PAREN: 
		return TOKEN_OUT_PAREN;
	case STATE_HALT_IN_BRACE:
		return TOKEN_IN_BRACE;
	case STATE_HALT_OUT_BRACE: 
		return TOKEN_OUT_BRACE;
	case STATE_HALT_EQUALS:
		return TOKEN_ASSIGNMENT_OP;
	case STATE_HALT_GT: 
		return TOKEN_BIN_OP_GT;
	case STATE_HALT_LT: 
		return TOKEN_BIN_OP_LT;
	case STATE_HALT_GTE:
		return TOKEN_BIN_OP_GTE;
	case STATE_HALT_LTE: 
		return TOKEN_BIN_OP_LTE;
	case STATE_HALT_EQUIVALENCE: 
		return TOKEN_BIN_OP_EQUALS;
	case STATE_HALT_NE:
		return TOKEN_BIN_OP_NE;
	case STATE_HALT_PLUS: 
		return TOKEN_BIN_OP_PLUS;
	case STATE_HALT_MINUS: 
		return TOKEN_OP_MINUS;
	case STATE_HALT_MULT:
		return TOKEN_BIN_OP_MULT;
	case STATE_HALT_DIVIDE:
		return TOKEN_BIN_OP_DIVIDE;
	case STATE_HALT_BANG: 
		return TOKEN_UNARY_OP_BANG;
	case STATE_HALT_AND: 
		return TOKEN_BIN_OP_AND;
	case STATE_HALT_OR: 
		return TOKEN_BIN_OP_OR;
	case STATE_HALT_EOF:
		return TOKEN_EOF;
	default:
		return TOKEN_INVALID_TOKEN_TYPE;
	}
}

FSM_BlockState FSM::transition(char c){
	if(c == EOF && state != STATE_START && state != STATE_COMMENT && state != STATE_HALT_EOF && state != STATE_HALT_OUT_BRACE && state != STATE_HALT_SEMICOLON){
		THROW_ERROR("Unexpected end of file");
	}

	switch(state){
	case STATE_START:
		if(isWhiteSpace(c)){			//Ignore whitespace.
			//Do nothing.
			state = STATE_START;
		} else if(isNumeric(c)){		//Start integer
			intVal = atoi(&c);
			state = STATE_HALT_INTEGER;
		} else {
			//Handle special characters
			switch(c){
			case '+':
				state = STATE_HALT_PLUS;
				break;
			case '-':
				state = STATE_HALT_MINUS;
				break;
			case '*':
				state = STATE_HALT_MULT;
				break;
			case '/':
				state = STATE_HALT_DIVIDE;
				break;
			case '(':
				state = STATE_HALT_IN_PAREN;
				break;
			case ')':
				state = STATE_HALT_OUT_PAREN;
				break;
			case '{':
				state = STATE_HALT_IN_BRACE;
				break;
			case '}':
				state = STATE_HALT_OUT_BRACE;
				break;
			case '=':
				state = STATE_HALT_EQUALS;
				break;
			case '>':
				state = STATE_HALT_GT;
				break;
			case '<':
				state = STATE_HALT_LT;
				break;
			case '!':
				state = STATE_HALT_BANG;
				break;
			case ';':
				state = STATE_HALT_SEMICOLON;
				break;
			case ',':
				state = STATE_HALT_COMMA;
				break;
			case '\"':
				state = STATE_STRING_READ;
				break;
			case '|':
				state = STATE_OR;
				break;
			case '&':
				state = STATE_AND;
				break;
			case EOF:
				state = STATE_HALT_EOF;
				break;
			default:
				//Nothing special, must be an identifier if its valid.
				if(isAlpha(c)){
					state = STATE_HALT_IDENTIFIER;
					currentString<<c;
				}else{
					return FSM_BLOCK;
				}
			}
		}
		break;
	
	case STATE_HALT_INTEGER:
		if(isNumeric(c)){
			//Continue reading the int.
			state = STATE_HALT_INTEGER;
			if(intVal > (INT_MAX - atoi(&c))/10){
				THROW_ERROR("Constant too large!");
			}
			intVal = 10*intVal + atoi(&c);
		} else if(isAlpha(c)) {
			state = STATE_FAIL;
			THROW_ERROR("Variable starting with numeric characters detected!!")
		} else {
			return FSM_BLOCK;
		}
		break;
	case STATE_HALT_IDENTIFIER:
		if(isAlpha(c) || isNumeric(c)){
			currentString<<c;
			state = STATE_HALT_IDENTIFIER;
		}else{
			return FSM_BLOCK;
		}
		break;
	case STATE_HALT_EQUALS:
		if(c == '='){
			state = STATE_HALT_EQUIVALENCE;
		}else{
			return FSM_BLOCK;
		}
		break;
	case STATE_HALT_GT:
		if(c == '='){
			state = STATE_HALT_GTE;
		}else{
			return FSM_BLOCK;
		}
		break;
	case STATE_HALT_LT:
		if(c == '='){
			state = STATE_HALT_LTE;
		}else{
			return FSM_BLOCK;
		}
		break;
	case STATE_HALT_BANG:
		if(c == '='){
			state = STATE_HALT_NE;
		}else{
			return FSM_BLOCK;
		}
		break;
	case STATE_HALT_DIVIDE:
		if(c == '/'){
			state = STATE_COMMENT;
		}else if(c == '*'){
			state = STATE_COMMENT_BLOCK;
		}else{
			return FSM_BLOCK;
		}
		break;
	case STATE_COMMENT:
		//Keep reading and ignoring input untill a new line comes up.
		if(c == '\n' || c == EOF){
			state = STATE_START;
		}		
		break;
	case STATE_COMMENT_BLOCK:
		//Keep reading and ignoring input until a star comes up.
		if(c == '*' || c == EOF){
			state = STATE_COMMENT_BLOCK_STAR;
		}	
		break;
	case STATE_COMMENT_BLOCK_STAR:
		if(c == '/'){
			state = STATE_START;
		}else{
			state = STATE_COMMENT_BLOCK;
		}
		break;

	case STATE_STRING_READ:
		if(c == '\\'){
			state = STATE_STRING_ESCAPE;
		}else if(c == '\"'){
			state = STATE_HALT_STRING_FINISH;
		}else{
			currentString<<c;
		}
		break;
	case STATE_STRING_ESCAPE:
		state = STATE_STRING_READ;
		currentString<<convertEscapeChar(c);
		break;
	case STATE_OR:
		if(c != '|'){
			return FSM_BLOCK;
		}
		state = STATE_HALT_OR;
		break;
	case STATE_AND:
		if(c != '&'){
			return FSM_BLOCK;
		}
		state = STATE_HALT_AND;
		break;

	//States which block on all input
	case STATE_HALT_SEMICOLON:
	case STATE_HALT_IN_PAREN:
	case STATE_HALT_OUT_PAREN:
	case STATE_HALT_IN_BRACE:
	case STATE_HALT_OUT_BRACE:
	case STATE_HALT_COMMA:
	case STATE_HALT_EQUIVALENCE:
	case STATE_HALT_GTE:
	case STATE_HALT_LTE:
	case STATE_HALT_NE:
	case STATE_HALT_MINUS:
	case STATE_HALT_MULT:
	case STATE_HALT_PLUS:
	case STATE_HALT_STRING_FINISH:
	case STATE_HALT_AND:
	case STATE_HALT_OR:
	case STATE_HALT_EOF:
		return FSM_BLOCK;
	default:
		THROW_ERROR("My tummy has got the rumblies!!");
	}

	//ASSUMPTION: Reaching this point means the FSM has not blocked.
	//Set the haltState based on the current state.
	if(isInHaltState()) {
		haltState = FSM_HALT;
	} else {
		haltState = FSM_NONE;
	}

	return haltState;
}