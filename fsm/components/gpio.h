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
	virtual ~gpio() = 0;

	virtual DF_ERROR setDirection(bool input) = 0;
	virtual DF_ERROR readPin(bool* level) = 0;
	virtual DF_ERROR writePin(bool level) = 0;

	DF_ERROR setInterrupt(DF_ERROR(*pf)()); //should be virtual? only use for odysseyx86

	std::thread listener();
	void stopListener() { m_stop = true; };


protected:
	int m_nPin;		//actual pin number 
	bool m_stop;
	bool m_input;

	std::function<DF_ERROR()> m_pf;
	virtual void monitorGPIO() = 0;
};

#endif
