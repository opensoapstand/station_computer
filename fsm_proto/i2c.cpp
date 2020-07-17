//***************************************
//
// <interrupt.cpp>
// <Contians functions that enables and manipulates i2c pins>
//
// created: <04-27-2020>
// by: <Gabriel Leung>
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "i2c.h"

//Activates chanel 1 i2c chip
void i2c::activate_Channel1(){

  MCP23017 mcp20(0, 0x20);      // use default address 0x20
  mcp20.openI2C();

  for(int i = 0; i<15; i++){

        mcp20.pinMode(i, OUTPUT);
        mcp20.digitalWrite(i, LOW);

    }
}

//Activates channel 2 i2c chip
void i2c::activate_Channel2(){

  MCP23017 mcp21(0, 0x21);
  mcp21.openI2C();

  for(int i = 0; i<15; i++){

        mcp21.pinMode(i, OUTPUT);
        mcp21.digitalWrite(i, LOW);

    }
}

//Set a channel 1 pin high
void i2c::setpin_HIGH_Channel1(int pin){

  MCP23017 mcp20(0, 0x20);      // use default address 0x20
  mcp20.openI2C();
  mcp20.pinMode(pin, OUTPUT);
  mcp20.digitalWrite(pin, HIGH);

}

//Set a channel 1 pin low
void i2c::setpin_LOW_Channel1(int pin){

  MCP23017 mcp20(0, 0x20);      // use default address 0x20
  mcp20.openI2C();
  mcp20.pinMode(pin, OUTPUT);
  mcp20.digitalWrite(pin, LOW);

}

//Set a channel 2 pin high
void i2c::setpin_HIGH_Channel2(int pin){

  MCP23017 mcp21(0, 0x21);      // use default address 0x20
  mcp21.openI2C();
  mcp21.pinMode(pin, OUTPUT);
  mcp21.digitalWrite(pin, HIGH);

}

//Set a channel 2 pin low
void i2c::setpin_LOW_Channel2(int pin){

  MCP23017 mcp21(0, 0x21);      // use default address 0x20
  mcp21.openI2C();
  mcp21.pinMode(pin, OUTPUT);
  mcp21.digitalWrite(pin, LOW);

}
