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

// CTOR
dispenser::dispenser()
{
    // default constructor to set all pin to nullptr
    // debugOutput::sendMessage("dispenser", MSG_INFO);

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
}

DF_ERROR dispenser::setup()
{
    // Set the pump PWM value to a nominal value

    the_8344->setup();

    the_8344->setPumpPWM(DEFAULT_PUMP_PWM);
    the_8344->setDispenseButtonLight(false);

    for (int i = 0; i < NUM_SOLENOID; i++)
        m_pSolenoid[i] = nullptr;

    m_pFlowsenor[NUM_FLOWSENSOR] = nullptr;

    for (int i = 0; i < NUM_PUMP; i++)
    {

        m_pPump[i] = nullptr;
    }

    millisAtLastCheck = MILLIS_INIT_DUMMY;
    previousDispensedVolume = 0;
}

/*
dispenser::dispenser(gpio *ButtonReference)
{

    // NOT USED. To be reimplemented maybe.


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
*/
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
// void dispenser::initDispenser(int slot)
// {
// }

// TODO: Call this function on Dispense onEntry()
// DF_ERROR dispenser::setSolenoid(int mcpAddress, int pin, int pos)
// {
//     debugOutput::sendMessage("-----dispenser::setSolenoid-----", MSG_INFO);

//     return OK;
// }

// // TODO: Function name is inaccurate...deduct sale would be better
// void dispenser::recordSale(int volume)
// {
//     // TODO: SQLite database Update.
// }

// TODO: This function could live somewhere else...linked to future maintenance.
// void dispenser::refill(int volume)
// {
//     // TODO: SQLite database Update.
// }

// bool dispenser::registerFlowSensorTick()
// {
//     //    cout << "Registering Flow!!" << endl << "Vol disp: " << m_nVolumeDispensed << endl << "vol per tick: " << m_nVolumePerTick << endl;
//     m_nVolumeDispensed += m_pDispensedProduct->getVolumePerTick();
// }

// double dispenser::getVolumeSinceLastPoll()
// {
//     int temp = m_nVolumeDispensedSinceLastPoll;

//     m_nVolumeDispensed += m_nVolumeDispensedSinceLastPoll;

//     return temp;
// }
/* ------Getters, Setters and Utilities------ */

DF_ERROR dispenser::setSlot(int slot)
{
    // first slot is 1
    this->slot = slot;
}

int dispenser::getSlot()
{
    // first slot is 1
    return this->slot;
}

product *dispenser::getProduct()
{
    return m_pDispensedProduct;
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

// Reset values onEntry()
DF_ERROR dispenser::initDispense(int nVolumeToDispense, double nPrice)
// DF_ERROR dispenser::initDispense(int nVolumeToDispense)
{

    DF_ERROR dfRet = ERROR_BAD_PARAMS;
    m_nVolumeTarget = nVolumeToDispense;

    m_price = nPrice;

    resetVolumeDispensed();
    m_nVolumeDispensedPreviously = 0;
    m_nVolumeDispensedSinceLastPoll = 0;
    // m_nVolumePerTick = getVolPerTick();
    // m_PWM = getPWM();

    // Set Start Time
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(m_nStartTime, 50, "%F %T", timeinfo);

    return dfRet;
}
DF_ERROR dispenser::stopDispense()
{
    DF_ERROR e_ret = ERROR_BAD_PARAMS;
    the_8344->setPumpsDisableAll();
    debugOutput::sendMessage("Pump disabled", MSG_INFO);

    m_isDispenseDone = true;

    m_nVolumeDispensedSinceLastPoll = 0;
    m_nVolumeDispensedPreviously = 0;

    return e_ret = OK;
}
string dispenser::getDispenseStartTime()
{
    return m_nStartTime;
}

void dispenser::setVolumeDispensedPreviously(double volume)
{
    m_nVolumeDispensedPreviously = volume;
}
double dispenser::getVolumeDispensedPreviously()
{
    return m_nVolumeDispensedPreviously;
}

bool dispenser::getIsDispenseTargetReached()
{
    bool bRet = false;

    if (m_nVolumeTarget <= getVolumeDispensed())
    {
        bRet = true;
    }
    return bRet;
}

void dispenser::resetVolumeDispensed()
{
    // m_nVolumeDispensed = 0;
    getProduct()->resetVolumeDispensed();
}

void dispenser::subtractFromVolumeDispensed(double volume_to_distract){
    double volume = getProduct()->getVolumeDispensed();
    getProduct()->setVolumeDispensed(volume - volume_to_distract);
    
}

double dispenser::getVolumeDispensed()
{
    // return m_nVolumeDispensed;
    return getProduct()->getVolumeDispensed();
}

// TODO: Call this function on Dispense onEntry()
DF_ERROR dispenser::setFlowsensor(int pin, int pos)
{
    DF_ERROR e_ret = ERROR_BAD_PARAMS;

    // *m_pIsDispensing = false;
    std::string msg = "Dispenser::setFlowsensor. Position: " + std::to_string(pos) + " (pin: " + std::to_string(pin) + ")";
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
    DF_ERROR e_ret = ERROR_BAD_PARAMS; // reset variable

    // Save the pump number of this instance
    pump_position = (unsigned char)(position + 1);

    return e_ret = OK;
}

// TODO: Refactor Pumping with switch and ternary in future...keep seperate for ease of testing.
// Reverse pump: Turn forward pin HIGH - Reverse pin LOW
DF_ERROR dispenser::setPumpDirectionForward()
{
    debugOutput::sendMessage("Pump direction: Forward.", MSG_INFO);
    the_8344->setPumpDirection(true);
}

//  dispenseButtonValue = productDispensers[m_active_pump_index].getDispenseButtonValue();

//    if (!dispenseButtonValue && dispenseButtonValueMemory)
//    {
//       debugOutput::sendMessage("Dispense Button negative Edge.", MSG_INFO);
//    }

//    if (dispenseButtonValue && !dispenseButtonValueMemory)
//    {
//       debugOutput::sendMessage("Start Dispensing", MSG_INFO);

//       // productDispensers[m_active_pump_index].setPumpPWM(125);

void dispenser::refresh()
{
    // periodical polling refresh  (because the dispense button has no interrupt trigger (i2c))
    bool egde = dispenseButtonValueMemory != getDispenseButtonValue();
    dispenseButtonValueEdgePositive = getDispenseButtonValue() && egde;
    dispenseButtonValueEdgeNegative = (!getDispenseButtonValue()) && egde;
    dispenseButtonValueMemory = getDispenseButtonValue();
}

void dispenser::resetDispenseButton()
{
    dispenseButtonValueMemory = false;
}

bool dispenser::getDispenseButtonEdgeNegative()
{
    return dispenseButtonValueEdgeNegative;
}

bool dispenser::getDispenseButtonEdgePositive()
{
    return dispenseButtonValueEdgePositive;
}

bool dispenser::getDispenseButtonValue()
{
    the_8344->getDispenseButtonStateDebounced();
}

void dispenser::dispenseButtonTimingreset()
{
    dispense_button_total_pressed_millis = 0;
    dispense_button_current_press_millis = 0;
    using namespace std::chrono;
    dispense_button_time_at_last_check_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void dispenser::dispenseButtonTimingUpdate()
{
    // CAUTION: rudimentary. Will not detect intermitting keypresses between calls. So, check frequently.

    using namespace std::chrono;
    uint64_t now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    if (getDispenseButtonValue())
    {
        uint64_t interval = now - dispense_button_time_at_last_check_epoch;
        dispense_button_total_pressed_millis += interval;
        dispense_button_current_press_millis += interval;
    }
    else
    {
        dispense_button_current_press_millis = 0;
        // do nothing;
    }
    dispense_button_time_at_last_check_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    debugOutput::sendMessage("Button press millis. Total:" + to_string(dispense_button_total_pressed_millis) + " Current press:" + to_string(dispense_button_current_press_millis), MSG_INFO);
    // return dispense_button_total_pressed_millis;
}

uint64_t dispenser::getButtonPressedTotalMillis()
{
    return dispense_button_total_pressed_millis;
}
uint64_t dispenser::getButtonPressedCurrentPressMillis()
{
    return dispense_button_current_press_millis;
}

// Reverse pump: Turn forward pin LOW - Reverse pin HIGH
DF_ERROR dispenser::setPumpDirectionReverse()
{
    debugOutput::sendMessage("Pump direction: reverse", MSG_INFO);
    the_8344->setPumpDirection(false);
}

// Stops pumping: Turn forward pin LOW - Reverse pin LOW
DF_ERROR dispenser::setPumpsDisableAll()
{
    debugOutput::sendMessage("Pump disable: all.", MSG_INFO);
    the_8344->setPumpsDisableAll();
    m_isPumpEnabled = false;
}

void dispenser::reversePumpForSetTimeMillis(int millis)
{
    if (millis > 0)
    {
        // get volume before
        double volume_before = getVolumeDispensed();

        debugOutput::sendMessage("Pump auto retraction. Reverse time millis: " + to_string(millis), MSG_INFO);
        usleep(100000); // give pump time to stop
        setPumpDirectionReverse();
        // setPumpPWM(255); // quick retract
        setPumpEnable();
        the_8344->virtualButtonPressHack();
        // ideally, a certain volume is retracted. --> on-time depends on speed
        usleep(millis * 1000);
        the_8344->virtualButtonUnpressHack();
        setPumpsDisableAll();
        setPumpDirectionForward();
        // setPumpPWM((uint8_t)(m_pDispensedProduct->getPWM()));

        // get volume after
        double volume_after = getVolumeDispensed();

        // vol diff
        double volume_diff = volume_after - volume_before;
        debugOutput::sendMessage("ERROR: volume change not yet taken into account... Volume reversed:  " + to_string(volume_diff), MSG_WARNING);

        subtractFromVolumeDispensed(volume_diff);
    }
    else
    {
        debugOutput::sendMessage("Pump auto retraction disabled. Reverse time millis: " + to_string(millis), MSG_INFO);
    }
}

bool dispenser::isPumpEnabled()
{
    return m_isPumpEnabled;
}

DF_ERROR dispenser::setPumpEnable()
{
    // first pump is 1.
    // still needs dispense button to actually get the pump to start
    debugOutput::sendMessage("Pump enable position: " + to_string(this->slot), MSG_INFO);
    the_8344->setPumpEnable(this->slot); // pump 1 to 4
    m_isPumpEnabled = true;
}

DF_ERROR dispenser::setPumpPWM(uint8_t value)
{
    debugOutput::sendMessage("Pump set speed. PWM [0..255]: " + to_string(value), MSG_INFO);
    the_8344->setPumpPWM((unsigned char)value);
}

DF_ERROR dispenser::preparePumpForDispenseTrigger()
{
    DF_ERROR e_ret;
    setPumpDirectionForward();
    setPumpPWM((uint8_t)(m_pDispensedProduct->getPWM()));
    setPumpEnable();
    return e_ret = OK;
}

DF_ERROR dispenser::startDispense()
{
    using namespace std::chrono;
    dispense_start_timestamp_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    dispenseButtonTimingreset();

    previous_dispense_state = FLOW_STATE_UNAVAILABLE;

    DF_ERROR e_ret = ERROR_MECH_PRODUCT_FAULT;
    debugOutput::sendMessage("Dispense start. Triggered pump:" + to_string(this->slot), MSG_INFO);

    // setPumpDirectionForward();
    // setPumpPWM((uint8_t)(m_pDispensedProduct->getPWM()));
    // // setPumpPWM((uint8_t)(m_pDispensedProduct->getPWMFromDB()));

    // setPumpEnable();
    preparePumpForDispenseTrigger();

    flowRateBufferIndex = 0;
    for (uint16_t i = 0; i < RUNNING_AVERAGE_WINDOW_LENGTH; i++)
    {
        flowRateBuffer[i].time_millis = MILLIS_INIT_DUMMY;
        flowRateBuffer[i].value = 0;
    }

    return e_ret = OK;
}

unsigned short dispenser::getPumpSpeed()
{
    the_8344->getPumpSpeed();
}

void dispenser::loadEmptyContainerDetectionEnabledFromDb()
{
    // val 0 = empty container detection not enabled
    // val 1 = empty container detection enabled

#ifdef USE_OLD_DATABASE
    m_isEmptyContainerDetectionEnabled = false;
#else
    rc = sqlite3_open(DB_PATH, &db);
    sqlite3_stmt *stmt;
    string sql_string = "SELECT has_empty_detection FROM machine";
    /* Create SQL statement for transactions */
    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);

    int val = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    m_isEmptyContainerDetectionEnabled = (val != 0);

#endif
}

bool dispenser::getEmptyContainerDetectionEnabled()
{
    return m_isEmptyContainerDetectionEnabled;
}

double dispenser::getVolumeDeltaAndReset()
{
    // will get volumeDelta since last call of this function

    double currentVolume = getVolumeDispensed();
    double deltaVolume = currentVolume - previousDispensedVolume;
    previousDispensedVolume = currentVolume;
    return deltaVolume;
}

double dispenser::getInstantFlowRate()
{

    using namespace std::chrono;
    uint64_t millis_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    double volumeDelta = getVolumeDeltaAndReset();
    double flowRate = 0;
    if (millisAtLastCheck != MILLIS_INIT_DUMMY)
    {
        uint64_t millisDelta = millis_since_epoch - millisAtLastCheck;
        // debugOutput::sendMessage("Avg flow rate: millisdelta: " + to_string(millisDelta), MSG_INFO);
        // debugOutput::sendMessage("Avg flow rate: volume delta: " + to_string(volumeDelta), MSG_INFO);
        flowRate = (volumeDelta / millisDelta) * 1000.0; // volume per second.
    }
    else
    {
        flowRate = 0;
    }

    millisAtLastCheck = millis_since_epoch;
    return flowRate;
}

Time_val dispenser::getVolumeDispensedNow()
{
    Time_val tv;
    tv.value = getVolumeDispensed();
    using namespace std::chrono;
    uint64_t millis_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    tv.time_millis = millis_since_epoch;
    return tv;
}

Time_val dispenser::getAveragedFlowRate(uint64_t window_length_millis)
{
    // check for most recent datapoint
    // check for earliest datapoint
    // if dt less then window, just give total average
    // if dt more then window, search for point closest to window

    Time_val result;
    Time_val most_recent;
    uint64_t most_recent_millis = 0;
    Time_val earliest;
    uint64_t earliest_millis = 0xFFFFFFFFFFFFFFFF;

    for (uint16_t i = 0; i < RUNNING_AVERAGE_WINDOW_LENGTH; i++)
    {
        // debugOutput::sendMessage("Avg flow rate: run through window buffer: " + to_string(flowRateBuffer[i].time_millis), MSG_INFO);
        if (flowRateBuffer[i].time_millis != MILLIS_INIT_DUMMY && flowRateBuffer[i].time_millis > most_recent_millis)
        {
            // more recent found
            most_recent_millis = flowRateBuffer[i].time_millis;
            most_recent.time_millis = flowRateBuffer[i].time_millis;
            most_recent.value = flowRateBuffer[i].value;
        }
        if (flowRateBuffer[i].time_millis != MILLIS_INIT_DUMMY && flowRateBuffer[i].time_millis < earliest_millis)
        {
            // earlier found
            earliest_millis = flowRateBuffer[i].time_millis;
            earliest.time_millis = flowRateBuffer[i].time_millis;
            earliest.value = flowRateBuffer[i].value;
        }
    }
    if (most_recent_millis == 0 || earliest_millis == 0xFFFFFFFFFFFFFFFF)
    {
        // debugOutput::sendMessage("Avg flow rate: no values yet.", MSG_INFO);
        result.value = 0;
        result.time_millis = most_recent_millis;
        return result;
    }

    uint64_t delta_t = most_recent_millis - earliest_millis;
    // debugOutput::sendMessage("Avg flow rate delta t: " + to_string(delta_t), MSG_INFO);
    // debugOutput::sendMessage("Avg flow rate most recent t: " + to_string(most_recent_millis), MSG_INFO);
    // debugOutput::sendMessage("Avg flow rate earliest t: " + to_string(earliest_millis), MSG_INFO);

    if (delta_t < 1)
    {
        // debugOutput::sendMessage("Avg flow rate: not enough values yet.", MSG_INFO);
        result.value = 0;
        result.time_millis = most_recent_millis;
        return result;
    }

    double avg_vol_per_sec;

    // window smaller than requested
    if (delta_t <= window_length_millis)
    {

        avg_vol_per_sec = ((most_recent.value - earliest.value) / delta_t) * 1000;
        // debugOutput::sendMessage("Avg flow rate: Less values then window request.", MSG_INFO);
        result.value = avg_vol_per_sec;
        result.time_millis = most_recent_millis;
        return result;
    }

    // window bigger than requested

    uint64_t requested_earliest_time_millis = most_recent.time_millis - window_length_millis;
    uint64_t deviation = 0xFFFFFFFFFFFFFFFF;
    Time_val window_earliest;

    for (uint16_t i = 0; i < RUNNING_AVERAGE_WINDOW_LENGTH; i++)
    {
        int64_t diff = requested_earliest_time_millis - flowRateBuffer[i].time_millis;
        if (most_recent.time_millis != flowRateBuffer[i].time_millis && abs(diff) <= deviation)
        {
            // find most accurate time point to account for window size
            deviation = abs(diff);
            window_earliest.time_millis = flowRateBuffer[i].time_millis;
            window_earliest.value = flowRateBuffer[i].value;
        }
    }
    delta_t = most_recent.time_millis - window_earliest.time_millis;

    avg_vol_per_sec = ((most_recent.value - window_earliest.value) / delta_t) * 1000;
    // debugOutput::sendMessage("Avg flow rate: Good values. between: "
    //     + to_string(window_earliest.time_millis)
    //     + " and "
    //     + to_string(most_recent.time_millis)
    //     , MSG_INFO);
    result.value = avg_vol_per_sec;
    result.time_millis = most_recent_millis;
    return result;
}

DF_ERROR dispenser::updateRunningAverageWindow()
{
    DF_ERROR e_ret;

    Time_val tv = getVolumeDispensedNow();

    flowRateBuffer[flowRateBufferIndex].time_millis = tv.time_millis;
    flowRateBuffer[flowRateBufferIndex].value = tv.value;
    // debugOutput::sendMessage("updateRunningAverageWindow: index: " + to_string(flowRateBufferIndex) + " " + to_string(tv.time_millis) + ": " + to_string(tv.value), MSG_INFO);

    flowRateBufferIndex++;
    if (flowRateBufferIndex >= RUNNING_AVERAGE_WINDOW_LENGTH)
    {
        flowRateBufferIndex = 0;
    }

    return e_ret = OK;
}

// // flow rate at pump running when pumpspeed is non zero. check intervals. --> 0.5s frame?! 1s frame?
// double dispenser::getFlowRate(){

//     // average. x steps.
//     // if speed is zero, reset average.

//     // get volume change since last check.
//     // get time since last check
//     // calc flow.

//     // update avg flowRate.

// }

Dispense_behaviour dispenser::getDispenseStatus()
{
    // CAUTION: we are not using motor speed feedback. Button press assumes motor running.
    // todo: button press does not necessarily mean pump is on. We should work with pump speed feedback

    updateRunningAverageWindow();
    dispenseButtonTimingUpdate();

    using namespace std::chrono;
    uint64_t millis_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    uint64_t dispense_time_millis = millis_since_epoch - dispense_start_timestamp_epoch;
    Time_val avg = getAveragedFlowRate(EMPTY_CONTAINER_DETECTION_FLOW_AVERAGE_WINDOW_MILLIS);
    debugOutput::sendMessage("Dispense flowRate " + to_string(EMPTY_CONTAINER_DETECTION_FLOW_AVERAGE_WINDOW_MILLIS) + "ms avg [ml/s]: " + to_string(avg.value) + " Dispense state time: " + to_string(dispense_time_millis), MSG_INFO);

    Dispense_behaviour state;

    if (!getDispenseButtonValue())
    {

        state = FLOW_STATE_NOT_PUMPING_NOT_DISPENSING;
    }
    else if (getButtonPressedCurrentPressMillis() < 3 * EMPTY_CONTAINER_DETECTION_FLOW_AVERAGE_WINDOW_MILLIS)
    {
        // button pressed (aka pumping)

        // at each button press, the average flow needs to ramp up again. --> not available
        state = FLOW_STATE_UNAVAILABLE;
    }
    else if (avg.value > getProduct()->getThresholdFlow())
    {
        // button pressed (aka pumping)
        // init time long enough for valid data

        state = FLOW_STATE_DISPENSING;
    }

    else if (previous_dispense_state == FLOW_STATE_DISPENSING)
    {
        // button pressed (aka pumping)
        // init time long enough for valid data
        // no flow detected

        // once it was dispensing, empty dispenser is detected immediatly if no product flows.
        // bugfix: if the button was release and repressed, the average was not correct at restart
        //          --> take into account. at top level (FLOW_STATE_UNAVAILABLE)
        state = FLOW_STATE_CONTAINER_EMPTY;
    }
    else if (getButtonPressedTotalMillis() > EMPTY_CONTAINER_DETECTION_MAXIMUM_PRIME_TIME_MILLIS)
    {
        // button pressed (aka pumping)
        // init time long enough for valid data
        // no flow detected
        // previous state was not dispensing

        // pump
        state = FLOW_STATE_CONTAINER_EMPTY;
    }
    else
    {
        // button pressed (aka pumping)
        // init time long enough for valid data
        // no flow detected
        // previous state was not dispensing
        // pumping time has exceeded set value

        state = FLOW_STATE_ATTEMTPING_TO_PRIME;
    }

    previous_dispense_state = state;
    return state;
}
