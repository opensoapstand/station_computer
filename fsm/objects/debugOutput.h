//***************************************
//
// debugOutput.h
// DEBUG output Definition:
// threadsafe outputs to console and
// override for other systems
//
// created: 15-06-2020
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************
#ifndef _DEBUGOUTPUT__H_
#define _DEBUGOUTPUT__H_

#include <string>

using namespace std;

// Dictionary for Console Output Message Level
typedef enum MESSAGE_LEVEL
{
	MSG_INFO,
	MSG_WARNING,
	MSG_ERROR,
	MSG_STATE,
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
	static string getCurrentDateTime();
	static string getCurrentDate();
	static string getCurrentTime();

protected:
};

#endif