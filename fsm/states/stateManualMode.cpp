//***************************************
//
// stateManualMode.cpp
// idle state class
//
// After stateInit: Waits for a dispense
// command from IPC QT Socket
//
// created: 29-06-2020
// by: Jason Wang & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "stateManualMode.h"

#define IDLE_STRING "Idle"

// Default CTOR
stateManualMode::stateManualMode()
{
}

// CTOR Linked to IP Thread Socket Listener
stateManualMode::stateManualMode(messageMediator *message)
{
}

// DTOR
stateManualMode::~stateManualMode()
{
   // delete stuff
}

// Overload for Debugger output
string stateManualMode::toString()
{
   return IDLE_STRING;
}

// Sets a looped Idle state
DF_ERROR stateManualMode::onEntry()
{
   m_state = MANUAL_MODE;
   m_state_requested = MANUAL_MODE;
   DF_ERROR e_ret = OK;
   debugOutput::sendMessage("test printer stuff.", INFO);
    

   return e_ret;
}

/*
* Advances State: If IP Thread detects DISPENSE
* command then advance to DISPENSE_IDLE
*/
DF_ERROR stateManualMode::onAction()
{
   DF_ERROR e_ret = OK;

   // Check if Command String is ready
   if (m_pMessaging->isCommandStringReadyToBeParsed())
   {
      DF_ERROR ret_msg;
      ret_msg = m_pMessaging->parseCommandString();

      if (m_pMessaging->getAction() == ACTION_QUIT_TEST)
      {
         debugOutput::sendMessage("back to idle", INFO);
         m_state_requested = IDLE;
      }
   }
   
   e_ret = OK;
  
   return e_ret;
}

// Advances to Dispense Idle
DF_ERROR stateManualMode::onExit()
{
   DF_ERROR e_ret = OK;
   return e_ret;
}
