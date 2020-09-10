//***************************************
//
// gpio.h
// GPIO Abstract Class Definition
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

class gpio
{
public:
	gpio();
	//gpio(int pin); //non i2c gpio
	virtual ~gpio() = 0;

	// GPIO Initilization
	virtual DF_ERROR setDirection(bool input) = 0;
	virtual DF_ERROR readPin(bool* level) = 0;
	virtual DF_ERROR writePin(bool level) = 0;

	// virtual DF_ERROR setPin_on(int address, int pinNum);
	// virtual DF_ERROR setPin_off(int address, int pinNum);

	// Getters
	virtual int getMCPAddress() {};
	virtual int getMCPPin() {};

	// Functions for Threaded GPIO Interrupts
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
