//***************************************
//
// i2cgpios.cpp
// class to handle i2c library for gpios 
//
// created: 10-06-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************
#include "i2cgpios.h"

i2cGpios::i2cGpios(){
    solenoid_1 = new MCP23017(1, X20);
    solenoid_2 = new MCP23017(1,X21);
    pump = new MCP23017(1, X22);

    solenoid_1->openI2C();
    solenoid_2->openI2C();
    pump->openI2C();
}