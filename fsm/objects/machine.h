//***************************************
//
// machine.cpp
// machine class owns the package for a dispense.
//
// Links to circuit board addresses and
// holds instructions for dispensing.
//
// created: 01-2022
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef _MACHINE__H_
#define _MACHINE__H_

#include "../dftypes.h"
#include "../components/gpio.h"
#include "../components/dsed8344.h"
#include "../components/odysseyx86gpio.h"
#include "product.h"
#include <sqlite3.h>

#include "../../library/printer/Adafruit_Thermal.h"

class machine
{
public:
    void pcb24VPowerSwitch(bool enableElseDisable);
    // void print_text(string text);
    void print_receipt(string name_receipt, string receipt_cost, string receipt_volume_formatted, string time_stamp, string char_units_formatted, string paymentMethod, string plu, string promoCode);
    gpio *switch_24V;
    void setup();
    pcb *getPcb();

    // getTemperature(); temp sensor
    // static pcb *control_pcb;
    pcb *control_pcb;
    Adafruit_Thermal *receipt_printer;
};

#endif