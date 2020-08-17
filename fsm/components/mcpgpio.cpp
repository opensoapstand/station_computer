//***************************************
//
// mcpgpio.cpp
// implementation of GPIO for i2c gpio extender
//
// Chip model: MCP23017 - 16 PIN Addresses
//
// created: 15-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "mcpgpio.h"
#include <iostream>

// CTOR
mcpGPIO::mcpGPIO(int i2caddress, int pin)
{
	debugOutput::sendMessage("------mcpGPIO------", INFO);
	m_nPin = pin;
	m_nAddress = convert_to_int(i2caddress);

	std::cout << m_nPin << std::endl;
	std::cout << m_nAddress << std::endl;

	// this->m_mcp = new MCP23017(DEFAULT_BUS, m_nAddress);

	// HACK: DEFAULT_BUS of 2 did not work.  BUS value of 1 works...
	this->m_mcp = new MCP23017(1, m_nAddress);

	//may need to modify the source file to ensure proper error is identified
	this->m_mcp->openI2C(); 
}

// DTOR
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

// may not be use or needed due to tiny xml
DF_ERROR mcpGPIO::readPin(bool * level) 
{
	debugOutput::sendMessage("readPin", INFO);
	
	DF_ERROR df_ret = ERROR_BAD_PARAMS;

	
	if (m_input && (nullptr != level)) {
		*level = this->m_mcp->digitalRead(m_nAddress);
		df_ret = OK;
	}

	return df_ret;
}

// Cassette controller base on high or low signal to acutate
DF_ERROR mcpGPIO::writePin(bool level) 
{
	debugOutput::sendMessage("writePin", INFO);

	DF_ERROR df_ret = ERROR_BAD_PARAMS;

	// 16 Pin Total for Chip Model
	// TODO: Can change magic number if model changes in future
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

// *** Getters/Setters and Utilities below ***

int mcpGPIO::convert_to_int(int addressNum)
{
	int hex_int = 0;
	
	hex_int = 16*(addressNum/10) + (addressNum%10); 	

	return hex_int;
}

int mcpGPIO::getMCPAddress()
{
	return m_nAddress;
}

int mcpGPIO::getMCPPin()
{
	return m_nPin;
}