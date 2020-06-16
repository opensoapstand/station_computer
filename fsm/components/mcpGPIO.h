//***************************************
//
// mcpgpio.h
// implementation of GPIO via i2c extender
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

protected:
	void monitorGPIO();

	MCP23017 * m_mcp;
};

#endif