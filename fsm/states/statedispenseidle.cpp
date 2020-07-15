//***************************************
//***************************************
//
// statedispenseidle.h
// dispense idle state class
//
// created: 26-06-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "statedispenseidle.h"

#define DISPENSE_IDLE_STRING  "Dispense Idle"

stateDispenseIdle::stateDispenseIdle()
{

}

stateDispenseIdle::stateDispenseIdle(messageMediator * message){
   
   //debugOutput::sendMessage("stateDispenseIdle(messageMediator * message)", INFO);
}


stateDispenseIdle::~stateDispenseIdle()
{

}

string stateDispenseIdle::toString()
{
   return DISPENSE_IDLE_STRING;
}

DF_ERROR stateDispenseIdle::onEntry()
{
   DF_ERROR e_ret  = OK;

   m_state = DISPENSE_IDLE;
   m_nextState = DISPENSE_IDLE;
   
   return e_ret;
}

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

DF_ERROR stateDispenseIdle::onExit()
{
    debugOutput debugInfo;
    DF_ERROR e_ret  = OK;

    m_state = DISPENSE_IDLE;
    m_nextState = DISPENSE;

   return e_ret;
}