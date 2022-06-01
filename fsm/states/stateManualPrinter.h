//***************************************
//
// stateManualPrinter.h
// idle state class
//
// created: 01-2022
// by:Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef STATEMANUALPRINTER__H_
#define STATEMANUALPRINTER__H_

#include "../dftypes.h"
#include "../objects/messageMediator.h"
#include "stateVirtual.h"
#include "../fsm.h"
#include "../objects/product.h"
#include "../objects/machine.h"


#include "../../library/printer/Adafruit_Thermal.h"
#include <string>

class stateManualPrinter : public stateVirtual
{
public:
    stateManualPrinter();
    stateManualPrinter(messageMediator *message); //debug through local network
    ~stateManualPrinter();

    dispenser *productDispensers;

    sqlite3 *db;
    int rc;

    string toString();
    DF_ERROR printTest();
    DF_ERROR displayPrinterStatus();
    DF_ERROR displayPrinterReachable();
    DF_ERROR sendPrinterStatus();
    void printTransaction(int transactionNumber);

    DF_ERROR setup_receipt_from_data_and_slot(int slot, double volume_dispensed, double volume_requested, double price, string time_stamp);
    DF_ERROR print_receipt(string name_receipt, string receipt_cost, string receipt_volume_formatted, string time_stamp, string units, string paymentMethod, string plu);
    DF_ERROR print_text(string text);
    DF_ERROR onEntry();
    DF_ERROR onAction();
    DF_ERROR onExit();
   

private:
    //int waitSerial;
    bool b_isContinuouslyChecking;
    Adafruit_Thermal printerr;
   
};

#endif
