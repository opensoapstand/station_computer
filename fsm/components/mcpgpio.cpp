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

#define DEFAULT_BUS 2 //i2cdetect tool to find the corresponding value
					  //Odyessey is 2 and Udoo is 0

mcpGPIO::mcpGPIO(int i2caddress, int pin)
{

	m_nPin = pin;
	m_nAddress = i2caddress;
	this->m_mcp = new MCP23017(DEFAULT_BUS, m_nAddress);

	//may need to modify the source file to ensure proper error is identified
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
	debugOutput debugInfo;
	debugInfo.sendMessage("setDirection", INFO);

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
	debugOutput debugInfo;
	debugInfo.sendMessage("readPin", INFO);
	
	DF_ERROR df_ret = ERROR_BAD_PARAMS;

	
	if (m_input && (nullptr != level)) {
		*level = this->m_mcp->digitalRead(m_nAddress);
		df_ret = OK;
	}

	return df_ret;
}

DF_ERROR mcpGPIO::writePin(bool level) //control of the cassettes
{
	debugOutput debugInfo;
	debugInfo.sendMessage("writePin", INFO);

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
		this->m_mcp->digitalWrite(m_nPin, LOW); 
		df_ret = OK;
	}

	return df_ret;
	
}

void mcpGPIO::monitorGPIO()
{
	//!!! look at oddyseyx86GPIO for example
}
