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
stateDispense::stateDispense(messageMediator * message){

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
   DF_ERROR e_ret  = OK;

   m_state = DISPENSE;
   m_nextState = DISPENSE;
   
   return e_ret;
}

/*
 * Checks state of FSM; Accepts incomming string to process for
 * Air, Water and Drink.  Sends signal to Solenoids to Dispense,
 * Based on string command
 */
DF_ERROR stateDispense::onAction(dispenser* cassettes)
{
   DF_ERROR e_ret  = ERROR_BAD_PARAMS;
   string temp;

   // Parse and check command String
   if(m_pMessaging->getStringReady())
   {
      temp = m_pMessaging->getCommandString();
   }
   else
   {
      return e_ret = OK;
   }
   
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

      // Check for Char then int pairing values            
      char solenoidChar;
      strcpy(&solenoidChar, &temp[1]);

      if(!isalpha(solenoidChar)) //for second char not an alphabet
      {
         debugOutput::sendMessage("Irrelevant input", INFO); 
         m_pMessaging->clearCommandString(); //make sure to clear the processed string for new input
           return e_ret = OK;
      }

      // TODO: Can seperate this into char parsing switch statment and further into function.
      if(AIR_CHAR == solenoidChar)
      {
         debugOutput::sendMessage("Activating position -> " + to_string(pos+1) + " solenoid -> AIR", INFO);
         debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(AIR)), INFO);

         if(nullptr != cassettes[pos].getDrink())
         {
            cassettes[pos].testSolenoidDispense(AIR);
         }
         // else
         // {
         //    debugOutput::sendMessage("Cassette " +  to_string(pos+1) + " is nullptr", INFO);
         // }
         
      }
      else if(WATER_CHAR == solenoidChar)
      {
         debugOutput::sendMessage("Activating position -> " + to_string(pos+1) + " solenoid -> WATER", INFO);
         debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(WATER)), INFO);
         cassettes[pos].testSolenoidDispense(WATER);

      }
      else if(DRINK_CHAR == solenoidChar)
      {
         debugOutput::sendMessage("------Dispensing Drink------", INFO);
         // debugOutput::sendMessage("Activating position -> " + to_string(pos+1) + " solenoid -> DRINK", INFO);
         debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(DRINK)), INFO);

         // cassettes[pos].testSolenoidDispense(DRINK);
         cassettes[pos].startDispense(DRINK);
      }
      else if(CLEAN_CHAR == solenoidChar)
      {
         debugOutput::sendMessage("------Cleaning Mode------", INFO);
         debugOutput::sendMessage("Activating position -> " + to_string(pos+1) + " solenoid -> WATER", INFO);
         debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(WATER)), INFO);

         debugOutput::sendMessage("Activating position -> " + to_string(pos+1) + " solenoid -> WATER", INFO);
         debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(DRINK)), INFO);

         // cassettes[pos].testSolenoidDispense(DRINK);
         cassettes[pos].cleanNozzle(WATER, AIR);

         m_pMessaging->sendMessage("!");

      }
      else{
         debugOutput::sendMessage("Irrelevant input", INFO); 
      }

      m_pMessaging->clearProcessString();        
      e_ret = OK;
   }

   return e_ret;
}

// Actions on leaving Dispense state
DF_ERROR stateDispense::onExit()
{
   DF_ERROR e_ret  = OK;

   // TODO: Does not seem to advance to Idle again...
   m_state = DISPENSE;

   
   //m_nextState = INIT; //go back for now

   return e_ret;
}