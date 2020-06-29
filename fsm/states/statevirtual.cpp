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
   //m_state = static_cast<DF_FSM>(state); //cast to proper enum type
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

//setter for state change
void stateVirtual::setNextState(DF_FSM *nextState)
{
   m_nextState = *nextState;

   debugOutput debugInfo;
   debugInfo.sendMessage("state changed -> new state [" + to_string(m_nextState) + "]", INFO);
}