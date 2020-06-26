//***************************************
//
// statedispense.h
// dispense state class
//
// created: 26-06-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "statedispense.h"

stateDispense::stateDispense(){
    m_stateCurrent = DF_FSM::DISPENSE;
}

stateDispense::stateDispense(messageMediator * message){

}

stateDispense::stateDispense(int inputDigit){

}

stateDispense::~stateDispense()
{

}


DF_ERROR stateDispense::onEntry()
{
   DF_ERROR e_ret  = OK;

   
   return e_ret;
}

DF_ERROR stateDispense::onAction(DF_FSM * nextState)
{
   DF_ERROR e_ret  = ERROR_BAD_PARAMS;

   if (nullptr != nextState)
   {
      // do stuff

   }

   return e_ret;
}

DF_ERROR stateDispense::onExit()
{
   DF_ERROR e_ret  = OK;

   return e_ret;
}