//***************************************
//
// stateinit.cpp
// init state class
//
// created: 12-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "stateinit.h"
#include <iostream>
#include <string>

stateInit::stateInit()
{
   //m_stateNext = static_cast<DF_FSM>(DF_FSM::INIT+1);
   for(int i = 0; i < CASSETTES_MAX; i++)
   {
      dispenserId[i] = nullptr;
      debugOutput::sendMessage("Object "+ to_string(i) + " has been created", INFO);
   }
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

   m_state = INIT; //ensure the current state is INIT
   m_nextState = INIT;

   this->m_pXMLSettings = new TiXmlDocument(XML_SETTINGS);
   
   //test to see if loaded correctly
   bool loadOkay = m_pXMLSettings->LoadFile();
   if(loadOkay){
      debugOutput::sendMessage("XML LOADED: " + string(XML_SETTINGS), INFO);
      e_ret = OK;
   }
   else
   {
      debugOutput::sendMessage("Problem loading XML file: " + string(XML_SETTINGS), ERROR);
      e_ret = ERROR_XMLFILE_NOT_FOUND;
   }
   return e_ret;
}

//start to initialize all related hardware
DF_ERROR stateInit::onAction()
{
   //need to check with tinyXML for hardware info
   DF_ERROR e_ret  = ERROR_BAD_PARAMS;

   m_state = INIT; //ensure the current state is INIT

   TiXmlElement *l_pDispenser;
   
   //setting up elements for proccessing xml file
   m_pRoot = m_pXMLSettings->FirstChildElement(DRINKFILL_STRING);
   m_pHardware = m_pRoot->FirstChildElement(HARDWARE_STRING);
   m_pDispenser = m_pHardware->FirstChildElement(DISPENSER_STRING);

   if(nullptr == m_pDispenser){
       debugOutput::sendMessage("m_pDispenser is null", INFO);
       return e_ret = ERROR_XMLFILE_NO_MATCH_CONTENT;
   }
   else{
      l_pDispenser = m_pDispenser;
   }

   e_ret = setDispenserId();

   if (nullptr != &m_nextState && e_ret == OK)
   {
      // set up major objects

      //load the xml dom parser

      //load the sql manager

      //call a create dispenser method

      //methods to load and test the other various items

      //pParam = pParam->FirstChildElement("solenoid");
           
      int idx = 0;

      while(nullptr != dispenserId[idx])
      {
         e_ret = setDispenser(l_pDispenser, idx);
         
         if(OK != e_ret) //if dispenser not set properly, return error
         {
            debugOutput::sendMessage("setDispenser did not return OK", INFO);
            return e_ret;
         }

         if(nullptr != l_pDispenser->NextSiblingElement(DISPENSER_STRING))
         {
            l_pDispenser = l_pDispenser->NextSiblingElement(DISPENSER_STRING);
         }

         idx++;
      }

      if(OK == e_ret)
      {
         m_nextState = IDLE;
      }
   }
   else
   {
      e_ret = ERROR_XMLFILE_NO_MATCH_CONTENT;
      debugOutput::sendMessage("setDispenserId did not return OK", INFO);

   }

   debugOutput::sendMessage("Hardware initialized...", INFO);
   return e_ret;
}

DF_ERROR stateInit::onExit()
{
   DF_ERROR e_ret  = OK;

   //close the xml file
   m_pXMLSettings->SaveFile();//!!! temperory.... no bult-in file close function

   m_state = INIT;
   m_nextState = IDLE; //once everything is good, move to idle state

   return e_ret;
}

DF_ERROR stateInit::setDispenserId()
{
   DF_ERROR e_ret = ERROR_XMLFILE_NO_MATCH_CONTENT;
   int l_counter = 0;

   if (m_pDispenser == NULL){
      e_ret = ERROR_XMLFILE_NO_MATCH_CONTENT;
   }
   else
   {
      TiXmlElement *l_p;

      for(l_p = m_pDispenser; l_p !=  NULL; l_p = l_p->NextSiblingElement())// != l_pDispenser->NextSiblingElement("dispenser"))
      {
         dispenserId[l_counter] = l_p->Attribute("id");
         l_counter++;
      }
      e_ret = OK;
   }

   return e_ret;
}

DF_ERROR stateInit::setDispenser(TiXmlElement *dispenserEle, int dispenserIdx)
{
   DF_ERROR e_ret = ERROR_XMLFILE_NO_MATCH_CONTENT;
   TiXmlElement *l_pSolenoid;

   if(dispenserEle->FirstChildElement(SOLENOID_STRING))
   {
      l_pSolenoid = dispenserEle->FirstChildElement(SOLENOID_STRING);
   } 
   else
   {
      debugOutput::sendMessage("dispenseElement was null", INFO);
      e_ret = ERROR_XMLFILE_NO_MATCH_CONTENT;
      return e_ret;
   }
   
   std::string sDispenser = dispenserId[dispenserIdx];
   debugOutput::sendMessage("Sort for dispenser:" + sDispenser, INFO);

   int l_type = 0;
   TiXmlElement *l_pSingleSolenoid = l_pSolenoid;

   while(nullptr != l_pSingleSolenoid) //should loop through 3 times
   {
      const char* typeCheck = getSolenoidXML(TYPE_STRING, l_pSingleSolenoid);
      std::string sType = typeCheck;

      if("" != typeCheck) //set dispenser parameters accrodingly [mcp|x86|ard]
      {
         if(MCP_STRING== sType)
         {
            //cassettes[dispenserIdx]->;
         }
         else if(X86_STRING == sType)
         {

         }
         else if(ARD_STRING == sType)
         {

         }
         debugOutput::sendMessage("dispenser: " + sDispenser + " |type " + sType, INFO);
         l_pSingleSolenoid = l_pSingleSolenoid -> NextSiblingElement();
      }
      else
      {
            debugOutput::sendMessage("done sorting dispense:" + sDispenser, INFO);
            //finish with current dispenser
      }
   }

   e_ret = OK;
   return e_ret;
}

//return the proper string regarding to solenoid from tinyXML
const char* stateInit::getSolenoidXML(const char* subHeader, TiXmlElement *solenoidEle)
{
   TiXmlElement *type = solenoidEle->FirstChildElement(subHeader);

   const char* char_ret;

   if(nullptr != type)
   {
      char_ret = type->GetText();
   }
   else
   {
      char_ret = {""}; //returning a empty char* if not found 
   }

   //debugOutput::sendMessage(char_ret, INFO);
   return char_ret;
}

