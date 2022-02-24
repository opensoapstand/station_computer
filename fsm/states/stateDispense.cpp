//***************************************
//
// statedispense.h
// dispense state class
//
// Recieves and interprets string command
// from messageMediator in FSM.
// Routes dispense instruction to GPIO's
//
// created: 01-2022
// by:Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
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
   //debugOutput::sendMessage("stateDispense(messageMediator * message)", MSG_INFO);
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
   m_state_requested = STATE_DISPENSE;
   productDispensers = g_productDispensers;
   DF_ERROR e_ret = OK;
   pos = m_pMessaging->getProductNumber();
   size = m_pMessaging->getRequestedVolume();
   pos = pos - 1;
   productDispensers[pos].getProduct()->productVolumeInfo();
   return e_ret;
}

/*
 * Checks state of FSM; Accepts incomming string to process for
 * Air, Water and Product.  Sends signal to Solenoids to Dispense,
 * Based on string command
 */
DF_ERROR stateDispense::onAction()
{
   productDispensers = g_productDispensers;
   DF_ERROR e_ret = ERROR_BAD_PARAMS;

   if (m_pMessaging->isCommandStringReadyToBeParsed())
   {
      m_pMessaging->parseCommandString();
   }

   // if (nullptr != &m_state_requested) // TODO: Do a Check if Button is Pressed
   // {

   // Check if UI has sent a ACTION_DISPENSE_END to finish the transaction, or, if dispensing is complete
   if (m_pMessaging->getAction() == ACTION_DISPENSE_END)
   {
      debugOutput::sendMessage("Stop dispensing (stop command received)", MSG_INFO);
      m_state_requested = STATE_DISPENSE_END;
      return e_ret = OK;
   }

   if (productDispensers[pos].getIsDispenseTargetReached())
   {
      debugOutput::sendMessage("Stop dispensing. Requested volume reached.", MSG_INFO);
      // productDispensers[pos].setIsDispenseComplete(true);

      // Send message to the UI that the target volume has been reached
      // TODO: Figure out a Cancel/completed volume from IPC if volume is hit

      // Logic compare present and last 3 states for volume..continue
      m_pMessaging->sendMessage("Target Hit");

      m_state_requested = STATE_DISPENSE_END;
      return e_ret = OK;
   }

   // TODO: Do a check if Pumps are operational
   // send IPC if pump fails

   // Send amount dispensed to UI (to show in Maintenance Mode, and/or animate filling)
   m_pMessaging->sendMessage(to_string(productDispensers[pos].getProduct()->getVolumeDispensed()));

   debugOutput::sendMessage("debug. target(small): " + to_string(productDispensers[pos].getProduct()->m_nVolumeTarget_s) + ". target(large): " + to_string(productDispensers[pos].getProduct()->m_nVolumeTarget_l) + "Vol dispensed: " + to_string(productDispensers[pos].getProduct()->getVolumeDispensed()), MSG_INFO);

   if (productDispensers[pos].getProduct()->getVolumeDispensedPreviously() == productDispensers[pos].getProduct()->getVolumeDispensed())
   {
      // no dispensing detected since the last check
      m_state_requested = STATE_DISPENSE_IDLE;
   }
   else
   {
      // continue dispensing
      //m_state_requested = STATE_DISPENSE;
      productDispensers[pos].getProduct()->m_nVolumeDispensedPreviously = productDispensers[pos].getProduct()->getVolumeDispensed();
   }

   usleep(500000);
   e_ret = OK;
   // }
   return e_ret;
}

// Actions on leaving Dispense state
DF_ERROR stateDispense::onExit()
{
   DF_ERROR e_ret = OK;
   return e_ret;
}
