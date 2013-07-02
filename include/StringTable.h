#ifndef STRINGTABLE_H
#define STRINGTABLE_H

#include <unordered_map>
#include <string>

using namespace std;

typedef std::unordered_map<string, int> Dictionary; 
typedef std::unordered_map<int, string> ReverseDictionary; 

class StringTable{
public:
	StringTable();
	~StringTable();

	/* 
	Inserts a string value into the string table.
	@param str The string being inserted
	@return The key value after insertion.
	*/
	int insert(string str);
	string lookUp(int x);
	
private:
	int count;
	Dictionary table;
	ReverseDictionary reverseTable;
};

#endif