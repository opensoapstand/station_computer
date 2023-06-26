//***************************************
//
// debugOutput.cpp
// DEBUG output Implementation:
// threadsafe outputs to console and
// override for other systems
//
// created: 15-06-2020
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "debugOutput.h"
#include "machine.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <fstream>
#include <ctime>

#include <sstream>
#include <iomanip>

using namespace std;
// pcb *machine::control_pcb = nullptr;

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
    switch_24V->setPinAsInputElseOutput(false); // set as output
}
pcb *machine::getPcb()
{
    return control_pcb;
}
// void machine::testtest(){
// // nothing here.
//     debugOutput::sendMessage("*** global machine test message", MSG_INFO);
// }

void machine::pcb24VPowerSwitch(bool enableElseDisable)
{

    switch_24V->writePin(enableElseDisable);
}

void machine::print_receipt(string name_receipt, string receipt_cost, string receipt_volume_formatted, string time_stamp, string char_units_formatted, string paymentMethod, string plu, string promoCode)
{

    std::string out1 = name_receipt + "\nPrice: $" + receipt_cost + " \nQuantity: " + receipt_volume_formatted + "\nTime: " + time_stamp;
    receipt_printer->printText(out1.c_str());

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
}

//pcb *machine::getTemperature();
// {
//     return 0;
// }double cTemp