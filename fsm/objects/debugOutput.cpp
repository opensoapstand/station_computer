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

using namespace std;

// FIXME: Can be moved to dftypes?
const string m_lvlArray[] = { "INFO", "WARNING", "ERROR" };
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
                cerr << m_lvlArray[lvl] + ": " + msg << endl;
	}
}
