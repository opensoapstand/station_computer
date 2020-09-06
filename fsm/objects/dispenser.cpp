//***************************************
//
// dispenser.cpp
// Dispenser (Model) class Implementation: 
//
// Manages and owns the package for a dispense.
// Links to circuit board addresses and
// holds instructions for dispensing.
//
// created: 25-06-2020
// by: Jason Wang & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************
#include "dispenser.h"

#define ACTIVATION_TIME 10
#define CLEAN_WATER_TIME 5
#define CLEAN_AIR_TIME 5

// CTOR
dispenser::dispenser(){
    //default constructor to set all pin to nullptr
    //debugOutput::sendMessage("dispenser", INFO);
    m_pDrink = nullptr;

    for (int i = 0; i < NUM_SOLENOID; i++)
        m_pSolenoid[i] = nullptr; 
      
    m_pFlowsenor[NUM_FLOWSENSOR] = nullptr;
      
    for (int i = 0; i < NUM_PUMP; i++)
        m_pPump[i] = nullptr;
}

// DTOR
dispenser::~dispenser(){
    debugOutput::sendMessage("~dispenser", INFO);
    // delete [] m_pDrink;

    // delete [] m_pSolenoid;
    // delete [] m_pFlowsenor;
    // delete [] m_pPump;
}

// TODO: Check and remove; stateinit should handle cassettes
void dispenser::initDispenser(int slot){

}


DF_ERROR dispenser::setSolenoid(int mcpAddress, int pin, int pos)
{
    debugOutput::sendMessage("-----dispenser::setSolenoid-----", INFO);
    DF_ERROR e_ret = ERROR_BAD_PARAMS; 
   
	if((X20 <= mcpAddress &&  X22 >= mcpAddress) && (MCP_PIN_START <= pin && MPC_PIN_END >= pin))
	{
        debugOutput::sendMessage("i2c Chip Prefix Address-" + to_string(mcpAddress) + ", Pin-" + to_string(pin) + " => solenoid set", INFO);

        size_t solenoids =  sizeof(m_pSolenoid) / sizeof(m_pSolenoid[0]);

        debugOutput::sendMessage("mcpAddress raw value: ", INFO);
        cout << mcpAddress << endl;
        debugOutput::sendMessage("pin raw value: ", INFO);
        cout << pin << endl;

        // Old Implementation
        // for(int i = 0; i < solenoids; i++) {
        //     std::cout << m_pSolenoid[i]->getMCPAddress() << std::endl;
        //     std::cout << m_pSolenoid[i]->getMCPPin() << std::endl;
        // }

        m_pSolenoid[pos] = new mcpGPIO(mcpAddress, pin);
		e_ret = OK;
	}
	else if(X20 > mcpAddress || X22 < mcpAddress)
	{
		return e_ret = ERROR_ELEC_WRONG_I2C_ADDRESS;
	}
	else if(MCP_PIN_START > pin || MPC_PIN_END < pin)
	{
		return e_ret = ERROR_BAD_PARAMS;
    }

    return e_ret;
}

DF_ERROR dispenser::setFlowsensor(int pin, int pos)
{
    DF_ERROR e_ret = ERROR_BAD_PARAMS;

    if(pos == 0)
    {
        m_pFlowsenor[pos] = new oddyseyx86GPIO(pin);
    }
    else
    {
        return e_ret = ERROR_MECH_FS_FAULT;
    }

    return e_ret;
}

DF_ERROR dispenser::setPump(int mcpAddress, int pin, int direction)
{
    DF_ERROR e_ret = ERROR_BAD_PARAMS; //reset variable    
    
	if((X20 <= mcpAddress &&  X22 >=mcpAddress) && (MCP_PIN_START <= pin && MPC_PIN_END >= pin))
	{
        m_pPump[direction] = new mcpGPIO(mcpAddress, pin);
		e_ret = OK;
	}
	else if(X20 > mcpAddress || X22 < mcpAddress)
	{
        //debugOutput::sendMessage("got here", INFO);
		return e_ret = ERROR_ELEC_WRONG_I2C_ADDRESS;
	}
	else if(MCP_PIN_START > pin || MPC_PIN_END < pin)
	{
		return e_ret = ERROR_BAD_PARAMS;
	}

    return e_ret;
}

// Disenses drinks by turning Solenoid Signal to HIGH then to LOW
DF_ERROR dispenser::startDispense(int pos){
    DF_ERROR e_ret = ERROR_MECH_DRINK_FAULT;

    // Solenoid Position Check
    if(pos != DRINK) {
        e_ret = ERROR_ELEC_PIN_DISPENSE;
        return e_ret;
    }

    // Dispense the Drink
    m_pSolenoid[pos]->writePin(HIGH);
    sleep(ACTIVATION_TIME);
    m_pSolenoid[pos]->writePin(LOW);

    return e_ret = OK;
}

// TODO: May not be nessecary...turn into an interrupt instead.
DF_ERROR dispenser::stopDispense(int pos){
    DF_ERROR e_ret = ERROR_BAD_PARAMS;

    return e_ret = OK;
}

// Cleans the nozzle by dispensing Water followed by Air
DF_ERROR dispenser::cleanNozzle(int posW, int posA){
    DF_ERROR e_ret = ERROR_BAD_PARAMS;

    if(posW != WATER && posA != AIR) {
        e_ret = ERROR_ELEC_PIN_CLEAN;
        return e_ret;
    }

    // Flush the lines with water
    m_pSolenoid[posW]->writePin(HIGH);
    sleep(CLEAN_WATER_TIME);
    m_pSolenoid[posW]->writePin(LOW);

    // Flush the lines with Air
    m_pSolenoid[posA]->writePin(HIGH);
    sleep(CLEAN_AIR_TIME);
    m_pSolenoid[posA]->writePin(LOW);

    return e_ret = OK;
}

drink* dispenser::getDrink(){
    return m_pDrink;
}

// Timer based
DF_ERROR dispenser::testSolenoidDispense(int pos){
    DF_ERROR e_ret = ERROR_BAD_PARAMS;

    m_pSolenoid[pos]->writePin(HIGH);
    sleep(ACTIVATION_TIME);
    m_pSolenoid[pos]->writePin(LOW);

    return e_ret = OK;
}

/* ------Getters, Setters and Utilities------ */

int dispenser::getI2CAddress(int pos){
    return m_pSolenoid[pos]->getMCPAddress();
}

int dispenser::getI2CPin(int pos){
    return m_pSolenoid[pos]->getMCPPin();
}