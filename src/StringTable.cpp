#include "StringTable.h"

StringTable::StringTable(){
	count = 1;
}

StringTable::~StringTable(){
}

int StringTable::insert(string str){
	//If its not in the table insert it. Otherwise do nothing.
	Dictionary::iterator itr = table.find(str);
	if(itr == table.end()){
		table.insert(Dictionary::value_type(str, count));
		reverseTable.insert(ReverseDictionary::value_type(count, str));
		count++;
		return count - 1;
	}else{
		return itr->second;
	}

	
}

string StringTable::lookUp(int x){
	ReverseDictionary::iterator itr = reverseTable.find(x);
	if(itr == reverseTable.end()){
		return NULL;
	}

	return itr->second;
}