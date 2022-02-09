//***************************************
//
// stateidle.cpp
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

#include "stateIdle.h"

#define IDLE_STRING "Idle"

// Default CTOR
stateIdle::stateIdle()
{
}

// CTOR Linked to IP Thread Socket Listener
stateIdle::stateIdle(messageMediator *message)
{
}

// DTOR
stateIdle::~stateIdle()
{
   // delete stuff
}

// Overload for Debugger output
string stateIdle::toString()
{
   return IDLE_STRING;
}

// Sets a looped Idle state
DF_ERROR stateIdle::onEntry()
{
   DF_ERROR e_ret = OK;

   // Set current and future states to IDLE
   m_state = IDLE;
   m_state_requested = IDLE;
   return e_ret;
}

/*
* Advances State: If IP Thread detects DISPENSE
* command then advance to DISPENSE_IDLE
*/
DF_ERROR stateIdle::onAction()
{
   DF_ERROR e_ret = ERROR_BAD_PARAMS;
   m_state = IDLE;

   if (nullptr != &m_state_requested)
   {

      // Check if Command String is ready
      if (m_pMessaging->isCommandStringReadyToBeParsed())
      {
         DF_ERROR ret_msg;
         ret_msg = m_pMessaging->parseCommandString();

         productDispensers = g_productDispensers;
         pos = m_pMessaging->getProductNumber();
         pos = pos - 1;

         // If ACTION_DISPENSE is received, enter Dispense state, else, stay in Idle state
         if (ACTION_DISPENSE == m_pMessaging->getAction())
         {
            debugOutput::sendMessage("Chosen product: " + std::to_string(pos), INFO);
            productDispensers[pos].getProduct()->initDispense();
            m_state_requested = DISPENSE_IDLE;
         }

         if (ACTION_TEST_PRINTER == m_pMessaging->getAction())
         {

            m_state_requested = MANUAL_MODE;
            
         }
      }
      else
      {
         m_state_requested = IDLE;
         // usleep(100000); // UNISTD Thread PAUSE
      }
      e_ret = OK;
   }
   return e_ret;
}

// Advances to Dispense Idle
DF_ERROR stateIdle::onExit()
{
   DF_ERROR e_ret = OK;
   return e_ret;
}
