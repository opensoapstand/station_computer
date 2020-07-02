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

class mcpGPIO : public gpio
{
public:
	mcpGPIO(int address);
	~mcpGPIO();

	//setter
	DF_ERROR setMCPPin(int pinNumber);

	//solenoid control, pin number should be inherited from gpio class
	DF_ERROR setDirection(bool input);
	DF_ERROR readPin(bool* level);
	DF_ERROR writePin(bool level);

	//pump contorl


    //address is to verify is the correct address
    //since pump should only work on address X22
    DF_ERROR setPump_Forward(int pinNumFWD, int pinNumREV);
    DF_ERROR setPump_Reverse(int pinNumFWD, int pinNumREV);
    DF_ERROR setPump_Off(int pinNumFWD, int pinNumREV);

protected:
	void monitorGPIO();

	MCP23017 * m_mcp;
	int m_nAddress; //address of the mcp chip
};

#endif