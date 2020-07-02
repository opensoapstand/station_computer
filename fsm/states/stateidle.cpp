//***************************************
//
// stateidle.cpp
// idle state class
//
// created: 29-06-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "stateidle.h"

stateIdle::stateIdle()
{

}

stateIdle::stateIdle(messageMediator * message)
{
   
}

stateIdle::stateIdle(int inputDigit){
   //init the cassette 
}

stateIdle::~stateIdle()
{
   //delete stuff
}

DF_ERROR stateIdle::onEntry()
{
   DF_ERROR e_ret  = OK;

   m_state = IDLE; //ensure current state is IDLE
   m_nextState = IDLE;

   return e_ret;
}

DF_ERROR stateIdle::onAction()
{
   DF_ERROR e_ret  = ERROR_BAD_PARAMS;
   m_state = IDLE;

   if (nullptr != &m_nextState)
   {

       //e_ret = OK;
   }

   return e_ret;
}

DF_ERROR stateIdle::onExit()
{
   DF_ERROR e_ret  = OK;

   m_state = IDLE;
   m_nextState = DISPENSE; //!!!will be needing more here

   return e_ret;
}