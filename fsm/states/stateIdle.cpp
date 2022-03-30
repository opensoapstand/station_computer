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
   m_state_requested = STATE_IDLE;
   DF_ERROR e_ret = OK;

   return e_ret;
}

DF_ERROR stateIdle::onAction()
{
   DF_ERROR e_ret = ERROR_BAD_PARAMS;

   if (nullptr != &m_state_requested)
   {

      // Check if Command String is ready
      if (m_pMessaging->isCommandStringReadyToBeParsed())
      {
         DF_ERROR ret_msg;
         ret_msg = m_pMessaging->parseCommandString();

         pos = m_pMessaging->getRequestedSlot();
         pos = pos - 1;

         if (ACTION_DISPENSE == m_pMessaging->getAction())
         {
            //  debugOutput::sendMessage("yoooooo", MSG_INFO);
            // m_selectedOrder.size = 'f';//m_pMessaging->getRequestedSize();
            // debugOutput::sendMessage("dwdwiieieiej", MSG_INFO);
            // m_selectedOrder.slot = 1; m_pMessaging->getRequestedSlot();
            // debugOutput::sendMessage("iieieiej", MSG_INFO);

            m_state_requested = STATE_DISPENSE_INIT;
         }

         if (ACTION_MANUAL_PRINTER == m_pMessaging->getAction())
         {
            m_state_requested = STATE_MANUAL_PRINTER;
         }
         
         if (ACTION_MANUAL_PUMP == m_pMessaging->getAction())
         {
            m_state_requested = STATE_MANUAL_PUMP;
         }

         if (ACTION_QUIT == m_pMessaging->getAction())
         {
            debugOutput::sendMessage("Request application exit.", MSG_INFO);
            m_state_requested = STATE_END;
         }

         if (ACTION_DEBUG == m_pMessaging->getAction())
         {
            productDispensers = g_productDispensers;
             debugOutput::sendMessage("beffooore reload parameters from product1", MSG_INFO);
             bool success = this->productDispensers[0].getProduct()->reloadParametersFromDb();
             debugOutput::sendMessage("after" + to_string(success), MSG_INFO);
         }

         if (ACTION_HELP == m_pMessaging->getAction())
         {
            debugOutput::sendMessage("\n Idle State. Available Commands: \n q: Quit(in independent mode)\n p: Test printer\n m: Test pumps\n b: Specific debug \n [1..4]d[l,s,t]: Enter dispense mode. [product number]d[size] \n [1..4]f[l,s,t]: If dispensing: to End Dispensing [product number]f[size]", MSG_INFO);
         }
      }
      else
      {
         m_state_requested = STATE_IDLE;
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
