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

stateVirtual::stateVirtual(){
   m_state = DF_FSM::START;
}

stateVirtual::stateVirtual(messageMediator * message)
{
   if (nullptr == message)
   {
      //bad things have occured
   }

   m_pMessaging = message;
}

stateVirtual::stateVirtual(int state)
{
   m_state = state;
}

stateVirtual::~stateVirtual()
{
   //clean up
}

