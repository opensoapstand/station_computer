//***************************************
//
// mcpgpio.h
// implementation of GPIO for native x86 pins on Oddysey board
//
// created: 15-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************
#ifndef _MCPGPIO__H__
#define _MCPGPIO__H__

#include "gpio.h"
#include "mcp23017.h"

class mcpGPIO :
	public gpio
{
public:
	mcpGPIO(int address);
	~mcpGPIO();

	DF_ERROR setDirection(bool input);
	DF_ERROR readPin(bool* level);
	DF_ERROR writePin(bool level);

	DF_ERROR setInterrupt(DF_ERROR(*pf)());

protected:
	void monitorGPIO();

	MCP23017 * m_mcp;
};

#endif