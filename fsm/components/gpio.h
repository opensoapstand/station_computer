//***************************************
//
// gpio.h
// class to abstract gpios
//
// created: 15-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************
#ifndef _GPIO__H__
#define _GPIO__H__

#include <functional>
#include <thread>

#include "../dftypes.h"
#include "../objects/debugoutput.h"
#include "definegpio.h"

class gpio
{
public:
	gpio();
	//gpio(int pin); //non i2c gpio
	virtual ~gpio() = 0;

	virtual DF_ERROR setDirection(bool input) = 0;
	virtual DF_ERROR readPin(bool* level) = 0;
	virtual DF_ERROR writePin(bool level) = 0;

	virtual int getMCPAddress() {};
	virtual int getMCPPin() {};

	DF_ERROR setInterrupt(DF_ERROR(*pf)()); 

	std::thread listener();
	void stopListener() { m_stop = true; };


protected:
	int m_nPin;
	bool m_stop;
	bool m_input;
	bool m_i2c;

	std::function<DF_ERROR()> m_pf;
	virtual void monitorGPIO() = 0;
};

#endif
