//***************************************
//
// stateidle.cpp
// idle state class
//
// HACK: JW What is difference between this and DispenseIdle?
//
// created: 29-06-2020
// by: Jason Wang & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "stateidle.h"

#define IDLE_STRING "Idle"

// Default CTOR
stateIdle::stateIdle()
{

}

// CTOR Linked to IPC
stateIdle::stateIdle(messageMediator * message)
{
   //debugOutput::sendMessage("stateIdle(messageMediator * message)", INFO);
}

// DTOR
stateIdle::~stateIdle()
{
   //delete stuff
}

// Overload for Debugger output
string stateIdle::toString()
{
   return IDLE_STRING;
}

// FIXME: See state dispense function header
DF_ERROR stateIdle::onEntry()
{
   DF_ERROR e_ret  = OK;

   m_state = IDLE; //ensure current state is IDLE
   m_nextState = IDLE;

   return e_ret;
}

// First Landing page...quite literally does nothing after initialization
// Should get better use out of this intermediate step...
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

// Advances to Dispense Idle
DF_ERROR stateIdle::onExit()
{
   DF_ERROR e_ret  = OK;

   m_state = IDLE;
   m_nextState = DISPENSE_IDLE; //!!!will be needing more here

   return e_ret;
}