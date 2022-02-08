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

// FIXME: See state dispense function header
DF_ERROR stateDispenseIdle::onEntry()
{
   DF_ERROR e_ret = OK;

   m_pMessaging->parseCommandString();

   debugOutput::sendMessage("In Idle Mode!", INFO);

   cassettes = g_cassettes;
   pos = m_pMessaging->getProductIndex();
   size = m_pMessaging->getRequestedVolume();
   pos = pos - 1;

   cassettes[pos].setIsDispenseComplete(false);
   if (cassettes[pos].getDrink()->getVolumeDispensed() == 0) {
       debugOutput::sendMessage("-----NEW TRANSACTION-----", INFO);
       // TODO: Priming Pumps and Registers for HIGH
       debugOutput::sendMessage("------Dispensing Drink------", INFO);
       // debugOutput::sendMessage("Activating position -> " + to_string(pos + 1) + " solenoid -> DRINK", INFO);
       // debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(DRINK)), INFO);

       cassettes[pos].getDrink()->startDispense(cassettes[pos].getDrink()->getTargetVolume(size), cassettes[pos].getDrink()->getPrice(size));
       cassettes[pos].setIsDispenseComplete(false);
       cassettes[pos].getDrink()->drinkInfo();
       cassettes[pos].getDrink()->drinkVolumeInfo();
       cassettes[pos].startDispense(cassettes[pos].getDrink()->getDrinkOption());

   }

   return e_ret;

}

// Idles after proper initilization;  Waits for a command from messageMediator
DF_ERROR stateDispenseIdle::onAction()
{
   DF_ERROR df_ret = ERROR_BAD_PARAMS;

   if (nullptr != &m_nextState)
   {

       if (m_pMessaging->isCommandReady())
       {
            m_pMessaging->parseCommandString();

                // Check if UI has sent a DISPENSE_END_CHAR to compelte the transaction, or, the taget has been hit, to enter into the DispenseEnd state
            if ((m_pMessaging->getAction() == DISPENSE_END_CHAR) ){

                m_nextState = DISPENSE_END;
                return df_ret = OK;
            }
       }
        
        if (cassettes[pos].getIsDispenseComplete()){

                m_nextState = DISPENSE_END;
                return df_ret = OK;
        }

       cassettes[pos].getDrink()->drinkVolumeInfo();

        // If volume has not changed, stay in Idle state, else, volume is changing, go to Dispense state...
       if (cassettes[pos].getDrink()->getVolumeDispensed() == cassettes[pos].getDrink()->getVolumeDispensedPreviously()){
           debugOutput::sendMessage("IDLING - COUNTDOWN!", INFO);
           m_nextState = DISPENSE_IDLE;
       }
       else {
           cassettes[pos].getDrink()->m_nVolumeDispensedPreviously = cassettes[pos].getDrink()->getVolumeDispensed();
           m_nextState = DISPENSE;
       }
      usleep(500000);
      df_ret = OK;
   }

   return df_ret;
}

// Advances to Dispense End with completed Dispense
DF_ERROR stateDispenseIdle::onExit()
{
   debugOutput::sendMessage("Exiting[" + toString() + "]", STATE_CHANGE);
   DF_ERROR e_ret = OK;

   if ((m_pMessaging->getAction() == DISPENSE_END_CHAR) || (cassettes[pos].getIsDispenseComplete())){
       debugOutput::sendMessage("Exiting Dispensing [" + toString() + "]", INFO);
       m_nextState = DISPENSE_END;
   }

   // TODO: If timeout occurs, then we can skip to cleaning cycle.
   // m_state = DISPENSE_END;
   // m_nextState = IDLE;

   return e_ret;
}

// Overload for Debugger output
string stateDispenseIdle::toString()
{
   return DISPENSE_IDLE_STRING;
}
