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
#include <string>

#define DISPENSE_STRING  "Dispense"

stateDispense::stateDispense()
{

}

stateDispense::stateDispense(messageMediator * message){

   debugOutput::sendMessage("stateDispense(messageMediator * message)", INFO);
}


stateDispense::~stateDispense()
{

}

string stateDispense::toString()
{
   return DISPENSE_STRING;
}

DF_ERROR stateDispense::onEntry()
{
   DF_ERROR e_ret  = OK;

   m_state = DISPENSE;
   m_nextState = DISPENSE;
   
   return e_ret;
}

DF_ERROR stateDispense::onAction()
{
   DF_ERROR e_ret  = ERROR_BAD_PARAMS;

   if (nullptr != &m_nextState)
   {
      // do stuff
   }

   return e_ret;
}

DF_ERROR stateDispense::onExit()
{
   DF_ERROR e_ret  = OK;

   m_state = DISPENSE;
   //m_nextState = INIT; //go back for now

   return e_ret;
}