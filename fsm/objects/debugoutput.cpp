//***************************************
//
// debugOutput.cpp
// threadsafe debug output class
// outputs to console, override for other systems
//
// created: 15-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "debugoutput.h"
#include <iostream>
#include <fstream>

using namespace std;

debugOutput::debugOutput()
{
	m_dbgLvl = INFO;
}


debugOutput::~debugOutput()
{

}

bool debugOutput::setMessageLevel(MESSAGE_LEVEL dbgLvl)
 {
	bool dbg_ret = false;

	m_dbgLvl = dbgLvl; 
	dbg_ret = true;

	return dbg_ret;
 }

//!!! this is not threadsafe at the moment
void debugOutput::sendMessage(std::string msg, MESSAGE_LEVEL lvl)
{
	if (lvl >= m_dbgLvl)
	{
		cerr << m_lvlArray[lvl] + ": " + msg << endl;
	}
}
