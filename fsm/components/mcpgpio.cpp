//***************************************
//
// mcpgpio.cpp
// GPIO Implementation for i2c gpio extender chip
//
// Chip model: MCP23017 - 16 PIN Addresses
//
// created: 15-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "mcpgpio.h"

// CTOR
mcpGPIO::mcpGPIO(){
    this->m_mcp = new MCP23017(2,34);
}

mcpGPIO::mcpGPIO(int i2caddress, int pin)
{
	debugOutput::sendMessage("------mcpGPIO------", INFO);
	m_nPin = pin;
	m_nAddress = convert_to_int(i2caddress);

	// debugOutput::sendMessage("i2c to int Address: ", INFO);
	// cout << m_nAddress << endl;;
	// debugOutput::sendMessage("Pin out: ", INFO);
	// cout << convert_to_int(m_nPin) << endl;

	// this->m_mcp = new MCP23017(DEFAULT_BUS, m_nAddress);

	// HACK: DEFAULT_BUS of 2 did not work.  BUS value of 1 works...
        this->m_mcp = new MCP23017(2, m_nAddress);

        openi2c();

}

// DTOR
mcpGPIO::~mcpGPIO()
{
	debugOutput::sendMessage("~mcpGPIO", INFO);

	this->m_mcp->closeI2C();
	delete (this->m_mcp);
}

// Setter for the pin direction for i2c Chip Address
DF_ERROR mcpGPIO::setDirection(bool input)
{
	debugOutput debugInfo;
	debugInfo.sendMessage("setDirection", INFO);

	DF_ERROR df_ret = OK;

	m_input = input;
	this->m_mcp->pinMode(m_nPin, OUTPUT);

        if (m_input) {
		this->m_mcp->pinMode(m_nPin, INPUT);
	}	

	return df_ret;
}

/* 
 * Read the pin for i2c Chip Address 
 * XXX: MAY be obolete due to XML parsing
 */
DF_ERROR mcpGPIO::readPin(bool * level)
{
        //debugOutput::sendMessage("readPin", INFO);
	
	DF_ERROR df_ret = ERROR_BAD_PARAMS;

	
        if (m_input && (nullptr != level)) {
                *level = this->m_mcp->digitalRead(m_nPin);
                df_ret = OK;
	}

        return df_ret;
}

/*
 * Write to i2c Chip Address pin on main board
 * Typically: Cassette controller base on high or low signal to acutate
 */
DF_ERROR mcpGPIO::writePin(bool level) 
{
	debugOutput::sendMessage("MCPGPIO writePin", INFO);

	DF_ERROR df_ret = ERROR_BAD_PARAMS;

	// 16 Pin Total for Chip Model
	// TODO: Can change magic number if model changes in future
	if(m_nPin < 0 && m_nPin > 15)
	{
		return df_ret; //pin number out of range
	}

	// cout << level << endl;
	// cout << HIGH << endl;

	if (HIGH == level) {
		cout << "Got HIGH signal" << endl;
		this->m_mcp->digitalWrite(m_nPin, HIGH);   //relies on bool to int
		df_ret = OK;
	}
	else
	{
		this->m_mcp->digitalWrite(m_nPin, LOW); 
		df_ret = OK;
	}

	return df_ret;
}

/*
 * Monitor i2c Chip Address pin on main board
 */
void mcpGPIO::monitorGPIO()
{
	// TODO: Implementation for Interrupt!
	//!!! look at oddyseyx86GPIO for example
        //cout << "hello! MCPGPIO" << endl;
    readPin(&level);
    if(level){
        if (!button_pressed){
            t1 = Clock::now();
            press_times++;
        }
        button_pressed = true;
    }
    else{
        if (button_pressed){
            button_pressed = false;
            t2 = Clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
            press_duration+= duration/1000.00;
        }
    }
}

/* ------ Getters, Setters and Utilities below ------ */

int mcpGPIO::convert_to_int(int addressNum)
{
	int hex_int = 0;
	
	hex_int = 16*(addressNum/10) + (addressNum%10); 	

	return hex_int;
}

int mcpGPIO::getMCPAddress()
{
	return m_nAddress;
}

int mcpGPIO::getMCPPin()
{
	return m_nPin;
}

bool mcpGPIO::openi2c()
{

    cout << "im here" << endl;
    if (this->m_mcp->openI2C()){
        return true;
    }else{
        return false;
    }

}
