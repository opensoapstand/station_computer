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
    // "SLOT_STATE_WARNING_PRIMING",
    "SLOT_STATE_PROBLEM_NEEDS_ATTENTION",
    "SLOT_STATE_DISABLED",
    "SLOT_STATE_INVALID"};

// const char *SLOT_STATE_STRINGS[] = {
//     "SLOT_STATE_AVAILABLE",
//     "SLOT_STATE_AVAILABLE_LOW_STOCK",
//     "SLOT_STATE_NOT_PRIMED",
//     "SLOT_STATE_PROBLEM_NEEDS_ATTENTION",
//     "SLOT_STATE_PROBLEM_EMPTY",
//     "SLOT_STATE_DISABLED_COMING_SOON",
//     "SLOT_STATE_DISABLED"};

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
            debugOutput::sendMessage("Dispenser: ERROR: Array size exceeded maximum limit (will stop adding):  " + std::to_string(size), MSG_ERROR);
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
            debugOutput::sendMessage("Dispenser: ERROR: Array size exceeded maximum limit (will stop adding):  " + std::to_string(size), MSG_ERROR);
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
    m_dispense_state = FLOW_STATE_UNAVAILABLE;
    m_previous_dispense_state = FLOW_STATE_UNAVAILABLE;
    using namespace std::chrono;
    previous_status_update_allowed_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    m_dispenserTicks = 0;
}

dispenser::~dispenser()
{
    debugOutput::sendMessage("Dispenser: destructor  " + to_string(getSlot()) + "(destroyed)", MSG_INFO);
    // delete m_pcb;
    m_pcb = nullptr;
}

bool dispenser::getAutoDispense()
{
    return m_auto_dispense;
}

void dispenser::setAutoDispense()
{
    m_auto_dispense = true;
}

void dispenser::resetDispenser()
{
    m_pcb->setPumpPWM(DEFAULT_PUMP_PWM);
    millisAtLastCheck = MILLIS_INIT_DUMMY;
    previousDispensedVolume = 0;
    m_auto_dispense = false;

    isPumpSoftStarting = false;
    pwm_actual_set_speed = 0;

    bool onlyOneProductPresent = m_dispense_pnumbers_count == 1 && m_additive_pnumbers_count == 0 && m_base_pnumber == m_dispense_pnumbers[0];

    debugOutput::sendMessage("Dispenser: m_dispense_pnumbers_count: " + std::to_string(m_dispense_pnumbers_count), MSG_INFO);
    debugOutput::sendMessage("Dispenser: m_additive_pnumbers_count: " + std::to_string(m_additive_pnumbers_count), MSG_INFO);
    debugOutput::sendMessage("Dispenser: m_base_pnumber: " + std::to_string(m_base_pnumber), MSG_INFO);
    debugOutput::sendMessage("Dispenser: m_base_pnumber: " + std::to_string(m_dispense_pnumbers[0]), MSG_INFO);

    if (onlyOneProductPresent)
    {
        // if only one product per dispenser, set it as default.
        setBasePNumberAsSingleDispenseSelectedProduct();
    }
    else
    {
        debugOutput::sendMessage("Dispenser: multiple products in dispenser. Set selected product to dummy for now.", MSG_INFO);
        setSelectedProduct(DUMMY_PNUMBER);
    }
}

DF_ERROR dispenser::setup(int slot_number, pcb *pcb, product *pnumbers)
// DF_ERROR dispenser::setup(machine *machine, product *pnumbers)
{
    // Set the pump PWM value to a nominal value
    m_slot = slot_number;
    m_pnumbers = pnumbers;
    m_pcb = pcb;

    DF_ERROR dfRet = OK;
    return dfRet;
}

DF_ERROR dispenser::loadGeneralProperties()
{
    debugOutput::sendMessage("Dispenser: Load properties for slot: " + std::to_string(m_slot), MSG_INFO);
    // ******* Sleep time between DB calls solved inconsistend readings from db!!!****
    // debugOutput::sendMessage("TEMPORARY HACK: base number is the selected product at startup. ", MSG_INFO);
    usleep(20000);
    loadDispenserParametersFromDb();
    // usleep(20000);
    // setSlotStateFromString(string slotStateText);
    usleep(20000);
    resetDispenser();

    DF_ERROR dfRet = OK;
    return dfRet;
}

void dispenser::refresh()
{
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

void dispenser::setSlotStateFromString(string slotStateText)
{
    m_slot_state = SLOT_STATE_AVAILABLE;
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

int dispenser::getSlot()
{
    // first slot is 1
    return m_slot;
}

product *dispenser::getSelectedProduct()
{
    return getProductFromPNumber(m_selected_pnumber);
}

int dispenser::getSelectedPNumber()
{
    return m_selected_pnumber;
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
    debugOutput::sendMessage("Dispenser: DB set up to have one dispense product per slot. Set Base PNumber as selected product: " + std::to_string(getBasePNumber()), MSG_INFO);
}

void dispenser::setPNumberAsSingleDispenseSelectedProduct(int pnumber)
{
    setSelectedProduct(pnumber);
    setActiveProduct(pnumber);
}

// bool dispenser::isCustomMixPNumberAsSelectedProduct(){
//     // deprecated, check it in the product itself.
//     return (CUSTOM_MIX_PNUMBER ==  m_selected_pnumber);
// }

// int dispenser::getCustomMixCount(){
//     return m_custom_mix_pnumbers_count;
// }

// int dispenser::getCustomMixPNumberFromMixIndex(int mixIndex)
// {
//     if (mixIndex >= m_custom_mix_pnumbers_count)
//     {
//         debugOutput::sendMessage("Dispenser: ASSERT ERROR, mixing does not contain this index: " + std::to_string(mixIndex), MSG_ERROR);
//         return DUMMY_PNUMBER;
//     }
//     return m_custom_mix_pnumbers[mixIndex];
// }

void dispenser::setCustomMixParametersToSelectedProduct(string pnumbers, string pnumberRatios)
{
    // int custom_mix_pnumbers[DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX];
    // double custom_mix_ratios[DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX];
    // int custom_mix_pnumbers_count;
    // int custom_mix_ratios_count;

    // dispenser::parseIntCsvString(pnumbers, custom_mix_pnumbers, custom_mix_pnumbers_count);
    // dispenser::parseDoubleCsvString(pnumberRatios, custom_mix_ratios, custom_mix_ratios_count);

    // if (custom_mix_pnumbers_count != custom_mix_ratios_count)
    // {
    //     debugOutput::sendMessage("ASSERT ERROR: Number of mix pnumbers is not equal to number of mixing ratios!", MSG_ERROR);
    //     return;
    // }

    // int pnumber;
    // m_custom_mix_pnumbers_count = 0;
    // m_custom_mix_ratios_count = 0;
    // for (int i = 0; i < custom_mix_pnumbers_count; i++)
    // {
    //     pnumber = custom_mix_pnumbers[i];
    //     if (isPNumberValidInThisDispenser(pnumber, true))
    //     {

    //         m_custom_mix_pnumbers[m_custom_mix_pnumbers_count] = custom_mix_pnumbers[i];
    //         m_custom_mix_ratios[m_custom_mix_pnumbers_count] = custom_mix_ratios[i];

    //         debugOutput::sendMessage("Mixing: Pnumber at mix position index: " + std::to_string(i) + " P-" + std::to_string(m_custom_mix_pnumbers[m_custom_mix_pnumbers_count]) + " ratio: " + std::to_string(m_custom_mix_ratios[m_custom_mix_pnumbers_count]), MSG_INFO);

    //         m_custom_mix_pnumbers_count++;
    //         m_custom_mix_ratios_count++;
    //     }
    // }

    // // setSelectedProduct(CUSTOM_MIX_PNUMBER);
    // getSelectedProduct()->parseMixPNumbersAndRatiosCsv(pnumbers, pnumberRatios);

    // debugOutput::sendMessage("Dispenser: Set active product as mixing product: (TODO: for now, will only set first component as single dispense product) ", MSG_WARNING);

    getSelectedProduct()->parseAndSetCustomMixRatios(pnumbers, pnumberRatios);
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

        if (CUSTOM_MIX_PNUMBER == pnumber)
        {
            isValid = true;
        }
    }

    if (!isValid)
    {
        debugOutput::sendMessage("ASSERT ERROR (unless dummy product is used (" +
                                     std::to_string(DUMMY_PNUMBER) + "): invalid pnumber as selected pnumber (" + std::to_string(pnumber) + ") in dispenser: " + std::to_string(getSlot()) + " (must be additive or base?: " + std::to_string(mustBeAdditiveOrBase) + ")",
                                 MSG_ERROR);
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
    debugOutput::sendMessage("Dispenser: Set Selected product to: P-" + to_string(pnumber) + "(can be single or mix) ", MSG_INFO);
    m_selected_pnumber = pnumber;
    getSelectedProduct()->resetVolumeDispensed();
    getSelectedProduct()->setTargetVolumeFromSize(SIZE_DUMMY); // reset target volume.
    return true;
}

void dispenser::setActiveProduct(int pnumber)
{
    if (!isPNumberValidInThisDispenser(pnumber, true))
    {
        debugOutput::sendMessage("Dispenser: ASSERT ERROR Attempt to set non dispensable product as Active product to: " + to_string(pnumber), MSG_ERROR);
    }
    debugOutput::sendMessage("Dispenser: Set Active product to: P-" + to_string(pnumber) + "(product that will come out of spout=active product) ", MSG_INFO);
    m_active_pnumber = pnumber;
}

// void dispenser::setSelectedSizeAsChar(char size)
// {
//     m_selectedSizeAsChar = size;
// };

char dispenser::getSelectedSizeAsChar()
{
    return getSelectedProduct()->getTargetVolumeAsChar();
}

double dispenser::getSelectedSizeAsVolume()
{
    return getSelectedProduct()->getTargetVolume();
}

product *dispenser::getProductFromPNumber(int pnumber)
{
    return &m_pnumbers[pnumber];
}

product *dispenser::getActiveProduct()
{
    // set the product that will actually be dispensed. (base or additive)
    // for now, just base product.
    // active products cannot be dispenseProducts (if the dispense product is not the same as a base or additive) e.g. ginger kombucha is not dispensable, its components (base kombucha, addtives: suger, ginger flavor) are.
    return getProductFromPNumber(m_active_pnumber);
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
        return DUMMY_PNUMBER;
    }
    if (position > m_additive_pnumbers_count)
    {
        debugOutput::sendMessage("ASSERT ERROR Dispenser: Additive number not existing. Requested:" + std::to_string(position) + " Max: " + std::to_string(m_additive_pnumbers_count), MSG_ERROR);
        return DUMMY_PNUMBER;
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
    while (status == SQLITE_ROW)
    {
        numberOfRecordsFound++;
        // debugOutput::sendMessage("Record found for slot : " + std::to_string(getSlot()), MSG_INFO);
        dispense_numbers_str = product::dbFieldAsValidString(stmt, 1);
        base_pnumber_str = product::dbFieldAsValidString(stmt, 2);
        additive_pnumbers_str = product::dbFieldAsValidString(stmt, 3);
        m_is_slot_enabled = sqlite3_column_int(stmt, 4);
        string status_text = product::dbFieldAsValidString(stmt, 5);

        setSlotStateFromString(status_text);

        status = sqlite3_step(stmt); // next record
        // every sqlite3_step returns a row. if it returns 0, it's run over all the rows.
    }
    //  debugOutput::sendMessage("Record loaded for slot : " + std::to_string(getSlot()), MSG_INFO);
    bool slot_loaded_from_db = false;
    if (numberOfRecordsFound == 1)
    {
        slot_loaded_from_db = true;
        dispenser::parseIntCsvString(dispense_numbers_str, m_dispense_pnumbers, m_dispense_pnumbers_count);
        dispenser::parseIntCsvString(additive_pnumbers_str, m_additive_pnumbers, m_additive_pnumbers_count);

        if (base_pnumber_str.empty())
        {
            // first dispense number in the list as base pnumber if base pnumber is empty.
            // this is handy if we have a basic dispenser, like with soap, that only dispenses pure products. No mixings. No need to then have the same pnumber as base_pnumber
            m_base_pnumber = m_dispense_pnumbers[0];
        }
        else
        {
            m_base_pnumber = std::stoi(base_pnumber_str);
        }

        debugOutput::sendMessage("Base pnumber: " + std::to_string(m_base_pnumber), MSG_INFO);
        string additives_str = "Additives: ";
        // debugOutput::sendMessage("Additives: ", MSG_INFO);
        for (int additivePosition = 1; additivePosition <= m_additive_pnumbers_count; additivePosition++)
        {
            additives_str += std::to_string(additivePosition) + ":" + std::to_string(getAdditivePNumber(additivePosition)) + ", ";
            // debugOutput::sendMessage(std::to_string(additivePosition) + " : " + std::to_string(getAdditivePNumber(additivePosition)), MSG_INFO);
        }
        debugOutput::sendMessage(additives_str, MSG_INFO);

        string dispensables_str = "Dispense pnumbers: ";
        // debugOutput::sendMessage("Next lines are a list of all m_dispense_pnumbers for this dispenser: ", MSG_INFO);
        for (int pos = 1; pos <= m_dispense_pnumbers_count; pos++)
        {
            dispensables_str += std::to_string(pos) + ":" + std::to_string(m_dispense_pnumbers[pos - 1]) + ", ";
            // debugOutput::sendMessage(std::to_string(pos) + " : " + std::to_string(m_dispense_pnumbers[pos - 1]), MSG_INFO);
        }
        debugOutput::sendMessage(dispensables_str, MSG_INFO);
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

bool dispenser::handleMixComponentTargetVolumeReached()
{
    // if last product is dispensed return true, (always true for non mixing products)
    // set next mixing product if mix,

    if (getSelectedProduct()->isMixingProduct())
    {
        m_mix_active_index--;
        debugOutput::sendMessage("Next Mix Pnumber index in mix-pnumbers sequence (if 0:base, if -1:finished, if >0:additive): " + to_string(m_mix_active_index), MSG_INFO);

        if (m_mix_active_index < 0)
        {
            finishActivePNumberDispense();
            finishSelectedProductDispense();
            return true;
        }
        else
        {
            finishActivePNumberDispense();
            setActiveProduct(getSelectedProduct()->getMixPNumber(m_mix_active_index));
            debugOutput::sendMessage("Dispenser: Set next active product in mix. Position:  " + to_string(m_mix_active_index) + " Active Pnumber: " + to_string(getActivePNumber()), MSG_INFO);
            debugOutput::sendMessage("Dispenser: dispensed volume at start. Active Pnumber: " + to_string(getActiveProductVolumeDispensed()), MSG_INFO);
            initActivePNumberDispense();
            return false;
        }
    }
    else
    {
        finishActivePNumberDispense();
        finishSelectedProductDispense();
        return true;
    }
}

DF_ERROR dispenser::initSelectedProductDispense(char size)
{
    debugOutput::sendMessage("Dispenser: Start Selected PNumber dispense at slot " + to_string(this->m_slot), MSG_INFO);
    using namespace std::chrono;
    dispense_start_timestamp_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    DF_ERROR dfRet = OK;
    m_pcb->sendEN258DefaultConfigurationToMCP23017(getSlot(), true);
    getSelectedProduct()->setTargetVolumeFromSize(size);
    resetSelectedProductVolumeDispensed();
    switch (m_pcb->get_pcb_version())
    {

    case (pcb::PcbVersion::DSED8344_NO_PIC):
    {
    }
    break;
    case pcb::PcbVersion::DSED8344_PIC_MULTIBUTTON:
    {
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
        debugOutput::sendMessage("Dispenser: Pcb not compatible. ", MSG_WARNING);
    }
    break;
    }

    // Dispensing a mix includes the base and additive pnumbers. Start with additives, base last.
    if (getSelectedProduct()->isMixingProduct())
    {
        debugOutput::sendMessage("Dispenser: Dispense product is a mix. Will dispense in multiple stages. ", MSG_INFO);

        // set the volumes for all different mixing parts
        for (int8_t mix_position = getSelectedProduct()->getMixProductsCount() - 1; mix_position >= 0; mix_position--)
        {
            int mixPnumber = getSelectedProduct()->getMixPNumber(mix_position);
            double mix_position_targetVolume = getSelectedProduct()->getTargetVolume() * getSelectedProduct()->getMixRatio(mix_position);
            getProductFromPNumber(mixPnumber)->setTargetVolume(mix_position_targetVolume);

            debugOutput::sendMessage("Dispenser: Dispense Product: P-" + std::to_string(getSelectedPNumber()) + " (part " + std::to_string(getSelectedProduct()->getMixProductsCount() - mix_position) + "/" + std::to_string(getSelectedProduct()->getMixProductsCount()) + ") : P-" + std::to_string(mixPnumber) + " target volume: " + std::to_string(getProductTargetVolume(mixPnumber)) + "ml", MSG_INFO);
        }

#ifndef ENABLE_PARALLEL_MIX
        m_mix_active_index = getSelectedProduct()->getMixProductsCount() - 1; // works with indexes (indexes start from 0)
        setActiveProduct(getSelectedProduct()->getMixPNumber(m_mix_active_index));
        debugOutput::sendMessage("Dispenser: Mix product at position: " + std::to_string(m_mix_active_index) + " . Pnumber: " + std::to_string(getActivePNumber()), MSG_INFO);
#else
        setActiveProduct(getSelectedProduct()->getBasePNumber());
        debugOutput::sendMessage("Dispenser: Parallel mixing. Which means: base pnumber is always enabled. base pnumber: " + std::to_string(getSelectedProduct()->getBasePNumber()), MSG_INFO);

        // spout solenoid always open during dispense cycle.
        m_pcb->setSpoutSolenoid(getSlot(), true);

#endif
    }
    else
    {
        debugOutput::sendMessage("Dispenser: Dispense product is not a mix. ", MSG_INFO);
        setActiveProduct(getSelectedPNumber());
    }

    dispenseButtonTimingreset();

    initActivePNumberDispense();

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(m_nStartTime, 50, "%F %T", timeinfo);
    return dfRet;
}

DF_ERROR dispenser::finishSelectedProductDispense()
{

    debugOutput::sendMessage("Dispenser: Stop selected PNumber dispense ", MSG_INFO);
    // Set End time
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(m_nEndTime, 50, "%F %T", timeinfo);

    m_pcb->setDispenseButtonLightsAllOff();

#ifdef ENABLE_PARALLEL_MIX
    stopParallelMixDispensing();
    usleep(END_OF_DISPENSE_SPOUT_SHUTOFF_DELAY_MICROS); // delay before shutting off spout solenoid. (prevent pressure built up (is ballooning flex duct in front of spout solenoid otherwise))
    m_pcb->setSpoutSolenoid(getSlot(), false);
#else

#endif

    // safety, to make sure all are off.
    m_pcb->disableAllSolenoidsOfSlot(getSlot());

    debugOutput::sendMessage("Dispenser: registers after disablingall solenoids. ", MSG_INFO);
    m_pcb->displayMCP23017IORegisters(getSlot());
    m_pcb->flowSensorsDisableAll(); // todo check: is this a better altnerative: m_pcb->flowSensorResetAndDisable(getSlot());
    DF_ERROR dfRet = OK;
    return dfRet;
}

string dispenser::getSelectedProductDispenseStartTime()
{
    return m_nStartTime;
}

string dispenser::getSelectedProductDispenseEndTime()
{
    return m_nEndTime;
}

DF_ERROR dispenser::initActivePNumberDispense()
{
    debugOutput::sendMessage("Dispenser: Start Active PNumber: " + to_string(getActivePNumber()) + " At slot " + to_string(this->m_slot), MSG_INFO);
    resetActiveProductVolumeDispensed();
    switch (m_pcb->get_pcb_version())
    {

    case (pcb::PcbVersion::DSED8344_NO_PIC):
    {
    }
    break;
    case pcb::PcbVersion::DSED8344_PIC_MULTIBUTTON:
    {
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
        debugOutput::sendMessage("Dispenser: Pcb not comptible. ", MSG_WARNING);
    }
    break;
    }

    this->m_pcb->flowSensorEnable(m_slot);

#ifndef ENABLE_PARALLEL_MIX
    setActiveProductSolenoid(true);
#endif
    // this->m_pcb->resetFlowSensorPulsesForDispenser(m_slot);

    // init state
    m_dispense_state = FLOW_STATE_NOT_PUMPING_NOT_DISPENSING;
    m_previous_dispense_state = FLOW_STATE_UNAVAILABLE; // hack needed to create edge

    initProductFlowRateCalculation();

    DF_ERROR e_ret = ERROR_MECH_PRODUCT_FAULT;
    return e_ret = OK;
}

DF_ERROR dispenser::finishActivePNumberDispense()
{
    debugOutput::sendMessage("Dispenser: Stop Active PNumber dispense " + to_string(getActivePNumber()), MSG_INFO);

    std::string activePNumber = to_string(getActivePNumber());
    double activeProductVolumeDispensed = getActiveProductVolumeDispensed();
    debugOutput::sendMessage("Active product " + activePNumber + ". volume dispensed: " + std::to_string(activeProductVolumeDispensed), MSG_INFO);
    double volume_remaining = getActiveProduct()->getVolumeRemaining();
    setMixDispenseReport(activePNumber, activeProductVolumeDispensed, volume_remaining);

    stopActiveDispensing();
#ifndef ENABLE_PARALLEL_MIX
    setActiveProductSolenoid(false);
#endif
    m_pcb->flowSensorsDisableAll(); // todo: is this a better altnerative: m_pcb->flowSensorResetAndDisable(getSlot());
    DF_ERROR dfRet = OK;

    m_dispense_state = FLOW_STATE_UNAVAILABLE;

    return dfRet;
}

void dispenser::startActiveDispensing()
{
    // actual pumping start
    debugOutput::sendMessage("Dispenser: Start active product dispensing.", MSG_INFO);
    switch (m_pcb->get_pcb_version())
    {
    case (pcb::DSED8344_NO_PIC):
    case (pcb::DSED8344_PIC_MULTIBUTTON):
    case (pcb::EN134_4SLOTS):
    case (pcb::EN134_8SLOTS):
    {
        m_pcb->setPumpSpeedPercentage(0); // pump speed is inverted!
        m_pcb->setPumpDirection(getSlot(), true);
    }
    break;
    }
    m_pcb->startPump(getSlot());

#ifdef ENABLE_PARALLEL_MIX
    m_pcb->setSpoutSolenoid(getSlot(), true);

    if (getSelectedProduct()->isMixingProduct())
    {
        // if SELECTED product is a mix, we open the active product solenoid.
        debugOutput::sendMessage("Dispenser: MIXING PRODUCT SET ACTIVE SOLENOID", MSG_INFO);
        setActiveProductSolenoid(true);
    }
    else
    {
        debugOutput::sendMessage("Dispenser: NOT MIXING PRODUCT SET SPOUT", MSG_INFO);
#endif

        m_pcb->setSpoutSolenoid(getSlot(), true);
        if (m_pcb->get_pcb_version() == pcb::PcbVersion::EN258_4SLOTS || m_pcb->get_pcb_version() == pcb::PcbVersion::EN258_8SLOTS)
        {
            setActiveProductSolenoid(true);
        }

#ifdef ENABLE_PARALLEL_MIX
    }
#endif
}

void dispenser::stopActiveDispensing()
{
    // actual pumping stop
    debugOutput::sendMessage("Dispenser: stop active product dispensing.", MSG_INFO);
    m_pcb->stopPump(getSlot());

#ifdef ENABLE_PARALLEL_MIX
    m_pcb->setSpoutSolenoid(getSlot(), false);

    if (getSelectedProduct()->isMixingProduct())
    {
        // if SELECTED product is a mix, we open the active product solenoid.
        // debugOutput::sendMessage("Dispenser: MIXING PRODUCT SET ACTIVE SOLENOID", MSG_INFO);
        setActiveProductSolenoid(false);
    }
    else
    {
        // debugOutput::sendMessage("Dispenser: NOT MIXING PRODUCT SET SPOUT", MSG_INFO);
#endif

        m_pcb->setSpoutSolenoid(getSlot(), false);
        if (m_pcb->get_pcb_version() == pcb::PcbVersion::EN258_4SLOTS || m_pcb->get_pcb_version() == pcb::PcbVersion::EN258_8SLOTS)
        {
            setActiveProductSolenoid(false);
        }

#ifdef ENABLE_PARALLEL_MIX
    }
#endif
}

/////////////////////////////////////////////////////////////////////////
// parallel mixing
#ifdef ENABLE_PARALLEL_MIX
void dispenser::startParallelMixDispensing()
{
    setParallelSolenoids();
}

void dispenser::stopParallelMixDispensing()
{
    for (int8_t mix_position = getSelectedProduct()->getMixProductsCount() - 1; mix_position >= 0; mix_position--)
    {
        int mixPnumber = getSelectedProduct()->getMixPNumber(mix_position);

        if (mixPnumber != getBasePNumber())
        {

            setProductSolenoid(mixPnumber, false);
        }
    }
}

#define BASE_START_DISPENSE_OFFSET_MILLILITERS 50 // amount to be dispensed before additives get mixed in.
void dispenser::setParallelSolenoids()
{

    // double baseVolumeDispensed = getProductFromPNumber(getBasePNumber())->getVolumeDispensed();
    double selectedProductVolumeDispensed = getSelectedProduct()->getVolumeDispensed();

    debugOutput::sendMessage("Dispenser: Set Mixing solenoids. volume: " + std::to_string(selectedProductVolumeDispensed), MSG_INFO);
    for (int8_t mix_position = getSelectedProduct()->getMixProductsCount() - 1; mix_position >= 0; mix_position--)
    {
        int mixPnumber = getSelectedProduct()->getMixPNumber(mix_position);

        if (mixPnumber != getBasePNumber())
        {
            // basePNumber is dealt with separately
            double mix_position_targetVolume = getSelectedProduct()->getTargetVolume() * getSelectedProduct()->getMixRatio(mix_position);

            // getProductFromPNumber(mixPnumber)->setTargetVolume(mix_position_targetVolume);

            if (selectedProductVolumeDispensed > BASE_START_DISPENSE_OFFSET_MILLILITERS && selectedProductVolumeDispensed - BASE_START_DISPENSE_OFFSET_MILLILITERS < mix_position_targetVolume)
            {
                debugOutput::sendMessage("Dispenser:mixPNumber " + std::to_string(mixPnumber) + " target volume: " + std::to_string(mix_position_targetVolume) + "solenoid ON ", MSG_INFO);
                // open solenoid and add additive to mix
                setProductSolenoid(mixPnumber, true);
            }
            else
            {
                debugOutput::sendMessage("Dispenser:mixPNumber " + std::to_string(mixPnumber) + " target volume: " + std::to_string(mix_position_targetVolume) + "solenoid OFF ", MSG_INFO);
                // close solenoid and do not add to mix
                setProductSolenoid(mixPnumber, false);
            }
        }
    }
}

// bool dispenser::isAdditiveNeeded(int additive)
// {
//     // there is no flowsensor for the additives, only for base product. So, we look a the total amount of the base product. e.g.
//     // e.g. when base product is between 50 and 200ml, additive should be enabled.

//     // if ()
// }

#endif

/////////////////////////////////////////////////////////////////////////
// dispenser volume

void dispenser::linkActiveProductVolumeUpdate()
{
    // DEPRECATED, if implemented, needs to relink callback function for every new active product
    product *active_product = getActiveProduct();
    auto lambdaFunc = [active_product]()
    { active_product->registerFlowSensorTickFromPcb(); };
    m_pcb->registerFlowSensorTickCallback(getSlot(), lambdaFunc);
    debugOutput::sendMessage("Dispenser: flow tick : deprecated", MSG_INFO);
}

void dispenser::linkDispenserFlowSensorTick()
{
    debugOutput::sendMessage("Dispenser: flow tick : callback.", MSG_INFO);
    auto lambdaFunc = [this]()
    {
        this->registerFlowSensorTickFromPcb();
    };
    m_pcb->registerFlowSensorTickCallback(getSlot(), lambdaFunc);
}

void dispenser::registerFlowSensorTickFromPcb()
{

    // from observation: customers keep the button pressed, even when empty is detected.
    // the flowsensor splutters, and 'fake ticks' keep on being added.
    // so, at least, the customer should restart pressing the button (it'll go to 'ramp up' state then, out of the empty state, and providing the opportunity for flow to restart if it was a fluke empty detection.)

    m_dispenserTicks++;
    if (getDispenseStatus() != FLOW_STATE_PRIME_FAIL_OR_EMPTY &&
        getDispenseStatus() != FLOW_STATE_UNAVAILABLE &&
        getDispenseStatus() != FLOW_STATE_PRIMING_OR_EMPTY)
    {

        // the actual dispensed produce gets always registered
        getActiveProduct()->registerFlowSensorTickFromPcb();

        // debugOutput::sendMessage("Dispenser: active flow tick : " + std::to_string(getSlot()) + " Active pnumber: " + getActiveProduct()->getPNumberAsPString() , MSG_INFO);

        if (getActivePNumber() != getSelectedPNumber())
        {
            // debugOutput::sendMessage("Dispenser: selected flow tick : " + std::to_string(getSlot()) + " Selected pnumber: " + getSelectedProduct()->getPNumberAsPString() , MSG_INFO);
            // if this is part of a mix, register the tick also for the mix volume (total volume)
            getSelectedProduct()->setVolumeDispensed(getActiveProduct()->getVolumePerTick(true) + getSelectedProduct()->getVolumeDispensed());
        }
    }
    else
    {
        debugOutput::sendMessage("Product volume will not be recorded in this flow state(FLOW_STATE_PRIME_FAIL_OR_EMPTY) Experimental. dispenser ticks: " + std::to_string(m_dispenserTicks), MSG_INFO);
    }
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
    // debugOutput::sendMessage("Dispenser: flofowolfflow : " + std::to_string(getProductVolumeDispensed(getActivePNumber())), MSG_INFO);
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
        debugOutput::sendMessage("Dispenser: No dispense button light. Deprecated ", MSG_WARNING);
    }

    break;
    case pcb::PcbVersion::DSED8344_PIC_MULTIBUTTON:
    {
        debugOutput::sendMessage("Dispenser: No dispense button light. Deprecated ", MSG_WARNING);
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
        debugOutput::sendMessage("Dispenser: Pcb not compatible. ", MSG_WARNING);
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
    return m_pcb->getDispenseButtonStateDebounced(m_slot);
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
    bool found = false;
    debugOutput::sendMessage("Dispenser: Set product solenoid for pnumber: " + to_string(pnumber), MSG_INFO);
    if (pnumber == getBasePNumber())
    {
        debugOutput::sendMessage("Dispenser: Base solenoid. Active: " + to_string(openElseClosed), MSG_INFO);
        m_pcb->setBaseSolenoid(this->m_slot, openElseClosed);
        found = true;
        return;
    }

    int additivePosition = 1;
    while (additivePosition <= m_additive_pnumbers_count && !found)
    {
        if (pnumber == getAdditivePNumber(additivePosition))
        {
            debugOutput::sendMessage("Dispenser: Additive solenoid at additive position: " + to_string(additivePosition) + ". Active: " + to_string(openElseClosed), MSG_INFO);
            m_pcb->setAdditiveSolenoid(this->m_slot, additivePosition, openElseClosed);
            found = true;
        }
        additivePosition++;
    }

    if (!found)
    {
        debugOutput::sendMessage("Dispenser: Solenoid not found!!!!", MSG_ERROR);
    }
}

void dispenser::setActiveProductSolenoid(bool openElseClosed)
{
    setProductSolenoid(getActivePNumber(), openElseClosed);
}

void dispenser::setSpoutSolenoid(bool openElseClosed)
{
    debugOutput::sendMessage("Dispenser: Set spout solenoid. Active: " + to_string(openElseClosed), MSG_ERROR);
    m_pcb->setSpoutSolenoid(this->m_slot, openElseClosed);
}

////////////////////////////////////////////////////////////////////////////
// I/O Pump

// TODO: Refactor Pumping with switch and ternary in future...keep seperate for ease of testing.
// Reverse pump: Turn forward pin HIGH - Reverse pin LOW
DF_ERROR dispenser::setPumpDirectionForward()
{
    DF_ERROR dfRet = OK;
    debugOutput::sendMessage("Dispenser: Pump direction: Forward.", MSG_INFO);
    m_pcb->setPumpDirection(this->m_slot, true);
    return dfRet;
}

// Reverse pump: Turn forward pin LOW - Reverse pin HIGH
DF_ERROR dispenser::setPumpDirectionReverse()
{
    DF_ERROR dfRet = OK;
    if (!m_isPumpReversalEnabled)
    {
        debugOutput::sendMessage("Dispenser: Pump reversal not allowed. Will not execute command.", MSG_WARNING);

        return dfRet;
    }
    debugOutput::sendMessage("Dispenser: Pump direction: reverse", MSG_INFO);
    m_pcb->setPumpDirection(this->m_slot, false);
    return dfRet;
}

// Stops pumping: Turn forward pin LOW - Reverse pin LOW
DF_ERROR dispenser::setPumpsDisableAll()
{
    // m_machine->pcb24VPowerSwitch(false);
    debugOutput::sendMessage("Dispenser: Pump disable: all.", MSG_INFO);
    m_pcb->setPumpsDisableAll();
    m_isSlotEnabled = false;
    isPumpSoftStarting = false;
    pwm_actual_set_speed = 0;
    DF_ERROR dfRet = OK;
    return dfRet;
}

void dispenser::reversePumpForSetTimeMillis(int millis)
{
    if (m_isPumpReversalEnabled)
    {
        if (millis > 0)
        {
            // get volume before
            double volume_before = getActiveProductVolumeDispensed();

            debugOutput::sendMessage("Dispenser: Pump auto retraction. Reverse time millis: " + to_string(millis), MSG_INFO);
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
            debugOutput::sendMessage("Dispenser: Retraction done. WARNING: check volume change correction subtraction. Volume reversed:  " + to_string(volume_diff), MSG_INFO);
        }
        else
        {
            debugOutput::sendMessage("Dispenser: Rectraction disabled. ms:" + to_string(millis), MSG_INFO);
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
    DF_ERROR dfRet = OK;
    return dfRet;
}

void dispenser::setEmptyContainerDetectionEnabled(bool isEnabled)
{
    m_isEmptyContainerDetectionEnabled = isEnabled;
};
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

    DF_ERROR dfRet = OK;
    if (!forwardElseReverse && !m_isPumpReversalEnabled)
    {
        debugOutput::sendMessage("Dispenser: Pump reversal not allowed. Will not execute command.", MSG_WARNING);

        return dfRet;
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
        debugOutput::sendMessage("Dispenser: Pump instant start", MSG_INFO);
        setPumpPWM(target_pwm, true);
        isPumpSoftStarting = false;
    }
    else
    {

        debugOutput::sendMessage("Dispenser: Pump slow start. from " + to_string(pwm_actual_set_speed) + " to " + to_string(target_pwm), MSG_INFO);
        setPumpPWM(0, false);
        isPumpSoftStarting = true;
    }
    return dfRet;
}

DF_ERROR dispenser::pumpSlowStopBlocking()
{
    DF_ERROR dfRet = OK;
    isPumpSoftStarting = false;
    if (pwm_actual_set_speed > 0)
    {
        if (SLOW_STOP_PERIOD_MILLIS == 0 || !m_isPumpSlowStartStopEnabled)
        {
            debugOutput::sendMessage("Dispenser: Pump instant stop", MSG_INFO);
            // no slow stop
        }
        else
        {
            debugOutput::sendMessage("Dispenser: Pump slow stop. from " + to_string(pwm_actual_set_speed) + " to " + to_string(0), MSG_INFO);
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
        debugOutput::sendMessage("Dispenser: Pump was already stopped.", MSG_INFO);
    }
    setPumpsDisableAll();

    return dfRet;
}

DF_ERROR dispenser::setPumpEnable()
{
    // first pump is 1.
    // still needs dispense button to actually get the pump to start
    debugOutput::sendMessage("Dispenser: Pump enable position: " + to_string(this->m_slot), MSG_INFO);
    // m_machine->pcb24VPowerSwitch(true);
    m_pcb->setPumpEnable(this->m_slot); // pump 1 to 4
    m_isSlotEnabled = true;
    DF_ERROR dfRet = OK;
    return dfRet;
}

DF_ERROR dispenser::setPumpPWM(uint8_t value, bool enableLog)
{
    if (enableLog)
    {
        debugOutput::sendMessage("Dispenser: Pump set speed. PWM [0..255]: " + to_string(value), MSG_INFO);
    }
    m_pcb->setPumpPWM((unsigned char)value);
    DF_ERROR dfRet = OK;
    return dfRet;
}

unsigned short dispenser::getPumpSpeed()
{
    return m_pcb->getPumpPWM();
}

////////////////////////////////////////////////////////////
// I/O Flow Sensor

// ------->Also, in case the terminal window is flooded: did you reload all services (developer tools 11)??
#ifdef INTERRUPT_DRIVE_FLOW_SENSOR_TICKS
// TODO: Call this function on Dispense onEntry()
DF_ERROR dispenser::initGlobalFlowsensorIO(int pin)
{
    DF_ERROR e_ret = ERROR_BAD_PARAMS;

    std::string msg = "Dispenser: initGlobalFlowsensorIO. Position: " + std::to_string(getSlot()) + " (pin: " + std::to_string(pin) + ")";
    debugOutput::sendMessage(msg, MSG_INFO);

    m_pFlowsensor = new FSModdyseyx86GPIO();
    m_pFlowsensor->setPinNumber(pin);
    m_pFlowsensor->setPinAsInputElseOutput(true);
    m_pFlowsensor->registerProduct(getSelectedProduct());

    // enable global interrupt driven flowsensor ticks, the alternative is polling over i2c

    m_pFlowsensor->startListener_flowsensor();
    e_ret = OK;

    return e_ret;
}
#endif

////////////////////////////////////////////////////////////
// Flow

// double dispenser::getProductVolumeDeltaAndReset()
// {
//     // will get volumeDelta since last call of this function

//     double currentVolume = getActiveProductVolumeDispensed();
//     double deltaVolume = currentVolume - previousDispensedVolume;
//     previousDispensedVolume = currentVolume;
//     return deltaVolume;
// }

void dispenser::initProductFlowRateCalculation()
{
    flowRateBufferIndex = 0;
    for (uint16_t i = 0; i < RUNNING_AVERAGE_WINDOW_LENGTH; i++)
    {
        flowRateBuffer[i].time_millis = MILLIS_INIT_DUMMY;
        flowRateBuffer[i].value = 0;
    }
}
// double dispenser::getProductFlowRateInstantaneous()
// {
//     using namespace std::chrono;
//     uint64_t millis_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
//     double volumeDelta = getProductVolumeDeltaAndReset();
//     double flowRate = 0;
//     if (millisAtLastCheck != MILLIS_INIT_DUMMY)
//     {
//         uint64_t millisDelta = millis_since_epoch - millisAtLastCheck;
//         // debugOutput::sendMessage("Avg flow rate: millisdelta: " + to_string(millisDelta), MSG_INFO);
//         // debugOutput::sendMessage("Avg flow rate: volume delta: " + to_string(volumeDelta), MSG_INFO);
//         flowRate = (volumeDelta / millisDelta) * 1000.0; // volume per second.
//     }
//     else
//     {
//         flowRate = 0;
//     }

//     millisAtLastCheck = millis_since_epoch;
//     return flowRate;
// }

Time_val dispenser::createAndGetActiveProductVolumeDispensedDatapoint()
{
    Time_val tv;
    // tv.value =  getActiveProductVolumeDispensed();
    tv.value = m_dispenserTicks * getActiveProduct()->getVolumePerTick(true);

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

const char *dispenser::getDispenseStatusAsString()
{
    Dispense_behaviour behaviour = getDispenseStatus();
    const char *behaviour_str = DISPENSE_BEHAVIOUR_STRINGS[behaviour];
    return behaviour_str;
}

string dispenser::getDispenseUpdateString()
{
    // string intro =
    string message = "Dispenser: Dispense Product: P-" + std::to_string(getSelectedPNumber());

    // add selected product total volume update.
    message += " (" + std::to_string(getSelectedProduct()->getVolumeDispensed()) + "/" + std::to_string(getProductTargetVolume(getSelectedPNumber())) + "ml ): ";

    if (getSelectedProduct()->isMixingProduct())
    {
#ifndef ENABLE_PARALLEL_MIX
        message +=
            "(mix part " + std::to_string(getSelectedProduct()->getMixProductsCount() - m_mix_active_index) + "/" + std::to_string(getSelectedProduct()->getMixProductsCount()) +
            " : P-" + std::to_string(getActivePNumber()) + " volume: " + std::to_string(getActiveProduct()->getVolumeDispensed()) + "/" + std::to_string(getProductTargetVolume(getActivePNumber())) + "ml )";

        ;
#else
        message +=
            "all parts mixed simultaneously.";

        ;
#endif
    }
    else
    {
        message +=
            "non mix. ";
        // message +=
        //     "non mix. (P-active=P-dispense): volume: " + std::to_string(getActiveProduct()->getVolumeDispensed()) + "/" + std::to_string(getProductTargetVolume(getActivePNumber())) + "ml";
    }
    return message;
}

// const char *dispenser::getSlotStateAsString()
// {
//     Slot_state state = getSlotState();
//     const char *state_str = SLOT_STATE_STRINGS[state];
//     return state_str;
// }

// void dispenser::setSlotState(Slot_state state)
// {
//     m_slot_state = state;
//     debugOutput::sendMessage("Slot state set to  : " + std::string(getSlotStateAsString()), MSG_INFO);
// }

// Slot_state dispenser::getSlotState()
// {
//     return m_slot_state;
// }

Dispense_behaviour dispenser::getDispenseStatus()
{
    return m_dispense_state;
}

void dispenser::updateDispenseStatus()
{
    Time_val avg = getAveragedProductFlowRate(EMPTY_CONTAINER_DETECTION_FLOW_AVERAGE_WINDOW_MILLIS);

    // debugOutput::sendMessage("minimummmmm: " + to_string(getSelectedProduct()->getThresholdFlow()), MSG_INFO);
    // debugOutput::sendMessage("maximummmmm ml/s: " + to_string(getSelectedProduct()->getThresholdFlow_max_allowed()), MSG_INFO);

    if (!getDispenseButtonValue())
    {
        // button not pressed --> not pumping

        m_dispense_state = FLOW_STATE_NOT_PUMPING_NOT_DISPENSING;
    }

    else if ((getButtonPressedCurrentPressMillis() <= (2 * EMPTY_CONTAINER_DETECTION_FLOW_AVERAGE_WINDOW_MILLIS)) &&
             (avg.value < getSelectedProduct()->getThresholdFlow() || avg.value >= getSelectedProduct()->getThresholdFlow_max_allowed()))
    {
        // flow rate needs to be ramped up for the first moving average window to be large enough

        // during init time for ramp up
        // no valid flow rate detected

        m_dispense_state = FLOW_STATE_RAMP_UP;
    }

    else if (avg.value >= getSelectedProduct()->getThresholdFlow() && avg.value <= getSelectedProduct()->getThresholdFlow_max_allowed())
    {
        // button pressed (aka pumping)
        // valid flow rate detected
        // init time long enough for valid data

        m_dispense_state = FLOW_STATE_DISPENSING;
    }

    // else if (m_previous_dispense_state == FLOW_STATE_DISPENSING)
    // {
    //     // button pressed (aka pumping)
    //     // init time long enough for valid data
    //     // no flow detected
    //     if (avg.value < getSelectedProduct()->getThresholdFlow())
    //     {
    //         debugOutput::sendMessage("Dispenser: Below threshold of minimum flow rate. minimum flow rate: " + to_string(getSelectedProduct()->getThresholdFlow()), MSG_INFO);
    //     }
    //     else if (avg.value >= getSelectedProduct()->getThresholdFlow_max_allowed())
    //     {
    //         debugOutput::sendMessage("Dispenser: Exceeds threshold of maximum flow rate. maximum flow rate: " + to_string(getSelectedProduct()->getThresholdFlow_max_allowed()), MSG_INFO);
    //     }
    //     else
    //     {
    //         debugOutput::sendMessage("Dispenser: Erroneous dispenser state: " + to_string(m_slot_state), MSG_INFO);
    //     }

    //     // once it was dispensing, empty dispenser is detected immediatly if no product flows.
    //     // bugfix: if the button was release and repressed, the average was not correct at restart
    //     //          --> take into account. at top level (FLOW_STATE_UNAVAILABLE)
    //     m_dispense_state = FLOW_STATE_EMPTY;
    // }
    else if (getButtonPressedTotalMillis() > EMPTY_CONTAINER_DETECTION_MAXIMUM_PRIME_TIME_MILLIS
        // || m_previous_dispense_state == FLOW_STATE_DISPENSING
        )
    {

        // button pressed (aka pumping)
        // init time long enough for valid data
        // no flow detected
        // if the previous state was dispensing, skip the timeout step 
        m_dispense_state = FLOW_STATE_PRIME_FAIL_OR_EMPTY;
    }
    else
    {
        // button pressed (aka pumping)
        // init time long enough for valid data
        // no flow detected
        // not timed out

        m_dispense_state = FLOW_STATE_PRIMING_OR_EMPTY;
    }

    if (m_previous_dispense_state != m_dispense_state)
    {
        const char *s_previous_state = DISPENSE_BEHAVIOUR_STRINGS[m_previous_dispense_state];
        const char *s_state_now = DISPENSE_BEHAVIOUR_STRINGS[m_dispense_state];
        debugOutput::sendMessage("Flow state change (from " + std::string(s_previous_state) + " to " + std::string(s_state_now) + ") . Flow rate at change: " + std::to_string(avg.value), MSG_INFO);
        debugOutput::sendMessage("Flow rate limits [ml/s] : min " + std::to_string(getSelectedProduct()->getThresholdFlow()) + " max: " + std::to_string(getSelectedProduct()->getThresholdFlow_max_allowed()) , MSG_INFO);
        debugOutput::sendMessage("button pressed time: current " + std::to_string(getButtonPressedCurrentPressMillis()) + " total (during this dispense): " + std::to_string(getButtonPressedTotalMillis()) , MSG_INFO);
    }

    m_previous_dispense_state = m_dispense_state;
}

// void dispenser::updateActiveProductState()
// {
//     //getActiveProduct()->updateProductState(getDispenseStatus(), m_isEmptyContainerDetectionEnabled);

//     switch (getDispenseStatus())
//     {
//         case FLOW_STATE_UNAVAILABLE:
//         {
//             break;
//         }
//         case FLOW_STATE_RAMP_UP:
//         {
//             break;
//         }
//         case FLOW_STATE_DISPENSING:
//         {
//             break;
//         }
//         case FLOW_STATE_PUMPING_NOT_DISPENSING:
//         {
//             break;
//         }
//         case FLOW_STATE_NOT_PUMPING_NOT_DISPENSING:
//         {
//             break;
//         }
//         case FLOW_STATE_PRIMING_OR_EMPTY:
//         {
//             break;
//         }
//         case FLOW_STATE_PRIME_FAIL_OR_EMPTY:
//         {
//             break;
//         }
//         case FLOW_STATE_EMPTY:
//         {
//             break;
//         }

//     // case PRODUCT_STATE_NOT_PRIMED:
//     // {
//     //     if (dispenseState == FLOW_STATE_EMPTY)
//     //     {
//     //         setProductStateToEmpty(isEmptyContainerDetectionEnabled);
//     //     }
//     //     if (dispenseState == FLOW_STATE_PRIME_FAIL_OR_EMPTY)
//     //     {
//     //         setProductState(PRODUCT_STATE_PROBLEM_NEEDS_ATTENTION);
//     //     }

//     //     if (dispenseState == FLOW_STATE_NOT_PUMPING_NOT_DISPENSING)
//     //     {
//     //         setProductState(PRODUCT_STATE_AVAILABLE);
//     //     }

//     //     if (dispenseState == FLOW_STATE_DISPENSING)
//     //     {
//     //         setProductState(PRODUCT_STATE_AVAILABLE);
//     //     }
//     //     break;
//     // }
//     // case PRODUCT_STATE_AVAILABLE:
//     // {

//     //     if (dispenseState == FLOW_STATE_PRIMING_OR_EMPTY)
//     //     {
//     //         setProductState(PRODUCT_STATE_NOT_PRIMED);
//     //     }
//     //     if (dispenseState == FLOW_STATE_EMPTY)
//     //     {
//     //         setProductStateToEmpty(isEmptyContainerDetectionEnabled);
//     //     }
//     //     if (dispenseState == FLOW_STATE_PRIME_FAIL_OR_EMPTY)
//     //     {
//     //         setProductState(PRODUCT_STATE_PROBLEM_NEEDS_ATTENTION);
//     //     }

//     //     break;
//     // }
//     // case PRODUCT_STATE_AVAILABLE_LOW_STOCK:
//     // {
//     //     if (dispenseState == FLOW_STATE_EMPTY)
//     //     {
//     //         setProductStateToEmpty(isEmptyContainerDetectionEnabled);
//     //     }
//     //     if (dispenseState == FLOW_STATE_PRIME_FAIL_OR_EMPTY)
//     //     {
//     //         setProductState(PRODUCT_STATE_PROBLEM_NEEDS_ATTENTION);
//     //     }
//     //     break;
//     // }
//     // case PRODUCT_STATE_PROBLEM_NEEDS_ATTENTION:
//     // {
//     //     if (dispenseState == FLOW_STATE_DISPENSING)
//     //     {
//     //         setProductState(PRODUCT_STATE_AVAILABLE);
//     //     }
//     //     break;
//     // }
//     // case PRODUCT_STATE_PROBLEM_EMPTY:
//     // {
//     //     if (dispenseState == FLOW_STATE_DISPENSING)
//     //     {
//     //         setProductState(PRODUCT_STATE_AVAILABLE);
//     //     }
//     //     break;
//     // }
//     // case PRODUCT_STATE_DISABLED_COMING_SOON:
//     // {
//     //     break;
//     // }
//     // case PRODUCT_STATE_DISABLED:
//     // {
//     //     // do nothing can only be altered when set to enabled
//     //     break;
//     // }

//     default:
//     {
//         debugOutput::sendMessage("Dispenser: Erroneous dispenser state: ", MSG_INFO);  // std::string(getDispenseStatus())
//     }
//     }
// }

void dispenser::setMixDispenseReport(std::string pNumber, double volumeDispensed, double volume_remaining)
{
    m_mixDispenseReport.insert({pNumber, {volumeDispensed, volume_remaining}});
}

std::map<std::string, std::vector<double>> dispenser::getMixDispenseReport()
{
    return m_mixDispenseReport;
}

void dispenser::resetMixDispenseReport()
{
    m_mixDispenseReport = {};
}
