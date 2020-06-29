//***************************************
//
// stateinit.h
// init state class
//
// created: 12-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "stateinit.h"

stateInit::stateInit()
{
   //m_stateNext = static_cast<DF_FSM>(DF_FSM::INIT+1);
}

stateInit::stateInit(messageMediator * message)
{
   
}

stateInit::stateInit(int inputDigit){
   //init the cassette 
}

stateInit::~stateInit()
{
   //delete stuff
}

DF_ERROR stateInit::onEntry()
{
   DF_ERROR e_ret  = OK;

   this->m_pXMLSettings = new TiXmlDocument(XML_SETTINGS);
   //test to see if loaded correctly
   
   return e_ret;
}

DF_ERROR stateInit::onAction(DF_FSM * nextState)
{
   //need to check with tinyXML for hardware info
   DF_ERROR e_ret  = ERROR_BAD_PARAMS;

   if (nullptr != nextState)
   {
      // set up major objects

      //load the xml dom parser

      //load the sql manager

      //call a create dispenser method

      //methods to load and test the other various items

   }

   return e_ret;
}

DF_ERROR stateInit::onExit()
{
   DF_ERROR e_ret  = OK;

   //close the xml file

   return e_ret;
}