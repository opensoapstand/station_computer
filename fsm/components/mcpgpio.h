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

#define DEFAULT_BUS 2 //i2cdetect tool to find the corresponding value
					  //Odyessey is 2 and Udoo is 0

class mcpGPIO : public gpio
{
public:
	mcpGPIO(int i2caddress, int pin);
	~mcpGPIO();

	//solenoid control, pin number should be inherited from gpio class
	DF_ERROR setDirection(bool input);
	DF_ERROR readPin(bool* level);
	DF_ERROR writePin(bool level);

	int getMCPAddress();
	int getMCPPin();

protected:
	void monitorGPIO();
	int	m_i2cAddress;
	MCP23017 * m_mcp;
	int m_nAddress; //address of the mcp chip

	int convert_to_int(int addressNum);
};

#endif