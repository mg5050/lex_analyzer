/*
Michael Gonzalez
02.28.2015
CS 280
Program #2
Lexical Analyzer
v1.1 "final"
lex.h - Defines token classes, constants, and declares functions
*/

#ifndef __LEX_INCLUDED__
#define __LEX_INCLUDED__

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "p2lex.h"

#define CharDigit_to_INT(a)	(a - '0')
#define INVALID_CHAR 		(-1)
#define INVALID_TOKEN		(-1)
#define MAX_TOKEN 			(11)
#define ___QUIET___ // hide messages prompting user, i.e. - easier correctness test

using namespace std;

class tkn_Builder;
class tkn_File;
class tkn_List;

bool 	KWCompareAhead_INPUT(const char * keyword);
bool 	SLASHPeek_INPUT();
bool 	getToken_FILE(tkn_List * tknList, const char * fname);
bool 	getToken_INPUT(tkn_List * tknList);
bool 	isDigit(char c);
bool 	isLetter(char c);
bool 	isOperator(char c);
bool 	isOperatorType(int type);
bool 	isValidPrefixType(int type);
bool 	isValidTokenChar(char c);
char 	toLower(char c);
int 	GetOperatorType(char c);
string 	CharVector_to_String(vector<char> v);
void 	TrimINPUT(string &str);
void 	lineSkip_INPUT();

class tkn_Builder
{
	private:
		vector<char> tkn;
		int type;
	public:
		tkn_Builder() : type(INVALID_TOKEN) {}
		bool validate()
		{
			if(internal_validate(type)) return true;
			#ifndef ___QUIET___
			cout << "\n\tERROR: ";
			display();
			#endif
			return false;
		}
		bool internal_validate(int possible_type)
		{
			if(tkn.size() == 0) return false;
			switch(possible_type)
			{
				case(ID): // starts with a letter and is followed by zero or more letters
				{
					for(int i = 0; i < tkn.size(); i++) if(!isLetter(tkn[i])) return false;
					return true;
				}
				case(INT):
				{
					for(int i = 0; i < tkn.size(); i++) if(!isDigit(tkn[i])) return false;
					return true;
				}
				case(STRING): // a sequence of characters, all on one line, enclosed in double quotes
				{
					int size = tkn.size();
					if(size < 2) return false;
					if(tkn[0] != '\"' || tkn[size - 1] != '\"') return false;
					for(int i = 0; i < tkn.size(); i++) if(tkn[i] == '\n') return false;
					return true;
				}
				case(PLUS):
				{
					if(tkn.size() == 1 && tkn[0] == '+') return true;
					return false;
				}
				case(MINUS):
				{
					if(tkn.size() == 1 && tkn[0] == '-') return true;
					return false;
				}
				case(STAR):
				{
					if(tkn.size() == 1 && tkn[0] == '*') return true;
					return false;
				}
				case(SLASH):
				{
					if(tkn.size() == 1 && tkn[0] == '/') return true;
					return false;
				}
				case(SET):
				{
					if
					(
						tkn.size() == 3			&&
						toLower(tkn[0]) == 's' 	&&
						toLower(tkn[1]) == 'e' 	&& 
						toLower(tkn[2]) == 't'
					) return true;
					return false;
				}
				case(PRINT):
				{
					if
					(
						tkn.size() == 5			&&
						toLower(tkn[0]) == 'p' 	&&
						toLower(tkn[1]) == 'r' 	&& 
						toLower(tkn[2]) == 'i' 	&&
						toLower(tkn[3]) == 'n' 	&& 
						toLower(tkn[4]) == 't'
					) return true;
					return false;
				}
				case(SC):
				{
					if(tkn.size() == 1 && tkn[0] == ';') return true;
					return false;
				}
				case(DONE): return false;
				case(INVALID_TOKEN): return false;
				default: return false;
			}
		}
		int size()
		{
			return tkn.size();
		}
		void display()
		{
			cout << "Vector: ";
			for(int i = 0; i < tkn.size(); i++) cout << tkn[i];
			cout << " | Type: " << getType() << endl;
		}
		void clear()
		{
			tkn.clear();
		}
		void reset()
		{
			clear();
			setType(INVALID_TOKEN);
		}
		void push(char c) // add element to partial token
		{
			tkn.push_back(c);
		}
		void pushmany(const char * word) // set token to specified char array
		{
			int i = 0;
			while(word[i] != '\0')
			{
				push(word[i]);
				i++;
			}
		}
		void pushstr(string word) // set token to specified string type
		{
			for(int i = 0; i < word.length(); i++) push(word[i]);
		}
		void pop() // remove last elem of partial token
		{
			tkn.pop_back();
		}
		int getType() // returns token type
		{
			return type;
		}
		void setType(int t) // returns token type
		{
			type = t;
		}
		string get()
		{
			return CharVector_to_String(tkn);
		}
		string getID() // returns string
		{
			return get();
		}
		int getINT() // return int
		{
			int val = 0, place_value = 1; // value of that sig fig
			for(int i = tkn.size() - 1; i >= 0; i--) // starting at least sig fig
			{
				val += CharDigit_to_INT(tkn[i]) * place_value;
				place_value *= 10;
			}
			return val;
		}
		string getSTRING() // returns string
		{
			string str = get();
			str.erase(0, 1); // erase leading "
			str.erase(str.length() - 1, 1); // erase trailing "
			return str;
		}
};

class tkn_File
{
	ifstream tknFile;
	public:
		tkn_File(const char * fname)
		{
			tknFile.open(fname, ifstream::in);
		}
		bool validFile()
		{
			if(!tknFile) return false;
			return true;
		}
		void lineSkip()
		{
			char c;
			while(tknFile.get(c) && c != '\n') {}
			if(c == EOF) tknFile.unget();
		}
		istream& unget()
		{
			return tknFile.unget();
		}
		istream& get(char &byte)
		{
			return tknFile.get(byte);
		}
		bool eof()
		{
			return tknFile.eof();
		}
		void close()
		{
			tknFile.close();
		}
		bool SLASHPeek() // look for slash ahead
		{
			char c;
			tknFile.get(c);
			if(tknFile.eof())
			{
				tknFile.unget();
				return false;
			}
			if(c == '/') return true;
			tknFile.unget();
			return false;
		}
		bool KWCompareAhead(const char * keyword) // keyword compare ahead
		{
			char c;
			int i = 0;
			while(keyword[i] != '\0')
			{
				if(tknFile.eof())
				{
					for(int j = 0; j < i; j++) tknFile.unget(); // unget characters, put file pointer back
					return false;
				}
				tknFile.get(c);
				if(!isLetter(c) || (toLower(c) != toLower(keyword[i]))) // not a letter or not a match
				{
					for(int j = 0; j <= i; j++) tknFile.unget(); // unget characters, put file pointer back
					return false;
				}
				i++;
			}
			return true;
		}
};

class tkn_List
{
	vector<tkn_Builder *> tknList;
	public:
		bool push(tkn_Builder * &tkn) // token pointer passed by reference
		{
			if(!tkn->validate()) // validate token
			{
				tkn = new tkn_Builder(); // reset token
				tkn->setType(DONE);
				return false;
			}
			tknList.push_back(tkn);
			tkn = new tkn_Builder(); // reset token
			return true;
		}
		int size()
		{
			return tknList.size();
		}
		tkn_Builder * retrieve(int index)
		{
			if(index >= tknList.size() || index < 0) return NULL;
			return tknList[index];
		}
		void display()
		{	
			int listsize = tknList.size();
			if(listsize == 0) 
			{
				#ifndef ___QUIET___
				cout << "\n\tNo tokens found.\n\n";
				#endif
				return;
			}
			int count[MAX_TOKEN] = {0};
			map<string, int> id;
			
			for(int i = 0; i < listsize; i++) 
			{
				count[tknList[i]->getType()] ++;
				if(tknList[i]->getType() == ID) id[tknList[i]->get()]++;
			}
			#ifndef ___QUIET___
			cout << "\nOUTPUT: Construct -> count\n";
			#endif
			
			cout << "\nTotal count:\n";
			if(count[ID] > 0) cout 		<< "\tID\t->\t" << count[ID] << endl;
			if(count[INT] > 0) cout 	<< "\tINT\t->\t" << count[INT] << endl;
			if(count[STRING] > 0) cout 	<< "\tSTRING\t->\t" << count[STRING] << endl;
			if(count[PLUS] > 0) cout 	<< "\tPLUS\t->\t" << count[PLUS] << endl;
			if(count[MINUS] > 0) cout 	<< "\tMINUS\t->\t" << count[MINUS] << endl;
			if(count[STAR] > 0) cout 	<< "\tSTAR\t->\t" << count[STAR] << endl;
			if(count[SLASH] > 0) cout 	<< "\tSLASH\t->\t" << count[SLASH] << endl;
			if(count[SET] > 0) cout 	<< "\tSET\t->\t" << count[SET] << endl;
			if(count[PRINT] > 0) cout 	<< "\tPRINT\t->\t" << count[PRINT] << endl;
			if(count[SC] > 0) cout 		<< "\tSC\t->\t" << count[SC] << endl;
			
			cout << "\nIdentifiers:\n";
			for(map<string, int>::iterator i = id.begin(); i != id.end(); i++) 
			{
				cout << '\t' << i->first << "\t->\t" << i->second << endl;
			}
			
			cout << endl;
		}
		void pushPrev(tkn_Builder * &tkn, tkn_File * tknFile) // push the previous token to list
		{
			tkn->pop();
			tknFile->unget();
			push(tkn); // something preceded operator
		}
		void pushPrev_INPUT(tkn_Builder * &tkn) // push the previous token to list
		{
			tkn->pop();
			cin.unget();
			push(tkn); // something preceded operator
		}
		void handleEOF(tkn_Builder * &tkn)
		{
			if(tkn->getType() != INVALID_TOKEN) push(tkn);
			tkn->setType(DONE);
		}
};

#endif