//***************************************
//
// debugOutput.cpp
// DEBUG output Implementation:
// threadsafe outputs to console and
// override for other systems
//
// created: 15-06-2020
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "debugOutput.h"
#include "machine.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <ctime>

#include <sstream>
#include <iomanip>


using namespace std;

machine::machine()
{
    m_button_lights_behaviour = Button_lights_behaviour::IDLE_ANIMATION_FROM_DB;
    // m_button_lights_behaviour = Button_lights_behaviour::IDLE_OFF;
    m_button_animation_program = 0;
}

void machine::setup(product *pnumbers)
{
    control_pcb = new pcb();
    m_pnumbers = pnumbers;
    receipt_printer = new Adafruit_Thermal();
    control_pcb->setup();
    control_pcb->setPumpPWM(DEFAULT_PUMP_PWM);
    // the 24V power has a master on/off switch
    switch_24V = new FSModdyseyx86GPIO(IO_PIN_ENABLE_24V);
    power24VEnabled = false;
    switch_24V->setPinAsInputElseOutput(false); // set as output
    syncSoftwareVersionWithDb();
    initProductDispensers();
    loadGeneralProperties();
}

int machine::getDispensersCount()
{
    // slots, dispensers, lines,... it's all the same
    return 4;
}

void machine::initProductDispensers()
{
    for (int slot_index = 0; slot_index < getDispensersCount(); slot_index++)
    {
        debugOutput::sendMessage("Init dispenser " + to_string(slot_index + 1), MSG_INFO);
        // m_g_machine.m_productDispensers[slot_index].setup(&this, g_pnumbers);
        m_productDispensers[slot_index].setup(control_pcb, m_pnumbers);
        m_productDispensers[slot_index].setSlot(slot_index + 1);
        m_productDispensers[slot_index].initGlobalFlowsensorIO(IO_PIN_FLOW_SENSOR);
        setFlowSensorCallBack(slot_index + 1);

        debugOutput::sendMessage("TEMPORARY HACK: base number is the selected product at startup. ", MSG_INFO);
        m_productDispensers[slot_index].setBasePNumberAsSelectedProduct();
    }
}

void machine::loadGeneralProperties()
{
    loadMachineParametersFromDb();
    usleep(20000);
    for (int slot_index = 0; slot_index < getDispensersCount(); slot_index++)
    {
        m_productDispensers[slot_index].loadGeneralProperties();
    }
}

machine::HardwareVersion machine::getHardwareVersion()
{
    return m_hardware_version;
}

// Function to convert string to HardwareVersion
void machine::setHardwareVersionFromString(const std::string &version)
{
    static const std::map<std::string, HardwareVersion> versionMap = {
        {"SS0.9", SS09},
        {"SS1", SS1},
        {"SS2", SS2},
        {"AP1", AP1},
        {"AP2", AP2}};

    auto it = versionMap.find(version);
    if (it != versionMap.end())
    {
        m_hardware_version = it->second;
    }
    else
    {
        m_hardware_version = UNKNOWN;
    }
}

pcb *machine::getPcb()
{
    return control_pcb;
}
// void machine::testtest(){
// // nothing here.
//     debugOutput::sendMessage("*** global machine test message", MSG_INFO);
// }

void machine::refresh()
{
    control_pcb->pcb_refresh();
    // the pcb inputs are not interrupt driven. So, periodical updates are required
    for (uint8_t slot_index = 0; slot_index < PRODUCT_DISPENSERS_MAX; slot_index++)
    {
        m_productDispensers[slot_index].refresh();
    }
}

void machine::setFlowSensorCallBack(int slot)
{
    // CALL THIS FOR EVERY ACTIVE PRODUCT CHANGE during a mixing dispense.

    int slot_index = slot -1 ;
    // control_pcb->registerFlowSensorTickCallback(std::bind(&dispenser::registerFlowSensorTickCallback, &m_productDispensers[slot_index]));
    m_productDispensers[slot_index].linkActiveProductVolumeUpdate();
}

void machine::syncSoftwareVersionWithDb()
{
    string sql_string = "UPDATE machine SET software_version_controller=\"" + std::string(CONTROLLER_VERSION) + "\";";
    executeSQLStatement(sql_string);
}

void machine::executeSQLStatement(string sql_string)
{
    rc = sqlite3_open(CONFIG_DB_PATH, &db);
    sqlite3_stmt *stmt;
    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);
    int status;
    status = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

string machine::getMachineId()
{
    return m_machine_id;
}
// void machine::loadButtonPropertiesFromDb()
// {
//     rc = sqlite3_open(CONFIG_DB_PATH, &db);
//     sqlite3_stmt *stmt;
//     string sql_string = "SELECT dispense_buttons_count FROM machine";
//     sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);
//     sqlite3_step(stmt);

//     int val = sqlite3_column_int(stmt, 0);

//     // button light effect program
//     m_button_animation_program = val / 1000;

//     // button count
//     int buttons_count = val % 1000;

//     sqlite3_finalize(stmt);
//     sqlite3_close(db);
//     // val = 4;
//     if (buttons_count == 1)
//     {
//         m_isMultiButtonEnabled = false;
//     }
//     else if (buttons_count == 4)
//     {
//         m_isMultiButtonEnabled = true;
//     }
//     else
//     {
//         m_isMultiButtonEnabled = false;
//         debugOutput::sendMessage("ASSERT Error: unimplemented number of dispense buttons. Default to single dispense button. Buttons indicated in db:" + to_string(m_isMultiButtonEnabled), MSG_ERROR);
//     }

//     debugOutput::sendMessage("Multiple dispense buttons enabled? : " + to_string(m_isMultiButtonEnabled), MSG_INFO);
//     debugOutput::sendMessage("Animation program number (0=no animation)? : " + to_string(m_button_animation_program), MSG_INFO);
// }

int machine::getButtonAnimationProgram()
{
    // 0 is no animation.
    return m_button_animation_program;
}

bool machine::getMultiDispenseButtonEnabled()
{
    return m_isMultiButtonEnabled;
}

void machine::setButtonLightsBehaviour(Button_lights_behaviour behaviour)
{
    m_button_lights_behaviour = behaviour;
}

void machine::resetButtonLightAnimation()
{
    using namespace std::chrono;
    uint64_t m_lights_animation_most_recent_step_millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    control_pcb->setSingleDispenseButtonLight(1, false);
    control_pcb->setSingleDispenseButtonLight(2, false);
    control_pcb->setSingleDispenseButtonLight(3, false);
    control_pcb->setSingleDispenseButtonLight(4, false);

    m_lights_animation_step = 0;
}

void machine::refreshButtonLightAnimation()
{

    if (m_button_lights_behaviour != m_button_lights_behaviour_memory)
    {
        resetButtonLightAnimation();
    }

    switch (m_button_lights_behaviour)
    {
    case IDLE_OFF:
    {
    }
    break;
    case IDLE_ANIMATION_FROM_DB:
    {
        if (getButtonAnimationProgram() == 1)
        {
            refreshButtonLightAnimationPingPong();
        }
        else if (getButtonAnimationProgram() == 2)
        {
            refreshButtonLightAnimationCaterpillar();
        }
    }
    break;
    default:
    {
        // assert error
    }
    }
    m_button_lights_behaviour_memory = m_button_lights_behaviour;
}

void machine::setMultiDispenseButtonLight(int slot, bool enableElseDisable)
{
    // output has to be set low for light to be on.
    debugOutput::sendMessage("slot light: " + to_string(slot) + "on else off: " + to_string(enableElseDisable), MSG_INFO);

    if (control_pcb->get_pcb_version() == pcb::PcbVersion::DSED8344_PIC_MULTIBUTTON)
    {
        // if (getMultiDispenseButtonEnabled())
        // {

        //     if (slot == 4)
        //     {
        //         m_pDispenseButton4[0]->writePin(!enableElseDisable);
        //     }
        //     else
        //     {

        //         this->control_pcb->setSingleDispenseButtonLight(slot, enableElseDisable);
        //     }
        // }
        // else
        // {
        //     this->control_pcb->setSingleDispenseButtonLight(1, enableElseDisable);
        // }

        debugOutput::sendMessage("ASSERT ERROR: functionality gone! Deprecated. ", MSG_ERROR);
    }
    else if (control_pcb->get_pcb_version() == pcb::PcbVersion::DSED8344_NO_PIC)
    {
        this->control_pcb->setSingleDispenseButtonLight(1, enableElseDisable);
    }
    else
    {
        this->control_pcb->setSingleDispenseButtonLight(slot, enableElseDisable);
    }
}

void machine::refreshButtonLightAnimationCaterpillar()
{

    using namespace std::chrono;
    uint64_t millis_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    uint64_t step_delta_millis = 250;

    if (millis_since_epoch > m_lights_animation_most_recent_step_millis + step_delta_millis)
    {
        m_lights_animation_step++;
        m_lights_animation_most_recent_step_millis = millis_since_epoch;
        if (m_lights_animation_step > 15)
        {
            m_lights_animation_step = 0;
        }

        switch (m_lights_animation_step)
        {
        case 0:
        {
            // control_pcb->setSingleDispenseButtonLight(2, false);
            control_pcb->setSingleDispenseButtonLight(1, true);
        }
        break;
        case 1:
        {
            // control_pcb->setSingleDispenseButtonLight(1, false);
            control_pcb->setSingleDispenseButtonLight(2, true);
        }
        break;
        case 2:
        {
            // control_pcb->setSingleDispenseButtonLight(2, false);
            control_pcb->setSingleDispenseButtonLight(3, true);
        }
        break;
        case 3:
        {
            // control_pcb->setSingleDispenseButtonLight(3, false);
            control_pcb->setSingleDispenseButtonLight(4, true);
        }
        break;
        case 4:
        {
            control_pcb->setSingleDispenseButtonLight(1, false);
            // control_pcb->setSingleDispenseButtonLight(3, true);
        }
        break;
        case 5:
        {
            control_pcb->setSingleDispenseButtonLight(2, false);
            // control_pcb->setSingleDispenseButtonLight(2, true);
        }
        break;
        case 6:
        {
            control_pcb->setSingleDispenseButtonLight(3, false);
            // control_pcb->setSingleDispenseButtonLight(2, true);
        }
        break;
        case 7:
        {
            control_pcb->setSingleDispenseButtonLight(4, false);
            // control_pcb->setSingleDispenseButtonLight(2, true);
        }
        break;
        case 8:
        {
            // control_pcb->setSingleDispenseButtonLight(2, false);
            control_pcb->setSingleDispenseButtonLight(4, true);
        }
        break;
        case 9:
        {
            // control_pcb->setSingleDispenseButtonLight(1, false);
            control_pcb->setSingleDispenseButtonLight(3, true);
        }
        break;
        case 10:
        {
            // control_pcb->setSingleDispenseButtonLight(2, false);
            control_pcb->setSingleDispenseButtonLight(2, true);
        }
        break;
        case 11:
        {
            // control_pcb->setSingleDispenseButtonLight(3, false);
            control_pcb->setSingleDispenseButtonLight(1, true);
        }
        break;
        case 12:
        {
            control_pcb->setSingleDispenseButtonLight(4, false);
            // control_pcb->setSingleDispenseButtonLight(3, true);
        }
        break;
        case 13:
        {
            control_pcb->setSingleDispenseButtonLight(3, false);
            // control_pcb->setSingleDispenseButtonLight(2, true);
        }
        break;
        case 14:
        {
            control_pcb->setSingleDispenseButtonLight(2, false);
            // control_pcb->setSingleDispenseButtonLight(2, true);
        }
        break;
        case 15:
        {
            control_pcb->setSingleDispenseButtonLight(1, false);
            // control_pcb->setSingleDispenseButtonLight(2, true);
        }
        break;
        default:
        {
            control_pcb->setSingleDispenseButtonLight(1, false);
            control_pcb->setSingleDispenseButtonLight(2, false);
            control_pcb->setSingleDispenseButtonLight(3, false);
            control_pcb->setSingleDispenseButtonLight(4, false);
        }
        break;
        }
    }
}

// ping pong
void machine::refreshButtonLightAnimationPingPong()
{

    using namespace std::chrono;
    uint64_t millis_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    uint64_t step_delta_millis = 250;

    if (millis_since_epoch > m_lights_animation_most_recent_step_millis + step_delta_millis)
    {
        m_lights_animation_step++;
        m_lights_animation_most_recent_step_millis = millis_since_epoch;
        if (m_lights_animation_step > 5)
        {
            m_lights_animation_step = 0;
        }

        switch (m_lights_animation_step)
        {
        case 0:
        {
            control_pcb->setSingleDispenseButtonLight(2, false);
            control_pcb->setSingleDispenseButtonLight(1, true);
        }
        break;
        case 1:
        {
            control_pcb->setSingleDispenseButtonLight(1, false);
            control_pcb->setSingleDispenseButtonLight(2, true);
        }
        break;
        case 2:
        {
            control_pcb->setSingleDispenseButtonLight(2, false);
            control_pcb->setSingleDispenseButtonLight(3, true);
        }
        break;
        case 3:
        {
            control_pcb->setSingleDispenseButtonLight(3, false);
            control_pcb->setSingleDispenseButtonLight(4, true);
        }
        break;
        case 4:
        {
            control_pcb->setSingleDispenseButtonLight(4, false);
            control_pcb->setSingleDispenseButtonLight(3, true);
        }
        break;
        case 5:
        {
            control_pcb->setSingleDispenseButtonLight(3, false);
            control_pcb->setSingleDispenseButtonLight(2, true);
        }
        break;
        default:
        {
            control_pcb->setSingleDispenseButtonLight(1, false);
            control_pcb->setSingleDispenseButtonLight(2, false);
            control_pcb->setSingleDispenseButtonLight(3, false);
            control_pcb->setSingleDispenseButtonLight(4, false);
        }
        break;
        }
    }
}

bool machine::getPcb24VPowerSwitchStatus()
{
    return power24VEnabled;
}

void machine::pcb24VPowerSwitch(bool enableElseDisable)
{
    if (power24VEnabled != enableElseDisable)
    {
        power24VEnabled = enableElseDisable;
        switch_24V->writePin(power24VEnabled);
    }
}

void machine::print_receipt(string name_receipt, string receipt_cost, string receipt_volume_formatted, string time_stamp, string char_units_formatted, string paymentMethod, string plu, string promoCode, bool sleep_until_printed)
{
    if (sleep_until_printed)
    {
        usleep(1500000);
    }

    std::string out1 = name_receipt + "\nPrice: $" + receipt_cost + " \nQuantity: " + receipt_volume_formatted + "\nTime: " + time_stamp;
    receipt_printer->printText(out1.c_str());

    debugOutput::sendMessage("Print receipt", MSG_INFO);
    debugOutput::sendMessage("Price" + receipt_cost, MSG_INFO);
    if (receipt_cost == "0.00")
    {
        debugOutput::sendMessage("Free Order", MSG_INFO);
        std::string out2 = "Promo Used: " + promoCode;
        receipt_printer->printText(out2.c_str());
        std::string out3 = "Enjoy your free product!";
        receipt_printer->printText(out3.c_str());
        std::string out4 = "Thank you for supporting \npackage-free!";
        receipt_printer->printText(out4.c_str());
    }
    else if (paymentMethod == "barcode" || paymentMethod == "barcode_EAN-13" || paymentMethod == "barcode_EAN-2")
    {

        if (plu.size() != 13 && plu.size() != 12)
        {
            // EAN13 codes need to be 13 digits, or else no barcode will be printed. If 12 dgits are provided, the last digit (checksum?!) is automatically generated
            debugOutput::sendMessage("ERROR: bar code invalid (" + plu + "). EAN13, Should be 13 digits" + to_string(plu.size()), MSG_INFO);

            std::string out = "\nPLU: " + plu + " (No barcode available)";
            receipt_printer->printText(out.c_str());
        }
        else
        {
            debugOutput::sendMessage("Barcode:" + plu, MSG_INFO);

            // receipt_printer->connectToPrinter();
            // receipt_printer->setBarcodeHeight(100);
            // receipt_printer->printBarcode(plu.c_str(), EAN13);
            // receipt_printer->disconnectPrinter();
            Adafruit_Thermal *printerr = new Adafruit_Thermal();
            printerr->connectToPrinter();
            printerr->setBarcodeHeight(100);
            printerr->printBarcode(plu.c_str(), EAN13);
            printerr->disconnectPrinter();
        }
    }

    else if (paymentMethod == "plu")
    {

        if (plu.size() != 13 && plu.size() != 12)
        {
            // EAN13 codes need to be 13 digits, or else no barcode will be printed. If 12 dgits are provided, the last digit (checksum?!) is automatically generated
            debugOutput::sendMessage("PLU without barcode:" + plu, MSG_INFO);
            std::string out = "PLU: " + plu;
            receipt_printer->printText(out.c_str());
        }
        else
        {
            debugOutput::sendMessage("PLU as barcode:" + plu, MSG_INFO);
            // // receipt_printer->connectToPrinter();
            // receipt_printer->setBarcodeHeight(100);
            // receipt_printer->printBarcode(plu.c_str(), EAN13);
            // // receipt_printer->disconnectPrinter();

            Adafruit_Thermal *printerr = new Adafruit_Thermal();
            printerr->connectToPrinter();
            printerr->setBarcodeHeight(100);
            printerr->printBarcode(plu.c_str(), EAN13);
            printerr->disconnectPrinter();
        }
    }
    else
    {
        debugOutput::sendMessage("ERROR: Not a valid payment method" + paymentMethod, MSG_INFO);
    }

    const char *out = "\n\n";
    receipt_printer->printText(out);

    if (sleep_until_printed)
    {

        usleep(3500000);
    }
    debugOutput::sendMessage("end sleep", MSG_INFO);
}

bool machine::getPumpReversalEnabled()
{
    // return m_isPumpReversalEnabled;
    return m_enable_pump_reversal;
}
bool machine::getPumpSlowStartStopEnabled()
{
    return m_enable_pump_ramping;
}

bool machine::getEmptyContainerDetectionEnabled()
{
    return m_has_empty_detection;
}

int machine::convertPStringToPNumber(const std::string &inputString)
{

    // P-xxx to xxx   e.g. P-12  --> 12
    // Check if the input string starts with "P-"
    if (inputString.find("P-") == 0)
    {
        // Extract the substring after "P-"
        std::string numberStr = inputString.substr(2);

        int number = 0;
        // Iterate through the characters of the substring and convert to integer
        for (char digitChar : numberStr)
        {
            // Check if the character is a valid digit
            if (isdigit(digitChar))
            {
                // Convert the character to integer and update the number
                number = number * 10 + (digitChar - '0');
            }
            else
            {
                // If a non-digit character is encountered, return -1 (invalid input)
                return -1;
            }
        }

        // Check if the number is within the valid range (0 to 9999)
        if (number >= 0 && number <= 9999)
        {
            return number;
        }
    }

    // Return -1 to indicate an invalid input or number out of range
    return -1;
}

void machine::loadMachineParametersFromDb()
{
    debugOutput::sendMessage("Machine load db par", MSG_INFO);

    int rc = sqlite3_open(CONFIG_DB_PATH, &db);
    sqlite3_stmt *stmt;
    string sql_string = "SELECT "
                        "machine_id,"
                        "soapstand_customer_id,"
                        "template,"
                        "location,"
                        "controller_type,"
                        "controller_id,"
                        "screen_type,"
                        "screen_id,"
                        "has_receipt_printer,"
                        "receipt_printer_is_online,"
                        "receipt_printer_has_paper,"
                        "has_tap_payment,"
                        "hardware_version,"
                        "software_version,"
                        "aws_port,"
                        "coupons_enabled,"
                        "has_empty_detection,"
                        "enable_pump_ramping,"
                        "enable_pump_reversal,"
                        "dispense_buttons_count,"
                        "maintenance_pwd,"
                        "show_transactions,"
                        "help_text_html,"
                        "idle_page_type,"
                        "admin_pwd,"
                        "alert_temperature,"
                        "software_version_controller,"
                        "is_enabled,"
                        "status_text"
                        " FROM machine"
                        ";";

    // string sql_string = "SELECT dispense_buttons_count FROM machine";

    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);

    int status;
    status = sqlite3_step(stmt);

    int numberOfRecordsFound = 0;
    string m_hardware_version_str;

    while (status == SQLITE_ROW)
    {
        numberOfRecordsFound++;

        m_machine_id = product::dbFieldAsValidString(stmt, 0);
        m_soapstand_customer_id = sqlite3_column_int(stmt, 1);
        m_template = product::dbFieldAsValidString(stmt, 2);
        m_location = product::dbFieldAsValidString(stmt, 3);
        m_controller_type = product::dbFieldAsValidString(stmt, 4);
        m_controller_id = product::dbFieldAsValidString(stmt, 5);
        m_screen_type = product::dbFieldAsValidString(stmt, 6);
        m_screen_id = product::dbFieldAsValidString(stmt, 7);
        m_has_receipt_printer = sqlite3_column_int(stmt, 8);
        m_receipt_printer_is_online = sqlite3_column_int(stmt, 9);
        m_receipt_printer_has_paper = sqlite3_column_int(stmt, 10);
        m_has_tap_payment = sqlite3_column_int(stmt, 11);
        m_hardware_version_str = product::dbFieldAsValidString(stmt, 12);
        m_software_version = product::dbFieldAsValidString(stmt, 13);
        m_aws_port = sqlite3_column_int(stmt, 14);
        m_coupons_enabled = sqlite3_column_int(stmt, 15);
        m_has_empty_detection = sqlite3_column_int(stmt, 16);
        m_enable_pump_ramping = sqlite3_column_int(stmt, 17);
        m_enable_pump_reversal = sqlite3_column_int(stmt, 18);
        m_dispense_buttons_count = sqlite3_column_int(stmt, 19);
        m_maintenance_pwd = product::dbFieldAsValidString(stmt, 20);
        m_show_transactions = sqlite3_column_int(stmt, 21);
        m_help_text_html = product::dbFieldAsValidString(stmt, 22);
        m_idle_page_type = product::dbFieldAsValidString(stmt, 23);
        m_admin_pwd = product::dbFieldAsValidString(stmt, 24);
        m_alert_temperature = product::dbFieldAsValidString(stmt, 25);
        m_software_version_controller = product::dbFieldAsValidString(stmt, 26);
        m_is_enabled = sqlite3_column_int(stmt, 27);
        m_status_text = product::dbFieldAsValidString(stmt, 28);

        if (numberOfRecordsFound > 1)
        {
            // assert error
            debugOutput::sendMessage("ASSERT Error: Machine table must have exactly one row. Found rows: " + std::to_string(numberOfRecordsFound), MSG_ERROR);
        }

        m_button_animation_program = m_dispense_buttons_count / 1000; // button light effect program
        int buttons_count = m_dispense_buttons_count % 1000;          // button count

        if (buttons_count == 1)
        {
            m_isMultiButtonEnabled = false;
        }
        else if (buttons_count == 4)
        {
            m_isMultiButtonEnabled = true;
        }
        else
        {
            m_isMultiButtonEnabled = false;
            debugOutput::sendMessage("ASSERT Error: unimplemented number of dispense buttons. Default to single dispense button. Buttons indicated in db:" + to_string(m_isMultiButtonEnabled), MSG_ERROR);
        }

        debugOutput::sendMessage("Multiple dispense buttons enabled? : " + to_string(m_isMultiButtonEnabled), MSG_INFO);
        debugOutput::sendMessage("Animation program number (0=no animation)? : " + to_string(m_button_animation_program), MSG_INFO);
        setHardwareVersionFromString(m_hardware_version_str);
        status = sqlite3_step(stmt); // next record
    }
    debugOutput::sendMessage("Machine load db: finished. status: " + to_string(status), MSG_INFO);
}