//***************************************
//
// stateidle.cpp
// idle state class
//
// After stateInit: Waits for a dispense
// command from IPC QT Socket
//
// created: 01-2022
// by:Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
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
   m_state_requested = IDLE;
   DF_ERROR e_ret = OK;

   // Set current and future states to IDLE
   //m_state = IDLE;
   return e_ret;
}

/*
* Advances State: If IP Thread detects DISPENSE
* command then advance to DISPENSE_IDLE
*/
DF_ERROR stateIdle::onAction()
{
   DF_ERROR e_ret = ERROR_BAD_PARAMS;
  // m_state = IDLE;

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

         if (ACTION_DISPENSE == m_pMessaging->getAction())
         {
            debugOutput::sendMessage("Chosen product: " + std::to_string(pos), INFO);
            productDispensers[pos].getProduct()->initDispense();
            m_state_requested = DISPENSE_IDLE;
         }

         if (ACTION_TEST_PRINTER == m_pMessaging->getAction())
         {
            m_state_requested = MANUAL_PRINTER;
         }

         if (ACTION_QUIT == m_pMessaging->getAction())
         {
            m_state_requested = END;
         }

         if (ACTION_HELP == m_pMessaging->getAction())
         {
            debugOutput::sendMessage("\n Idle State. Available Commands: \n q: Quit(in independent mode)\n p: Test printer \n [1..4]d[l,s,t]: Enter dispense mode. [product number]d[size] \n [1..4]f[l,s,t]: If dispensing: to End Dispensing [product number]f[size]", INFO);
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
