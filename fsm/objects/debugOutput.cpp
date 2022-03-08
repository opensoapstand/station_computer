//***************************************
//
// debugOutput.cpp
// DEBUG output Implementation:
// threadsafe outputs to console and
// override for other systems
//
// created: 15-06-2020
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "debugOutput.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <fstream>

using namespace std;

// FIXME: Can be moved to dftypes?
const string m_lvlArray[] = {"INFO", "WARNING", "ERROR", "STATE", "MESSAGE_MAX"};
MESSAGE_LEVEL debugOutput::m_dbgLvl = MSG_INFO;

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
	// https://stackoverflow.com/questions/2393345/how-to-append-text-to-a-text-file-in-c
	
	std::ofstream outfile;
	outfile.open("/home/df-admin/drinkfill/log/soapstand_log_fsm_all.txt", std::ios_base::app); // append instead of overwrite // will close automatically at destruction

	using namespace std::chrono;
    uint64_t millis_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();



	if (lvl >= debugOutput::m_dbgLvl)
	{
		// original. outputting this might lead to segmentation errors
		cerr << to_string(millis_since_epoch)  + " : " + m_lvlArray[lvl] + ": " +  msg << endl;

		outfile << to_string(millis_since_epoch)  + " : " + m_lvlArray[lvl] + ": " +  msg << endl;

		
		//	cout << lvl << endl;
		//sprintf("tetst",);
		//cout << m_lvlArray[lvl] + ": " + msg << endl;

		// 	//https://stackoverflow.com/questions/9469790/execution-of-printf-and-segmentation-fault
		// 	// debugOutput::sendMessage("oddyseyx86GPIO::setPinAsInputElseOutput ", MSG_INFO);
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
