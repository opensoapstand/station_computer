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

class dispenser
{
   public:
      dispenser();
      ~dispenser();

      void initDispenser(int slot);

      DF_ERROR startDispense();
      DF_ERROR stopDispense();
      DF_ERROR cleanNozzle();

      drink getDrink();

   private:
      drink * m_pDrink;

      gpio * m_pSolenoid[3]; //air,drink, and water solenoid control
      gpio * m_pFlowsenor;
      gpio * m_pPump[2]; //forward and reverse pin control
};

#endif