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

#include <iostream>

i2cGpios::i2cGpios(){
    solenoid_1 = MCP23017(1,0x20);
    solenoid_2 = MCP23017(1,0x21);
    //pump = MCP23017(1, 0x22);

    solenoid_1.openI2C();
    solenoid_2.openI2C();
    //pump.openI2C();
}

i2cGpios::~i2cGpios(){
    for(int i = 0; i <=15; i++){
        solenoid_1.digitalWrite(i, LOW);
        solenoid_2.digitalWrite(i, LOW);
        //pump.digitalWrite(i, LOW);
    }
}

void i2cGpios::setPinMode_out(int address, int pinNum){
    if(address == X20)
    {
        solenoid_1.pinMode(pinNum, OUTPUT);
    }
    else if (address == X21){
        solenoid_2.pinMode(pinNum, OUTPUT);
    }
    else if (address == X22){
        //pump.pinMode(pinNum, OUTPUT);
    }
    else
    {
        std::clog << "i2c address not available\n";
    }  
}

void i2cGpios::setPin_on(int address, int pinNum){
    if(address == X20)
    {
        solenoid_1.digitalWrite(pinNum, HIGH);

        std::clog << "Address:" << address << " Pin: " << pinNum << " is on\n";
    }
    else if (address == X21){
        solenoid_2.digitalWrite(pinNum, HIGH);
        std::clog << "Address:" << address << " Pin: " << pinNum << " is on\n";

    }
    else if (address == X22){
        //pump.digitalWrite(pinNum, HIGH);
        std::clog << "Address:" << address << " Pin: " << pinNum << " is on\n";

    }
    else
    {
        std::clog << "i2c address not available\n";
    }  
}

void i2cGpios::setPin_off(int address, int pinNum){
    if(address == X20)
    {
        solenoid_1.digitalWrite(pinNum, LOW);
        std::clog << "Address:" << address << " Pin: " << pinNum << " is off\n";
    }
    else if (address == X21){
        solenoid_2.digitalWrite(pinNum, LOW);
        std::clog << "Address:" << address << " Pin: " << pinNum << " is off\n";
    }
    else if (address == X22){
        //pump.digitalWrite(pinNum, LOW);
        std::clog << "Address:" << address << " Pin: " << pinNum << " is off\n";

    }
    else
    {
        std::clog << "i2c address not available\n";
    }  
}