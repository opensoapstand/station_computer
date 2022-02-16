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
#include <chrono>

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
    // m_pDispensedProduct = nullptr;

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
    the_8344->setDispenseButtonLight(false);

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
    m_pDispensedProduct = nullptr;

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

    // delete [] m_pDispensedProduct;

    // delete [] m_pSolenoid;
    // delete [] m_pFlowsenor;
    // delete [] m_pPump;
}

// TODO: Check and remove; stateinit should handle productDispensers
void dispenser::initDispenser(int slot)
{
}

// TODO: Call this function on Dispense onEntry()
// DF_ERROR dispenser::setSolenoid(int mcpAddress, int pin, int pos)
// {
//     debugOutput::sendMessage("-----dispenser::setSolenoid-----", MSG_INFO);

//     return OK;
// }
DF_ERROR dispenser::setSlot(int slot){
    this->slot = slot;
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
        m_pFlowsenor[pos]->setPinAsInputElseOutput(true);
        m_pFlowsenor[pos]->registerProduct(m_pDispensedProduct);
        m_pFlowsenor[pos]->startListener_flowsensor();
        e_ret = OK;
    }
    else
    {
        return e_ret = ERROR_MECH_FS_FAULT;
    }

    return e_ret;
}

DF_ERROR dispenser::setButtonsShutdownAndMaintenance()
{

    m_pPWRorMM[0] = new oddyseyx86GPIO(391);
    m_pPowerOff[0] = new oddyseyx86GPIO(340);
    m_pMM[0] = new oddyseyx86GPIO(341);
    m_pPWRorMM[0]->setPinAsInputElseOutput(true);
    m_pPowerOff[0]->setPinAsInputElseOutput(true);
    m_pMM[0]->setPinAsInputElseOutput(true);
    m_pPWRorMM[0]->startListener_buttons_powerAndMaintenance();
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
DF_ERROR dispenser::setPumpDirectionForward()
{
    debugOutput::sendMessage("-----FORWARD Pump-----", MSG_INFO);
    the_8344->setPumpDirectionForwardElseReverse(true);
}

bool dispenser::getDispenseButtonValue(){
    the_8344->getButton();
}

// Reverse pump: Turn forward pin LOW - Reverse pin HIGH
DF_ERROR dispenser::setPumpDirectionReverse()
{
    debugOutput::sendMessage("-----REVERSE Pump-----", MSG_INFO);
    the_8344->setPumpDirectionForwardElseReverse(false);
}

// Stops pumping: Turn forward pin LOW - Reverse pin LOW
DF_ERROR dispenser::setPumpsDisableAll()
{
    debugOutput::sendMessage("-----Stop Pump-----", MSG_INFO);
    the_8344->setPumpsDisableAll();
    m_isPumpEnabled = false;
}

bool dispenser::isPumpEnabled()
{
    return m_isPumpEnabled;
}

DF_ERROR dispenser::setPumpEnable(int pos)
{
    // still needs dispense button to actually get the pump to start
    debugOutput::sendMessage("-----Start Pump-----", MSG_INFO);
    the_8344->setPumpEnable(pos);
    m_isPumpEnabled = true;
}

DF_ERROR dispenser::setPumpPWM(uint8_t value)
{
    debugOutput::sendMessage("-----Set PWM-----", MSG_INFO);
    the_8344->setPumpPWM((unsigned char)value);
}

// Disenses products by turning Solenoid Signal to HIGH then to LOW
DF_ERROR dispenser::startDispense()
{
    DF_ERROR e_ret = ERROR_MECH_PRODUCT_FAULT;
    debugOutput::sendMessage("Dispense start. Triggered pump:" + to_string(this->slot), MSG_INFO);

    setPumpDirectionForward();
    setPumpPWM((uint8_t)(m_pDispensedProduct->getPWM()));
    setPumpEnable(this->slot);
    return e_ret = OK;
}

double dispenser::getDispensedVolume(){
    return m_pDispensedProduct->getVolumeDispensed();
}


unsigned short dispenser::getPumpSpeed(){
    the_8344->getPumpSpeed();
}


double dispenser::getVolumeDeltaAndReset(){
    // will get volumeDelta since last call of this function

    
    double currentVolume = getDispensedVolume()
    double deltaVolume = previousDispensedVolume - currentVolume;
    previousDispensedVolume = currentVolume;
    return deltaVolume;
}
double dispenser::getInstantFlowRate(){
    
    using namespace std::chrono;
    uint64_t millis_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

}

double dispenser::getAveragedFlowRate1s(){
    
}

// flow rate at pump running when pumpspeed is non zero. check intervals. --> 0.5s frame?! 1s frame?
double dispenser::getFlowRate(){

    // average. x steps. 
    // if speed is zero, reset average.
    
    // get volume change since last check.
    // get time since last check
    // calc flow.

    // update avg flowrate.

}

DF_ERROR dispenser::stopDispense()
{
    DF_ERROR e_ret = ERROR_BAD_PARAMS;
    the_8344->setPumpsDisableAll();
    debugOutput::sendMessage("Pump disabled", MSG_INFO);

    // XXX: Disable Button - Linked thru State Virtual
    // e_ret = disconnectButton();
    m_isDispenseDone = true;

    return e_ret = OK;
}

// DF_ERROR dispenser::connectButton()
// {
//     return OK;
//     // return m_pButton[0]->writePin(HIGH);
// }

// DF_ERROR dispenser::disconnectButton()
// {
//     return OK;
//     // return m_pButton[0]->writePin(LOW);
// }

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
    return m_pDispensedProduct;
}

// Timer based
// DF_ERROR dispenser::testSolenoidDispense(int pos)
// {
//     DF_ERROR e_ret = ERROR_BAD_PARAMS;

//     //m_pSolenoid[pos]->writePin(HIGH);
//     sleep(TEST_ACTIVATION_TIME);
//     //m_pSolenoid[pos]->writePin(LOW);

//     return e_ret = OK;
// }

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
        m_pDispensedProduct = product;
    }
    else
    {
        debugOutput::sendMessage("Set Product Error!", MSG_ERROR);
    }
}
