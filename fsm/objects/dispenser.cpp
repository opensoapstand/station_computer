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

#define ACTIVATION_TIME 5
#define TEST_ACTIVATION_TIME 3
#define PRIME_PUMP_TIME 1
#define CLEAN_WATER_TIME 1
#define CLEAN_AIR_TIME 1

#define MAX_BUF 64

// pcb *dispenser::the_pcb = nullptr;

const char *DISPENSE_BEHAVIOUR_STRINGS[] = {
    "FLOW_STATE_UNAVAILABLE",
    "FLOW_STATE_RAMP_UP",
    "FLOW_STATE_DISPENSING",
    "FLOW_STATE_PUMPING_NOT_DISPENSING",
    "FLOW_STATE_NOT_PUMPING_NOT_DISPENSING",
    "FLOW_STATE_PRIMING_OR_EMPTY",
    "FLOW_STATE_PRIME_FAIL_OR_EMPTY",
    "FLOW_STATE_EMPTY"};

const char *DISPENSER_STATE_STRINGS[] = {
    "DISPENSER_STATE_AVAILABLE",
    "DISPENSER_STATE_AVAILABLE_LOW_STOCK",
    "DISPENSER_STATE_WARNING_PRIMING",
    "DISPENSER_STATE_PROBLEM_NEEDS_ATTENTION",
    "DISPENSER_STATE_PROBLEM_EMPTY",
    "DISPENSER_STATE_DISABLED_COMING_SOON",
    "DISPENSER_STATE_DISABLED"};

// CTOR
dispenser::dispenser()
{
    // default constructor to set all pin to nullptr
    // debugOutput::sendMessage("dispenser", MSG_INFO);

    // TODO: Need to build Product Object reference
    // m_pDispensedProduct = nullptr;

    // If we haven't instantiated and initialized the hardware yet we
    // do it here.  Note that the pointer is declared as static so we
    // will only ever create one pcb() class no matter how many
    // times we create a dispenser() class.
    // if (the_pcb == nullptr)
    // {
    //     the_pcb = new pcb();
    // }
    dispense_state = FLOW_STATE_UNAVAILABLE;
    previous_dispense_state = FLOW_STATE_UNAVAILABLE;
}

DF_ERROR dispenser::setup(pcb *pcbtest)
{
    // Set the pump PWM value to a nominal value

    // the_pcb->setup();
    the_pcb = pcbtest;

    the_pcb->setPumpPWM(DEFAULT_PUMP_PWM);

    m_pFlowsenor[NUM_FLOWSENSOR] = nullptr;

    millisAtLastCheck = MILLIS_INIT_DUMMY;
    previousDispensedVolume = 0;
    isPumpSoftStarting = false;
    pwm_actual_set_speed = 0;
}

void dispenser::refresh()
{

    the_pcb->pcb_refresh();

    // periodical polling refresh  (because the dispense button has no interrupt trigger (i2c))
    bool egde = dispenseButtonValueMemory != getDispenseButtonValue();
    dispenseButtonValueEdgePositive = getDispenseButtonValue() && egde;
    dispenseButtonValueEdgeNegative = (!getDispenseButtonValue()) && egde;
    dispenseButtonValueMemory = getDispenseButtonValue();

    // status update time
    using namespace std::chrono;
    uint64_t now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    if (previous_status_update_allowed_epoch + DISPENSE_STATUS_UPDATE_DELTA_MILLIS < now)
    {
        previous_status_update_allowed_epoch = now;
        isStatusUpdateSendAndPrintAllowed = true;
    }
    else
    {
        isStatusUpdateSendAndPrintAllowed = false;
    }

    dispenseButtonTimingUpdate();
    pumpSlowStartHandler();
}

// DTOR
dispenser::~dispenser()
{
    debugOutput::sendMessage("~dispenser", MSG_INFO);

    delete the_pcb;
    the_pcb = nullptr;

    using namespace std::chrono;
    previous_status_update_allowed_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

string dispenser::getFinalPLU(char size, double price)
{

    string base_plu = getProduct()->getBasePLU(size);
    char chars_plu_dynamic_formatted[MAX_BUF];

    std::string paymentMethod = getProduct()->getPaymentMethod();
    if (paymentMethod == "plu")
    {
        return base_plu;
    }

    if (size == SIZE_CUSTOM_CHAR)
    {

        if (paymentMethod == "barcode" || paymentMethod == "barcode_EAN-13")
        {
            if (base_plu.size() != 8)
            {
                // debugOutput::sendMessage("Custom plu: " + plu, MSG_INFO);
                debugOutput::sendMessage("ERROR custom plu length must be of length eight. (e.g. standard drinkfill preamble(627987) + 2digit product code) : " + base_plu, MSG_WARNING);
                string fake_plu = "66666666";
                base_plu = fake_plu;
            }

            snprintf(chars_plu_dynamic_formatted, sizeof(chars_plu_dynamic_formatted), "%5.2f", price); // will always be at least 5 chars long e.g. 3.456 --> " 3.46" , 1234.456 --> "1234.45"
        }
        else if (paymentMethod == "barcode_EAN-2")
        {
            if (base_plu.size() != 7)
            {
                // debugOutput::sendMessage("Custom plu: " + plu, MSG_INFO);
                debugOutput::sendMessage("ERROR custom plu length must be of length seven. provided: " + base_plu, MSG_WARNING);
                string fake_plu = "6666666";
                base_plu = fake_plu;
            }

            snprintf(chars_plu_dynamic_formatted, sizeof(chars_plu_dynamic_formatted), "%6.2f", price); // will always be at least 6 chars long e.g. 3.456 --> "  3.45" , 1234.456 --> "1234.46"
        }
        else
        {
            debugOutput::sendMessage("ERROR Payment method not expected: " + paymentMethod, MSG_ERROR);
            string fake_plu = "66666666";
            base_plu = fake_plu;
        }

        string plu_dynamic_price = (chars_plu_dynamic_formatted);

        string plu_dynamic_formatted = base_plu + plu_dynamic_price;

        // 3.14 --> " 3.14" --> " 314" --> "0314"
        // 140.00 -->
        std::string toReplace(".");
        size_t pos = plu_dynamic_formatted.find(toReplace);
        if (pos != -1)
        {
            plu_dynamic_formatted.replace(pos, toReplace.length(), "");
        }

        std::string toReplace2(" ");
        pos = plu_dynamic_formatted.find(toReplace2);
        while (pos != -1)
        {
            plu_dynamic_formatted.replace(pos, toReplace2.length(), "0");
            pos = plu_dynamic_formatted.find(toReplace2);
        }

        if (plu_dynamic_formatted.length() != 12)
        {
            debugOutput::sendMessage("ERROR Generated barcode has an error. Was the price more than 99.99?  Its length should be twelve: " + plu_dynamic_formatted, MSG_ERROR);
        }
        return plu_dynamic_formatted;
    }

    return base_plu;
}

void dispenser::setAllDispenseButtonLightsOff()
{
    for (int slot = 1; slot < 5; slot++)
    {
        debugOutput::sendMessage("slot light off: " + to_string(slot), MSG_INFO);
        setMultiDispenseButtonLight(slot, false);
    }
}

void dispenser::setMultiDispenseButtonLight(int slot, bool enableElseDisable)
{
    // output has to be set low for light to be on.
    debugOutput::sendMessage("slot light: " + to_string(slot) + "on else off: " + to_string(enableElseDisable), MSG_INFO);

    if (the_pcb->get_pcb_version() == pcb::PcbVersion::DSED8344_PIC_MULTIBUTTON)
    {
        if (getMultiDispenseButtonEnabled())
        {

            if (this->slot == 4)
            {
                m_pDispenseButton4[0]->writePin(!enableElseDisable);
            }
            else
            {

                this->the_pcb->setSingleDispenseButtonLight(slot, enableElseDisable);
            }
        }
        else
        {
            this->the_pcb->setSingleDispenseButtonLight(1, enableElseDisable);
        }
    }
    else if (the_pcb->get_pcb_version() == pcb::PcbVersion::DSED8344_NO_PIC)
    {
        this->the_pcb->setSingleDispenseButtonLight(1, enableElseDisable);
    }
    else
    {
        this->the_pcb->setSingleDispenseButtonLight(slot, enableElseDisable);
    }
}

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

DF_ERROR dispenser::loadGeneralProperties()
{

    debugOutput::sendMessage("Load general properties:", MSG_INFO);
    // ******* Sleep time between DB calls solved inconsistend readings from db!!!****
    usleep(20000);
    loadDispenserStateFromDb();
    usleep(20000);
    loadEmptyContainerDetectionEnabledFromDb();
    usleep(20000);
    loadPumpRampingEnabledFromDb();
    usleep(20000);
    loadPumpReversalEnabledFromDb();
    usleep(20000);
    loadMultiDispenseButtonEnabledFromDb();
    usleep(20000);
    //  the_pcb->setSingleDispenseButtonLight(this->slot, false);

    resetVolumeDispensed();
}

DF_ERROR dispenser::startDispense()
{
    using namespace std::chrono;
    dispense_start_timestamp_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    dispenseButtonTimingreset();

    this->the_pcb->flowSensorEnable(slot);
    this->the_pcb->resetFlowSensorTotalPulses(slot);

    dispense_state = FLOW_STATE_NOT_PUMPING_NOT_DISPENSING;
    previous_dispense_state = FLOW_STATE_UNAVAILABLE;

    DF_ERROR e_ret = ERROR_MECH_PRODUCT_FAULT;
    debugOutput::sendMessage("Dispense start at slot " + to_string(this->slot), MSG_INFO);

    initFlowRateCalculation();

    return e_ret = OK;
}

// Reset values onEntry()
DF_ERROR dispenser::initDispense(int nVolumeToDispense, double nPrice)
{

    DF_ERROR dfRet = ERROR_BAD_PARAMS;
    m_nVolumeTarget = nVolumeToDispense;

    m_price = nPrice;

    resetVolumeDispensed();

    // if (the_pcb->get_pcb_version() == 2){
    if (the_pcb->get_pcb_version() == pcb::PcbVersion::DSED8344_PIC_MULTIBUTTON)
    {
        if (getMultiDispenseButtonEnabled())
        {
            setMultiDispenseButtonLight(getSlot(), true);
        }
        else
        {
            setMultiDispenseButtonLight(1, true);
        }
        setPumpEnable(); // Added at time of EN-134 integration. Why did things work earlier onwards?
    }
    else if (the_pcb->get_pcb_version() == pcb::PcbVersion::EN134_4SLOTS || the_pcb->get_pcb_version() == pcb::PcbVersion::EN134_8SLOTS)
    {
        the_pcb->sendEN134DefaultConfigurationToPCA9534(slot, true);
        setPumpEnable();
        setMultiDispenseButtonLight(getSlot(), true);
    }
    else if (the_pcb->get_pcb_version() == pcb::PcbVersion::DSED8344_NO_PIC)
    {
        setMultiDispenseButtonLight(getSlot(), true);
    }
    else
    {
        debugOutput::sendMessage("No dispense button light enabled. ", MSG_WARNING);
    }

    // Set Start Time
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(m_nStartTime, 50, "%F %T", timeinfo);

    return dfRet;
}
DF_ERROR dispenser::stopDispense()
{
    debugOutput::sendMessage("stop dispense actions...", MSG_INFO);
    // if (getMultiDispenseButtonEnabled())
    // {
    setAllDispenseButtonLightsOff();
    // }

    the_pcb->flowSensorsDisableAll();

    // Set End time
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(m_nEndTime, 50, "%F %T", timeinfo);
}
string dispenser::getDispenseStartTime()
{
    return m_nStartTime;
}
string dispenser::getDispenseEndTime()
{
    return m_nEndTime;
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
    getProduct()->resetVolumeDispensed();
}

void dispenser::subtractFromVolumeDispensed(double volume_to_distract)
{
    double volume = getProduct()->getVolumeDispensed();
    getProduct()->setVolumeDispensed(volume - volume_to_distract);
}

double dispenser::getVolumeRemaining()
{
    return getProduct()->getVolumeRemaining();
}

double dispenser::getVolumeDispensed()
{
    // return m_nVolumeDispensed;
    return getProduct()->getVolumeDispensed();
}

// TODO: Call this function on Dispense onEntry()
DF_ERROR dispenser::initGlobalFlowsensorIO(int pin, int pos)
{
    DF_ERROR e_ret = ERROR_BAD_PARAMS;

    // *m_pIsDispensing = false;
    std::string msg = "Dispenser::initGlobalFlowsensorIO. Position: " + std::to_string(pos) + " (pin: " + std::to_string(pin) + ")";
    // debugOutput::sendMessage("-----dispenser::initGlobalFlowsensorIO-----", MSG_INFO);
    debugOutput::sendMessage(msg, MSG_INFO);

    if ((pos >= 0) && (pos < 4))
    {
        // Instantiate, set input, spin up a flowsensor thread.
        // gets created at every instance. Which is not ok as there is only one pin that gets looked at multiple times. Hence, if there are four slots, for every tick, things will get triggered four times (even an edge) because it's processed four times (but seems to work)
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

DF_ERROR dispenser::initDispenseButton4Light()
{
    if (the_pcb->get_pcb_version() == pcb::PcbVersion::DSED8344_PIC_MULTIBUTTON)
    {
        m_pDispenseButton4[0] = new oddyseyx86GPIO(IO_PIN_BUTTON_4);
        m_pDispenseButton4[0]->setPinAsInputElseOutput(false);
    }
}

DF_ERROR dispenser::initButtonsShutdownAndMaintenance()
{

    m_pPowerOffOrMaintenanceModeButtonPressed[0] = new oddyseyx86GPIO(IO_PIN_BUTTON_MAINTENANCE_SHUTDOWN_EDGE_DETECTOR);
    m_pButtonPowerOff[0] = new oddyseyx86GPIO(IO_PIN_BUTTON_MAINTENANCE);
    m_pButtonDisplayMaintenanceMode[0] = new oddyseyx86GPIO(IO_PIN_BUTTON_SHUTDOWN);

    m_pPowerOffOrMaintenanceModeButtonPressed[0]->setPinAsInputElseOutput(true);
    m_pButtonPowerOff[0]->setPinAsInputElseOutput(true);
    m_pButtonDisplayMaintenanceMode[0]->setPinAsInputElseOutput(true);
    m_pPowerOffOrMaintenanceModeButtonPressed[0]->startListener_buttons_powerAndMaintenance();
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
    the_pcb->setPumpDirection(this->slot, true);
}

bool dispenser::getIsStatusUpdateAllowed()
{
    // prevent flooding with messages by limiting amount of traffic let through
    return isStatusUpdateSendAndPrintAllowed;
}

void dispenser::logUpdateIfAllowed(string message)
{
    // only use this for updates that would happen at every stateDispense refresh cycle.
    if (isStatusUpdateSendAndPrintAllowed)
    {
        debugOutput::sendMessage(message,
                                 MSG_INFO);
    }
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
    the_pcb->getDispenseButtonStateDebounced(this->slot);
}

void dispenser::dispenseButtonTimingreset()
{
    dispense_button_total_pressed_millis = 0;
    dispense_button_current_press_millis = 0;
    dispense_button_press_count_during_dispensing = 0;
    using namespace std::chrono;
    dispense_button_time_at_last_check_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void dispenser::addDispenseButtonPress()
{
    dispense_button_press_count_during_dispensing++;
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
}

int dispenser::getDispenseButtonPressesDuringDispensing()
{
    return dispense_button_press_count_during_dispensing;
}

uint64_t dispenser::getButtonPressedTotalMillis()
{
    return dispense_button_total_pressed_millis;
}
uint64_t dispenser::getButtonPressedCurrentPressMillis()
{
    return dispense_button_current_press_millis;
}

void dispenser::setSolenoid(bool openElseClosed)
{
    the_pcb->setSolenoid(this->slot, openElseClosed);
}

// Reverse pump: Turn forward pin LOW - Reverse pin HIGH
DF_ERROR dispenser::setPumpDirectionReverse()
{
    DF_ERROR e_ret = OK;
    if (!getPumpReversalEnabled())
    {
        debugOutput::sendMessage("Pump reversal not allowed. Will not execute command.", MSG_WARNING);

        return e_ret;
    }
    debugOutput::sendMessage("Pump direction: reverse", MSG_INFO);
    the_pcb->setPumpDirection(this->slot, false);
}

// Stops pumping: Turn forward pin LOW - Reverse pin LOW
DF_ERROR dispenser::setPumpsDisableAll()
{
    debugOutput::sendMessage("Pump disable: all.", MSG_INFO);
    the_pcb->setPumpsDisableAll();
    m_isSlotEnabled = false;
    isPumpSoftStarting = false;
    pwm_actual_set_speed = 0;
}

void dispenser::reversePumpForSetTimeMillis(int millis)
{
    if (getPumpReversalEnabled())
    {
        if (millis > 0)
        {
            // get volume before
            double volume_before = getVolumeDispensed();

            debugOutput::sendMessage("Pump auto retraction. Reverse time millis: " + to_string(millis), MSG_INFO);
            pumpSlowStart(false);

            the_pcb->virtualButtonPressHack(this->slot);
            // ideally, a certain volume is retracted. --> on-time depends on speed

            using namespace std::chrono;
            uint64_t retraction_start = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            bool retraction_done = false;
            while (!retraction_done)
            {
                uint64_t now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
                if (now - retraction_start > millis)
                {
                    retraction_done = true;
                }
                pumpSlowStartHandler();
            }

            // usleep(millis * 1000);
            the_pcb->virtualButtonUnpressHack(this->slot); // needed! To have the button pressing sequence right. (fake button presses can mess the button detection up)

            pumpSlowStopBlocking();

            // setPumpsDisableAll();
            setPumpDirectionForward();
            // setPumpPWM((uint8_t)(m_pDispensedProduct->getPWM()));

            // get volume after
            double volume_after = getVolumeDispensed();

            // vol diff
            double volume_diff = volume_after - volume_before;

            subtractFromVolumeDispensed(volume_diff);
            debugOutput::sendMessage("Retraction done. WARNING: check volume change correction subtraction. Volume reversed:  " + to_string(volume_diff), MSG_INFO);
        }
        else
        {
            debugOutput::sendMessage("Rectraction disabled. ms:" + to_string(millis), MSG_INFO);
        }
    }
}

bool dispenser::isSlotEnabled()
{
    return m_isSlotEnabled;
}

DF_ERROR dispenser::pumpSlowStartHandler()
{

    if (isPumpSoftStarting)
    {
        uint8_t target_pwm = (uint8_t)(m_pDispensedProduct->getPWM());

        if (pwm_actual_set_speed < target_pwm)
        {
            using namespace std::chrono;
            uint64_t now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

            uint64_t delta = now - slowStartMostRecentIncreaseEpoch;

            while (delta > SLOW_START_INCREASE_PERIOD_MILLIS)
            {
                delta = delta - SLOW_START_INCREASE_PERIOD_MILLIS;
                pwm_actual_set_speed++;
                setPumpPWM(pwm_actual_set_speed, false);
            }
            slowStartMostRecentIncreaseEpoch = now - delta; // if increments are a less than SLOW_START_INCREASE_PERIOD_MILLIS, they need to increment ...
        }
        else
        {
            isPumpSoftStarting = false;
        }
    }
}

DF_ERROR dispenser::pumpSlowStart(bool forwardElseReverse)
{

    if (!forwardElseReverse && !getPumpReversalEnabled())
    {
        debugOutput::sendMessage("Pump reversal not allowed. Will not execute command.", MSG_WARNING);
        DF_ERROR e_ret = OK;

        return e_ret;
    }

    // // set set speed to zero, as there is no real speed feedback, we can't guarantee a claimed initial set speed to be true

    using namespace std::chrono;
    slowStartMostRecentIncreaseEpoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    uint8_t target_pwm = (uint8_t)(m_pDispensedProduct->getPWM());

    if (forwardElseReverse)
    {
        setPumpDirectionForward();
    }
    else
    {
        setPumpDirectionReverse();
    }
    usleep(10000); // make sure direction is set well
    setPumpEnable();
    if (SLOW_START_INCREASE_PERIOD_MILLIS == 0 || !getPumpSlowStartStopEnabled())
    {
        debugOutput::sendMessage("Pump instant start", MSG_INFO);
        setPumpPWM(target_pwm, true);
        isPumpSoftStarting = false;
    }
    else
    {

        debugOutput::sendMessage("Pump slow start. from " + to_string(pwm_actual_set_speed) + " to " + to_string(target_pwm), MSG_INFO);
        setPumpPWM(0, false);
        isPumpSoftStarting = true;
    }
}

DF_ERROR dispenser::pumpSlowStopBlocking()
{
    isPumpSoftStarting = false;
    if (pwm_actual_set_speed > 0)
    {
        if (SLOW_STOP_PERIOD_MILLIS == 0 || !getPumpSlowStartStopEnabled())
        {
            debugOutput::sendMessage("Pump instant stop", MSG_INFO);
            // no slow stop
        }
        else
        {
            debugOutput::sendMessage("Pump slow stop. from " + to_string(pwm_actual_set_speed) + " to " + to_string(0), MSG_INFO);
            the_pcb->virtualButtonPressHack(this->slot);

            for (int i = pwm_actual_set_speed; i >= 0; --i)
            {
                setPumpPWM(i, false);
                usleep(SLOW_STOP_PERIOD_MILLIS * 1000); // one second ramp up to full speed --> 255 steps ==> 4ms per step.
            }
            the_pcb->virtualButtonUnpressHack(this->slot);
        }
    }
    else
    {
        debugOutput::sendMessage("Pump was already stopped.", MSG_INFO);
    }
    setPumpsDisableAll();
}

DF_ERROR dispenser::setPumpEnable()
{
    // first pump is 1.
    // still needs dispense button to actually get the pump to start
    debugOutput::sendMessage("Pump enable position: " + to_string(this->slot), MSG_INFO);
    the_pcb->setPumpEnable(this->slot); // pump 1 to 4
    m_isSlotEnabled = true;
}

DF_ERROR dispenser::setPumpPWM(uint8_t value, bool enableLog)
{
    if (enableLog)
    {
        debugOutput::sendMessage("Pump set speed. PWM [0..255]: " + to_string(value), MSG_INFO);
    }
    the_pcb->setPumpPWM((unsigned char)value);
}

unsigned short dispenser::getPumpSpeed()
{
    the_pcb->getPumpPWM();
}

void dispenser::loadMultiDispenseButtonEnabledFromDb()
{

    rc = sqlite3_open(DB_PATH, &db);
    sqlite3_stmt *stmt;
    string sql_string = "SELECT dispense_buttons_count FROM machine";
    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);

    int val = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    // val = 4;
    if (val == 1)
    {
        m_isMultiButtonEnabled = false;
    }
    else if (val == 4)
    {
        m_isMultiButtonEnabled = true;
    }
    else
    {
        m_isMultiButtonEnabled = false;
        debugOutput::sendMessage("ASSERT Error: unimplemented number of dispense buttons. Default to single dispense button. Buttons indicated in db:" + to_string(m_isMultiButtonEnabled), MSG_ERROR);
    }

    debugOutput::sendMessage("Multiple dispense buttons enabled? : " + to_string(m_isMultiButtonEnabled), MSG_INFO);
}

bool dispenser::getMultiDispenseButtonEnabled()
{
    return m_isMultiButtonEnabled;
}

void dispenser::loadPumpReversalEnabledFromDb()
{
    // val 0 = pump reversal not enabled
    // val 1 = pump reversal enabled. Will take retraction time from products

    rc = sqlite3_open(DB_PATH, &db);
    sqlite3_stmt *stmt;
    string sql_string = "SELECT enable_pump_reversal FROM machine";

    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);

    int val = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    m_isPumpReversalEnabled = (val != 0);

    debugOutput::sendMessage("Pump reversal enabled? : " + to_string(m_isPumpReversalEnabled), MSG_INFO);
}

bool dispenser::getPumpReversalEnabled()
{
    return m_isPumpReversalEnabled;
}

void dispenser::loadPumpRampingEnabledFromDb()
{
    // val 0 = pump slow start stop not enabled
    // val 1 = pump slow start, slow stop enabled (with hardwired ramp up / ramp down time)

    rc = sqlite3_open(DB_PATH, &db);
    sqlite3_stmt *stmt;
    string sql_string = "SELECT enable_pump_ramping FROM machine";

    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);

    int val = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    m_isPumpSlowStartStopEnabled = (val != 0);

    debugOutput::sendMessage("Pump ramping enabled? : " + to_string(m_isPumpSlowStartStopEnabled) + "(val: " + to_string(val) + ")", MSG_INFO);
}

bool dispenser::getPumpSlowStartStopEnabled()
{
    return m_isPumpSlowStartStopEnabled;
}

void dispenser::loadDispenserStateFromDb()
{
    rc = sqlite3_open(DB_PATH, &db);
    sqlite3_stmt *stmt;
    string sql_string = "SELECT status_text_slot_" + to_string(getSlot()) + " FROM machine;";

    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);

    int status;
    status = sqlite3_step(stmt);
    string dispenserStateText = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    if (dispenserStateText.find("DISPENSER_STATE_AVAILABLE") != string::npos)
    {
        setDispenserState(DISPENSER_STATE_AVAILABLE);
    }
    else if (dispenserStateText.find("DISPENSER_STATE_AVAILABLE_LOW_STOCK") != string::npos)
    {
        setDispenserState(DISPENSER_STATE_AVAILABLE_LOW_STOCK);
    }
    else if (dispenserStateText.find("DISPENSER_STATE_WARNING_PRIMING") != string::npos)
    {
        setDispenserState(DISPENSER_STATE_WARNING_PRIMING);
    }
    else if (dispenserStateText.find("DISPENSER_STATE_PROBLEM_NEEDS_ATTENTION") != string::npos)
    {
        setDispenserState(DISPENSER_STATE_PROBLEM_NEEDS_ATTENTION);
    }
    else if (dispenserStateText.find("DISPENSER_STATE_PROBLEM_EMPTY") != string::npos)
    {
        setDispenserState(DISPENSER_STATE_PROBLEM_EMPTY);
    }
    else if (dispenserStateText.find("DISPENSER_STATE_DISABLED_COMING_SOON") != string::npos)
    {
        setDispenserState(DISPENSER_STATE_DISABLED_COMING_SOON);
    }
    else if (dispenserStateText.find("DISPENSER_STATE_DISABLED") != string::npos)
    {
        setDispenserState(DISPENSER_STATE_DISABLED);
    }
    else
    {
        setDispenserState(DISPENSER_STATE_AVAILABLE);
    }
    debugOutput::sendMessage("Set dispenser state to : " + std::string(getDispenserStateAsString()), MSG_INFO);
}

void dispenser::loadEmptyContainerDetectionEnabledFromDb()
{
    // val 0 = empty container detection not enabled
    // val 1 = empty container detection enabled

    rc = sqlite3_open(DB_PATH, &db);
    sqlite3_stmt *stmt;
    string sql_string = "SELECT has_empty_detection FROM machine";

    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);

    int val = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    m_isEmptyContainerDetectionEnabled = (val != 0);

    debugOutput::sendMessage("Empty container detection enabled? : " + to_string(m_isEmptyContainerDetectionEnabled), MSG_INFO);
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

void dispenser::initFlowRateCalculation()
{
    flowRateBufferIndex = 0;
    for (uint16_t i = 0; i < RUNNING_AVERAGE_WINDOW_LENGTH; i++)
    {
        flowRateBuffer[i].time_millis = MILLIS_INIT_DUMMY;
        flowRateBuffer[i].value = 0;
    }
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
    // WARNING: check here if buffer length is enough to accomodate for requested length.
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

const char *dispenser::getDispenseStatusAsString()
{
    Dispense_behaviour behaviour = getDispenseStatus();
    const char *behaviour_str = DISPENSE_BEHAVIOUR_STRINGS[behaviour];
    return behaviour_str;
}

const char *dispenser::getDispenserStateAsString()
{
    Dispenser_state state = getDispenserState();
    const char *state_str = DISPENSER_STATE_STRINGS[state];
    return state_str;
}

void dispenser::setDispenserState(Dispenser_state state)
{
    // disabled states are only manually changeable.
    if (getDispenserState() != DISPENSER_STATE_DISABLED_COMING_SOON || getDispenserState() != DISPENSER_STATE_DISABLED)
    {
        dispenser_state = state;
    }
}

Dispenser_state dispenser::getDispenserState()
{

    return dispenser_state;
}

void dispenser::updateDispenserState()
{
//        FLOW_STATE_UNAVAILABLE = 0,
//    FLOW_STATE_RAMP_UP,
//    FLOW_STATE_DISPENSING,
//    FLOW_STATE_PUMPING_NOT_DISPENSING,
//    FLOW_STATE_NOT_PUMPING_NOT_DISPENSING,
//    FLOW_STATE_PRIMING_OR_EMPTY,
//    FLOW_STATE_PRIME_FAIL_OR_EMPTY,
//    FLOW_STATE_EMPTY
    switch (dispenser_state)
    {
    case DISPENSER_STATE_WARNING_PRIMING:
    {
        if (getDispenseStatus() == FLOW_STATE_EMPTY)
        {
            dispenser_state = DISPENSER_STATE_PROBLEM_EMPTY;
        }
        if (getDispenseStatus() == FLOW_STATE_PRIME_FAIL_OR_EMPTY)
        {
            dispenser_state = DISPENSER_STATE_PROBLEM_NEEDS_ATTENTION;
        }
       
        if (getDispenseStatus() == FLOW_STATE_NOT_PUMPING_NOT_DISPENSING)
        {
            dispenser_state = DISPENSER_STATE_AVAILABLE;
        }
       
        if (getDispenseStatus() == FLOW_STATE_DISPENSING)
        {
            dispenser_state = DISPENSER_STATE_AVAILABLE;
        }
        break;
    }
    case DISPENSER_STATE_AVAILABLE:
    {

        if (getDispenseStatus() == FLOW_STATE_PRIMING_OR_EMPTY){
            dispenser_state = DISPENSER_STATE_WARNING_PRIMING;
        }
        if (getDispenseStatus() == FLOW_STATE_EMPTY)
        {
            dispenser_state = DISPENSER_STATE_PROBLEM_EMPTY;
        }
        if (getDispenseStatus() == FLOW_STATE_PRIME_FAIL_OR_EMPTY)
        {
            dispenser_state = DISPENSER_STATE_PROBLEM_NEEDS_ATTENTION;
        }
       
        break;
    }
    case DISPENSER_STATE_AVAILABLE_LOW_STOCK:
    {
        if (getDispenseStatus() == FLOW_STATE_EMPTY)
        {
            dispenser_state = DISPENSER_STATE_PROBLEM_EMPTY;
        }
        if (getDispenseStatus() == FLOW_STATE_PRIME_FAIL_OR_EMPTY)
        {
            dispenser_state = DISPENSER_STATE_PROBLEM_NEEDS_ATTENTION;
        }
        break;
    }
    case DISPENSER_STATE_PROBLEM_NEEDS_ATTENTION:
    {
        if (getDispenseStatus() == FLOW_STATE_DISPENSING)
        {
            dispenser_state = DISPENSER_STATE_AVAILABLE;
        }
        break;
    }
    case DISPENSER_STATE_PROBLEM_EMPTY:
    {
        if (getDispenseStatus() == FLOW_STATE_DISPENSING)
        {
            dispenser_state = DISPENSER_STATE_AVAILABLE;
        }
        break;
    }
    case DISPENSER_STATE_DISABLED_COMING_SOON:
    {
        break;
    }
    case DISPENSER_STATE_DISABLED:
    {
        // do nothing can only be altered when set to enabled
        break;
    }

    default:
    {
        debugOutput::sendMessage("Erroneous dispenser state: " + to_string(dispenser_state), MSG_INFO);
    }
    }
}

Dispense_behaviour dispenser::getDispenseStatus()
{

    // logUpdateIfAllowed("Button press time [ms]. Total:" + to_string(dispense_button_total_pressed_millis) + " Current:" + to_string(dispense_button_current_press_millis));
    using namespace std::chrono;
    uint64_t millis_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    uint64_t dispense_time_millis = millis_since_epoch - dispense_start_timestamp_epoch;
    Time_val avg = getAveragedFlowRate(EMPTY_CONTAINER_DETECTION_FLOW_AVERAGE_WINDOW_MILLIS);

    // restricted status update
    // logUpdateIfAllowed("Dispense flowRate " + to_string(EMPTY_CONTAINER_DETECTION_FLOW_AVERAGE_WINDOW_MILLIS) + "ms avg [ml/s]: " + to_string(avg.value) + " Dispense state time: " + to_string(dispense_time_millis));

    return dispense_state;
}

void dispenser::updateDispenseStatus()
{
    Time_val avg = getAveragedFlowRate(EMPTY_CONTAINER_DETECTION_FLOW_AVERAGE_WINDOW_MILLIS);

    // if (dispense_state == FLOW_STATE_PRIME_FAIL_OR_EMPTY && avg.value < getProduct()->getThresholdFlow())
    // {
    //     // once failed, stay failed until proven otherwise by having a good flow.
    //     dispense_state = FLOW_STATE_PRIME_FAIL_OR_EMPTY;
    // }
    // else if (dispense_state == FLOW_STATE_EMPTY && avg.value < getProduct()->getThresholdFlow())
    // {
    //     // once empty, stay empty until proven otherwise by having a good flow.
    //     dispense_state = FLOW_STATE_EMPTY;
    // }
    // else

    if (!getDispenseButtonValue())
    {
        dispense_state = FLOW_STATE_NOT_PUMPING_NOT_DISPENSING;
    }
    else if ((getButtonPressedCurrentPressMillis() < EMPTY_CONTAINER_DETECTION_FLOW_AVERAGE_WINDOW_MILLIS) && avg.value < getProduct()->getThresholdFlow())
    {
        // flow rate needs to be ramped up until stable.
        dispense_state = FLOW_STATE_RAMP_UP;
    }
    else if (avg.value >= getProduct()->getThresholdFlow())
    {
        // button pressed (aka pumping)
        // init time long enough for valid data
        dispense_state = FLOW_STATE_DISPENSING;
    }

    else if (previous_dispense_state == FLOW_STATE_DISPENSING)
    {
        // button pressed (aka pumping)
        // init time long enough for valid data
        // no flow detected

        // once it was dispensing, empty dispenser is detected immediatly if no product flows.
        // bugfix: if the button was release and repressed, the average was not correct at restart
        //          --> take into account. at top level (FLOW_STATE_UNAVAILABLE)
        dispense_state = FLOW_STATE_EMPTY;
    }
    else if (getButtonPressedTotalMillis() > EMPTY_CONTAINER_DETECTION_MAXIMUM_PRIME_TIME_MILLIS)
    {
        // button pressed (aka pumping)
        // init time long enough for valid data
        // no flow detected
        // previous state was not dispensing

        // pump
        dispense_state = FLOW_STATE_PRIME_FAIL_OR_EMPTY;
    }
    else
    {
        // button pressed (aka pumping)
        // init time long enough for valid data
        // no flow detected
        // previous state was not dispensing
        // pumping time has exceeded set value

        dispense_state = FLOW_STATE_PRIMING_OR_EMPTY;
    }

    previous_dispense_state = dispense_state;
}
