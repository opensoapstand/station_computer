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
	m_nAddress = 0;
	m_stop = false;
	m_input = false;	
}

gpio::gpio(int address)
{
	m_nAddress = address;
	m_stop = false;
	m_input = false;
}


gpio::~gpio()
{
	//kill thread!
}

DF_ERROR gpio::setInterrupt(DF_ERROR(*pf)())
{
	DF_ERROR df_ret = ERROR_BAD_PARAMS;

	if (nullptr != pf) {
		m_pf = pf;
		df_ret = OK;
	}
	
	return df_ret;;
}

//call this with code that looks like
// std::thread tGPIOListener tgpio = <gpioinstance>->listener();
// tgpio.join();
std::thread gpio::listener()
{
	DF_ERROR df_ret = ERROR_BAD_PARAMS;
	m_stop = true;

	while (!m_stop) {
		monitorGPIO();
	}

	m_stop = true;  //reset
	return;
}
