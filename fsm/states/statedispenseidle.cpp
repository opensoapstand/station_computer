//***************************************
//
// statedispenseidle.h
// dispense idle state class
//
// Recieves and interprets string command from FSM.
// Routes dispense instruction to GPIO's
// HACK: JW What is difference between this and Idle?
//
// created: 26-06-2020
// by: Jason Wang & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "statedispenseidle.h"

#define DISPENSE_IDLE_STRING  "Dispense Idle"

// Default CTOR
stateDispenseIdle::stateDispenseIdle()
{

}

// CTOR Linked to IPC
stateDispenseIdle::stateDispenseIdle(messageMediator * message){
   
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
   DF_ERROR e_ret  = OK;

   m_state = DISPENSE_IDLE;
   m_nextState = DISPENSE_IDLE;
   
   return e_ret;
}

// Idles after proper initilization;  Waits for a command from messageMediator
DF_ERROR stateDispenseIdle::onAction(dispenser* cassettes)
{
   debugOutput debugInfo;
   DF_ERROR df_ret  = ERROR_BAD_PARAMS;

   if (nullptr != &m_nextState)
   {
      // do stuff
      //debugInfo.sendMessage("onAction() for state [" + std::to_string((int)m_nextState) + "]", INFO);
      m_nextState = DISPENSE;
      return df_ret = OK;
   }

   return df_ret;
}

/*DF_FSM stateDispenseIdle::onAction(dispenser* dispenseObj)
{
    DF_FSM df_state_ret  = DISPENSE_IDLE;

    if (nullptr != &m_nextState)
    {
        // do stuff
        //debugOutput::sendMessage("onAction() for state [" + std::to_string((int)m_nextState) + "]", INFO);
          
        return df_state_ret = DISPENSE;
    }

    return df_state_ret;
}*/

// Advances to Dispense State with successful onAction()
DF_ERROR stateDispenseIdle::onExit()
{
    debugOutput debugInfo;
    DF_ERROR e_ret  = OK;

    m_state = DISPENSE_IDLE;
    m_nextState = DISPENSE;

   return e_ret;
}