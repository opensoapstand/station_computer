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
#include "drink.h"

class dispenser
{
   public:
      dispenser();
      ~dispenser();

   private:
      drink * m_pDrink;
   
};

#endif