//***************************************
//
// gpio.h
// GPIO Abstract Class Implementation
//
// created: 15-06-2020
// by: Denis Londry & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "gpio.h"

// CTOR
gpio::gpio()
{
	//m_nAddress = -1; //set negative one for illegal i2c address
	m_nPin = -1; //set negative one for illegal pin address
	m_stop = false;
	m_input = false;
	m_i2c = false; //no i2c chip associate with it 
	gpioThread = nullptr;
	m_pDrink = nullptr;
}

// DTOR
gpio::~gpio()
{
	//kill thread!
	this->stopListener();
}

// kick off a listener
void gpio::startListener()
{
	debugOutput::sendMessage("-----startListener-----", INFO);
	DF_ERROR df_ret = ERROR_BAD_PARAMS;

	if ((nullptr ==  gpioThread) && (nullptr != m_pDrink)){
		gpioThread = new std::thread(&gpio::listener, this);
		df_ret = OK;
	} else {
		debugOutput::sendMessage("Did not pass null check", INFO);
	}
}

// TODO: Threaded GPIO interrupt...
// call this with code that looks like
// std::thread tGPIOListener tgpio = <gpioinstance>->listener();
// std::thread gpio::listener()
void gpio::listener()
{
	cout << "Spin up GPIO Thread" << endl;
	DF_ERROR df_ret = ERROR_BAD_PARAMS;
	m_stop = true;

	while (!m_stop) {
		monitorGPIO();
	}

	m_stop = true;  //reset
	// return;
}
