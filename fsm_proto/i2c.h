//***************************************
//
// <i2c.h>
// <Defines i2c class and functions>
//
// created: <04-27-2020>
// by: <Gabriel Leung>
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include <iostream>
#include "mcp23017/mcp23017.h"
using namespace std;

class i2c{

  public:

    //i2c channel 1 - MCP23017 mcp20(0, 0x20);
    void activate_Channel1();
    void setpin_HIGH_Channel1(int pin);
    void setpin_LOW_Channel1(int pin);

    //i2c channel 2 - MCP23017 mcp21(0, 0x21);
    void activate_Channel2();
    void setpin_HIGH_Channel2(int pin);
    void setpin_LOW_Channel2(int pin);


};
