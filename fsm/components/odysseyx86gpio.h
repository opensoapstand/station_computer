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
#include "../objects/messageMediator.h"
#include <poll.h>

class messageMediator;

class oddyseyx86GPIO : public gpio
{
public:
	oddyseyx86GPIO();
	oddyseyx86GPIO(int address);
	//        oddyseyx86GPIO(messageMediator * message);
	~oddyseyx86GPIO();

	//setter
	DF_ERROR setFlowPin(int pinNumber);

	//
	DF_ERROR setDirection(bool input);
	DF_ERROR readPin(bool *level);
	DF_ERROR writePin(bool level);

	bool reader = true;

protected:
	void monitorGPIO();
	void monitorGPIO_PWR();
	string command_to_string(string cmd);
	messageMediator *m_pMessaging;

private:
	struct pollfd pfd;

	double tickcounter;

	char compareChar2;

	bool readButtonPin(int pin);
};

#endif
