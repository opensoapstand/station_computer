//***************************************
//
// dispenser.cpp
// dispenser class owns the package for a dispense
//
// created: 25-06-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************
#include "dispenser.h"

dispenser::dispenser(){
    //default constructor to set all pin to nullptr
    m_pDrink = nullptr;

    for (int i = 0; i < NUM_SOLENOID; i++)
        m_pSolenoid[i] = nullptr; 
      
    m_pFlowsenor[NUM_FLOWSENSOR] = nullptr;
      
    for (int i = 0; i < NUM_PUMP; i++)
        m_pPump[i] = nullptr;

    debugOutput::sendMessage("Object has been created", INFO);
}

dispenser::~dispenser(){
    // delete [] m_pDrink;

    // delete [] m_pSolenoid;
    // delete [] m_pFlowsenor;
    // delete [] m_pPump;
}

void dispenser::initDispenser(int slot){

}

//Setters
DF_ERROR dispenser::setSolenoid(int mcpAddress, int pin, int pos)
{
    DF_ERROR e_ret = ERROR_BAD_PARAMS; 
    
	if((X20 <= mcpAddress &&  X22 >=mcpAddress) && (MCP_PIN_START <= pin && MPC_PIN_END >= pin))
	{
        //m_pSolenoid[pos] = new mcpGPIO(mcpAddress, pin);
		e_ret = OK;
	}
	else if(X20 >= mcpAddress || X22 <= mcpAddress)
	{
		e_ret = ERROR_WRONG_I2C_ADDRESS;
	}
	else if(MCP_PIN_START >= pin || MPC_PIN_END <= pin)
	{
		e_ret = ERROR_BAD_PARAMS;
    }

    return e_ret;
}

DF_ERROR dispenser::setFlowsensor(int pin)
{
    DF_ERROR df_ret = ERROR_BAD_PARAMS;

    if(OK != df_ret)
    {
        return df_ret = ERROR_FS_FAULT;
    };

    return df_ret;
}

DF_ERROR dispenser::setPump(int mcpAddress, int forwardPin, int direction)
{
    DF_ERROR e_ret = ERROR_BAD_PARAMS; //reset variable    
    
	if((X20 <= mcpAddress &&  X22 >=mcpAddress) && (MCP_PIN_START <= forwardPin && MPC_PIN_END >= forwardPin))
	{
        //m_pPump[direction] = new mcpGPIO(mcpAddress, forwardPin);
		e_ret = OK;
	}
	else if(X20 >= mcpAddress || X22 <= mcpAddress)
	{
		e_ret = ERROR_WRONG_I2C_ADDRESS;
	}
	else if(MCP_PIN_START >= forwardPin || MPC_PIN_END <= forwardPin)
	{
		e_ret = ERROR_BAD_PARAMS;
	}

    return e_ret;
}


//
DF_ERROR dispenser::startDispense(){

}
DF_ERROR dispenser::stopDispense(){

}
DF_ERROR dispenser::cleanNozzle(){

}

drink dispenser::getDrink(){

}