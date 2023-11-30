//***************************************
//
// gpio_odyssey.h
// GPIO Definitions for
// NATIVE x86 pins on Oddysey board
//
// created: 15-06-2020
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************
#ifndef _ODDYSEYX86GPIO__H__
#define _ODDYSEYX86GPIO__H__

// #include "gpio.h"
#include "../objects/messageMediator.h"
// #include <poll.h>

class messageMediator;

class gpio_odyssey
{
public:
	gpio_odyssey();
	gpio_odyssey(int address);
	~gpio_odyssey();

	DF_ERROR setPinAsInputElseOutput(bool input);
	DF_ERROR readPin(bool *level);
	DF_ERROR writePin(bool level);
	private:
		int m_nPin;
};

#endif
