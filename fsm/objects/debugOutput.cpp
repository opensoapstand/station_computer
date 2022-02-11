//***************************************
//
// debugOutput.cpp
// DEBUG output Implementation:
// threadsafe outputs to console and
// override for other systems
//
// created: 15-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "debugOutput.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>

using namespace std;

// FIXME: Can be moved to dftypes?
const string m_lvlArray[] = {"INFO", "WARNING", "ERROR", "STATE_CHANGE", "PIN_CHANGE", "MESSAGE_MAX"};
MESSAGE_LEVEL debugOutput::m_dbgLvl = INFO;

// Default CTOR
debugOutput::debugOutput()
{
}

// DTOR
debugOutput::~debugOutput()
{
}

// Setter for Debugger Console
// MESSAGELEVEL: Debug message
bool debugOutput::setMessageLevel(MESSAGE_LEVEL dbgLvl)
{
	bool dbg_ret = false;

	debugOutput::m_dbgLvl = dbgLvl;
	dbg_ret = true;

	return dbg_ret;
}

/* 
* !!! this is not threadsafe at the moment
* Has issues spitting out raw addresses; cout on next line for this.
*/
void debugOutput::sendMessage(std::string msg, MESSAGE_LEVEL lvl)
{
	if (lvl >= debugOutput::m_dbgLvl)
	{
		// original. outputting this might lead to segmentation errors
		cerr << m_lvlArray[lvl] + ": " + msg << endl;

		//	cout << lvl << endl;
		//sprintf("tetst",);
		//cout << m_lvlArray[lvl] + ": " + msg << endl;

		// 	//https://stackoverflow.com/questions/9469790/execution-of-printf-and-segmentation-fault
		// 	// debugOutput::sendMessage("oddyseyx86GPIO::setDirection ", INFO);
		//     std::string msg = m_lvlArray[lvl] + ":" + msg;

		// 	//https://stackoverflow.com/questions/7352099/stdstring-to-char/7352131
		// 	//std::string str = "string";
		// 	char *cstr = new char[msg.length() + 1];
		// 	strcpy(cstr, msg.c_str());

		// 	printf("%s",cstr);
		// 	delete [] cstr;

		//     // printf("%s",name);
		//    //printf("%s",msg);
		// 	fflush(stdout);
	}
}
