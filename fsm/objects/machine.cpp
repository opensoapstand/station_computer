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

void machine::setup()
{
    // if ((the_pcb->get_pcb_version() == pcb::PcbVersion::DSED8344_PIC_MULTIBUTTON) && this->slot == 4)
    // {
    //     m_pDispenseButton4[0]->writePin(!enableElseDisable);
    // }
    // else
    // {
    //     this->the_pcb->setSingleDispenseButtonLight(slot, enableElseDisable);
    // }
    //  if (control_pcb == nullptr)
    // {
    control_pcb = new pcb();
    // }

    receipt_printer = new Adafruit_Thermal();
    control_pcb->setup();
    control_pcb->setPumpPWM(DEFAULT_PUMP_PWM);
    // the 24V power has a master on/off switch
    switch_24V = new oddyseyx86GPIO(IO_PIN_ENABLE_24V);
    power24VEnabled = false;
    switch_24V->setPinAsInputElseOutput(false); // set as output
    syncSoftwareVersionWithDb();
}

void machine::loadGeneralProperties()
{
    loadButtonPropertiesFromDb();
    usleep(20000);
}

pcb *machine::getPcb()
{
    return control_pcb;
}
// void machine::testtest(){
// // nothing here.
//     debugOutput::sendMessage("*** global machine test message", MSG_INFO);
// }

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

void machine::loadButtonPropertiesFromDb()
{
    rc = sqlite3_open(CONFIG_DB_PATH, &db);
    sqlite3_stmt *stmt;
    string sql_string = "SELECT dispense_buttons_count FROM machine";
    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);

    int val = sqlite3_column_int(stmt, 0);

    // button light effect program
    m_button_animation_program = val / 1000;

    // button count
    int buttons_count = val % 1000;

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    // val = 4;
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
}

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

// pcb *machine::getTemperature();
//  {
//      return 0;
//  }double cTemp