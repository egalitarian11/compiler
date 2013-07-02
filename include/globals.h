#ifndef GLOBALS_H
#define GLOBALS_H
#include <stdio.h>

//Macro for error throwing.
#define THROW_ERROR(msg) printf("\n\nEncoutnered error: \"%s\" in File: \"%s\" at Line:%i", msg, __FILE__, __LINE__);\
	throw 0;

//Enum of all token types.
enum TokenType {
	TOKEN_INVALID_TOKEN_TYPE, //Indicates that the token is evil or somehow possessed. Certainly not something that should be parsed.
	TOKEN_KEYWORD_INT, TOKEN_KEYWORD_IF, TOKEN_KEYWORD_ELSE, TOKEN_KEYWORD_WHILE, TOKEN_KEYWORD_DO, TOKEN_KEYWORD_CONTINUE, TOKEN_KEYWORD_BREAK, TOKEN_KEYWORD_RETURN, //Keywords
	TOKEN_STRING, TOKEN_INT, TOKEN_IDENTIFIER,
	TOKEN_SEMICOLON, TOKEN_IN_PAREN, TOKEN_OUT_PAREN, TOKEN_IN_BRACE, TOKEN_OUT_BRACE, TOKEN_COMMA,
	TOKEN_UNARY_OP_BANG,
	TOKEN_OP_MINUS,
	TOKEN_BIN_OP_PLUS, TOKEN_BIN_OP_MULT, TOKEN_BIN_OP_DIVIDE, TOKEN_BIN_OP_AND, TOKEN_BIN_OP_OR,
	TOKEN_BIN_OP_GT, TOKEN_BIN_OP_LT, TOKEN_BIN_OP_GTE, TOKEN_BIN_OP_LTE, TOKEN_BIN_OP_NE, TOKEN_BIN_OP_EQUALS,
	TOKEN_ASSIGNMENT_OP,
	TOKEN_EOF //Indicates the end of file.
}; 

struct Token{
	TokenType type; //The type of the token
	int info; //Int value if type is TOKEN_INT, key in string table if type is TOKEN_STRING
};



#endif