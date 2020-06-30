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

   m_state = INIT; //ensure the current state is INIT
   m_nextState = INIT;

   this->m_pXMLSettings = new TiXmlDocument(XML_SETTINGS);
   //test to see if loaded correctly
   //debugOutput::sendMessage("Loading XML", INFO);
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

   TiXmlElement *pParam;

   // while(pRoot){
   //    if(pRoot -> FirstChildElement())
   //    {
   //       //debugOutput::sendMessage(pRoot->GetText(), INFO);
   //       std::cout<<pRoot->FirstAttribute()<<std::endl;
   //       pRoot = pRoot ->NextSiblingElement();
   //    }
   // }
   
   //setting up elements for proccessing xml file
   m_pRoot = m_pXMLSettings->FirstChildElement("DRINKFILL");
   m_pHardware = m_pRoot->FirstChildElement("hardware");
   m_pDispenser = m_pHardware->FirstChildElement("dispenser");

   e_ret = setDispenserId();

   if (nullptr != &m_nextState && e_ret == OK)
   {
      // set up major objects

      //load the xml dom parser

      //load the sql manager

      //call a create dispenser method

      //methods to load and test the other various items

      //pParam = pParam->FirstChildElement("solenoid");
      // int i = 0;

      // while(pParam){
      //    //debugOutput::sendMessage(pParam->GetText(), INFO);
      //    pParam = pParam->NextSiblingElement("solenoid");
      // }

      m_nextState = IDLE;
      debugOutput::sendMessage("Hardware initialized...", INFO);
      //e_ret = OK;
   }
   else
   {
      e_ret = ERROR_XMLFILE_NO_MATCH_CONTENT;
   }

   return e_ret;
}

DF_ERROR stateInit::onExit()
{
   DF_ERROR e_ret  = OK;

   //close the xml file
   //m_pXMLSettings->SaveFile();//!!! temperory.... no bult-in file close function

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
      for(TiXmlElement *l_p = m_pDispenser; l_p !=  NULL; l_p = l_p->NextSiblingElement())// != l_pDispenser->NextSiblingElement("dispenser"))
      {
         dispenserId[l_counter] = l_p->Attribute("id");

         debugOutput::sendMessage(dispenserId[l_counter], INFO);

         //if(nullptr == l_pDispenser->NextSiblingElement("dispenser"))
         //   break;

         //l_pDispenser = l_pDispenser->NextSiblingElement("dispenser");
         l_counter++;
      }

      e_ret = OK;
   }

   return e_ret;
}