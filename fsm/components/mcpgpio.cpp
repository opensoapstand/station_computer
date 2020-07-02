//***************************************
//
// mcpgpio.cpp
// implementation of GPIO for i2c gpio extender
//
// created: 15-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "mcpgpio.h"
#include <iostream>

#define DEFAULT_BUS 2

#define X20 20
#define X21 21
#define X22 22

mcpGPIO::mcpGPIO(int address)
{
	debugOutput::sendMessage("mcpGPIO", INFO);

	this->m_nAddress = address;
	this->m_mcp = new MCP23017(DEFAULT_BUS, m_nAddress);

	this->m_mcp->openI2C();
}

mcpGPIO::~mcpGPIO()
{
	debugOutput::sendMessage("~mcpGPIO", INFO);
	this->m_mcp->closeI2C();
	delete (this->m_mcp);
}

DF_ERROR mcpGPIO::setDirection(bool input)
{
	debugOutput::sendMessage("setDirection", INFO);
	DF_ERROR df_ret = OK;

	m_input = input;
	this->m_mcp->pinMode(m_nPin, OUTPUT);

	if (m_input) {
		this->m_mcp->pinMode(m_nPin, INPUT);
	}	

	return df_ret;
}

DF_ERROR mcpGPIO::readPin(bool * level) //may not be use or needed 
{
	debugOutput::sendMessage("readPin", INFO);
	DF_ERROR df_ret = ERROR_BAD_PARAMS;

	
	if (m_input && (nullptr != level)) {
		*level = this->m_mcp->digitalRead(m_nAddress);
		df_ret = OK;
	}

	return df_ret;
}

DF_ERROR mcpGPIO::writePin(bool level) //control of the cassettes
{
	debugOutput::sendMessage("writePin", INFO);
	DF_ERROR df_ret = ERROR_BAD_PARAMS;

	if(m_nPin < 0 && m_nPin > 15)
	{
		return df_ret; //pin number out of range
	}

	if (HIGH == level) {
		this->m_mcp->digitalWrite(m_nPin, HIGH);   //relies on bool to int
		df_ret = OK;
	}
	else
	{
		this->m_mcp->digitalWrite(m_nPin, LOW);   //relies on bool to int
		df_ret = OK;
	}
	

	return df_ret;
}

void mcpGPIO::monitorGPIO()
{
	//!!! look at oddyseyx86GPIO for example
}
