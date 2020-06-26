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
   DF_ERROR e_ret  = ERROR_BAD_PARAMS;

   if (nullptr != nextState)
   {
      // do stuff
      

   }

   return e_ret;
}

DF_ERROR stateDispenseIdle::onExit()
{
   DF_ERROR e_ret  = OK;

   return e_ret;
}