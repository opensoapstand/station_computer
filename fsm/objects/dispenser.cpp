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

    debugOutput debugInfo;
    debugInfo.sendMessage("Object has been created", INFO);
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
DF_ERROR dispenser::setSolenoid(int drinkPin, int waterPin, int airPin)
{
    DF_ERROR df_ret = ERROR_BAD_PARAMS;

    //df_ret = m_pSolenoid[DRINK]->setMCPPin(drinkPin);
    if(OK != df_ret)
    {
        return df_ret = ERROR_DRINK_FAULT;
    }
    
    df_ret = ERROR_BAD_PARAMS; //reset variable
    //df_ret = m_pSolenoid[WATER]->setMCPPin(waterPin);
    if(OK != df_ret)
    {
        return df_ret = ERROR_WATER_FAULT;
    }
    
    df_ret = ERROR_BAD_PARAMS; //reset variable
    //df_ret = m_pSolenoid[AIR]->setMCPPin(airPin);
    if(OK != df_ret)
    {
        return df_ret = ERROR_AIR_FAULT;
    }

    return df_ret;
}

DF_ERROR dispenser::setFlowsensor(int pin)
{
    DF_ERROR df_ret = ERROR_BAD_PARAMS;

    //df_ret = m_pFlowsenor[FLOW]->setFlowPin(pin);
    if(OK != df_ret)
    {
        return df_ret = ERROR_FS_FAULT;
    };

    return df_ret;
}

DF_ERROR dispenser::setPump(int forwardPin, int reversePin)
{

    DF_ERROR df_ret = ERROR_BAD_PARAMS; //reset variable
    //df_ret = m_pPump[FORWARD]->setMCPPin(forwardPin);
    if(OK != df_ret)
    {
        return df_ret = ERROR_FPUMP_FAULT;
    }
    
    df_ret = ERROR_BAD_PARAMS; //reset variable
    //df_ret = m_pPump[REVERSE]->setMCPPin(reversePin);
    if(OK != df_ret)
    {
        return df_ret = ERROR_RPUMP_FAULT;
    }

    return df_ret;
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