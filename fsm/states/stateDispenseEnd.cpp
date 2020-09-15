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

   debugOutput::sendMessage("stateDispenseEnd(messageMediator * message)", INFO);
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
      // TODO: Cleaning Nozzle
      // debugOutput::sendMessage("------Cleaning Mode------", INFO);
      // debugOutput::sendMessage("Activating position -> " + to_string(pos+1) + " solenoid -> WATER", INFO);
      // debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(WATER)), INFO);
      // debugOutput::sendMessage("Activating position -> " + to_string(pos+1) + " solenoid -> WATER", INFO);
      // debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(DRINK)), INFO);

      // // cassettes[pos].testSolenoidDispense(DRINK);
      // cassettes[pos].cleanNozzle(WATER, AIR);
      // m_pMessaging->clearProcessString();
      m_pMessaging->clearCommandString(); 
      e_ret = OK;
   }

   return e_ret;
}

// Actions on leaving Dispense state
DF_ERROR stateDispenseEnd::onExit()
{
   DF_ERROR e_ret  = OK;

   // TODO: Does not seem to advance to Idle again...
   m_state = DISPENSE_END;
   m_nextState = IDLE; //go back for now

   return e_ret;
}