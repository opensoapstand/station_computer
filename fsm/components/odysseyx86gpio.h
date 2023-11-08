//***************************************
//
// oddyseyX86gpio.h
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
	// DF_ERROR setFlowPin(int pinNumber);

	//
	DF_ERROR setPinAsInputElseOutput(bool input);
	DF_ERROR readPin(bool *level);
	DF_ERROR writePin(bool level);

protected:
	void monitorGPIO_Flowsensor(bool* abortLoop);
	// void monitorGPIO_Buttons_powerAndMaintenance();
	string command_to_string(string cmd);
	messageMediator *m_pMessaging;

private:
	struct pollfd pfd;

	double tickcounter;

	char flowsensor_state_memory;
	char button_state_memory;
	bool flowsensor_stable_edge;
	bool flowsensor_stable_state;
	bool flowsensor_stable_state_memory;

	uint64_t flowsensor_most_recent_edge_millis;


	bool readButtonPin(int pin);
};

#endif
