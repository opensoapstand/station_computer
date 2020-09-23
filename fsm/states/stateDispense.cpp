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

#include "../fsm.h"

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
   DF_ERROR e_ret = OK;

   // TODO: Priming Pumps and Registers for HIGH

   m_nextState = DISPENSE;

   // TODO: Status Check
   // Do a check if there is not enough stock i.e. 350 order 250 left in tank
   return e_ret;
}

/*
 * Checks state of FSM; Accepts incomming string to process for
 * Air, Water and Drink.  Sends signal to Solenoids to Dispense,
 * Based on string command
 */
DF_ERROR stateDispense::onAction()
{
   DF_ERROR e_ret = ERROR_BAD_PARAMS;
   cassettes = g_cassettes;
   string temp;

   if (nullptr != &m_nextState) // TODO: Do a Check if Button is Pressed
   {

      // State Check
      if (pos != 1)
      {
      }
      if (cassettes[pos].getIsDispenseComplete())
      {
         debugOutput::sendMessage("Exiting Dispensing [" + toString() + "]" + to_string(cassettes[pos].getIsDispenseComplete()), INFO);
         m_nextState = DISPENSE_END;
         return e_ret = OK;
      }
      else
      {
         debugOutput::sendMessage("Keep Dispensing [" + toString() + "]", INFO);
         // m_nextState = DISPENSE_IDLE;
      }

      // TODO: Do a check if Pumps are operational
      // send IPC if pump fails

      // TODO: Check the Volume dispensed so far

      // FIXME: Move this to Idle...Parse and check command String
      if (m_pMessaging->getStringReady())
      {
         temp = m_pMessaging->getCommandString();
      }
      else
      {
         return e_ret = ERROR_NETW_NO_COMMAND;
      }

      // Logic compare present and last 3 states for volume..continue

      cassettes[pos].getDrink()->drinkInfo();

      // TODO: Figure out a Cancel/completed volume from IPC if volume is hit

      // std::thread tGPIOListener tgpio = <gpioinstance>->listener();

      //only allow [cassette_num][A/D/W] to be keyboard input for now...
      //eg. 1a -> cassette 1 for air solenoid

      pos = -1;
      // do stuff
      char posChar;
      strcpy(&posChar, &temp[0]);

      if (isdigit(posChar)) //first character should be string
      {
         pos = atoi(&posChar) - 1;

         if (CASSETTES_MAX < pos || 0 > pos)
         {
            debugOutput::sendMessage("Irrelevant input", ERROR);
            m_pMessaging->clearProcessString();
            return e_ret = OK; //require valid cassettes
         }

         // drinkPtr(new drink(posChar));
      }
      else
      {
         // Error Handling
         debugOutput::sendMessage("Irrelevant input", INFO);
         m_pMessaging->clearProcessString(); //make sure to clear the processed string for new input
         return e_ret = OK;                  //require valid cassettes
      }

      // Check for Char then int pairing values
      char solenoidChar;
      strcpy(&solenoidChar, &temp[1]);

      if (!isalpha(solenoidChar)) //for second char not an alphabet
      {
         debugOutput::sendMessage("Irrelevant input", INFO);
         m_pMessaging->clearCommandString(); //make sure to clear the processed string for new input
         return e_ret = OK;
      }

      // TODO: Can seperate this into char parsing switch statment and further into function.
      switch (solenoidChar)
      {
      case AIR_CHAR:
         debugOutput::sendMessage("------Dispensing AIR------", INFO);
         debugOutput::sendMessage("Activating position -> " + to_string(pos + 1) + " solenoid -> AIR", INFO);
         debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(AIR)), INFO);

         // FIXME: Should we do a null check for every dispense...
         if (nullptr != cassettes[pos].getDrink())
         {
            cassettes[pos].testSolenoidDispense(AIR);
         }
         else
         {
            debugOutput::sendMessage("Cassette " + to_string(pos + 1) + " is nullptr", INFO);
         }
         break;

      case WATER_CHAR:
         debugOutput::sendMessage("------Dispensing WATER------", INFO);
         debugOutput::sendMessage("Activating position -> " + to_string(pos + 1) + " solenoid -> WATER", INFO);
         debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(WATER)), INFO);

         cassettes[pos].testSolenoidDispense(WATER);
         break;

      case DRINK_CHAR:
         debugOutput::sendMessage("------Dispensing Drink------", INFO);
         debugOutput::sendMessage("Activating position -> " + to_string(pos + 1) + " solenoid -> DRINK", INFO);
         debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(DRINK)), INFO);

         // cassettes[pos].getDrink()->drinkInfo();

         cassettes[pos].startDispense(DRINK);
         // this->onExit();

         // FIXME: FSM loop is not looping/pushing through to onExit().
         sleep(2);
         // onExit();

         // XXX: Move this to Drink as interrupt...
         cassettes[pos].setIsDispenseComplete(true);
         break;

      case DISPENSE_END_CHAR:

         // TODO: Shift this to DISPENSE_END
         debugOutput::sendMessage("------Cleaning Mode------", INFO);
         debugOutput::sendMessage("Activating position -> " + to_string(pos + 1) + " solenoid -> WATER", INFO);
         debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(WATER)), INFO);
         debugOutput::sendMessage("Activating position -> " + to_string(pos + 1) + " solenoid -> WATER", INFO);
         debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(DRINK)), INFO);

         cassettes[pos].testSolenoidDispense(DRINK);
         cassettes[pos].cleanNozzle(WATER, AIR);

         onExit();
         break;

      default:
         break;
      }
      m_pMessaging->clearCommandString();
      e_ret = OK;
   }

   return e_ret;
}

// Actions on leaving Dispense state
DF_ERROR stateDispense::onExit()
{
   DF_ERROR e_ret = OK;
   debugOutput::sendMessage("Dispense OnEXIT", INFO);

   cassettes[pos].setIsDispenseComplete(false);
   // // TODO: Does not seem to advance to Idle again..
   // Shut down Registers!

   return e_ret;
}