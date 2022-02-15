//***************************************
//
// dgbgpio.h
// GPIO Debugger Implementation:
// System level child class of GPIO
//
// created: 19-06-2020
// by:Lode Ameije & Ash Singla
//
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************
#include "dbggpio.h"

// Default Ctor
dbggpio::dbggpio()
{
}

// Ctor with Pin Address Reference
dbggpio::dbggpio(int address)
{
}

// Dtor
dbggpio::~dbggpio()
{
}

// XXX: This does not have a reference anywhere!
// Setter for the pin direction
DF_ERROR dbggpio::setPinAsInputElseOutput(bool input)
{
	debugOutput debugInfo;
	debugInfo.sendMessage("dbggpio::setPinAsInputElseOutput=-=-=-=", MSG_INFO);

	DF_ERROR df_ret = OK;

	m_input = input;
	// this->m_mcp->pinMode(m_nAddress, OUTPUT);
	// if (m_input) {
	// 	this->m_mcp->pinMode(m_nAddress, INPUT);
	// }

	return df_ret;
}

// XXX: This does not have a reference anywhere!
// Getter for the pin HIGH or LOW
DF_ERROR dbggpio::readPin(bool *level)
{
	debugOutput debugInfo;
	debugInfo.sendMessage("readPin", MSG_INFO);

	DF_ERROR df_ret = ERROR_BAD_PARAMS;

	// if (m_input && (nullptr != level)) {
	// 	*level = this->m_mcp->digitalRead(m_nAddress);
	// 	df_ret = OK;
	// }

	return df_ret;
}

// XXX: This does not have a reference anywhere!
// Acuation for the pin; true = HIGH - false = LOW
DF_ERROR dbggpio::writePin(bool level)
{
	debugOutput debugInfo;
	debugInfo.sendMessage("writePin", MSG_INFO);

	DF_ERROR df_ret = ERROR_BAD_PARAMS;

	// if (!m_input) {
	// 	this->m_mcp->digitalWrite(m_nAddress, level);   //relies on bool to int
	// 	df_ret = OK;
	// }

	return df_ret;
}