/*
Michael Gonzalez
02.28.2015
CS 280
Program #2
Lexical Analyzer
v1.1 "final"
lex.cpp - Defines lexical functions
*/

#include "lex.h"

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

bool getToken_FILE(tkn_List * tknList, const char * fname)
{
	tkn_Builder * tkn = new tkn_Builder();
	tkn_File tknFile(fname);
	if(!tknFile.validFile()) return false;
	char byte;
	
	while(tknFile.get(byte) && tkn->getType() != DONE)
	{
		tkn->push(byte); // add byte to partial token
		switch(byte) // parsing first byte of sequence
		{
			case('s'): // SET, set keyword
			case('S'):
			{
				if(tkn->getType() == INVALID_TOKEN) // first char is S
				{
					if(tknFile.KWCompareAhead("et"))
					{
						tkn->setType(SET);
						tkn->pushmany("et");
						tknList->push(tkn);
					}
					else tkn->setType(ID); // must be ID
				}
				else if(tkn->getType() == INT) tknList->pushPrev(tkn, &tknFile);
				break;
			}
			case('p'): // PRINT, print keyword
			case('P'):
			{
				if(tkn->getType() == INVALID_TOKEN) // first char is P
				{
					if(tknFile.KWCompareAhead("rint"))
					{
						tkn->setType(PRINT);
						tkn->pushmany("rint");
						tknList->push(tkn);
					}
					else tkn->setType(ID);
				}
				else if(tkn->getType() == INT) tknList->pushPrev(tkn, &tknFile);
				break;
			}
			case 'a' ... 'o':
			case 'q' ... 'r':
			case 't' ... 'z':
			case 'A' ... 'O':
			case 'Q' ... 'R':
			case 'T' ... 'Z':
			{
				if(tkn->getType() == INVALID_TOKEN) tkn->setType(ID);
				else if(tkn->getType() == INT) tknList->pushPrev(tkn, &tknFile);
				break;
			}
			case '0' ... '9':
			{
				if(tkn->getType() == INVALID_TOKEN) tkn->setType(INT);
				else if(tkn->getType() == ID) tknList->pushPrev(tkn, &tknFile);
				break;
			}
			case('"'): // STRING, start of string
			{
				if(tkn->getType() == STRING) tknList->push(tkn); // closing "
				else if(tkn->getType() == INVALID_TOKEN) tkn->setType(STRING); // opening "
				break;
			}
			case(';'):
			case('*'):
			case('-'):
			case('+'): // PLUS, plus operator
			{
				if(tkn->getType() == INVALID_TOKEN) 
				{
					tkn->setType(GetOperatorType(byte));
					tknList->push(tkn);
				}
				else tknList->pushPrev(tkn, &tknFile); // push previous token
				break;
			}
			case ('/'): // SLASH, division operator and start of comment
			{	
				if(tkn->getType() == INVALID_TOKEN)
				{
					if(tknFile.SLASHPeek()) 
					{
						tknFile.lineSkip(); // we found a comment
						tkn->clear();
					}
					else // we found a division operator
					{
						tkn->setType(SLASH);
						tknList->push(tkn);
					}
				}
				else tknList->pushPrev(tkn, &tknFile); // push previous token
				break;
			}
			default: // everything else is treated as whitespace
			{
				if(tkn->getType() == STRING) break;// only strings allowed to have whitespace
				if(tkn->getType() != INVALID_TOKEN) tknList->pushPrev(tkn, &tknFile); // push previous token
				else tkn->clear(); // wipe whitespace from tkn
				break; // keep looking
			}
		}
	}
	if(tknFile.eof()) tknList->handleEOF(tkn);
	tknFile.close();
	return true;
}

bool getToken_INPUT(tkn_List * tknList)
{
	tkn_Builder * tkn = new tkn_Builder();
	char byte;
	
	while(cin.get(byte) && tkn->getType() != DONE)
	{
		tkn->push(byte); // add byte to partial token
		switch(byte) // parsing first byte of sequence
		{
			case('s'): // SET, set keyword
			case('S'):
			{
				if(tkn->getType() == INVALID_TOKEN) // first char is S
				{
					if(KWCompareAhead_INPUT("et"))
					{
						tkn->setType(SET);
						tkn->pushmany("et");
						tknList->push(tkn);
					}
					else tkn->setType(ID); // must be ID
				}
				else if(tkn->getType() == INT) tknList->pushPrev_INPUT(tkn);
				break;
			}
			case('p'): // PRINT, print keyword
			case('P'):
			{
				if(tkn->getType() == INVALID_TOKEN) // first char is P
				{
					if(KWCompareAhead_INPUT("rint"))
					{
						tkn->setType(PRINT);
						tkn->pushmany("rint");
						tknList->push(tkn);
					}
					else tkn->setType(ID);
				}
				else if(tkn->getType() == INT) tknList->pushPrev_INPUT(tkn);
				break;
			}
			case 'a' ... 'o':
			case 'q' ... 'r':
			case 't' ... 'z':
			case 'A' ... 'O':
			case 'Q' ... 'R':
			case 'T' ... 'Z':
			{
				if(tkn->getType() == INVALID_TOKEN) tkn->setType(ID);
				else if(tkn->getType() == INT) tknList->pushPrev_INPUT(tkn);
				break;
			}
			case '0' ... '9':
			{
				if(tkn->getType() == INVALID_TOKEN) tkn->setType(INT);
				else if(tkn->getType() == ID) tknList->pushPrev_INPUT(tkn);
				break;
			}
			case('"'): // STRING, start of string
			{
				if(tkn->getType() == STRING) tknList->push(tkn); // closing "
				else if(tkn->getType() == INVALID_TOKEN) tkn->setType(STRING); // opening "
				break;
			}
			case(';'):
			case('*'):
			case('-'):
			case('+'): // PLUS, plus operator
			{
				if(tkn->getType() == INVALID_TOKEN) 
				{
					tkn->setType(GetOperatorType(byte));
					tknList->push(tkn);
				}
				else tknList->pushPrev_INPUT(tkn); // push previous token
				break;
			}
			case ('/'): // SLASH, division operator and start of comment
			{	
				if(tkn->getType() == INVALID_TOKEN)
				{
					if(SLASHPeek_INPUT()) 
					{
						lineSkip_INPUT(); // we found a comment
						tkn->clear();
					}
					else // we found a division operator
					{
						tkn->setType(SLASH);
						tknList->push(tkn);
					}
				}
				else tknList->pushPrev_INPUT(tkn); // push previous token
				break;
			}
			default: // everything else is treated as whitespace
			{
				if(tkn->getType() == STRING) break;// only strings allowed to have whitespace
				if(tkn->getType() != INVALID_TOKEN) tknList->pushPrev_INPUT(tkn); // push previous token
				else tkn->clear(); // wipe whitespace from tkn
				break; // keep looking
			}
		}
	}
	if(cin.eof()) tknList->handleEOF(tkn);
	return true;
}

string CharVector_to_String(vector<char> v)
{
	string str;
	for(int i = 0; i < v.size(); i++) 
	{
		str += v[i];
	}
	return str;
}

bool isOperator(char c)
{
	if(c == '+' || c == '-' || c == '*' || c == '/') return true;
	return false;
}

bool isLetter(char c)
{
	if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return true;
	return false;
}

bool isDigit(char c)
{
	if(c >= '0' && c <= '9') return true;
	return false;
}

char toLower(char c)
{
	if(c >= 'A' && c <= 'Z') return c + 32;
	return c;
}

bool isOperatorType(int type)
{
	if(type == PLUS || type == MINUS || type == STAR || type == SLASH) return true;
	return false;
}

int GetOperatorType(char c)
{
	switch(c)
	{
		case('+'): return PLUS;
		case('-'): return MINUS;
		case('*'): return STAR;
		case('/'): return SLASH;
		case(';'): return SC;
	}
	return INVALID_TOKEN;
}

bool isValidPrefixType(int type) // is this type of character allowed to be directly next to another type? e.g. - word+word2
{
	if(isOperatorType(type) || type == SC) return true;
	return false;
}

void TrimINPUT(string &str)
{
	int i = 0, len = str.length();
	if(len == 0) return;
	while(i < len && !isValidTokenChar(str[i])) { i++; } // trim front
	int pos = i;
	if(pos > 0) str.erase(0, pos);
	len = str.length();
	i = len - 1;
	while(i >= 0 && !isValidTokenChar(str[i])) { i--; } // trim back
	pos = i + 1;
	if(pos < len - 1) str.erase(pos, len - pos);
	return;
}

bool isValidTokenChar(char c)
{
	if(isOperator(c) || isLetter(c) || isDigit(c) || c == '\"' || c ==';') return true;
	return false;
}

bool KWCompareAhead_INPUT(const char * keyword) // keyword compare ahead, INPUT version
{
	char c;
	int i = 0;
	while(keyword[i] != '\0')
	{
		if(cin.eof())
		{
			for(int j = 0; j < i; j++) cin.unget(); // unget characters, put file pointer back
			return false;
		}
		cin.get(c);
		if(!isLetter(c) || (toLower(c) != toLower(keyword[i]))) // not a letter or not a match
		{
			for(int j = 0; j <= i; j++) cin.unget(); // unget characters, put file pointer back
			return false;
		}
		i++;
	}
	return true;
}

bool SLASHPeek_INPUT() // look for slash ahead, INPUT version
{
	char c;
	cin.get(c);
	if(cin.eof())
	{
		cin.unget();
		return false;
	}
	if(c == '/') return true;
	cin.unget();
	return false;
}

void lineSkip_INPUT()
{
	char c;
	while(cin.get(c) && c != '\n') {}
	if(c == EOF) cin.unget();
}