//***************************************
//
// stateidle.cpp
// idle state class
//
// created: 29-06-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "stateidle.h"

#define IDLE_STRING "Idle"

stateIdle::stateIdle()
{

}

stateIdle::stateIdle(messageMediator * message)
{
   //debugOutput::sendMessage("stateIdle(messageMediator * message)", INFO);
}


stateIdle::~stateIdle()
{
   //delete stuff
}

string stateIdle::toString()
{
   return IDLE_STRING;
}

DF_ERROR stateIdle::onEntry()
{
   DF_ERROR e_ret  = OK;

   m_state = IDLE; //ensure current state is IDLE
   m_nextState = IDLE;

   return e_ret;
}

DF_ERROR stateIdle::onAction(dispenser* cassettes)
{
   DF_ERROR e_ret  = ERROR_BAD_PARAMS;
   m_state = IDLE;

   if (nullptr != &m_nextState)
   {
      m_nextState = DISPENSE_IDLE;
      e_ret = OK;
   }

   return e_ret;
}

DF_ERROR stateIdle::onExit()
{
   DF_ERROR e_ret  = OK;

   m_state = IDLE;
   m_nextState = DISPENSE_IDLE; //!!!will be needing more here

   return e_ret;
}