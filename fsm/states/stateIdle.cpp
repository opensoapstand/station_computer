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
   //debugOutput::sendMessage("stateIdle(messageMediator * message)", INFO);
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
   m_nextState = IDLE;
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

   if (nullptr != &m_nextState)
   {

      // Check if Command String is ready
      if (m_pMessaging->isCommandReady())
      {
         DF_ERROR ret_msg;
         ret_msg = m_pMessaging->parseCommandString();
        
                 
         cassettes = g_cassettes;
         pos = m_pMessaging->getProductNumber();
         pos = pos - 1;

         // If ACTION_DISPENSE is received, enter Dispense state, else, stay in Idle state
         if (m_pMessaging->getAction() == ACTION_DISPENSE){
            debugOutput::sendMessage("Dispense selected product (1,2,3 or 4)", INFO);
            //  debugOutput::sendMessage("position" + std::to_string(pos), INFO); // give sometimes segmentation error
            
             cassettes[pos].getDrink()->initDispense();
             m_nextState = DISPENSE_IDLE;
         }
      }
      else
      {
         m_nextState = IDLE;
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
