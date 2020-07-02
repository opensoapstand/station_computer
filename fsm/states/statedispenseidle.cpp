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

stateDispenseIdle::stateDispenseIdle(){
    m_stateCurrent = DF_FSM::DISPENSE;
}

stateDispenseIdle::stateDispenseIdle(messageMediator * message){

}

stateDispenseIdle::stateDispenseIdle(int inputDigit){

}

stateDispenseIdle::~stateDispenseIdle()
{

}


DF_ERROR stateDispenseIdle::onEntry()
{
   DF_ERROR e_ret  = OK;

   
   return e_ret;
}

DF_ERROR stateDispenseIdle::onAction(DF_FSM * nextState)
{
   debugOutput debugInfo;
   DF_ERROR df_ret  = ERROR_BAD_PARAMS;

   if (nullptr != nextState)
   {
      // do stuff
      debugInfo.sendMessage("onAction() for state [" + std::to_string((int)*nextState) + "]", INFO);
          
      return df_ret = OK;
   }

   return df_ret;
}

DF_FSM stateDispenseIdle::onAction(DF_FSM *nextState, dispenser* dispenseObj)
{
    debugOutput debugInfo;
    DF_FSM df_state_ret  = DISPESE_IDLE;

    if (nullptr != nextState)
    {
        // do stuff
        debugInfo.sendMessage("onAction() for state [" + std::to_string((int)*nextState) + "]", INFO);
          
        return df_state_ret = DISPENSE;
    }

    return df_state_ret;
}

DF_ERROR stateDispenseIdle::onExit(DF_FSM *curState)
{
    debugOutput debugInfo;
    DF_ERROR e_ret  = OK;

    debugInfo.sendMessage("onExit() for state [" + std::to_string((int)*curState) + "]", INFO);


   return e_ret;
}