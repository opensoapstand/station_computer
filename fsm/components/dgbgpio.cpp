//***************************************
//
// dgbgpio.h.h
// system level child class that are use for debug only
//
// created: 19-06-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************
#include "dbggpio.h"

dbggpio::dbggpio()
{

}

dbggpio::dbggpio(int address)
{

}

dbggpio::~dbggpio()
{

}

DF_ERROR dbggpio::setDirection(bool input)
{
	debugOutput::sendMessage("setDirection", INFO);
	DF_ERROR df_ret = OK;

	m_input = input;
	// this->m_mcp->pinMode(m_nAddress, OUTPUT);
	// if (m_input) {
	// 	this->m_mcp->pinMode(m_nAddress, INPUT);
	// }	

	return df_ret;
}

DF_ERROR dbggpio::readPin(bool * level)
{
	debugOutput::sendMessage("readPin", INFO);
	DF_ERROR df_ret = ERROR_BAD_PARAMS;

	
	// if (m_input && (nullptr != level)) {
	// 	*level = this->m_mcp->digitalRead(m_nAddress);
	// 	df_ret = OK;
	// }

	return df_ret;
}

DF_ERROR dbggpio::writePin(bool level)
{
	debugOutput::sendMessage("writePin", INFO);
	DF_ERROR df_ret = ERROR_BAD_PARAMS;


	// if (!m_input) {
	// 	this->m_mcp->digitalWrite(m_nAddress, level);   //relies on bool to int
	// 	df_ret = OK;
	// }

	return df_ret;
}