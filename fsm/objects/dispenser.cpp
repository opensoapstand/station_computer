//***************************************
//
// dispenser.cpp
// Dispenser (Model) class Implementation:
//
// Manages and owns the package for one product to dispense.
// Links to circuit board addresses and
// holds instructions for dispensing.
//
// created: 25-06-2020
// by:Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************
#include "dispenser.h"

#define DEFAULT_PUMP_PWM 0x80

#define ACTIVATION_TIME 5
#define TEST_ACTIVATION_TIME 3
#define PRIME_PUMP_TIME 1
#define CLEAN_WATER_TIME 1
#define CLEAN_AIR_TIME 1

dsed8344 *dispenser::the_8344 = nullptr;

dispenser::dispenser()
{
    //default constructor to set all pin to nullptr
    //debugOutput::sendMessage("dispenser", MSG_INFO);

    // TODO: Need to build Product Object reference
    // m_pSelectedProduct = nullptr;

    // If we haven't instantiated and initialized the hardware yet we
    // do it here.  Note that the pointer is declared as static so we
    // will only ever create one dsed8344() class no matter how many
    // times we create a dispenser() class.
    if (the_8344 == nullptr)
    {
        the_8344 = new dsed8344();
    }

    // Set the pump PWM value to a nominal value
    the_8344->setPumpPWM(DEFAULT_PUMP_PWM);
    the_8344->setButtonPower(false);

    for (int i = 0; i < NUM_SOLENOID; i++)
        m_pSolenoid[i] = nullptr;

    m_pFlowsenor[NUM_FLOWSENSOR] = nullptr;

    for (int i = 0; i < NUM_PUMP; i++)
        m_pPump[i] = nullptr;
}

// CTOR
dispenser::dispenser(gpio *ButtonReference)
{
    //default constructor to set all pin to nullptr
    //debugOutput::sendMessage("dispenser", MSG_INFO);
    m_pButton[0] = ButtonReference;
    //m_pButtonPress[0] = ButtonReference
    m_pSelectedProduct = nullptr;

    for (int i = 0; i < NUM_SOLENOID; i++)
        m_pSolenoid[i] = nullptr;

    m_pFlowsenor[NUM_FLOWSENSOR] = nullptr;

    for (int i = 0; i < NUM_PUMP; i++)
        m_pPump[i] = nullptr;
}

// DTOR
dispenser::~dispenser()
{
    debugOutput::sendMessage("~dispenser", MSG_INFO);

    delete the_8344;
    the_8344 = nullptr;

    // delete [] m_pSelectedProduct;

    // delete [] m_pSolenoid;
    // delete [] m_pFlowsenor;
    // delete [] m_pPump;
}

// TODO: Check and remove; stateinit should handle productDispensers
void dispenser::initDispenser(int slot)
{
}

// TODO: Call this function on Dispense onEntry()
DF_ERROR dispenser::setSolenoid(int mcpAddress, int pin, int pos)
{
    debugOutput::sendMessage("-----dispenser::setSolenoid-----", MSG_INFO);

    return OK;
}

// TODO: Call this function on Dispense onEntry()
DF_ERROR dispenser::setFlowsensor(int pin, int pos)
{
    DF_ERROR e_ret = ERROR_BAD_PARAMS;

    // *m_pIsDispensing = false;
    std::string msg = "-----dispenser::setFlowsensor-----Position: " + std::to_string(pos) + " (pin: " + std::to_string(pin) + ")";
    // debugOutput::sendMessage("-----dispenser::setFlowsensor-----", MSG_INFO);
    debugOutput::sendMessage(msg, MSG_INFO);

    if ((pos >= 0) && (pos < 4))
    {
        // Instantiate, set input, spin up a flowsensor thread.
        m_pFlowsenor[pos] = new oddyseyx86GPIO(pin);
        m_pFlowsenor[pos]->setDirection(true);
        m_pFlowsenor[pos]->registerProduct(m_pSelectedProduct);
        m_pFlowsenor[pos]->startListener();
        e_ret = OK;
    }
    else
    {
        return e_ret = ERROR_MECH_FS_FAULT;
    }

    return e_ret;
}

DF_ERROR dispenser::setPowerOffListener()
{

    m_pPWRorMM[0] = new oddyseyx86GPIO(391);
    m_pPowerOff[0] = new oddyseyx86GPIO(340);
    m_pMM[0] = new oddyseyx86GPIO(341);
    m_pPWRorMM[0]->setDirection(true);
    m_pPowerOff[0]->setDirection(true);
    m_pMM[0]->setDirection(true);
    m_pPWRorMM[0]->startListenerPWR();
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
    debugOutput::sendMessage("-----FORWARD Pump-----", MSG_INFO);
    the_8344->setPumpDirection(true);
}

// Reverse pump: Turn forward pin LOW - Reverse pin HIGH
DF_ERROR dispenser::reversePump()
{
    debugOutput::sendMessage("-----REVERSE Pump-----", MSG_INFO);
    the_8344->setPumpDirection(false);
}

// Stops pumping: Turn forward pin LOW - Reverse pin LOW
DF_ERROR dispenser::stopPump()
{
    debugOutput::sendMessage("-----Stop Pump-----", MSG_INFO);
    the_8344->stopPump();
}

// 
DF_ERROR dispenser::startPump(int pos)
{
    debugOutput::sendMessage("-----Start Pump-----", MSG_INFO);
    the_8344->startPump(pos);
}

DF_ERROR dispenser::setPumpPWM(int8_t value)
{
    debugOutput::sendMessage("-----Set PWM-----", MSG_INFO);
    the_8344->setPumpPWM((unsigned char)value);
    
}

// Disenses products by turning Solenoid Signal to HIGH then to LOW
DF_ERROR dispenser::startDispense(int pos)
{
    DF_ERROR e_ret = ERROR_MECH_PRODUCT_FAULT;
    debugOutput::sendMessage("-----Start Dispense-----", MSG_INFO);
    // XXX: Prepare Button - Linked thru State Virtual
    // e_ret = connectButton();

    // Solenoid Position Check
    //    if(pos != PRODUCT) {
    //        e_ret = ERROR_ELEC_PIN_DISPENSE;
    //        return e_ret;
    //    }

    // Open Product Solenoid
    // debugOutput::sendMessage("Trigger solenoid:", MSG_INFO);
    // m_pSolenoid[pos]->writePin(HIGH);
    debugOutput::sendMessage("Triggered pump:" + to_string(pos), MSG_INFO);

    // If Still start pump!
    // if(m_isStill && (m_pPump != nullptr) ) {
    //sleep(PRIME_PUMP_TIME);
    forwardPump();
    // the_8344->setPumpPWM((unsigned char)(m_pSelectedProduct->getPWM()));
    setPumpPWM((unsigned char)(m_pSelectedProduct->getPWM()));


    debugOutput::sendMessage("PWM SET!", MSG_INFO);
    //cout << the_8344->getPumpPWM();
    
    
    //the_8344->startPump(pos);
    startPump(pos);


    //}
    return e_ret = OK;
}

//
DF_ERROR dispenser::stopDispense(int pos)
{
    DF_ERROR e_ret = ERROR_BAD_PARAMS;
    // Stop Pump
    //    if(m_isStill && m_pPump != nullptr ) {
    the_8344->stopPump();
    //        sleep(PRIME_PUMP_TIME);
    //    }

    // Shut Solenoid
    // m_pSolenoid[pos]->writePin(LOW);
    debugOutput::sendMessage("Pump disabled", MSG_INFO);

    // XXX: Disable Button - Linked thru State Virtual
    // e_ret = disconnectButton();
    m_isDispenseDone = true;

    return e_ret = OK;
}

DF_ERROR dispenser::connectButton()
{
    return OK;
    // return m_pButton[0]->writePin(HIGH);
}

DF_ERROR dispenser::disconnectButton()
{
    return OK;
    // return m_pButton[0]->writePin(LOW);
}

// // Cleans the nozzle by dispensing Water followed by Air
// DF_ERROR dispenser::cleanNozzle(int posW, int posA){
//     DF_ERROR e_ret = ERROR_BAD_PARAMS;

//     if(posW != WATER && posA != AIR) {
//         e_ret = ERROR_ELEC_PIN_CLEAN;
//         return e_ret;
//     }

//     // Flush the lines with water
//     debugOutput::sendMessage("Water Cleanse", MSG_INFO);
//     //m_pSolenoid[posW]->writePin(HIGH);
//     sleep(CLEAN_WATER_TIME);
//     //m_pSolenoid[posW]->writePin(LOW);

//     // Flush the lines with Air
//     debugOutput::sendMessage("Air Release", MSG_INFO);
//     //m_pSolenoid[posA]->writePin(HIGH);
//     sleep(CLEAN_AIR_TIME);
//     //m_pSolenoid[posA]->writePin(LOW);

//     return e_ret = OK;
// }

product *dispenser::getProduct()
{
    return m_pSelectedProduct;
}

// Timer based
DF_ERROR dispenser::testSolenoidDispense(int pos)
{
    DF_ERROR e_ret = ERROR_BAD_PARAMS;

    //m_pSolenoid[pos]->writePin(HIGH);
    sleep(TEST_ACTIVATION_TIME);
    //m_pSolenoid[pos]->writePin(LOW);

    return e_ret = OK;
}

/* ------Getters, Setters and Utilities------ */
int dispenser::getI2CAddress(int pos)
{
    //return m_pSolenoid[pos]->getMCPAddress();
}

int dispenser::getI2CPin(int pos)
{
    debugOutput::sendMessage("getI2C Error!", MSG_ERROR);
    //return m_pSolenoid[pos]->getMCPPin();
}

DF_ERROR dispenser::setProduct(product *product)
{
    if (product != nullptr)
    {
        m_pSelectedProduct = product;
    }
    else
    {
        debugOutput::sendMessage("Set Product Error!", MSG_ERROR);
    }
}
