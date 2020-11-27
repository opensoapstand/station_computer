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
       pos = m_pMessaging->getnOption();
       pos = pos - 1;
       cassettes[pos].getDrink()->drinkVolumeInfo();
       if (cassettes[pos].getDrink()->getVolumeDispensed() == 0) {
           debugOutput::sendMessage("-----NEW TRANSACTION-----", INFO);
           // TODO: Priming Pumps and Registers for HIGH
           debugOutput::sendMessage("------Dispensing Drink------", INFO);
           // debugOutput::sendMessage("Activating position -> " + to_string(pos + 1) + " solenoid -> DRINK", INFO);
           // debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(DRINK)), INFO);

           cassettes[pos].getDrink()->setTargetVolume(m_pMessaging->getnTargetVolume());

           cassettes[pos].getDrink()->startDispense(cassettes[pos].getDrink()->getTargetVolume());
           cassettes[pos].setIsDispenseComplete(false);
           cassettes[pos].getDrink()->drinkInfo();
           cassettes[pos].getDrink()->drinkVolumeInfo();
           cassettes[pos].startDispense(DRINK);

           //m_nextState = DISPENSE;

           // TODO: Status Check
           // Do a check if there is not enough stock i.e. 350 order 250 left in tank
           return e_ret;
       }
   else {
       debugOutput::sendMessage("-----BUSY TRANSACTION-----", INFO);
       onAction();
   }

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

   m_pMessaging->getPositionReady();

   if (nullptr != &m_nextState) // TODO: Do a Check if Button is Pressed
   {
      if ( (m_pMessaging->getcCommand() == DISPENSE_END_CHAR) || (cassettes[pos].getIsDispenseComplete()) )
      {
         debugOutput::sendMessage("Exiting Dispensing [" + toString() + "]" + to_string(cassettes[pos].getIsDispenseComplete()), INFO);
         m_nextState = DISPENSE_END;
         return e_ret = OK;
      }
      else
      {
         // debugOutput::sendMessage("Keep Dispensing [" + toString() + "]", INFO);
      }

      // TODO: Do a check if Pumps are operational
      // send IPC if pump fails
      cassettes[pos].getDrink()->getVolumeDispensed();

      if (cassettes[pos].getDrink()->getVolumeDispensedPreviously() == cassettes[pos].getDrink()->getVolumeDispensed()){
          //debugOutput::sendMessage("IDLE - Timer should be ticking!", INFO);

          m_nextState = DISPENSE_IDLE;
      }
      else {
          debugOutput::sendMessage("DISPENSING! - RESET TIMER!", INFO);
          m_pMessaging->sendMessage("Reset Timer");
          m_nextState = DISPENSE;
          cassettes[pos].getDrink()->m_nVolumeDispensedPreviously = cassettes[pos].getDrink()->getVolumeDispensed();
      }



      //debugOutput::sendMessage(outputString, INFO);

      cassettes[pos].getDrink()->drinkVolumeInfo();

      // TODO: Figure out a Cancel/completed volume from IPC if volume is hit
      // Logic compare present and last 3 states for volume..continue
      if (cassettes[pos].getDrink()->isDispenseComplete())
      {
         cassettes[pos].setIsDispenseComplete(true);
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
