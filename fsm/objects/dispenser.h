//***************************************
//
// dispenser.h
// dispenser class owns the package for a dispense
//
// created: 12-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef _DISPENSER__H_
#define _DISPENSER__H_

#include "../dftypes.h"
#include "../components/gpio.h"
#include "drink.h"

#define NUM_SOLENOID 3 //12v for drink,water, and air
#define NUM_PUMP 2 //forward and reverse pin
#define NUM_FLOWSENSOR  1 

#define DRINK 0
#define WATER 1
#define AIR 2

#define FORWARD 0
#define REVERSE 1

#define FLOW 0

class dispenser
{
   public:
      dispenser();
      ~dispenser();

      void initDispenser(int slot);

      //private variable setters
      DF_ERROR setSolenoid(int drinkPin, int waterPin, int airPin);
      DF_ERROR setFlowsensor(int pin);
      DF_ERROR setPump(int forwardPin, int reversePin);

      DF_ERROR startDispense();
      DF_ERROR stopDispense(); //reached dispense value
      DF_ERROR cleanNozzle();

      drink getDrink();

   private:
      drink * m_pDrink;

      gpio * m_pSolenoid[NUM_SOLENOID]; //air,drink, and water solenoid control
      gpio * m_pFlowsenor[NUM_FLOWSENSOR];
      gpio * m_pPump[NUM_PUMP]; //forward and reverse pin control
};

#endif