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

#define ACTIVATION_TIME 5
#define TEST_ACTIVATION_TIME 3
#define PRIME_PUMP_TIME 1
#define CLEAN_WATER_TIME 1
#define CLEAN_AIR_TIME 1

dispenser::dispenser(){
    //default constructor to set all pin to nullptr
    //debugOutput::sendMessage("dispenser", INFO);

    // TODO: Need to build Drink Object reference
    // m_pDrink = nullptr;

    for (int i = 0; i < NUM_SOLENOID; i++)
        m_pSolenoid[i] = nullptr; 
      
    m_pFlowsenor[NUM_FLOWSENSOR] = nullptr;
      
    for (int i = 0; i < NUM_PUMP; i++)
        m_pPump[i] = nullptr;
}

// CTOR
dispenser::dispenser(gpio* ButtonReference){
    //default constructor to set all pin to nullptr
    //debugOutput::sendMessage("dispenser", INFO);
    m_pButton[0] = ButtonReference;
    //m_pButtonPress[0] = ButtonReference
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

// TODO: Call this function on Dispense onEntry()
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
        m_pSolenoid[pos]->writePin(LOW);
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

// TODO: Call this function on Dispense onEntry()
DF_ERROR dispenser::setFlowsensor(int pin, int pos)
{
    DF_ERROR e_ret = ERROR_BAD_PARAMS;

    // *m_pIsDispensing = false;

    if(pos == 0)
    {
        // Instantiate, set input, spin up a flowsensor thread.
        m_pFlowsenor[pos] = new oddyseyx86GPIO(pin);
        m_pFlowsenor[pos]->setDirection(true);
        m_pFlowsenor[pos]->registerDrink(m_pDrink);
        m_pFlowsenor[pos]->startListener();
    }
    else
    {
        return e_ret = ERROR_MECH_FS_FAULT;
    }

    return e_ret;
}

// TODO: Call this function on Dispense onEntry()
DF_ERROR dispenser::setPump(int mcpAddress, int pin, int direction)
{
    DF_ERROR e_ret = ERROR_BAD_PARAMS; //reset variable    
    
	if((X20 <= mcpAddress &&  X22 >=mcpAddress) && (MCP_PIN_START <= pin && MPC_PIN_END >= pin))
	{
        m_pPump[direction] = new mcpGPIO(mcpAddress, pin);
		e_ret = OK;

        // FIXME: Refactor..Message mediator should set this as a m_pdrink Object
        m_isStill = true;
	}
	else if(X20 > mcpAddress || X22 < mcpAddress)
        {
		return e_ret = ERROR_ELEC_WRONG_I2C_ADDRESS;
        // FIXME: Refactor..Message mediator should set this as a m_pdrink Object
        m_isStill = false;
	}
	else if(MCP_PIN_START > pin || MPC_PIN_END < pin)
	{
        // FIXME: Refactor..Message mediator should set this as a m_pdrink Object
        m_isStill = false;
		return e_ret = ERROR_BAD_PARAMS;
	}
    return e_ret;
}

// TODO: Refactor Pumping with switch and ternary in future...keep seperate for ease of testing.
// Reverse pump: Turn forward pin HIGH - Reverse pin LOW
DF_ERROR dispenser::forwardPump()
{
    debugOutput::sendMessage("-----FORWARD Pump-----", INFO);   
    m_pPump[FORWARD]->writePin(HIGH);
    m_pPump[REVERSE]->writePin(LOW);
}

// Reverse pump: Turn forward pin LOW - Reverse pin HIGH
DF_ERROR dispenser::reversePump()
{
    debugOutput::sendMessage("-----REVERSE Pump-----", INFO);   
    m_pPump[FORWARD]->writePin(LOW);
    m_pPump[REVERSE]->writePin(HIGH);
}

// Stops pumping: Turn forward pin LOW - Reverse pin LOW
DF_ERROR dispenser::stopPump()
{
    debugOutput::sendMessage("-----Stop Pump-----", INFO);   
    m_pPump[FORWARD]->writePin(LOW);
    // m_pPump[REVERSE]->writePin(LOW);
}

// Disenses drinks by turning Solenoid Signal to HIGH then to LOW
DF_ERROR dispenser::startDispense(int pos){
    DF_ERROR e_ret = ERROR_MECH_DRINK_FAULT;
    debugOutput::sendMessage("-----Start Dispense-----", INFO);
    // XXX: Prepare Button - Linked thru State Virtual
    // e_ret = connectButton();

    // Solenoid Position Check
    if(pos != DRINK) {
        e_ret = ERROR_ELEC_PIN_DISPENSE;
        return e_ret;
    }

    // Open Drink Solenoid
    // debugOutput::sendMessage("Trigger solenoid:", INFO);
    m_pSolenoid[pos]->writePin(HIGH);
    debugOutput::sendMessage("Triggered pump:", INFO);
 
    // If Still start pump!
    if(m_isStill && (m_pPump != nullptr) ) {
        // m_pPump[pos]->writePin(HIGH);
        sleep(PRIME_PUMP_TIME);
        forwardPump();
        // reversePump();
    }
    return e_ret = OK;
}

// 
DF_ERROR dispenser::stopDispense(int pos){
    DF_ERROR e_ret = ERROR_BAD_PARAMS;
    // Stop Pump
    if(m_isStill && m_pPump != nullptr ) {
        // m_pPump[pos]->writePin(LOW);
        stopPump();
        sleep(PRIME_PUMP_TIME);
    }

    // Shut Solenoid
    m_pSolenoid[pos]->writePin(LOW);
    debugOutput::sendMessage("UnTriggered pump:", INFO);

    // XXX: Disable Button - Linked thru State Virtual
    // e_ret = disconnectButton();
    m_isDispenseDone = true;


    return e_ret = OK;
}

DF_ERROR dispenser::connectButton(){
    return m_pButton[0]->writePin(HIGH);    
}

DF_ERROR dispenser::disconnectButton(){
    return m_pButton[0]->writePin(LOW);
}

//DF_ERROR dispenser::setButtonPress(int address_num, int pin_num){
//    m_pButtonPress = new mcpGPIO(address_num, pin_num);
//    m_pButtonPress->setDirection(true);
//   // m_pButtonPress->startButtonListener();
//}

//double dispenser::getButtonPressDuration(){
//    return m_pButtonPress->getPressDuration();
//}

//int dispenser::getButtonPressTimes(){
//    return m_pButtonPress->getPressAmount();
//}

//void dispenser::resetButtonPressTimes(){
//    m_pButtonPress->resetPressAmount();
//}

//void dispenser::resetButtonPressDuration(){
//    m_pButtonPress->resetPressDuration();
//}

// Cleans the nozzle by dispensing Water followed by Air
DF_ERROR dispenser::cleanNozzle(int posW, int posA){
    DF_ERROR e_ret = ERROR_BAD_PARAMS;

    if(posW != WATER && posA != AIR) {
        e_ret = ERROR_ELEC_PIN_CLEAN;
        return e_ret;
    }

    // Flush the lines with water
    debugOutput::sendMessage("Water Cleanse", INFO);
    m_pSolenoid[posW]->writePin(HIGH);
    sleep(CLEAN_WATER_TIME);
    m_pSolenoid[posW]->writePin(LOW);

    // Flush the lines with Air
    debugOutput::sendMessage("Air Release", INFO);
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
    sleep(TEST_ACTIVATION_TIME);
    m_pSolenoid[pos]->writePin(LOW);

    return e_ret = OK;
}

/* ------Getters, Setters and Utilities------ */
int dispenser::getI2CAddress(int pos){
    return m_pSolenoid[pos]->getMCPAddress();
}

int dispenser::getI2CPin(int pos){
    debugOutput::sendMessage("getI2C Error!", ERROR);
    return m_pSolenoid[pos]->getMCPPin();
}

DF_ERROR dispenser::setDrink(drink* drink){
    if(drink != nullptr) {
        m_pDrink = drink;
    } else {
        debugOutput::sendMessage("Set Drink Error!", ERROR);
    }
}
