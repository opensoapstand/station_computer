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

void machine::testtest(){
// nothing here.
    debugOutput::sendMessage("*** global machine test message", MSG_INFO);
}



void machine::print_receipt(string name_receipt, string receipt_cost, string receipt_volume_formatted, string time_stamp, string units, string paymentMethod, string plu, string promoCode){
    print_text(name_receipt + "\nPrice: $" + receipt_cost + " \nQuantity: " + receipt_volume_formatted + "\nTime: " + time_stamp);
    debugOutput::sendMessage("Price" + receipt_cost, MSG_INFO);
    if (receipt_cost == "0.00"){
        debugOutput::sendMessage("Free Order", MSG_INFO);
        print_text("Promo Used: " + promoCode);
        print_text("Enjoy your free product!");
        print_text("Thank you for supporting \npackage-free!");
    }
    else if (paymentMethod == "barcode" || paymentMethod == "barcode_EAN-13" || paymentMethod == "barcode_EAN-2")
    {

        if (plu.size() != 13 && plu.size() != 12)
        {
            // EAN13 codes need to be 13 digits, or else no barcode will be printed. If 12 dgits are provided, the last digit (checksum?!) is automatically generated
            debugOutput::sendMessage("ERROR: bar code invalid (" + plu + "). EAN13, Should be 13 digits" + to_string(plu.size()), MSG_INFO);
            print_text("\nPLU: " + plu + " (No barcode available)");
        }
        else
        {
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
            debugOutput::sendMessage("PLU without barcode:" + plu,  MSG_INFO);
            print_text("PLU: " + plu);
        }
        else
        {
            debugOutput::sendMessage("PLU as barcode:" + plu,  MSG_INFO);
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
    print_text("\n\n\n");

}

void machine::print_text(string text)
{
    string printerstring = text;
    string printer_command_string = "echo '\n" + printerstring + "' > /dev/ttyS4";
    system(printer_command_string.c_str());
}
