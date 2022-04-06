//***************************************
//
// dgbgpio.h
// GPIO Debugger Definition:
// System level child class of GPIO
//
// created: 19-06-2020
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************
#ifndef _DBGGPIO__H__
#define _DBGGPIO__H__

#include "gpio.h"
#include "../objects/debugOutput.h"

class dbggpio : public gpio
{
public:
	dbggpio();
	dbggpio(int address);
	~dbggpio();

	DF_ERROR setPinAsInputElseOutput(bool input);
	DF_ERROR readPin(bool *level);
	DF_ERROR writePin(bool level);

protected:
	//void monitorGPIO();
};

#endif