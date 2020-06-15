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

#include "gpio.h"


gpio::gpio()
{
	m_nAddress = -1;  //illegal on prupose
	m_stop = false;
}

gpio::gpio(int address)
{
	m_nAddress = address;
	m_stop = false;
}


gpio::~gpio()
{
	//kill thread!
}

//call this with code that looks like
// std::thread tGPIOListener tgpio = <gpioinstance>->listener();
// tgpio.join();
std::thread gpio::listener()
{
	DF_ERROR df_ret = ERROR_BAD_PARAMS;
	m_stop = true;

	while (!m_stop) {
		//do stuff but this is a prototype so we skip
	}

	m_stop = true;  //reset
	return DF_ERROR();
}
