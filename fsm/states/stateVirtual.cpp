//***************************************
//
// statevirtual.cpp
// virtual state class
//
// created: 12-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "stateVirtual.h"
#include <iostream>

stateVirtual::stateVirtual()
{
   m_state = START;
}

stateVirtual::stateVirtual(messageMediator *message)
{
   if (nullptr == message)
   {
      //bad things have occured
   }
   m_state = START;

   m_pMessaging = message;

   // Hardcoded drink class for testing.
   // testDrinks[0] = new drink(1, "Drink1", 355, 355, 4.00, false);
   // testDrinks[1] = new drink(2, "Drink2", 355, 355, 4.00, false);
   // testDrinks[2] = new drink(3, "Drink3", 355, 355, 4.00, false);
   // testDrinks[3] = new drink(4, "Drink4", 355, 355, 4.00, false);
   // testDrinks[4] = new drink(5, "Drink5", 355, 355, 4.00, false);
   // testDrinks[5] = new drink(6, "Drink6", 355, 355, 4.00, false);
   // testDrinks[6] = new drink(7, "Drink7", 355, 355, 4.00, false);
   // testDrinks[7] = new drink(8, "Drink8", 355, 355, 4.00, false);
   // testDrinks[8] = new drink(9, "Drink9", 355, 355, 4.00, false);
}

stateVirtual::~stateVirtual()
{
   //clean up
}

//getter for next state from fsm
DF_FSM stateVirtual::getNextState()
{
   return m_nextState;
}

//getter for current state from fsm
DF_FSM stateVirtual::getCurrentState()
{
   return m_state;
}

// gpio* stateVirtual::getSolenoid(int pos, int type)
// {
//    gpio* temp = cassettes[pos]->;
//    return ;
// }

gpio *stateVirtual::getButton()
{
   return m_pButton[NUM_BUTTON];
}
