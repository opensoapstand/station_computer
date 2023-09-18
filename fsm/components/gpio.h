//***************************************
//
// gpio.h
// GPIO Abstract Class Definition
//
// created: 15-06-2020
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************
#ifndef _GPIO__H__
#define _GPIO__H__

#include <functional>
#include <thread>

#include "../dftypes.h"
#include "../objects/debugOutput.h"
#include "../objects/product.h"  // lode uncomment

class gpio
{
public:
	gpio();
	//gpio(int pin); //non i2c gpio
	virtual ~gpio() = 0;

	// GPIO Initilization
	virtual DF_ERROR setPinAsInputElseOutput(bool input) = 0;
	virtual DF_ERROR readPin(bool *level) = 0;
	virtual DF_ERROR writePin(bool level) = 0;
	// virtual void test()=0;

	// Getters
	// TODO these chouldn't be here, MCP does not exist at this level
	virtual int getMCPAddress(){};
	virtual int getMCPPin(){};

	// Functions for Threaded GPIO Interrupts
	void registerProduct(product *pDispenser) { m_pDispenser = pDispenser; } 

	void startListener_flowsensor();
	// void startListener_buttons_powerAndMaintenance();
	// void startButtonListener();
	void listener_flowsensor();
	// void listener_buttons_powerAndMaintenance();
	void stopListener_flowsensor() { m_stop = true; }

protected:
	int m_nPin;
	bool m_stop;
	bool m_input;
	bool m_i2c;

	// Interrupt Function Definition
	product* m_pDispenser; //UNCOMMENT LODE
	virtual void monitorGPIO_Flowsensor(bool* abortLoop) = 0;
	// virtual void monitorGPIO_Buttons_powerAndMaintenance() = 0;
	std::thread *gpioThread;
};

#endif
