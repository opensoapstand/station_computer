//***************************************
//
// dgbgpio.h.h
// system level child class that are use for debug only
//
// created: 19-06-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************
#ifndef _DBGGPIO__H__
#define _DBGGPIO__H__

#include "gpio.h"

class dbggpio : public gpio{
public:
    dbggpio();
	dbggpio(int address);
	~dbggpio();

	DF_ERROR setDirection(bool input);
	DF_ERROR readPin(bool* level);
	DF_ERROR writePin(bool level);

protected:
	void monitorGPIO();
};

#endif