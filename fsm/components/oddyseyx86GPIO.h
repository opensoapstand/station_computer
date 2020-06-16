//***************************************
//
// oddyseyX86gpio.h
// implementation of GPIO for native x86 pins on Oddysey board
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

class oddyseyx86GPIO :
	public gpio
{
public:
	oddyseyx86GPIO(int address);
	~oddyseyx86GPIO();

	DF_ERROR setDirection(bool input);
	DF_ERROR readPin(bool* level);
	DF_ERROR writePin(bool level);

protected:
	void monitorGPIO();
};

#endif

