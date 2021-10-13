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


#define DEFAULT_PUMP_PWM  0x80


#define ACTIVATION_TIME 5
#define TEST_ACTIVATION_TIME 3
#define PRIME_PUMP_TIME 1
#define CLEAN_WATER_TIME 1
#define CLEAN_AIR_TIME 1


dsed8344 *dispenser::the_8344 = nullptr;


dispenser::dispenser(){
    //default constructor to set all pin to nullptr
    //debugOutput::sendMessage("dispenser", INFO);

    // TODO: Need to build Drink Object reference
    // m_pDrink = nullptr;

    // If we haven't instantiated and initialized the hardware yet we
    // do it here.  Note that the pointer is declared as static so we
    // will only ever create one dsed8344() class no matter how many
    // times we create a dispenser() class.
    if (the_8344 == nullptr)
    {
	the_8344 = new dsed8344 ();
    }

    // Set the pump PWM value to a nominal value
    the_8344->setPumpPWM (DEFAULT_PUMP_PWM);
    the_8344->setButtonPower(false);
    
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

    delete the_8344;
    the_8344 = nullptr;

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
   
    return OK;
}

// TODO: Call this function on Dispense onEntry()
DF_ERROR dispenser::setFlowsensor(int pin, int pos)
{
    DF_ERROR e_ret = ERROR_BAD_PARAMS;

    // *m_pIsDispensing = false;

    debugOutput::sendMessage("-----dispenser::setFlowsensor-----", INFO);

//    if(pos == 0)
//    {
        // Instantiate, set input, spin up a flowsensor thread.
        m_pFlowsenor[pos] = new oddyseyx86GPIO(pin);
        m_pFlowsenor[pos]->setDirection(true);
        m_pFlowsenor[pos]->registerDrink(m_pDrink);
        m_pFlowsenor[pos]->startListener();
	e_ret = OK;
//    }
//    else
//    {
//        return e_ret = ERROR_MECH_FS_FAULT;
//    }

    return e_ret;
}

// TODO: Call this function on Dispense onEntry()
DF_ERROR dispenser::setPump(int mcpAddress, int pin, int position)
{
    DF_ERROR e_ret = ERROR_BAD_PARAMS; //reset variable    

    // Save the pump number of this instance
    pump_position = (unsigned char)(position + 1);
    
    return e_ret = OK;
}

// TODO: Refactor Pumping with switch and ternary in future...keep seperate for ease of testing.
// Reverse pump: Turn forward pin HIGH - Reverse pin LOW
DF_ERROR dispenser::forwardPump()
{
    debugOutput::sendMessage("-----FORWARD Pump-----", INFO);   
    the_8344->setPumpDirection (true);
}

// Reverse pump: Turn forward pin LOW - Reverse pin HIGH
DF_ERROR dispenser::reversePump()
{
    debugOutput::sendMessage("-----REVERSE Pump-----", INFO);   
    the_8344->setPumpDirection (false);
}

// Stops pumping: Turn forward pin LOW - Reverse pin LOW
DF_ERROR dispenser::stopPump()
{
    debugOutput::sendMessage("-----Stop Pump-----", INFO);   
    the_8344->stopPump ();
}

// Disenses drinks by turning Solenoid Signal to HIGH then to LOW
DF_ERROR dispenser::startDispense (int pos){
    DF_ERROR e_ret = ERROR_MECH_DRINK_FAULT;
    debugOutput::sendMessage("-----Start Dispense-----", INFO);
    // XXX: Prepare Button - Linked thru State Virtual
    // e_ret = connectButton();

    // Solenoid Position Check
//    if(pos != DRINK) {
//        e_ret = ERROR_ELEC_PIN_DISPENSE;
//        return e_ret;
//    }

    // Open Drink Solenoid
    // debugOutput::sendMessage("Trigger solenoid:", INFO);
    // m_pSolenoid[pos]->writePin(HIGH);
    debugOutput::sendMessage("Triggered pump:"+to_string(pos), INFO);
 
    // If Still start pump!
   // if(m_isStill && (m_pPump != nullptr) ) {
        //sleep(PRIME_PUMP_TIME);
        forwardPump();
        the_8344->setPumpPWM((unsigned char)(m_pDrink->getPWM()));
        debugOutput::sendMessage("PWM SET!", INFO);
        cout << the_8344->getPumpPWM();
        the_8344->startPump(pos);
    //}
    return e_ret = OK;
}

// 
DF_ERROR dispenser::stopDispense(int pos){
    DF_ERROR e_ret = ERROR_BAD_PARAMS;
    // Stop Pump
//    if(m_isStill && m_pPump != nullptr ) {
        the_8344->stopPump();
//        sleep(PRIME_PUMP_TIME);
//    }

    // Shut Solenoid
    // m_pSolenoid[pos]->writePin(LOW);
    debugOutput::sendMessage("UnTriggered pump:", INFO);

    // XXX: Disable Button - Linked thru State Virtual
    // e_ret = disconnectButton();
    m_isDispenseDone = true;


    return e_ret = OK;
}

DF_ERROR dispenser::connectButton(){
    return OK;
    // return m_pButton[0]->writePin(HIGH);    
}

DF_ERROR dispenser::disconnectButton(){
    return OK;
    // return m_pButton[0]->writePin(LOW);
}

// Cleans the nozzle by dispensing Water followed by Air
DF_ERROR dispenser::cleanNozzle(int posW, int posA){
    DF_ERROR e_ret = ERROR_BAD_PARAMS;

    if(posW != WATER && posA != AIR) {
        e_ret = ERROR_ELEC_PIN_CLEAN;
        return e_ret;
    }

    // Flush the lines with water
    debugOutput::sendMessage("Water Cleanse", INFO);
    //m_pSolenoid[posW]->writePin(HIGH);
    sleep(CLEAN_WATER_TIME);
    //m_pSolenoid[posW]->writePin(LOW);

    // Flush the lines with Air
    debugOutput::sendMessage("Air Release", INFO);
    //m_pSolenoid[posA]->writePin(HIGH);
    sleep(CLEAN_AIR_TIME);
    //m_pSolenoid[posA]->writePin(LOW);

    return e_ret = OK;
}

drink* dispenser::getDrink(){
    return m_pDrink;
}

// Timer based
DF_ERROR dispenser::testSolenoidDispense(int pos){
    DF_ERROR e_ret = ERROR_BAD_PARAMS;

    //m_pSolenoid[pos]->writePin(HIGH);
    sleep(TEST_ACTIVATION_TIME);
    //m_pSolenoid[pos]->writePin(LOW);

    return e_ret = OK;
}

/* ------Getters, Setters and Utilities------ */
int dispenser::getI2CAddress(int pos){
    //return m_pSolenoid[pos]->getMCPAddress();
}

int dispenser::getI2CPin(int pos){
    debugOutput::sendMessage("getI2C Error!", ERROR);
    //return m_pSolenoid[pos]->getMCPPin();
}

DF_ERROR dispenser::setDrink(drink* drink){
    if(drink != nullptr) {
        m_pDrink = drink;
    } else {
        debugOutput::sendMessage("Set Drink Error!", ERROR);
    }
}
