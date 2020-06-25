//***************************************
//
// i2cgpios.h
// class to handle i2c library for gpios 
//
// created: 10-06-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************
#include "mcp23017/mcp23017.h"

#define X20 20
#define X21 21
#define X22 22

#define I2C_FILE_ADDRESS 2

class i2cGpios{

public:
    i2cGpios();
    ~i2cGpios();

    void setPinMode_out(int address, int pinNum);

    void setPin_on(int address, int pinNum);
    void setPin_off(int address, int pinNum);

    void setPump_Forward(int addressFWD, int pinNumFWD, int addressREV, int pinNumREV);
    void setPump_Reverse(int addressFWD, int pinNumFWD, int addressREV, int pinNumREV);
    void setPump_Off(int addressFWD, int pinNumFWD, int addressREV, int pinNumREV);

private:
    MCP23017 solenoid_1;
    MCP23017 solenoid_2;
    MCP23017 pump;
};