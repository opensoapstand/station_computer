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
	//m_nAddress = -1; //set negative one for illegal i2c address
	m_nPin = -1; //set negative one for illegal pin address
	m_stop = false;
	m_input = false;
	//m_i2c = false; //no i2c chip associate with it 
}

gpio::gpio(int pin)
{
	//m_nAddress = -1; //not i2c expanded gpio
	m_nPin = pin;
	m_stop = false;
	m_input = false;
	//m_i2c = false; //no i2c chip associate with it 
}

// gpio::gpio(int address, int pin)
// {
// 	m_nAddress = address; //i2c expanded gpio
// 	m_nPin = pin;
// 	m_stop = false;
// 	m_input = false;
// 	m_i2c = true; //there is i2c chip associated with it 
// }

DF_ERROR gpio::setPin(int pinNumber){

	DF_ERROR df_Ret = ERROR_BAD_PARAMS;

	if(pinNumber < 0 || pinNumber > 15)
		return df_Ret;
	else{
		m_nPin = pinNumber;
		df_Ret = OK;
	}

	return df_Ret;
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


