//***************************************
//
// mcpgpio.h
// GPIO Definition for i2c gpio extender chip
//
// Chip model: MCP23017 - 16 PIN Addresses
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
#include "../../library/i2c/mcp23017/mcp23017.h"

#include <iostream>
#include <cstdio>

#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

#define DEFAULT_BUS 2 //i2cdetect tool to find the corresponding value
					  //Odyssey is 2 and Udoo is 0

class mcpGPIO : public gpio
{
public:
	mcpGPIO(int i2caddress, int pin);
	~mcpGPIO();

	//solenoid control, pin number should be inherited from gpio class
	DF_ERROR setDirection(bool input);
        DF_ERROR readPin(bool* level);
        DF_ERROR writePin(bool level);

	// Getters
	int getMCPAddress();
        int getMCPPin();

        double getPressDuration(){return press_duration;}
        int getPressAmount(){return press_times;}
        void resetPressAmount(){press_times=0;}
        void resetPressDuration(){press_duration=0.0;}

private:
        bool button_pressed = false;
        double press_duration=0.0;
        int press_times=0;

protected:
        void monitorGPIO();
	int	m_i2cAddress;
	MCP23017 * m_mcp;
	int m_nAddress; //address of the mcp chip

	// Utility
	int convert_to_int(int addressNum);

        bool level;
        Clock::time_point t1;
        Clock::time_point t2;
};

#endif
