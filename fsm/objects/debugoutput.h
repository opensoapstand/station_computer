//***************************************
//
// debugOutput.h
// DEBUG output Definition:
// threadsafe outputs to console and 
// override for other systems
//
// created: 15-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************
#ifndef _DEBUGOUTPUT__H_
#define _DEBUGOUTPUT__H_

#include <string>

using namespace std;

// Dictionary for Console Output Message Level
typedef enum MESSAGE_LEVEL {
	INFO,
	WARN,
	ERROR,
	STATE_CHANGE,
	PIN_CHANGE,
	MESSAGE_MAX
} MESSAGE_LEVEL;

class debugOutput
{
public:
	debugOutput();
	~debugOutput();

	//setter
	static bool setMessageLevel(MESSAGE_LEVEL dbgLvl);	
	static void sendMessage(string msg, MESSAGE_LEVEL lvl);

	static MESSAGE_LEVEL m_dbgLvl;
protected:
};

#endif