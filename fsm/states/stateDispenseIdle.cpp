//***************************************
//
// statedispenseidle.h
// dispense idle state class
//
// Dispense Idle state while waiting for user to
// Press button to dispense - Flips between
// Dispense and Dispense Idle.
// Receives and interprets string command from FSM.
//
// created: 26-06-2020
// by: Jason Wang & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "stateDispenseIdle.h"

#define DISPENSE_IDLE_STRING "Dispense Idle"

// Default CTOR
stateDispenseIdle::stateDispenseIdle()
{
}

// CTOR Linked to IPC
stateDispenseIdle::stateDispenseIdle(messageMediator *message)
{
   //debugOutput::sendMessage("stateDispenseIdle(messageMediator * message)", INFO);
}

// DTOR
stateDispenseIdle::~stateDispenseIdle()
{
}

// Overload for Debugger output
string stateDispenseIdle::toString()
{
   return DISPENSE_IDLE_STRING;
}

// FIXME: See state dispense function header
DF_ERROR stateDispenseIdle::onEntry()
{
   DF_ERROR e_ret = OK;

   m_state = DISPENSE_IDLE;
   m_nextState = DISPENSE_IDLE;

   return e_ret;
}

// Idles after proper initilization;  Waits for a command from messageMediator
DF_ERROR stateDispenseIdle::onAction(dispenser *cassettes)
{
   debugOutput debugInfo;
   DF_ERROR df_ret = ERROR_BAD_PARAMS;

   if (nullptr != &m_nextState)
   {
      // do stuff
      //debugInfo.sendMessage("onAction() for state [" + std::to_string((int)m_nextState) + "]", INFO);
      if (dispenserSetup()->getIsDispenseComplete())
      {
         debugOutput::sendMessage("Exiting Dispensing [" + toString() + "]", INFO);
         m_state = DISPENSE_END;
         m_nextState = IDLE;
      }
      else
      {
         debugOutput::sendMessage("Keep Dispensing [" + toString() + "]", INFO);
         m_state = DISPENSE_IDLE;
         m_nextState = DISPENSE;
      }

      m_nextState = DISPENSE;
      df_ret = OK;
   }

   return df_ret;
}

// Advances to Dispense State with successful onAction()
DF_ERROR stateDispenseIdle::onExit()
{
   DF_ERROR e_ret = OK;

   // debugOutput::sendMessage("StateDispenseIdle OnExit()", INFO);
   // if (dispenserSetup()->getIsDispenseComplete())
   // {
   //    debugOutput::sendMessage("Exiting Dispensing [" + toString() + "]", INFO);
      m_state = DISPENSE_END;
      m_nextState = IDLE;
   // }
   // else
   // {
   //    debugOutput::sendMessage("Keep Dispensing [" + toString() + "]", INFO);
      m_state = DISPENSE_IDLE;
      m_nextState = DISPENSE;
   // }

   return e_ret;
}