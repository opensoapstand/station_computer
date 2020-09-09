//***************************************
//
// oddyseyX86gpio.h
// GPIO Definitions for 
// NATIVE x86 pins on Oddysey board
//
// created: 15-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************
#ifndef _ODDYSEYX86GPIO__H__
#define _ODDYSEYX86GPIO__H__

#include "gpio.h"
#include <poll.h>

class oddyseyx86GPIO : public gpio
{
public:
	oddyseyx86GPIO();
	oddyseyx86GPIO(int address);
	~oddyseyx86GPIO();

	//setter
	DF_ERROR setFlowPin(int pinNumber);

	//
	DF_ERROR setDirection(bool input);
	DF_ERROR readPin(bool* level);
	DF_ERROR writePin(bool level);
	// DF_ERROR setPin_on(int address, int pinNum);
	// DF_ERROR setPin_off(int address, int pinNum);

	DF_ERROR setButton_on();
	DF_ERROR setButton_off();

protected:
	void monitorGPIO();
	string command_to_string(string cmd);

private:
	struct pollfd pfd;
};

#endif

