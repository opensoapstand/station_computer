//***************************************
//
// FSModdyseyx86GPIO.h
// GPIO Definitions for
// NATIVE x86 pins on Oddysey board
//
// created: 15-06-2020
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************
#ifndef _FSModdyseyx86GPIO__H__
#define _FSModdyseyx86GPIO__H__

#include "gpio.h"
// #include "../objects/messageMediator.h"
// #include <poll.h>

// class messageMediator;

class FSModdyseyx86GPIO : public gpio
{
public:
	FSModdyseyx86GPIO();
	FSModdyseyx86GPIO(int pinNumber);
	~FSModdyseyx86GPIO();

	void setPinNumber(int pinNumber);

	DF_ERROR setPinAsInputElseOutput(bool input);
	DF_ERROR readPin(bool *level);
	DF_ERROR writePin(bool level);

protected:
	void monitorGPIO_Flowsensor(bool* abortLoop);
	// void monitorGPIO_Buttons_powerAndMaintenance();
	string command_to_string(string cmd);
	// messageMediator *m_pMessaging;

private:
	// struct pollfd pfd;

	double tickcounter;
	// int m_nPin;

	char flowsensor_state_memory;
	char button_state_memory;
	bool flowsensor_stable_edge;
	bool flowsensor_stable_state;
	bool flowsensor_stable_state_memory;

	uint64_t flowsensor_most_recent_edge_millis;


	bool readButtonPin(int pin);
};

#endif
