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

#include "stateDispenseEnd.h"
#include <cstring>

#define DISPENSE_END_STRING "Dispense End"

// CTOR
stateDispenseEnd::stateDispenseEnd()
{

}

// CTOR Linked to IPC
stateDispenseEnd::stateDispenseEnd(messageMediator * message){

   //debugOutput::sendMessage("stateDispense(messageMediator * message)", INFO);
}

// DTOR
stateDispenseEnd::~stateDispenseEnd()
{

}

// Overload for Debugger output
string stateDispenseEnd::toString()
{
   return DISPENSE_END_STRING;
}

/*
 * Called from FSM loop to check state before inAction execution
 * TODO: Create State Class to hold this function definition
 */ 
DF_ERROR stateDispenseEnd::onEntry()
{
   DF_ERROR e_ret  = OK;

   m_state = DISPENSE_END;
   m_nextState = IDLE;
   
   return e_ret;
}

DF_ERROR stateDispenseEnd::onAction(dispenser* cassettes)
{
   DF_ERROR e_ret  = ERROR_BAD_PARAMS;
   string temp;
   
   if (nullptr != &m_nextState)
   {
      //only allow [cassette_num][A/D/W] to be keyboard input for now...
      //eg. 1a -> cassette 1 for air solenoid

      int pos = -1;
      // do stuff
      char posChar;
      strcpy(&posChar, &temp[0]);

      if(isdigit(posChar)) //first character should be string
      {
         pos = atoi(&posChar) - 1;

         if(CASSETTES_MAX < pos || 0 > pos)
         {
            debugOutput::sendMessage("Irrelevant input", ERROR); 
            m_pMessaging->clearProcessString();  
            return e_ret = OK; //require valid cassettes
         }
      }
      else
      {
         // Error Handling
         debugOutput::sendMessage("Irrelevant input", INFO);
         m_pMessaging->clearProcessString(); //make sure to clear the processed string for new input
         return e_ret = OK; //require valid cassettes
      }

      debugOutput::sendMessage("------Cleaning Mode------", INFO);
      debugOutput::sendMessage("Activating position -> " + to_string(pos+1) + " solenoid -> WATER", INFO);
      debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(WATER)), INFO);
      debugOutput::sendMessage("Activating position -> " + to_string(pos+1) + " solenoid -> WATER", INFO);
      debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(DRINK)), INFO);

      // cassettes[pos].testSolenoidDispense(DRINK);
      cassettes[pos].cleanNozzle(WATER, AIR);
      m_pMessaging->clearProcessString();        
      e_ret = OK;
   }

   return e_ret;
}

// Actions on leaving Dispense state
DF_ERROR stateDispenseEnd::onExit()
{
   DF_ERROR e_ret  = OK;

   // TODO: Does not seem to advance to Idle again...
   m_state = IDLE;
   m_nextState = IDLE; //go back for now

   return e_ret;
}