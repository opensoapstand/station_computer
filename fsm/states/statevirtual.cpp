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

#include "statevirtual.h"
#include <iostream>

stateVirtual::stateVirtual(){
   m_state = START;
}

stateVirtual::stateVirtual(messageMediator * message)
{
   if (nullptr == message)
   {
      //bad things have occured
   }

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
