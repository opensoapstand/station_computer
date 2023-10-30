//***************************************
//
// debugOutput.cpp
// DEBUG output Implementation:
// threadsafe outputs to console and
// override for other systems
//
// created: 15-06-2020
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "debugOutput.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <fstream>
#include <ctime>

#include <sstream>
#include <iomanip>

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

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
string debugOutput::getCurrentDateTime()
{
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	// strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tstruct);

	using namespace std::chrono;
	uint64_t millis_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	uint64_t millis = millis_since_epoch % 1000;

	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tstruct);
	string timeStr = std::string(buf);

	string returnString = timeStr + "." + std::to_string(millis);
	return returnString;
}

string debugOutput::getCurrentDate()
{
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);

	return buf;
}

string debugOutput::getCurrentTime()
{
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	using namespace std::chrono;
	uint64_t millis_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	uint64_t millis = millis_since_epoch % 1000;

	strftime(buf, sizeof(buf), "%H:%M:%S", &tstruct);
	string timeStr = std::string(buf);

	string returnString = timeStr + "." + std::to_string(millis);
	return returnString;
}

void debugOutput::sendMessage(std::string msg, MESSAGE_LEVEL lvl)
{
	// https://stackoverflow.com/questions/2393345/how-to-append-text-to-a-text-file-in-c

	std::ofstream outfile;

	string log_path = "/home/df-admin/production/logging/controller/controller_" + getCurrentDate() + ".txt";

	outfile.open(log_path, std::ios_base::app); // append instead of overwrite // will close automatically at destruction

	using namespace std::chrono;
	uint64_t millis_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

	if (lvl >= debugOutput::m_dbgLvl)
	{

		cerr << getCurrentDateTime() + " " + m_lvlArray[lvl] + ": " + msg << endl;
		outfile << getCurrentTime() + " " + m_lvlArray[lvl] + ": " + msg << endl;
	}
}
