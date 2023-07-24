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
    bool getPcb24VPowerSwitchStatus();
    // void print_text(string text);
    void print_receipt(string name_receipt, string receipt_cost, string receipt_volume_formatted, string time_stamp, string char_units_formatted, string paymentMethod, string plu, string promoCode, bool sleep_until_printed);
    gpio *switch_24V;
    bool power24VEnabled = false;
    void setup();
    pcb *getPcb();

    pcb *getTemperature(); 
    // void getTemperature(temperature sensor); 
    // static pcb *control_pcb;
    pcb *control_pcb;
    Adafruit_Thermal *receipt_printer;
    void refreshRunningLightCaterpillar();
    void refreshRunningLightPingPong();
    void resetRunningLight();
    uint64_t m_lights_animation_most_recent_step_millis;
    uint8_t m_lights_animation_step;
};

#endif