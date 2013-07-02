#include "Parser.h"
#include <string>
//Utilites to help with Parsing

/*
	Verify that 
*/
void Parser::MatchToken(TokenType type){
	if(nextToken.type != type){
		THROW_ERROR("Unexpected Token");	
	}
	NextToken();
}

void Parser::NextToken(){
	nextToken = scanner.getNextToken();
}

void Parser::Program(){
	codeGen.writeLine(".386P");
	codeGen.writeLine("INCLUDELIB LIBCMT");
	codeGen.writeLine(".MODEL FLAT");
	codeGen.writeLine("_TEXT SEGMENT");

	while(nextToken.type != TOKEN_EOF){
		ExDef();
	}

	codeGen.writeLine("_TEXT ENDS");
	codeGen.writeLine("END");
}

void Parser::ExDef(){
	int name;
	switch(nextToken.type){
	case TOKEN_KEYWORD_INT:
		NextToken();
		//Get the name of the function/identifier
		name = nextToken.info;
		MatchToken(TOKEN_IDENTIFIER);
		Desc(name);
		break;
	case TOKEN_IDENTIFIER:
		name = nextToken.info;
		NextToken();
		FuncDef(name);
		break;
	default:
		THROW_ERROR("Bad token found");
	}
}

void Parser::Desc(int name){
	switch(nextToken.type){
	case TOKEN_IN_PAREN:
		//is a function
		FuncDef(name);
		break;
	case TOKEN_COMMA:
	case TOKEN_SEMICOLON:
		//Is a declartion
		symbolTable.insertSymbol(name, false, false);
		Decl();
		MatchToken(TOKEN_SEMICOLON);
		break;
	default:
		THROW_ERROR("Bad token found");
	}	
}

void Parser::Decl(){
	while(nextToken.type == TOKEN_COMMA){
		NextToken();
		Token id = nextToken;
		MatchToken(TOKEN_IDENTIFIER);
		symbolTable.insertSymbol(id.info, false, false);
	}
}

void Parser::FuncDef(int funcName){
	//Set up function
	string name = strTable.lookUp(funcName);
	stringstream temp;
	temp << "PUBLIC _" << strTable.lookUp(funcName);
	codeGen.writeLine(temp.str());
	temp.str("");
	temp << "_" + name << " PROC NEAR";
	codeGen.writeLine(temp.str());
	codeGen.writeLine("PUSH EBP");
	codeGen.writeLine("MOV EBP , ESP");

	//Function code
	symbolTable.insertSymbol(funcName, true, false);
	MatchToken(TOKEN_IN_PAREN);
	symbolTable.newLexLevel();
	int paramNum = 0;
	ParamList(&paramNum);
	symbolTable.setFuncArgNum(funcName, paramNum);
	MatchToken(TOKEN_OUT_PAREN);
	CS(-1,-1, true);

	//Default return 0
	codeGen.writeLine("MOV EAX, 0");
	codeGen.writeLine("MOV ESP , EBP");
	codeGen.writeLine("POP EBP");
	codeGen.writeLine("RET");

	//Clean up function and return
	temp.str("");
	temp << "_" << name << " ENDP";
	codeGen.writeLine(temp.str());
}

void Parser::ParamList(int* paramNum){
	int numParams = 0;

	switch(nextToken.type){
	case TOKEN_KEYWORD_INT:
		NextToken();
		symbolTable.insertSymbol(nextToken.info, false, true);
		MatchToken(TOKEN_IDENTIFIER);
		numParams = 1;
		while(nextToken.type == TOKEN_COMMA){
			NextToken();
			MatchToken(TOKEN_KEYWORD_INT);
			symbolTable.insertSymbol(nextToken.info, false, true);
			MatchToken(TOKEN_IDENTIFIER);
			numParams++;
		}

		break;
	case TOKEN_OUT_PAREN:
		break;
	default:
		THROW_ERROR("Bad token found in parameter list");
	}
	*paramNum = numParams;
}

void Parser::CS(int loopTop, int loopBottom, bool isFuncDef){
	MatchToken(TOKEN_IN_BRACE);
	if(!isFuncDef){
		symbolTable.newLexLevel();
	}

	if(nextToken.type == TOKEN_KEYWORD_INT){
		DecList();
	}
	while(nextToken.type != TOKEN_OUT_BRACE){
		State(loopTop, loopBottom);
	}
	symbolTable.leaveLexLevel();
	MatchToken(TOKEN_OUT_BRACE);
}

void Parser::DecList(){
	while(nextToken.type == TOKEN_KEYWORD_INT){
		DataDef();
	}
}

void Parser::DataDef(){
	MatchToken(TOKEN_KEYWORD_INT);
	Token id = nextToken;
	MatchToken(TOKEN_IDENTIFIER);
	symbolTable.insertSymbol(id.info, false, false);
	Decl();
	MatchToken(TOKEN_SEMICOLON);
}

void Parser::State(int loopTop, int loopBottom){
	stringstream temp;

	switch(nextToken.type){
	case TOKEN_KEYWORD_IF:
		IfLoop(loopTop, loopBottom);
		break;
	case TOKEN_KEYWORD_WHILE:
	case TOKEN_KEYWORD_DO:
		BreakableLoop();
		break;
	case TOKEN_KEYWORD_CONTINUE:
		temp << "JMP L"<< loopTop;
		codeGen.writeLine(temp.str());
		NextToken();
		MatchToken(TOKEN_SEMICOLON);
		break;
	case TOKEN_KEYWORD_BREAK:
		temp << "JMP L" << loopBottom;
		codeGen.writeLine(temp.str());
		NextToken();
		MatchToken(TOKEN_SEMICOLON);
		break;
	case TOKEN_KEYWORD_RETURN:
		NextToken();
		
		if(nextToken.type != TOKEN_SEMICOLON){
			Exp();
			codeGen.writeLine("POP EAX");
		}else{
			codeGen.writeLine("MOV EAX, 0");
		}

		codeGen.writeLine("MOV ESP , EBP");
		codeGen.writeLine("POP EBP");
		codeGen.writeLine("RET");

		MatchToken(TOKEN_SEMICOLON);
		break;
	case TOKEN_SEMICOLON:
		NextToken();
		break;
	case TOKEN_IN_BRACE:
		CS(loopTop, loopBottom, false);
		break;
	default:
		Exp();
		MatchToken(TOKEN_SEMICOLON);
		codeGen.writeLine("POP EAX");
	}
}

void Parser::IfLoop(int loopTop, int loopBottom){
	//Loop top
	int pastIf = codeGen.getNextLabel();
	int pastElse = codeGen.getNextLabel();
	stringstream temp;

	MatchToken(TOKEN_KEYWORD_IF);
	MatchToken(TOKEN_IN_PAREN);
	Exp();
	MatchToken(TOKEN_OUT_PAREN);
	//If condition
	codeGen.writeLine("POP EAX ; Checking if condition");
	codeGen.writeLine("CMP EAX , 0");
	temp << "JE L" << pastIf;
	codeGen.writeLine(temp.str());

	//If body
	State(loopTop, loopBottom);

	//Skip the else loop
	temp.str("");
	temp << "JMP L" << pastElse;
	codeGen.writeLine(temp.str());
	
	//Else
	codeGen.writeLabel(pastIf);
	if(nextToken.type == TOKEN_KEYWORD_ELSE){
		NextToken();
		//Else Body
		State(loopTop, loopBottom);
	}
	codeGen.writeLabel(pastElse);
}

void Parser::BreakableLoop(){
	int topLabel = codeGen.getNextLabel();
	int bottomLabel = codeGen.getNextLabel();
	stringstream temp;

	switch(nextToken.type){
	case TOKEN_KEYWORD_WHILE:
		NextToken();
		codeGen.writeLabel(topLabel);

		//Loop condition
		MatchToken(TOKEN_IN_PAREN);
		Exp();
		MatchToken(TOKEN_OUT_PAREN);

		codeGen.writeLine("POP EAX");
		codeGen.writeLine("CMP EAX , 0");
		temp << "JE L" << bottomLabel << " ;Exit WHILE loop";
		codeGen.writeLine(temp.str());

		//loop code
		State(topLabel, bottomLabel);

		//loop bottom
		temp.str("");
		temp << "JMP L" << topLabel << "; Back to WHILE condition";
		codeGen.writeLine(temp.str());
		codeGen.writeLabel(bottomLabel);
		break;
	case TOKEN_KEYWORD_DO:
		int continueLabel = codeGen.getNextLabel();
		NextToken();

		//loop top
		codeGen.writeLabel(topLabel);

		//loop code
		State(continueLabel, bottomLabel);

		//loop bottom
		codeGen.writeLabel(continueLabel);
		MatchToken(TOKEN_KEYWORD_WHILE);
		MatchToken(TOKEN_IN_PAREN);
		Exp();
		MatchToken(TOKEN_OUT_PAREN);
		MatchToken(TOKEN_SEMICOLON);
		codeGen.writeLine("POP EAX");
		codeGen.writeLine("CMP EAX , 0");
		temp << "JNE L" << topLabel;
		codeGen.writeLine(temp.str());
		//Place to break to.
		codeGen.writeLabel(bottomLabel);
		break;
	}
}

void Parser::Exp(){
	Or(true);
	while(nextToken.type == TOKEN_BIN_OP_OR){
		NextToken();
		Or(false);

		codeGen.writeLine("POP ECX");
		codeGen.writeLine("POP EAX");
		codeGen.writeLine("MOV EDX, 0");
		codeGen.writeLine("OR EAX, ECX");
		codeGen.writeLine("CMP EAX, 0");

		int label = codeGen.getNextLabel();
		stringstream temp;
		temp << "JE L" << label;
		codeGen.writeLine(temp.str());
		codeGen.writeLine("INC EDX");
		codeGen.writeLabel(label);
		codeGen.writeLine("PUSH EDX");
	}
}

void Parser::Or(bool possibleLValue){
	And(possibleLValue);
	while(nextToken.type == TOKEN_BIN_OP_AND){
		NextToken();
		And(false);

		codeGen.writeLine("POP ECX");
		codeGen.writeLine("POP EAX");
		codeGen.writeLine("MOV EDX, 0");
		codeGen.writeLine("IMUL ECX");
		codeGen.writeLine("CMP EAX, 0");

		int label = codeGen.getNextLabel();
		stringstream temp;
		temp << "JE L" << label;
		codeGen.writeLine(temp.str());
		codeGen.writeLine("INC EDX");
		codeGen.writeLabel(label);
		codeGen.writeLine("PUSH EDX");

	}
}

void Parser::And(bool possibleLValue){
	RelEq(possibleLValue);
	while(nextToken.type == TOKEN_BIN_OP_EQUALS || nextToken.type == TOKEN_BIN_OP_NE){
		Token opSymbol = nextToken;
		NextToken();
		RelEq(false);

		stringstream temp;
		codeGen.writeLine("POP ECX");
		codeGen.writeLine("POP EAX");
		codeGen.writeLine("MOV EDX, 0");
		codeGen.writeLine("CMP EAX, ECX");
		
		int label = codeGen.getNextLabel();

		switch(opSymbol.type){
		case TOKEN_BIN_OP_EQUALS:
			temp << "JNE L" << label;
			codeGen.writeLine(temp.str());
			break;
		case TOKEN_BIN_OP_NE:
			temp << "JE L" << label;
			codeGen.writeLine(temp.str());
			break;
		}
		
		codeGen.writeLine("INC EDX");
		codeGen.writeLabel(label);
		codeGen.writeLine("PUSH EDX");
	}
}

void Parser::RelEq(bool possibleLValue){
	RelOp(possibleLValue);
	while(nextToken.type == TOKEN_BIN_OP_GT || nextToken.type == TOKEN_BIN_OP_LT || nextToken.type == TOKEN_BIN_OP_GTE || nextToken.type == TOKEN_BIN_OP_LTE){
		Token opSymbol = nextToken;
		NextToken();
		RelOp(false);

		stringstream temp;
		codeGen.writeLine("POP ECX");
		codeGen.writeLine("POP EAX");
		codeGen.writeLine("MOV EDX, 0");
		codeGen.writeLine("CMP EAX, ECX");
		
		int label = codeGen.getNextLabel();

		switch(opSymbol.type ){
		case TOKEN_BIN_OP_GT:
			temp << "JLE L" << label;
			codeGen.writeLine(temp.str());
			break;
		case TOKEN_BIN_OP_LT:
			temp << "JGE L" << label;
			codeGen.writeLine(temp.str());
			break;
		case TOKEN_BIN_OP_GTE:
			temp << "JL L" << label;
			codeGen.writeLine(temp.str());
			break;
		case TOKEN_BIN_OP_LTE:
			temp << "JG L" << label;
			codeGen.writeLine(temp.str());
			break;
		}

		codeGen.writeLine("INC EDX");
		codeGen.writeLabel(label);
		codeGen.writeLine("PUSH EDX");
	}
}

void Parser::RelOp(bool possibleLValue){
	Term(possibleLValue);
	while(nextToken.type == TOKEN_OP_MINUS || nextToken.type == TOKEN_BIN_OP_PLUS){
		Token opSymbol = nextToken;
		NextToken();
		Term(false);

		codeGen.writeLine("POP ECX");
		codeGen.writeLine("POP EAX");

		//Generate code for the ops
		if(opSymbol.type == TOKEN_BIN_OP_PLUS){
			
			codeGen.writeLine("ADD EAX, ECX");
			codeGen.writeLine("PUSH EAX");
		}

		if(opSymbol.type == TOKEN_OP_MINUS){
			codeGen.writeLine("SUB EAX, ECX");
			codeGen.writeLine("PUSH EAX");
		}
	}
}

void Parser::Term(bool possibleLValue){
	Factor(possibleLValue);
	while(nextToken.type == TOKEN_BIN_OP_DIVIDE || nextToken.type == TOKEN_BIN_OP_MULT){
		Token opSymbol = nextToken;
		
		NextToken();
		Factor(false);

		codeGen.writeLine("POP ECX");
		codeGen.writeLine("POP EAX");

		//Generate code for the ops
		if(opSymbol.type == TOKEN_BIN_OP_DIVIDE){
			codeGen.writeLine("CDQ");
			codeGen.writeLine("IDIV ECX");
			codeGen.writeLine("PUSH EAX");
		}

		if(opSymbol.type == TOKEN_BIN_OP_MULT){
			codeGen.writeLine("IMUL ECX");
			codeGen.writeLine("PUSH EAX");
		}
	}
}

void Parser::Factor(bool possibleLValue){
	stringstream temp;
	int label;

	switch(nextToken.type){
	case TOKEN_IDENTIFIER:
		IdPreExp(possibleLValue);
		break;
	case TOKEN_INT:
		temp << "PUSH " << nextToken.info;
		codeGen.writeLine(temp.str());
		NextToken();
		break;
	case TOKEN_IN_PAREN:
		NextToken();
		Exp();
		MatchToken(TOKEN_OUT_PAREN);
		break;
	case TOKEN_OP_MINUS: //Unary minus  
		NextToken();
		Factor(false);
		codeGen.writeLine("POP EAX");
		codeGen.writeLine("NEG EAX");
		codeGen.writeLine("PUSH EAX");
		break;
	case TOKEN_UNARY_OP_BANG:
		NextToken();
		Factor(false);
		label = codeGen.getNextLabel();
		codeGen.writeLine("POP EAX");
		codeGen.writeLine("MOV EDX, 0");
		codeGen.writeLine("CMP EAX, 0");
		temp << "JNE L" << label;
		codeGen.writeLine(temp.str());
		codeGen.writeLine("INC EDX");
		codeGen.writeLabel(label);
		codeGen.writeLine("PUSH EDX");
		break;
	default:
		THROW_ERROR("Bad token found in expression");
	}
}

void Parser::IdPreExp(bool possibleLValue){
	stringstream temp;
	Token id = nextToken;
	SymbolProperties symProps;
	bool isLocal = true;
	int argNum = 0;
	bool hasStringArgs;
	string funcName;
	NextToken();

	switch(nextToken.type){
	case TOKEN_ASSIGNMENT_OP:
		//Assignment
		if(!possibleLValue){
			THROW_ERROR("Left of = must be l-value");
		}
		symProps = symbolTable.getSymbolProps(id.info);
		NextToken();
		Exp();
		codeGen.writeLine("POP EAX");
		temp << "MOV " << symProps.address << ", EAX";
		codeGen.writeLine(temp.str());
		codeGen.writeLine("PUSH EAX");
		break;
	case TOKEN_IN_PAREN:
		//Function call
		try{
			symProps = symbolTable.getSymbolProps(id.info);
			funcName = symProps.address;
		}catch(...){
			//Function isn't defined locally
			isLocal = false;
			
			temp<< "_" << strTable.lookUp(id.info);
			funcName = temp.str();

			temp.str("");
			temp << "EXTERN " << funcName << " : NEAR";
			codeGen.writeLine(temp.str());
		}

		NextToken();

		if(nextToken.type != TOKEN_OUT_PAREN){
			ExpList(&argNum, &hasStringArgs);
		}

		MatchToken(TOKEN_OUT_PAREN);

		//Constraint checks for local functions
		if(isLocal){
			if(!symProps.isFunc){
				THROW_ERROR("Found variable, expected function.");
			}

			if(symProps.argNum != argNum){
				THROW_ERROR("Incorrect number of arguments passed to a function");
			}

			if(hasStringArgs){
				THROW_ERROR("Invalid arguement type. Found string expected int.");
			}
		}

		//Reverse the arguements
		for(int x = 0; x < argNum/2; x++){
			temp.str("");
			temp << "MOV EAX, [ESP + " << 4*x << "]" << " ; Swapping arguments";
			codeGen.writeLine(temp.str());

			temp.str("");
			temp << "MOV ECX, [ESP + " << 4*(argNum - 1 - x) << "]";
			codeGen.writeLine(temp.str());

			temp.str("");
			temp << "MOV [ESP + " << 4*x << "], ECX" ;
			codeGen.writeLine(temp.str());

			temp.str("");
			temp << "MOV [ESP + " << 4*(argNum - 1 - x) << "], EAX" ;
			codeGen.writeLine(temp.str());
		}

		//Call the function
		temp.str("");
		temp << "CALL " << funcName;
		codeGen.writeLine(temp.str());

		//Clean up parameters
		temp.str("");
		temp << "ADD ESP, " << + 4*(argNum);
		codeGen.writeLine(temp.str());

		//Push results into EAX
		codeGen.writeLine("PUSH EAX");
		break;
	default:
		//Identifier
		symProps = symbolTable.getSymbolProps(id.info);
		if(symProps.isFunc){
			THROW_ERROR("Found function, expected variable.");
		}

		temp << "MOV EAX , " << symProps.address;
		codeGen.writeLine(temp.str());
		codeGen.writeLine("PUSH EAX");
		break;
	}
}

void Parser::ExpList(int* argNum, bool* hasStringArgs){
	stringstream temp;
	int argCount = 0;
	*hasStringArgs = false;

	if(nextToken.type == TOKEN_STRING){
		//Clear misuse of the getNextLabel function to overcome variable redefinition.
		int stringNum = codeGen.getNextLabel();

		//Put string in data segment
		codeGen.writeLine("_TEXT ENDS");
		codeGen.writeLine("_DATA SEGMENT");
		temp << "STRNAME" << stringNum <<" BYTE "; 
		
		string theString = strTable.lookUp(nextToken.info);
		//Dirty string hack #4
		if(theString.front() == '\"' ){
			theString = theString.substr(2);
		}else{
			temp<<"\"";
		}
		temp << theString;

		//Dirty string hack #3
		if(temp.str().back() == '\"'){
			temp.seekp((long)temp.tellp() - 2);
		}else{
			temp << "\"";
		}
		temp << ", 00h";
		codeGen.writeLine(temp.str());
		codeGen.writeLine("_DATA ENDS");
		codeGen.writeLine("_TEXT SEGMENT");

		//push string address onto the stack
		temp.str("");
		temp<<"PUSH OFFSET STRNAME" << stringNum;
		codeGen.writeLine(temp.str());
		NextToken();	

		*hasStringArgs = true;
	}else{
		Exp();
	}

	argCount++;
	while(nextToken.type != TOKEN_OUT_PAREN){
		MatchToken(TOKEN_COMMA);
		if(nextToken.type == TOKEN_STRING){
			//Clear misuse of the getNextLabel function to overcome variable redefinition.
			int stringNum = codeGen.getNextLabel();

			//Put string in data segment
			codeGen.writeLine("_TEXT ENDS");
			codeGen.writeLine("_DATA SEGMENT");
			temp << "STRNAME" << stringNum <<" BYTE "; 
		
			string theString = strTable.lookUp(nextToken.info);
			//Dirty string hack #4
			if(theString.front() == '\"' ){
				theString = theString.substr(2);
			}else{
				temp<<"\"";
			}
			temp << theString;

			//Dirty string hack #3
			if(temp.str().back() == '\"'){
				temp.seekp((long)temp.tellp() - 2);
			}else{
				temp << "\"";
			}
			temp << ", 00h";
			codeGen.writeLine(temp.str());
			codeGen.writeLine("_DATA ENDS");
			codeGen.writeLine("_TEXT SEGMENT");

			//push string address onto the stack
			temp.str("");
			temp<<"PUSH OFFSET STRNAME" << stringNum;
			codeGen.writeLine(temp.str());
			NextToken();	

			*hasStringArgs = true;
		}else{
			Exp();
		}
		argCount++;
		
	}
	*argNum = argCount;
}


Parser::Parser(char* fileName, char* output){
	scanner.setUp(fileName, &strTable);
	codeGen.setup(output);
	symbolTable.associateCodeGen(&codeGen);
	symbolTable.associateStringTable(&strTable);
	NextToken();
}

Parser::~Parser(){

}

void Parser::parse(){
	Program();
}