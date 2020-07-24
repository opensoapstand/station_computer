//***************************************
//
// <dispense.h>
// <Defines dispense class and functions>
//
// created: <04-27-2020>
// by: <Gabriel Leung>
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "gpio.h"
#include "interrupt.h"
#include "i2c.h"
#include "mcp23017/mcp23017.h"
#include <array>
#include <iostream>
using namespace std;

class dispense{

  public:

    void dispenseLOW(int sensor, int drink, int water, int air);
    void dispenseHIGH(int sensor, int drink, int water, int air);

};
