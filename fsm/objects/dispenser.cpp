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
// by:Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************
#include "dispenser.h"
#include <chrono>
#include <sstream>
// #include "machine.h"

#define ACTIVATION_TIME 5
#define TEST_ACTIVATION_TIME 3
#define PRIME_PUMP_TIME 1
#define CLEAN_WATER_TIME 1
#define CLEAN_AIR_TIME 1

#define MAX_BUF 64

// pcb *dispenser::m_pcb = nullptr;

const char *DISPENSE_BEHAVIOUR_STRINGS[] = {
    "FLOW_STATE_UNAVAILABLE",
    "FLOW_STATE_RAMP_UP",
    "FLOW_STATE_DISPENSING",
    "FLOW_STATE_PUMPING_NOT_DISPENSING",
    "FLOW_STATE_NOT_PUMPING_NOT_DISPENSING",
    "FLOW_STATE_PRIMING_OR_EMPTY",
    "FLOW_STATE_PRIME_FAIL_OR_EMPTY",
    "FLOW_STATE_EMPTY"};

const char *SLOT_STATE_STRINGS[] = {
    "SLOT_STATE_AVAILABLE",
    "SLOT_STATE_AVAILABLE_LOW_STOCK",
    "SLOT_STATE_WARNING_PRIMING",
    "SLOT_STATE_PROBLEM_NEEDS_ATTENTION",
    "SLOT_STATE_PROBLEM_EMPTY",
    "SLOT_STATE_DISABLED_COMING_SOON",
    "SLOT_STATE_DISABLED"};

void dispenser::parseIntCsvString(const std::string &csvString, int *intArray, int &size) //
{
    std::stringstream ss(csvString);
    std::string token;
    size = 0;

    while (std::getline(ss, token, ','))
    {
        if (size < DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX)
        {
            intArray[size++] = std::stoi(token);
        }
        else
        {
            debugOutput::sendMessage("ERROR: Array size exceeded maximum limit (will stop adding):  " + std::to_string(size), MSG_ERROR);
        }
    }
}
void dispenser::parseDoubleCsvString(const std::string &csvString, double *doubleArray, int &size)
{
    std::stringstream ss(csvString);
    std::string token;
    size = 0;

    while (std::getline(ss, token, ','))
    {
        if (size < DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX)
        {
            doubleArray[size++] = std::stod(token);
        }
        else
        {
            debugOutput::sendMessage("ERROR: Array size exceeded maximum limit (will stop adding):  " + std::to_string(size), MSG_ERROR);
        }
    }
}

dispenser::dispenser()
{
    // default constructor to set all pin to nullptr
    // debugOutput::sendMessage("dispenser", MSG_INFO);

    // TODO: Need to build Product Object reference
    // m_selected_pnumber = nullptr;

    // If we haven't instantiated and initialized the hardware yet we
    // do it here.  Note that the pointer is declared as static so we
    // will only ever create one pcb() class no matter how many
    // times we create a dispenser() class.
    // if (m_pcb == nullptr)
    // {
    //     m_pcb = new pcb();
    // }
    dispense_state = FLOW_STATE_UNAVAILABLE;
    previous_dispense_state = FLOW_STATE_UNAVAILABLE;
}

dispenser::~dispenser()
{
    debugOutput::sendMessage("~dispenser", MSG_INFO);

    delete m_pcb;
    m_pcb = nullptr;

    using namespace std::chrono;
    previous_status_update_allowed_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

DF_ERROR dispenser::setup(pcb *pcb, product *pnumbers)
// DF_ERROR dispenser::setup(machine *machine, product *pnumbers)
{
    // Set the pump PWM value to a nominal value
    // m_machine = machine;
    m_pnumbers = pnumbers;
    m_pcb = pcb;
    // m_pcb = m_machine->getPcb();

    m_pcb->setPumpPWM(DEFAULT_PUMP_PWM);

    // m_pFlowsensor[NUM_FLOWSENSOR] = nullptr;

    millisAtLastCheck = MILLIS_INIT_DUMMY;
    previousDispensedVolume = 0;
    isPumpSoftStarting = false;
    pwm_actual_set_speed = 0;
    // resetDispenserVolumeDispensed();
    resetActiveProductVolumeDispensed();
    resetSelectedProductVolumeDispensed();
}

void dispenser::refresh()
{

    // m_pcb->pcb_refresh(); // do pcb refresh in machine class.

    // periodical polling refresh  (because the dispense button has no interrupt trigger (i2c))
    // bool egde = dispenseButtonValueMemory != getDispenseButtonValue();
    // dispenseButtonValueEdgePositive = getDispenseButtonValue() && egde;
    // dispenseButtonValueEdgeNegative = (!getDispenseButtonValue()) && egde;
    // dispenseButtonValueMemory = getDispenseButtonValue();

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

// void dispenser::setAllDispenseButtonLightsOff()
// {
//     for (int slot = 1; slot < 5; slot++)
//     {
//         debugOutput::sendMessage("slot light off: " + to_string(slot), MSG_INFO);
//         setMultiDispenseButtonLight(slot, false);
//     }
// }

bool dispenser::isSlotEnabled()
{
    return m_isSlotEnabled;
}
DF_ERROR dispenser::setSlot(int slot)
{
    // first slot is 1
    m_slot = slot;
}

int dispenser::getSlot()
{
    // first slot is 1
    return m_slot;
}

product *dispenser::getSelectedProduct()
{
    return &m_pnumbers[m_selected_pnumber];
}

void dispenser::setAdditiveFromPositionAsSingleDispenseSelectedProduct(int position)
{
    int pnumber = getAdditivePNumber(position);
    setPNumberAsSingleDispenseSelectedProduct(pnumber);
    debugOutput::sendMessage("Dispenser: Set Additive PNumber as selected product: " + std::to_string(pnumber), MSG_WARNING);
}

void dispenser::setBasePNumberAsSingleDispenseSelectedProduct()
{
    setPNumberAsSingleDispenseSelectedProduct(getBasePNumber());
    debugOutput::sendMessage("Dispenser: Set Base PNumber as selected product: " + std::to_string(getBasePNumber()), MSG_WARNING);
}

void dispenser::setPNumberAsSingleDispenseSelectedProduct(int pnumber)
{
    setSelectedProduct(pnumber);
    setActiveProduct(pnumber);
}

int dispenser::getMixPNumberFromMixIndex(int mixIndex)
{
    if (mixIndex >= m_custom_mix_pnumbers_count)
    {
        debugOutput::sendMessage("Dispenser: ASSERT ERROR, mixing does not contain this index: " + std::to_string(mixIndex), MSG_ERROR);
        return DUMMY_PNUMBER;
    }
    return m_custom_mix_pnumbers[mixIndex];
}
void dispenser::setCustomMixParametersAsSelectedProduct(string pnumbers, string pnumberRatios)
{
    int custom_mix_pnumbers[DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX];
    double custom_mix_ratios[DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX];
    int custom_mix_pnumbers_count;
    int custom_mix_ratios_count;

    dispenser::parseIntCsvString(pnumbers, custom_mix_pnumbers, custom_mix_pnumbers_count);
    dispenser::parseDoubleCsvString(pnumberRatios, custom_mix_ratios, custom_mix_ratios_count);

    if (custom_mix_pnumbers_count != custom_mix_ratios_count)
    {
        debugOutput::sendMessage("ASSERT ERROR: Number of mix pnumbers is not equal to number of mixing ratios!", MSG_ERROR);
        return;
    }

    int pnumber;
    m_custom_mix_pnumbers_count = 0;
    m_custom_mix_ratios_count = 0;
    for (int i = 0; i < custom_mix_pnumbers_count; i++)
    {
        pnumber = custom_mix_pnumbers[i];
        if (isPNumberValidInThisDispenser(pnumber, true))
        {

            m_custom_mix_pnumbers[m_custom_mix_pnumbers_count] = custom_mix_pnumbers[i];
            m_custom_mix_ratios[m_custom_mix_pnumbers_count] = custom_mix_ratios[i];

            debugOutput::sendMessage("Pnumber at position: " + std::to_string(i) + " P-" + std::to_string(m_custom_mix_pnumbers[m_custom_mix_pnumbers_count]) + " ratio: " + std::to_string(m_custom_mix_ratios[m_custom_mix_pnumbers_count]), MSG_INFO);

            m_custom_mix_pnumbers_count++;
            m_custom_mix_ratios_count++;
        }
    }

    debugOutput::sendMessage("Dispenser: Set active product as mixing product: (TODO: for now, will only set first component as single dispense product) ", MSG_WARNING);
}

bool dispenser::isPNumberValidInThisDispenser(int pnumber, bool mustBeAdditiveOrBase)
{
    bool isValid = false;
    if (getBasePNumber() == pnumber)
    {
        isValid = true;
    }

    for (int i = 0; i < m_additive_pnumbers_count; i++)
    {
        if (m_additive_pnumbers[i] == pnumber)
        {
            isValid = true;
        }
    }

    if (!mustBeAdditiveOrBase)
    {
        // mixes are products, made up of other products.
        for (int i = 0; i < m_dispense_pnumbers_count; i++)
        {
            if (m_dispense_pnumbers[i] == pnumber)
            {
                isValid = true;
            }
        }
    }

    if (!isValid)
    {
        debugOutput::sendMessage("ASSERT ERROR: invalid pnumber as selected pnumber in dispenser: " + std::to_string(pnumber), MSG_ERROR);
    }
    return isValid;
}

bool dispenser::setSelectedProduct(int pnumber)
{
    // WARNING: this is about dispensePNumbers! use active PNumber for the pnumber that's actually being dispensed.

    // to do check if number is available.
    if (!isPNumberValidInThisDispenser(pnumber, false))
    {
        return false;
    }

    m_selected_pnumber = pnumber;

    return true;
}

void dispenser::setActiveProduct(int pnumber)
{
    if (!isPNumberValidInThisDispenser(pnumber, true))
    {
        debugOutput::sendMessage("Dispenser: ASSERT ERROR Attempt to set non dispensable product as Active product to: " + to_string(pnumber), MSG_ERROR);
    }
    debugOutput::sendMessage("Dispenser: (product that will come out of spout=active product) Set Active product to: P-" + to_string(pnumber), MSG_INFO);
    m_active_pnumber = pnumber;
}

void dispenser::setSelectedSizeAsChar(char size)
{
    m_selectedSizeAsChar = size;
};

char dispenser::getSelectedSizeAsChar()
{
    return m_selectedSizeAsChar;
}

double dispenser::getSelectedSizeAsVolume()
{
    return getSelectedProduct()->getTargetVolume();
}

product *dispenser::getProductFromPNumber(int pnumber)
{
    return &m_pnumbers[m_active_pnumber];
}

product *dispenser::getActiveProduct()
{
    // set the product that will actually be dispensed. (base or additive)
    // for now, just base product.
    // active products cannot be dispenseProducts (if the dispense product is not the same as a base or additive) e.g. ginger kombucha is not dispensable, its components (base kombucha, addtives: suger, ginger flavor) are.
    return &m_pnumbers[m_active_pnumber];
}

DF_ERROR dispenser::loadGeneralProperties()
{
    debugOutput::sendMessage("Dispenser: Load general properties:", MSG_INFO);
    // ******* Sleep time between DB calls solved inconsistend readings from db!!!****
    usleep(20000);
    loadDispenserParametersFromDb();
    usleep(20000);
    analyseSlotState();
    usleep(20000);
}

int dispenser::getSelectedPNumber()
{
    return m_selected_pnumber;
}

int dispenser::getActivePNumber()
{
    return m_active_pnumber;
}

int dispenser::getBasePNumber()
{
    return m_base_pnumber;
}

int dispenser::getAdditivePNumber(int position)
{
    // starts from 1
    // additive 1, additive 2, ...
    if (position == 0)
    {
        debugOutput::sendMessage("ASSERT ERROR Dispenser: Additive position cannot be zero. Starts from 1.", MSG_ERROR);
        return PRODUCT_DUMMY;
    }
    if (position > m_additive_pnumbers_count)
    {
        debugOutput::sendMessage("ASSERT ERROR Dispenser: Additive number not existing. Requested:" + std::to_string(position) + " Max: " + std::to_string(m_additive_pnumbers_count), MSG_ERROR);
        return PRODUCT_DUMMY;
    }

    return m_additive_pnumbers[position - 1];
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

bool dispenser::loadDispenserParametersFromDb()
{
    int rc = sqlite3_open(CONFIG_DB_PATH, &db);
    sqlite3_stmt *stmt;
    string sql_string = "SELECT "
                        "slot_id,"
                        "dispense_pnumbers,"
                        "base_pnumber,"
                        "additive_pnumbers,"
                        "is_enabled,"
                        "status_text"
                        " FROM slots WHERE slot_id=" +
                        std::to_string(getSlot()) + ";";

    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);

    int status;
    status = sqlite3_step(stmt);

    int numberOfRecordsFound = 0;

    string base_pnumber_str;
    string dispense_numbers_str;
    string additive_pnumbers_str;
    debugOutput::sendMessage("Dispenser load from db. Status : " + std::to_string(status) + " sql: " + sql_string, MSG_INFO);
    while (status == SQLITE_ROW)
    {
        numberOfRecordsFound++;

        debugOutput::sendMessage("Record found for slot : " + std::to_string(getSlot()), MSG_INFO);

        dispense_numbers_str = product::dbFieldAsValidString(stmt, 1);
        base_pnumber_str = product::dbFieldAsValidString(stmt, 2);
        additive_pnumbers_str = product::dbFieldAsValidString(stmt, 3);
        m_is_slot_enabled = sqlite3_column_int(stmt, 4);
        m_status_text = product::dbFieldAsValidString(stmt, 5);

        status = sqlite3_step(stmt); // next record
        // every sqlite3_step returns a row. if it returns 0, it's run over all the rows.
    }
    //  debugOutput::sendMessage("Record loaded for slot : " + std::to_string(getSlot()), MSG_INFO);
    bool slot_loaded_from_db = false;
    if (numberOfRecordsFound == 1)
    {
        slot_loaded_from_db = true;

        // int ;
        //       int* ;
        //       int m_additive_pnumbers_count;
        //       int* m_additive_pnumbers;

        dispenser::parseIntCsvString(dispense_numbers_str, m_dispense_pnumbers, m_dispense_pnumbers_count);
        dispenser::parseIntCsvString(dispense_numbers_str, m_additive_pnumbers, m_additive_pnumbers_count);

        // debugOutput::sendMessage("feijwefijweifwejif : " + base_pnumber_str, MSG_INFO);
        if (base_pnumber_str.empty())
        {
            // first dispense number in the list as base pnumber if base pnumber is empty.
            // this is handy if we have a basic dispenser, like with soap, that only dispenses pure products. No mixings. No need to then have the same pnumber as base_pnumber
            m_base_pnumber = m_dispense_pnumbers[0];
        }

        debugOutput::sendMessage("INFO: loaded slot: " + std::to_string(m_slot) + " Base pnumber: " + std::to_string(m_base_pnumber), MSG_INFO);
    }
    else if (numberOfRecordsFound > 1)
    {
        // multiple records found
        debugOutput::sendMessage("ERROR: Multiple db records found for slot: " + std::to_string(m_slot), MSG_ERROR);
    }
    else
    {
        // no records found

        // debugOutput::sendMessage("No db record for product: " + std::to_string(m_pnumber), MSG_INFO);
        // debugOutput::sendMessage("no records for: " + sql_string, MSG_INFO);
    }

    if (!slot_loaded_from_db)
    {
        debugOutput::sendMessage("ERROR: Slot NOT loaded from db. slot: " + std::to_string(m_slot), MSG_ERROR);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return true;
}

///////////////////////////////////////////////////////////////////////////
// dispense commands

DF_ERROR dispenser::initSelectedProductDispense(char size, double nPrice)
{

    // Dispensing a mix includes the base and additive pnumbers. Start with additives, base last.
    // needs a state machine to control all this.

    DF_ERROR dfRet = OK;
    getSelectedProduct()->setTargetVolumeFromSize(size);

    m_dispenserVolumeTarget = getSelectedProduct()->getTargetVolume();

    m_price = nPrice;

    // resetDispenserVolumeDispensed();
    resetSelectedProductVolumeDispensed();

    switch (m_pcb->get_pcb_version())
    {

    case (pcb::PcbVersion::DSED8344_NO_PIC):
    {
        // setMultiDispenseButtonLight(getSlot(), true);
    }

    break;
    case pcb::PcbVersion::DSED8344_PIC_MULTIBUTTON:
    {
        // if (m_machine->getMultiDispenseButtonEnabled())
        // {
        //     setMultiDispenseButtonLight(getSlot(), true);
        // }
        // else
        // {
        //     setMultiDispenseButtonLight(1, true);
        // }
        // setPumpEnable(); // Added at time of EN-134 integration. Why did things work earlier onwards?
    }
    break;
    case (pcb::PcbVersion::EN134_4SLOTS):
    case (pcb::PcbVersion::EN134_8SLOTS):
    {

        m_pcb->sendEN134DefaultConfigurationToPCA9534(getSlot(), true);
        m_pcb->setSingleDispenseButtonLight(getSlot(), true);
    }
    break;
    case (pcb::PcbVersion::EN258_4SLOTS):
    case (pcb::PcbVersion::EN258_8SLOTS):
    {
        m_pcb->setSingleDispenseButtonLight(getSlot(), true);
    }
    break;
    default:
    {
        debugOutput::sendMessage("Pcb not comptible. ", MSG_WARNING);
    }
    break;
    }

    initActivePNumberDispense(getSelectedProduct()->getTargetVolume());
}

DF_ERROR dispenser::startSelectedProductDispense()
{
    debugOutput::sendMessage("Dispenser: Start Selected PNumber dispense at slot " + to_string(this->m_slot), MSG_INFO);
    using namespace std::chrono;
    dispense_start_timestamp_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    startActivePNumberDispense(); // hack for now

    dispenseButtonTimingreset();

    // this->m_pcb->flowSensorEnable(m_slot);
    // // this->m_pcb->resetFlowSensorPulsesForDispenser(m_slot);

    // // init state
    // dispense_state = FLOW_STATE_NOT_PUMPING_NOT_DISPENSING;
    // previous_dispense_state = FLOW_STATE_UNAVAILABLE; // hack needed to create edge

    // initProductFlowRateCalculation();

    // DF_ERROR e_ret = ERROR_MECH_PRODUCT_FAULT;
    // return e_ret = OK;

    // Set Start Time
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(m_nStartTime, 50, "%F %T", timeinfo);
}

DF_ERROR dispenser::stopSelectedProductDispense()
{
    debugOutput::sendMessage("Dispenser: Stop selected PNumber dispense ", MSG_INFO);
    stopActivePNumberDispense(); // hack for now.

    // Set End time
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(m_nEndTime, 50, "%F %T", timeinfo);
}

string dispenser::getSelectedProductDispenseStartTime()
{
    return m_nStartTime;
}

string dispenser::getSelectedProductDispenseEndTime()
{
    return m_nEndTime;
}

DF_ERROR dispenser::initActivePNumberDispense(double volume)
{
    DF_ERROR dfRet = OK;
    getActiveProduct()->setTargetVolume(volume);

    resetActiveProductVolumeDispensed();

    switch (m_pcb->get_pcb_version())
    {

    case (pcb::PcbVersion::DSED8344_NO_PIC):
    {
        // setMultiDispenseButtonLight(getSlot(), true);
    }
    break;
    case pcb::PcbVersion::DSED8344_PIC_MULTIBUTTON:
    {
        // if (m_machine->getMultiDispenseButtonEnabled())
        // {
        //     setMultiDispenseButtonLight(getSlot(), true);
        // }
        // else
        // {
        //     setMultiDispenseButtonLight(1, true);
        // }
        // setPumpEnable(); // Added at time of EN-134 integration. Why did things work earlier onwards?
    }
    break;
    case (pcb::PcbVersion::EN134_4SLOTS):
    case (pcb::PcbVersion::EN134_8SLOTS):
    {

        setPumpEnable();
    }
    break;
    case (pcb::PcbVersion::EN258_4SLOTS):
    case (pcb::PcbVersion::EN258_8SLOTS):
    {

        setPumpEnable();
    }
    break;
    default:
    {
        debugOutput::sendMessage("Pcb not comptible. ", MSG_WARNING);
    }
    break;
    }

    return dfRet;
}

DF_ERROR dispenser::startActivePNumberDispense()
{
    debugOutput::sendMessage("Dispenser: Start Active PNumber at slot " + to_string(this->m_slot), MSG_INFO);

    // dispenseButtonTimingreset();

    this->m_pcb->flowSensorEnable(m_slot);

    setActiveProductSolenoid(true);
    // this->m_pcb->resetFlowSensorPulsesForDispenser(m_slot);

    // init state
    dispense_state = FLOW_STATE_NOT_PUMPING_NOT_DISPENSING;
    previous_dispense_state = FLOW_STATE_UNAVAILABLE; // hack needed to create edge

    initProductFlowRateCalculation();

    DF_ERROR e_ret = ERROR_MECH_PRODUCT_FAULT;
    return e_ret = OK;
}

DF_ERROR dispenser::stopActivePNumberDispense()
{
    debugOutput::sendMessage("Dispenser: Stop Active PNumber  dispense ", MSG_INFO);

    setActiveProductSolenoid(false);
    m_pcb->setDispenseButtonLightsAllOff();

    m_pcb->flowSensorsDisableAll();
}

/////////////////////////////////////////////////////////////////////////
// dispenser volume

void dispenser::linkActiveProductVolumeUpdate()
{
    product *active_product = getActiveProduct();
    auto lambdaFunc = [active_product]()
    { active_product->registerFlowSensorTickFromPcb(); };
    m_pcb->registerFlowSensorTickCallback(getSlot(), lambdaFunc);
}

// double dispenser::getDispenserVolumeDispensed()
// {
//     // return m_pcb->getFlowSensorPulsesForDispenser(m_slot) // this justreturns ticks. not volume. should be an addition of all the volumes already dispensed.;
//     return m_dispenser_volume_dispensed;
// }
// REPLACE BY SelectedProductTargetVolume
// void dispenser::resetDispenserVolumeDispensed()
// {
//     m_dispenser_volume_dispensed = 0;
//     // return m_pcb->resetFlowSensorPulsesForDispenser(m_slot);
// }

// bool dispenser::isDispenserVolumeTargetReached()
// {
//     return m_dispenserVolumeTarget <= getDispenserVolumeDispensed();
// }

/////////////////////////////////////////////////////////////////////////
// active product volume
bool dispenser::isActiveProductVolumeTargetReached()
{
    return isProductVolumeTargetReached(getActivePNumber());
}

void dispenser::resetActiveProductVolumeDispensed()
{
    resetProductVolumeDispensed(getActivePNumber());
}

void dispenser::subtractActiveFromProductVolumeDispensed(double volume_to_distract)
{
    subtractFromProductVolumeDispensed(getActivePNumber(), volume_to_distract);
}

double dispenser::getActiveProductVolumeRemaining()
{
    return getProductVolumeRemaining(getActivePNumber());
}

double dispenser::getActiveProductVolumeDispensed()
{
    // return m_nVolumeDispensed;
    return getProductVolumeDispensed(getActivePNumber());
}

/////////////////////////////////////////////////////////////////////////
// selected product volume
bool dispenser::isSelectedProductVolumeTargetReached()
{
    return isProductVolumeTargetReached(getSelectedPNumber());
}

void dispenser::resetSelectedProductVolumeDispensed()
{
    resetProductVolumeDispensed(getSelectedPNumber());
}

void dispenser::subtractSelectedFromProductVolumeDispensed(double volume_to_distract)
{
    subtractFromProductVolumeDispensed(getSelectedPNumber(), volume_to_distract);
}

double dispenser::getSelectedProductVolumeRemaining()
{
    return getProductVolumeRemaining(getSelectedPNumber());
}

double dispenser::getSelectedProductVolumeDispensed()
{
    // return m_nVolumeDispensed;
    return getProductVolumeDispensed(getSelectedPNumber());
}

/////////////////////////////////////////////////////////////////////////
// product volume

double dispenser::getProductTargetVolume(int pnumber)
{
    return getProductFromPNumber(pnumber)->getTargetVolume();
}

bool dispenser::isProductVolumeTargetReached(int pnumber)
{

    // check pnumber part of dispenser.

    bool bRet = false;

    if (getProductTargetVolume(pnumber) <= getProductVolumeDispensed(pnumber))
    {
        bRet = true;
    }
    return bRet;
}

void dispenser::resetProductVolumeDispensed(int pnumber)
{
    getProductFromPNumber(pnumber)->resetVolumeDispensed();
}

void dispenser::subtractFromProductVolumeDispensed(int pnumber, double volume_to_distract)
{
    double volume = getProductVolumeDispensed(pnumber);
    getProductFromPNumber(pnumber)->setVolumeDispensed(volume - volume_to_distract);
}

double dispenser::getProductVolumeRemaining(int pnumber)
{
    return getProductFromPNumber(pnumber)->getVolumeRemaining();
}

double dispenser::getProductVolumeDispensed(int pnumber)
{
    // return m_nVolumeDispensed;
    return getProductFromPNumber(pnumber)->getVolumeDispensed();
}

////////////////////////////////////////////////////////////////////////////
// I/O Button

void dispenser::setDispenseButtonLight(bool onElseOff)
{
    switch (m_pcb->get_pcb_version())
    {

    case (pcb::PcbVersion::DSED8344_NO_PIC):
    {
        debugOutput::sendMessage("No dispense button light. Deprecated ", MSG_WARNING);
    }

    break;
    case pcb::PcbVersion::DSED8344_PIC_MULTIBUTTON:
    {
        debugOutput::sendMessage("No dispense button light. Deprecated ", MSG_WARNING);
    }
    break;
    case (pcb::PcbVersion::EN134_4SLOTS):
    case (pcb::PcbVersion::EN134_8SLOTS):
    {

        m_pcb->setSingleDispenseButtonLight(getSlot(), onElseOff);
    }
    break;
    case (pcb::PcbVersion::EN258_4SLOTS):
    case (pcb::PcbVersion::EN258_8SLOTS):
    {

        m_pcb->setSingleDispenseButtonLight(getSlot(), onElseOff);
    }
    break;
    default:
    {
        debugOutput::sendMessage("Pcb not comptible. ", MSG_WARNING);
    }
    break;
    }
}

bool dispenser::getDispenseButtonEdgeNegative()
{
    // return dispenseButtonValueEdgeNegative;
    return m_pcb->getDispenseButtonEdgeNegative(m_slot);
}

bool dispenser::getDispenseButtonEdgePositive()
{
    // return dispenseButtonValueEdgePositive;
    return m_pcb->getDispenseButtonEdgePositive(m_slot);
}

bool dispenser::getDispenseButtonValue()
{
    m_pcb->getDispenseButtonStateDebounced(m_slot);
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

////////////////////////////////////////////////////////////////////////////
// I/O Solenoid

void dispenser::setProductSolenoid(int pnumber, bool openElseClosed)
{
    bool found =false;
    if (pnumber == getBasePNumber()){
        m_pcb->setBaseSolenoid(this->m_slot, openElseClosed);
        found = true;
        return;
    }

    int additivePosition = 1;
    while(additivePosition <= ADDITIVES_PER_SLOT_COUNT_MAX && !found){
        if(pnumber == getAdditivePNumber(additivePosition)){
            m_pcb->setAdditiveSolenoid(this->m_slot, additivePosition, openElseClosed);
            found = true;
        }
        additivePosition++;
    }

    if(!found){
        debugOutput::sendMessage("Dispenser: Solenoid not found!!!!", MSG_ERROR);
    }
    

}

void dispenser::setActiveProductSolenoid(bool openElseClosed)
{
    setProductSolenoid(getActivePNumber(), openElseClosed);
}

void dispenser::setSpoutSolenoid(bool openElseClosed)
{
    m_pcb->setSpoutSolenoid(this->m_slot, openElseClosed);
}

////////////////////////////////////////////////////////////////////////////
// I/O Pump

// TODO: Refactor Pumping with switch and ternary in future...keep seperate for ease of testing.
// Reverse pump: Turn forward pin HIGH - Reverse pin LOW
DF_ERROR dispenser::setPumpDirectionForward()
{
    debugOutput::sendMessage("Pump direction: Forward.", MSG_INFO);
    m_pcb->setPumpDirection(this->m_slot, true);
}

// Reverse pump: Turn forward pin LOW - Reverse pin HIGH
DF_ERROR dispenser::setPumpDirectionReverse()
{
    DF_ERROR e_ret = OK;
    if (!m_isPumpReversalEnabled)
    {
        debugOutput::sendMessage("Pump reversal not allowed. Will not execute command.", MSG_WARNING);

        return e_ret;
    }
    debugOutput::sendMessage("Pump direction: reverse", MSG_INFO);
    m_pcb->setPumpDirection(this->m_slot, false);
}

// Stops pumping: Turn forward pin LOW - Reverse pin LOW
DF_ERROR dispenser::setPumpsDisableAll()
{
    // m_machine->pcb24VPowerSwitch(false);
    debugOutput::sendMessage("Pump disable: all.", MSG_INFO);
    m_pcb->setPumpsDisableAll();
    m_isSlotEnabled = false;
    isPumpSoftStarting = false;
    pwm_actual_set_speed = 0;
}

void dispenser::reversePumpForSetTimeMillis(int millis)
{
    if (m_isPumpReversalEnabled)
    {
        if (millis > 0)
        {
            // get volume before
            double volume_before = getActiveProductVolumeDispensed();

            debugOutput::sendMessage("Pump auto retraction. Reverse time millis: " + to_string(millis), MSG_INFO);
            pumpSlowStart(false);

            // m_pcb->virtualButtonPressHack(this->m_slot);
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
            // m_pcb->virtualButtonUnpressHack(this->m_slot); // needed! To have the button pressing sequence right. (fake button presses can mess the button detection up)

            pumpSlowStopBlocking();

            // setPumpsDisableAll();
            setPumpDirectionForward();

            // get volume after
            double volume_after = getActiveProductVolumeDispensed();

            // vol diff
            double volume_diff = volume_after - volume_before;

            subtractFromProductVolumeDispensed(getActivePNumber(), volume_diff);
            debugOutput::sendMessage("Retraction done. WARNING: check volume change correction subtraction. Volume reversed:  " + to_string(volume_diff), MSG_INFO);
        }
        else
        {
            debugOutput::sendMessage("Rectraction disabled. ms:" + to_string(millis), MSG_INFO);
        }
    }
}

DF_ERROR dispenser::pumpSlowStartHandler()
{

    if (isPumpSoftStarting)
    {
        uint8_t target_pwm = (uint8_t)(getSelectedProduct()->getPWM());

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

void dispenser::setPumpReversalEnabled(bool isEnabled)
{
    m_isPumpReversalEnabled = isEnabled;
};
void dispenser::setPumpSlowStartStopEnabled(bool isEnabled)
{
    m_isPumpSlowStartStopEnabled = isEnabled;
}

DF_ERROR dispenser::pumpSlowStart(bool forwardElseReverse)
{

    if (!forwardElseReverse && !m_isPumpReversalEnabled)
    {
        debugOutput::sendMessage("Pump reversal not allowed. Will not execute command.", MSG_WARNING);
        DF_ERROR e_ret = OK;

        return e_ret;
    }

    // // set set speed to zero, as there is no real speed feedback, we can't guarantee a claimed initial set speed to be true

    using namespace std::chrono;
    slowStartMostRecentIncreaseEpoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    uint8_t target_pwm = (uint8_t)(getSelectedProduct()->getPWM());

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
    if (SLOW_START_INCREASE_PERIOD_MILLIS == 0 || !m_isPumpSlowStartStopEnabled)
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
        if (SLOW_STOP_PERIOD_MILLIS == 0 || !m_isPumpSlowStartStopEnabled)
        {
            debugOutput::sendMessage("Pump instant stop", MSG_INFO);
            // no slow stop
        }
        else
        {
            debugOutput::sendMessage("Pump slow stop. from " + to_string(pwm_actual_set_speed) + " to " + to_string(0), MSG_INFO);
            // m_pcb->virtualButtonPressHack(this->m_slot);

            for (int i = pwm_actual_set_speed; i >= 0; --i)
            {
                setPumpPWM(i, false);
                usleep(SLOW_STOP_PERIOD_MILLIS * 1000); // one second ramp up to full speed --> 255 steps ==> 4ms per step.
            }
            // m_pcb->virtualButtonUnpressHack(this->m_slot);
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
    debugOutput::sendMessage("Pump enable position: " + to_string(this->m_slot), MSG_INFO);
    // m_machine->pcb24VPowerSwitch(true);
    m_pcb->setPumpEnable(this->m_slot); // pump 1 to 4
    m_isSlotEnabled = true;
}

DF_ERROR dispenser::setPumpPWM(uint8_t value, bool enableLog)
{
    if (enableLog)
    {
        debugOutput::sendMessage("Pump set speed. PWM [0..255]: " + to_string(value), MSG_INFO);
    }
    m_pcb->setPumpPWM((unsigned char)value);
}

unsigned short dispenser::getPumpSpeed()
{
    m_pcb->getPumpPWM();
}

////////////////////////////////////////////////////////////
// I/O Flow Sensor

// TODO: Call this function on Dispense onEntry()
DF_ERROR dispenser::initGlobalFlowsensorIO(int pin)
{
    DF_ERROR e_ret = ERROR_BAD_PARAMS;

    // *m_pIsDispensing = false;
    std::string msg = "Dispenser::initGlobalFlowsensorIO. Position: " + std::to_string(getSlot()) + " (pin: " + std::to_string(pin) + ")";
    // debugOutput::sendMessage("-----dispenser::initGlobalFlowsensorIO-----", MSG_INFO);
    debugOutput::sendMessage(msg, MSG_INFO);

    m_pFlowsensor = new FSModdyseyx86GPIO();
    m_pFlowsensor->setPinNumber(pin);
    m_pFlowsensor->setPinAsInputElseOutput(true);
    m_pFlowsensor->registerProduct(getSelectedProduct());
    m_pFlowsensor->startListener_flowsensor();
    e_ret = OK;

    return e_ret;
}

////////////////////////////////////////////////////////////
// Flow

double dispenser::getProductVolumeDeltaAndReset()
{
    // will get volumeDelta since last call of this function

    double currentVolume = getActiveProductVolumeDispensed();
    double deltaVolume = currentVolume - previousDispensedVolume;
    previousDispensedVolume = currentVolume;
    return deltaVolume;
}

void dispenser::initProductFlowRateCalculation()
{
    flowRateBufferIndex = 0;
    for (uint16_t i = 0; i < RUNNING_AVERAGE_WINDOW_LENGTH; i++)
    {
        flowRateBuffer[i].time_millis = MILLIS_INIT_DUMMY;
        flowRateBuffer[i].value = 0;
    }
}
double dispenser::getProductFlowRateInstantaneous()
{
    using namespace std::chrono;
    uint64_t millis_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    double volumeDelta = getProductVolumeDeltaAndReset();
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

Time_val dispenser::createAndGetActiveProductVolumeDispensedDatapoint()
{
    Time_val tv;
    tv.value = getActiveProductVolumeDispensed();
    using namespace std::chrono;
    uint64_t millis_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    tv.time_millis = millis_since_epoch;
    return tv;
}

Time_val dispenser::getAveragedProductFlowRate(uint64_t window_length_millis)
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

DF_ERROR dispenser::updateActiveProductFlowRateRunningAverageWindow()
{
    DF_ERROR e_ret;

    Time_val tv = createAndGetActiveProductVolumeDispensedDatapoint();

    flowRateBuffer[flowRateBufferIndex].time_millis = tv.time_millis;
    flowRateBuffer[flowRateBufferIndex].value = tv.value;
    // debugOutput::sendMessage("updateActiveProductFlowRateRunningAverageWindow: index: " + to_string(flowRateBufferIndex) + " " + to_string(tv.time_millis) + ": " + to_string(tv.value), MSG_INFO);

    flowRateBufferIndex++;
    if (flowRateBufferIndex >= RUNNING_AVERAGE_WINDOW_LENGTH)
    {
        flowRateBufferIndex = 0;
    }

    return e_ret = OK;
}

//////////////////////////////////////////////////////////////
//  Dispenser state

void dispenser::analyseSlotState()
{
    string slotStateText = m_status_text;

    if (slotStateText.find("SLOT_STATE_AVAILABLE") != string::npos)
    {
        setSlotState(SLOT_STATE_AVAILABLE);
    }
    else if (slotStateText.find("SLOT_STATE_AVAILABLE_LOW_STOCK") != string::npos)
    {
        setSlotState(SLOT_STATE_AVAILABLE_LOW_STOCK);
    }
    else if (slotStateText.find("SLOT_STATE_WARNING_PRIMING") != string::npos)
    {
        setSlotState(SLOT_STATE_WARNING_PRIMING);
    }
    else if (slotStateText.find("SLOT_STATE_PROBLEM_NEEDS_ATTENTION") != string::npos)
    {
        setSlotState(SLOT_STATE_PROBLEM_NEEDS_ATTENTION);
    }
    else if (slotStateText.find("SLOT_STATE_PROBLEM_EMPTY") != string::npos)
    {
        setSlotState(SLOT_STATE_PROBLEM_EMPTY);
    }
    else if (slotStateText.find("SLOT_STATE_DISABLED_COMING_SOON") != string::npos)
    {
        setSlotState(SLOT_STATE_DISABLED_COMING_SOON);
    }
    else if (slotStateText.find("SLOT_STATE_DISABLED") != string::npos)
    {
        setSlotState(SLOT_STATE_DISABLED);
    }
    else
    {
        setSlotState(SLOT_STATE_AVAILABLE);
    }
    debugOutput::sendMessage("Dispenser: Slot " + std::to_string(getSlot()) + ". State (loaded from db): " + std::string(getSlotStateAsString()) + "(db value: " + std::string(slotStateText) + ")", MSG_INFO);
}

const char *dispenser::getDispenseStatusAsString()
{
    Dispense_behaviour behaviour = getDispenseStatus();
    const char *behaviour_str = DISPENSE_BEHAVIOUR_STRINGS[behaviour];
    return behaviour_str;
}

const char *dispenser::getSlotStateAsString()
{
    Slot_state state = getSlotState();
    const char *state_str = SLOT_STATE_STRINGS[state];
    return state_str;
}

void dispenser::setSlotState(Slot_state state)
{
    slot_state = state;
}

Slot_state dispenser::getSlotState()
{

    return slot_state;
}

void dispenser::updateSlotState()
{
    switch (slot_state)
    {
    case SLOT_STATE_WARNING_PRIMING:
    {
        if (getDispenseStatus() == FLOW_STATE_EMPTY)
        {
            slot_state = SLOT_STATE_PROBLEM_EMPTY;
        }
        if (getDispenseStatus() == FLOW_STATE_PRIME_FAIL_OR_EMPTY)
        {
            slot_state = SLOT_STATE_PROBLEM_NEEDS_ATTENTION;
        }

        if (getDispenseStatus() == FLOW_STATE_NOT_PUMPING_NOT_DISPENSING)
        {
            slot_state = SLOT_STATE_AVAILABLE;
        }

        if (getDispenseStatus() == FLOW_STATE_DISPENSING)
        {
            slot_state = SLOT_STATE_AVAILABLE;
        }
        break;
    }
    case SLOT_STATE_AVAILABLE:
    {

        if (getDispenseStatus() == FLOW_STATE_PRIMING_OR_EMPTY)
        {
            slot_state = SLOT_STATE_WARNING_PRIMING;
        }
        if (getDispenseStatus() == FLOW_STATE_EMPTY)
        {
            slot_state = SLOT_STATE_PROBLEM_EMPTY;
        }
        if (getDispenseStatus() == FLOW_STATE_PRIME_FAIL_OR_EMPTY)
        {
            slot_state = SLOT_STATE_PROBLEM_NEEDS_ATTENTION;
        }

        break;
    }
    case SLOT_STATE_AVAILABLE_LOW_STOCK:
    {
        if (getDispenseStatus() == FLOW_STATE_EMPTY)
        {
            slot_state = SLOT_STATE_PROBLEM_EMPTY;
        }
        if (getDispenseStatus() == FLOW_STATE_PRIME_FAIL_OR_EMPTY)
        {
            slot_state = SLOT_STATE_PROBLEM_NEEDS_ATTENTION;
        }
        break;
    }
    case SLOT_STATE_PROBLEM_NEEDS_ATTENTION:
    {
        if (getDispenseStatus() == FLOW_STATE_DISPENSING)
        {
            slot_state = SLOT_STATE_AVAILABLE;
        }
        break;
    }
    case SLOT_STATE_PROBLEM_EMPTY:
    {
        if (getDispenseStatus() == FLOW_STATE_DISPENSING)
        {
            slot_state = SLOT_STATE_AVAILABLE;
        }
        break;
    }
    case SLOT_STATE_DISABLED_COMING_SOON:
    {
        break;
    }
    case SLOT_STATE_DISABLED:
    {
        // do nothing can only be altered when set to enabled
        break;
    }

    default:
    {
        debugOutput::sendMessage("Erroneous dispenser state: " + to_string(slot_state), MSG_INFO);
    }
    }
}

Dispense_behaviour dispenser::getDispenseStatus()
{

    // logUpdateIfAllowed("Button press time [ms]. Total:" + to_string(dispense_button_total_pressed_millis) + " Current:" + to_string(dispense_button_current_press_millis));
    using namespace std::chrono;
    uint64_t millis_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    uint64_t dispense_time_millis = millis_since_epoch - dispense_start_timestamp_epoch;
    Time_val avg = getAveragedProductFlowRate(EMPTY_CONTAINER_DETECTION_FLOW_AVERAGE_WINDOW_MILLIS);

    // restricted status update
    // logUpdateIfAllowed("Dispense flowRate " + to_string(EMPTY_CONTAINER_DETECTION_FLOW_AVERAGE_WINDOW_MILLIS) + "ms avg [ml/s]: " + to_string(avg.value) + " Dispense state time: " + to_string(dispense_time_millis));

    return dispense_state;
}

void dispenser::updateDispenseStatus()
{
    Time_val avg = getAveragedProductFlowRate(EMPTY_CONTAINER_DETECTION_FLOW_AVERAGE_WINDOW_MILLIS);

    // debugOutput::sendMessage("minimummmmm: " + to_string(getSelectedProduct()->getThresholdFlow()), MSG_INFO);
    // debugOutput::sendMessage("maximummmmm ml/s: " + to_string(getSelectedProduct()->getThresholdFlow_max_allowed()), MSG_INFO);

    if (!getDispenseButtonValue())
    {
        dispense_state = FLOW_STATE_NOT_PUMPING_NOT_DISPENSING;
    }
    else if ((getButtonPressedCurrentPressMillis() < EMPTY_CONTAINER_DETECTION_FLOW_AVERAGE_WINDOW_MILLIS) &&
             (avg.value < getSelectedProduct()->getThresholdFlow() || avg.value >= getSelectedProduct()->getThresholdFlow_max_allowed()))
    {
        // flow rate needs to be ramped up until stable.  (or ramped down in situations I can't imagine)
        dispense_state = FLOW_STATE_RAMP_UP;
    }
    // else if (avg.value >= getSelectedProduct()->getThresholdFlow() )
    else if (avg.value >= getSelectedProduct()->getThresholdFlow() && avg.value < getSelectedProduct()->getThresholdFlow_max_allowed())
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
        if (avg.value < getSelectedProduct()->getThresholdFlow())
        {
            debugOutput::sendMessage("Below threshold of minimum flow rate. minimum flow rate: " + to_string(getSelectedProduct()->getThresholdFlow()), MSG_INFO);
        }
        else if (avg.value >= getSelectedProduct()->getThresholdFlow_max_allowed())
        {
            debugOutput::sendMessage("Exceeds threshold of maximum flow rate. maximum flow rate: " + to_string(getSelectedProduct()->getThresholdFlow_max_allowed()), MSG_INFO);
        }
        else
        {
            debugOutput::sendMessage("Erroneous dispenser state: " + to_string(slot_state), MSG_INFO);
        }

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
