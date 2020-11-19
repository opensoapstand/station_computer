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

int total_dispensed_prev2 = 0;

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

   m_pMessaging->getPositionReady();

   debugOutput::sendMessage("In Idle Mode!", INFO);

   cassettes = g_cassettes;
   pos = m_pMessaging->getnOption();
   pos = pos - 1;

   return e_ret;
}

// Idles after proper initilization;  Waits for a command from messageMediator
DF_ERROR stateDispenseIdle::onAction()
{
   debugOutput debugInfo;
   DF_ERROR df_ret = ERROR_BAD_PARAMS;

   if (nullptr != &m_nextState)
   {
       if (m_pMessaging->getcCommand() != DISPENSE_END_CHAR){
             //debugOutput::sendMessage("in idle", INFO);
       }

       //debugOutput::sendMessage("in idle", INFO);

//       int total_dispensed = cassettes[pos].getDrink()->getVolumeDispensed();

//       if (total_dispensed_prev2 == total_dispensed){
//           //debugOutput::sendMessage("remain in idle", INFO);
//           total_dispensed = cassettes[pos].getDrink()->getVolumeDispensed();
//           m_nextState = DISPENSE_IDLE;
//       }
//       else {
//           m_nextState = DISPENSE;
//           total_dispensed_prev2 = total_dispensed;
//       }



       m_nextState = DISPENSE;



       //debugInfo.sendMessage("onAction() for state [" + std::to_string((int)m_nextState) + "]", INFO);

      // FIXME: State Check
      // if (dispenserSetup()->getIsDispenseComplete()) // Exit if Dispense limit is hit
      // {
      //    onExit();
      // }
      // TODO: else if to check button interrupt swap to Dispensing
      // {
      //    debugOutput::sendMessage("Dispensing [" + toString() + "]", INFO);
      //    m_state = DISPENSE;
      //    m_nextState = DISPENSE_IDLE;
      // }
      // else // Assume no button push and not complete Keep IdleDispensing
      // {
      //    // debugOutput::sendMessage("Keep Idling [" + toString() + "]", INFO);
      //    m_state = DISPENSE_IDLE;
      //    m_nextState = DISPENSE;
      // }

      // FIXME: No Coordination for Idles...Just go to Dispense for now.
      //m_nextState = DISPENSE;
      df_ret = OK;
   }

   return df_ret;
}

// Advances to Dispense End with completed Dispense
DF_ERROR stateDispenseIdle::onExit()
{
   // debugOutput::sendMessage("Exiting[" + toString() + "]", STATE_CHANGE);
   DF_ERROR e_ret = OK;
   debugOutput::sendMessage("exit idle", INFO);

   // debugOutput::sendMessage("Keep Dispensing [" + toString() + "]", INFO);
   // m_state = DISPENSE_IDLE;
   // m_nextState = DISPENSE;

   // TODO: If timeout occurs, then we can skip to cleaning cycle.
   // m_state = DISPENSE_END;
   // m_nextState = IDLE;

   return e_ret;
}
