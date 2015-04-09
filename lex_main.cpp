/*
Michael Gonzalez
02.28.2015
CS 280
Program #2
Lexical Analyzer
v1.1 "final"
lex_main.cpp - Tests getToken functions
*/

#include "lex.h"

int ConsoleRoutine(tkn_List * tknList);

int main(int argc, char * argv[]) 
{
	if(argc > 2) // first argument is program name
	{
		cout << "\n\tERROR: " << argc - 1 << " arguments given.\n\tClosing program...\n\n";
		return false;
	}

	tkn_List tknList;
	
	if(argc == 1) 
	{
		#ifndef ___QUIET___ // hide messages that make stdin vary from file input, i.e. - easier checking by test program
		cout << "\n\tNo filename specified.\n\tReading input from console...\n\n";
		#endif
		return ConsoleRoutine(&tknList);
	}

	bool open = getToken_FILE(&tknList, argv[1]);
	if(!open)
	{
		cout << "\n\tERROR: Could not open file.\n\tClosing program...\n\n";
		return false;
	}
	
	tknList.display();
	return false;
}

int ConsoleRoutine(tkn_List * tknList)
{
	#ifndef ___QUIET___ // hide messages that make stdin vary from file input, i.e. - easier checking by test program
	cout << "\nINPUT:\n-Reading from standard input\n\n";
	#endif
	getToken_INPUT(tknList);
	tknList->display();
	return false;
}