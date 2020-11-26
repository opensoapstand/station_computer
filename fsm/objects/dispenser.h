//***************************************
//
// dispenser.cpp
// dispenser class owns the package for a dispense.
//
// Links to circuit board addresses and
// holds instructions for dispensing.
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
#include "../components/mcpgpio.h"
#include "../components/odysseyx86gpio.h"
#include "drink.h"

// Total Number of Devices
#define NUM_SOLENOID 3 //12v for drink,water, and air
#define NUM_PUMP 2 //forward and reverse pin
#define NUM_FLOWSENSOR  1 
#define NUM_BUTTON 1

// Range of Dispenser Still Option positions that use pumps
#define PUMP_OPTION_START_POSITION 5
#define PUMP_OPTION_STOP_POSITION 8

#define DRINK 0
#define WATER 1
#define AIR 2

#define FORWARD 0
#define REVERSE 1

#define FLOW 0

#define X20 20
#define X21 21
#define X22 22

#define MCP_PIN_START 0
#define MPC_PIN_END 15

class dispenser
{
   public:
      dispenser();
      dispenser(gpio* buttonReference);
      ~dispenser();

      void initDispenser(int slot);

      //private variable setters
      DF_ERROR setSolenoid(int mcpAddress, int pin, int pos);
      DF_ERROR setPump(int mcpAddress, int pin, int direction); //0 as forward and 1 as reverse
      DF_ERROR setFlowsensor(int pinint, int pos);

      DF_ERROR startDispense(int pos);
      DF_ERROR stopDispense(int pos); //reached dispense value

      DF_ERROR forwardPump(); //reached dispense value
      DF_ERROR reversePump(); //reached dispense value
      DF_ERROR stopPump(); //reached dispense value

      DF_ERROR connectButton();
      DF_ERROR disconnectButton();

      bool getIsDispenseComplete(){return m_isDispenseDone;}
      void setIsDispenseComplete(bool isDispenseComplete){m_isDispenseDone = isDispenseComplete;}

      void setm_pIsDispenseDone(){*m_pIsDispensing = false;}
      void setm_pIsDispensing(){*m_pIsDispensing = true;}
      void setm_pRestartDispense(){*m_pIsDispensing = false;}
      
      DF_ERROR cleanNozzle(int posW, int posA);

      DF_ERROR testSolenoidDispense(int pos);

      drink* getDrink();

      DF_ERROR setDrink(drink* drink);

      int getI2CAddress(int pos);
      int getI2CPin(int pos);

   private:
      bool m_isDispenseDone; // XXX: Remove later.
      bool m_isStill;

      bool* m_pIsDispensing;

      bool m_isDispenseNew;

      drink* m_pDrink;

      DF_ERROR * m_pthreadError;

      // Pointers to Addresses set in State Init

      gpio *m_pSolenoid[NUM_SOLENOID]; //air,drink, and water solenoid control
      gpio *m_pFlowsenor[NUM_FLOWSENSOR];
      gpio *m_pPump[NUM_PUMP]; //forward and reverse pin control
      
      // Button reference m_pButton[1] in stateVirtual; IPC shared due to Arduino!
      gpio *m_pButton[NUM_BUTTON];
};

#endif
