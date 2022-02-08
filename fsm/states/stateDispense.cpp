//***************************************
//
// statedispense.h
// dispense state class
//
// Recieves and interprets string command
// from messageMediator in FSM.
// Routes dispense instruction to GPIO's
//
// created: 26-06-2020
// by: Jason Wang & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "stateDispense.h"
#include <cstring>

#define DISPENSE_STRING "Dispense"

// CTOR
stateDispense::stateDispense()
{
}

// CTOR Linked to IPC
stateDispense::stateDispense(messageMediator *message)
{
   //debugOutput::sendMessage("stateDispense(messageMediator * message)", INFO);
}

// DTOR
stateDispense::~stateDispense()
{
}

// Overload for Debugger output
string stateDispense::toString()
{
   return DISPENSE_STRING;
}

/*
 * Called from FSM loop to check state before inAction execution
 * TODO: Create State Class to hold this function definition
 */
DF_ERROR stateDispense::onEntry()
{
       cassettes = g_cassettes;
       DF_ERROR e_ret = OK;
       pos = m_pMessaging->getProductNumber();
       size = m_pMessaging->getRequestedVolume();
       pos = pos - 1;
       cassettes[pos].getDrink()->drinkVolumeInfo();
       return e_ret;
}

/*
 * Checks state of FSM; Accepts incomming string to process for
 * Air, Water and Drink.  Sends signal to Solenoids to Dispense,
 * Based on string command
 */
DF_ERROR stateDispense::onAction()
{
   // debugOutput::sendMessage("+stateDispense::onAction()", INFO); 
   cassettes = g_cassettes;
   DF_ERROR e_ret = ERROR_BAD_PARAMS;

   if (m_pMessaging->isCommandReady())
   {
      m_pMessaging->parseCommandString();
   }

   if (nullptr != &m_nextState) // TODO: Do a Check if Button is Pressed
   {
       // Check if UI has sent a ACTION_DISPENSE_END to finish the transaction, or, if dispensing is complete
      if ( m_pMessaging->getAction() == ACTION_DISPENSE_END) 
      {
         // debugOutput::sendMessage("Exiting Dispensing [" + toString() + "]" + to_string(cassettes[pos].getIsDispenseComplete()), INFO);
         debugOutput::sendMessage("Stop dispensing (stop command received)", INFO);
         m_nextState = DISPENSE_END;
         return e_ret = OK;
      }
     
      if ( cassettes[pos].getIsDispenseComplete()) 
      {
         // debugOutput::sendMessage("Exiting Dispensing [" + toString() + "]" + to_string(cassettes[pos].getIsDispenseComplete()), INFO);
         debugOutput::sendMessage("Stop dispensing. Requested volume reached.", INFO);
         m_nextState = DISPENSE_END;
         return e_ret = OK;
      }
     

      // TODO: Do a check if Pumps are operational
      // send IPC if pump fails

      // Send amount dispensed to UI (to show in Maintenance Mode, and/or animate filling)
      m_pMessaging->sendMessage(to_string(cassettes[pos].getDrink()->getVolumeDispensed()));

      
      if (cassettes[pos].getDrink()->getVolumeDispensedPreviously() == cassettes[pos].getDrink()->getVolumeDispensed()){
         // no dispensing detected since the last check    
         m_nextState = DISPENSE_IDLE;
      }
      else
      {
         // continue dispensing
         m_nextState = DISPENSE;
         cassettes[pos].getDrink()->m_nVolumeDispensedPreviously = cassettes[pos].getDrink()->getVolumeDispensed();
      }

      cassettes[pos].getDrink()->drinkVolumeInfo();

      // TODO: Figure out a Cancel/completed volume from IPC if volume is hit
      // Logic compare present and last 3 states for volume..continue
      if (cassettes[pos].getDrink()->isDispenseComplete())
      {
         cassettes[pos].setIsDispenseComplete(true);
         // Send message to the UI that the target volume has been reached
         m_pMessaging->sendMessage("Target Hit");
      }

      usleep(500000);
      e_ret = OK;

   }
   // debugOutput::sendMessage("-stateDispense::onAction()", INFO);
   return e_ret;
}

// Actions on leaving Dispense state
DF_ERROR stateDispense::onExit()
{
   DF_ERROR e_ret = OK;
   //cassettes[pos].stopDispense(DRINK);

   cassettes[pos].setIsDispenseComplete(false);

   return e_ret;
}
