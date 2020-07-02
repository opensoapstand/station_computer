//***************************************
//
// debugOutput.h
// threadsafe debug output class
// outputs to console, override for other systems
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

typedef enum MESSAGE_LEVEL {
	INFO,
	WARN,
	ERROR,
	MESSAGE_MAX
} MESSAGE_LEVEL;

class debugOutput
{
public:
	debugOutput();
	~debugOutput();

	//setter
	bool setMessageLevel(MESSAGE_LEVEL dbgLvl);
	
	
	void sendMessage(string msg, MESSAGE_LEVEL lvl);


protected:
	MESSAGE_LEVEL m_dbgLvl;
	string m_lvlArray[MESSAGE_MAX] = { "INFO", "WARNING", "ERROR" };
};

#endif