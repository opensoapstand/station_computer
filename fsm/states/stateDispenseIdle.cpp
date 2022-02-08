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

   cassettes = g_cassettes;
   pos = m_pMessaging->getProductNumber();
   pos = pos - 1;
   size = m_pMessaging->getRequestedVolume();

   cassettes[pos].setIsDispenseComplete(false);

   if (cassettes[pos].getDrink()->getVolumeDispensed() == 0) {
       // TODO this should be a separate state (dispense_init)

       cassettes[pos].getDrink()->startDispense(cassettes[pos].getDrink()->getTargetVolume(size), cassettes[pos].getDrink()->getPrice(size));
       cassettes[pos].setIsDispenseComplete(false);
       assettes[pos].getDrink(c)->drinkInfo();
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

            // Check if UI has sent a ACTION_DISPENSE_END to compelte the transaction, or, the taget has been hit, to enter into the DispenseEnd state
            if ((m_pMessaging->getAction() == ACTION_DISPENSE_END) ){

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
            //    debugOutput::sendMessage("IDLING - COUNTDOWN!", INFO);
            debugOutput::sendMessage("Wait for volume to change to go to dispensing state", INFO);
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
   DF_ERROR e_ret = OK;

   if ((m_pMessaging->getAction() == ACTION_DISPENSE_END) || (cassettes[pos].getIsDispenseComplete())){
    //    debugOutput::sendMessage("Exiting Dispensing [" + toString() + "]", INFO);
       debugOutput::sendMessage("Exiting Dispensing", INFO);
       m_nextState = DISPENSE_END;
   }

   return e_ret;
}

// Overload for Debugger output
string stateDispenseIdle::toString()
{
   return DISPENSE_IDLE_STRING;
}
