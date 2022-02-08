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
//    gpio* temp = productDispensers[pos]->;
//    return ;
// }

gpio *stateVirtual::getButton()
{
   return m_pButton[NUM_BUTTON];
}
