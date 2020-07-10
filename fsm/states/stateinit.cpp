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

#define INIT_STRING "Init"

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
   for(int i = 0; i < CASSETTES_MAX; i++)
   {
      dispenserId[i] = nullptr;
      debugOutput::sendMessage("Object "+ to_string(i) + " has been created", INFO);
   }
}

stateInit::~stateInit()
{
   //delete stuff
}

string stateInit::toString()
{
   return INIT_STRING;
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

      debugOutput::sendMessage("Set up button: ------------------" , INFO);

      e_ret = setButton(m_pHardware, idx);

      if(OK != e_ret) //if solenoid not set properly, return error
      {
         debugOutput::sendMessage("setButton did not return OK", INFO);
         return e_ret;
      }

      while(nullptr != dispenserId[idx])
      {
         debugOutput::sendMessage("Sort for dispenser:" + string(dispenserId[idx]), INFO);
         e_ret = setDispenserSolenoid(l_pDispenser, idx);
         
         if(OK != e_ret) //if solenoid not set properly, return error
         {
            debugOutput::sendMessage("setDispenserSolenoid did not return OK", INFO);
            return e_ret;
         }

         e_ret  = ERROR_BAD_PARAMS; //reset e_ret
         e_ret = setDispenserPump(l_pDispenser, idx);

         if(OK != e_ret) //if flowsensor not set properly, return error
         {
            debugOutput::sendMessage("setDispenserPump did not return OK", INFO);
            return e_ret;
         }

         e_ret  = ERROR_BAD_PARAMS; //reset e_ret
         e_ret = setDispenserFlowSensor(l_pDispenser, idx);

         if(OK != e_ret) //if flowsensor not set properly, return error
         {
            debugOutput::sendMessage("setDispenserFlowsensor did not return OK", INFO);
            return e_ret;
         }
        

         //check for nullptr
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

//extracting the id of the dispenser
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

DF_ERROR stateInit::setDispenserSolenoid(TiXmlElement *dispenserEle, int dispenserIdx)
{
   DF_ERROR e_ret = ERROR_XMLFILE_NO_MATCH_CONTENT;
   TiXmlElement *l_pSolenoid;

   if(dispenserEle->FirstChildElement(SOLENOID_STRING))
   {
      //get the node of solenoid
      l_pSolenoid = dispenserEle->FirstChildElement(SOLENOID_STRING); 
   } 
   else
   {
      debugOutput::sendMessage("Solenoid element is null", INFO);
      e_ret = ERROR_XMLFILE_NO_MATCH_CONTENT;
      return e_ret;
   }
   
   int l_pos = 0;
   TiXmlElement *l_pSingleSolenoid = l_pSolenoid;

   while(nullptr != l_pSingleSolenoid && l_pos < NUM_SOLENOID) //should loop through 3 times
   {
      string typeCheck = getXML(TYPE_STRING, l_pSingleSolenoid);

      if("" != typeCheck) //set dispenser parameters accrodingly [mcp|x86|ard]
      {
         if(MCP_STRING == typeCheck) //ensures solenoid is control with mcp pins
         {
            int address_num = atoi(getXML(I2CADDRESS_STRING, l_pSingleSolenoid));
            int pin_num = atoi(getXML(IO_STRING, l_pSingleSolenoid));
            cassettes[dispenserIdx]->setSolenoid(address_num, pin_num, l_pos);

            debugOutput::sendMessage("Solenoid:   " + to_string(l_pos) + " |type " + typeCheck 
                                     + " |address " + to_string(address_num) + " |pin " + to_string(pin_num), INFO);
         }
         else
         {
            //wrong xml content found
            e_ret = ERROR_XMLFILE_NO_MATCH_CONTENT;
            return e_ret;
         }

         l_pSingleSolenoid = l_pSingleSolenoid -> NextSiblingElement(SOLENOID_STRING);
      }
      else
      {
            debugOutput::sendMessage("done sorting dispense:" + string(dispenserId[dispenserIdx]), INFO);
            //finish with current dispenser
      }
      l_pos++;
   }

   e_ret = OK;
   return e_ret;
}

DF_ERROR stateInit::setDispenserFlowSensor(TiXmlElement *dispenserEle, int dispenserIdx)
{
   DF_ERROR e_ret = ERROR_XMLFILE_NO_MATCH_CONTENT;
   TiXmlElement *l_pFlowsensor;

   if(dispenserEle->FirstChildElement(FLOWSENSOR_STRING))
   {
      //get the node of flowsnesor
      l_pFlowsensor = dispenserEle->FirstChildElement(FLOWSENSOR_STRING); 
   } 
   else
   {
      debugOutput::sendMessage("Flowsensor element is null", INFO);
      e_ret = ERROR_XMLFILE_NO_MATCH_CONTENT;
      return e_ret;
   }
   
   int l_pos = 0;
   TiXmlElement *l_pSingleFlowsensor = l_pFlowsensor;

   while(nullptr != l_pSingleFlowsensor && l_pos < NUM_FLOWSENSOR) //should loop through once times
   {
      string typeCheck = getXML(TYPE_STRING, l_pSingleFlowsensor);

      if("" != typeCheck) //set dispenser parameters accrodingly [mcp|x86|ard]
      {
         if(X86_STRING == typeCheck) //ensures flowsensor is control with x86 pin
         {
            int pin_num = atoi(getXML(IO_STRING, l_pSingleFlowsensor));
            debugOutput::sendMessage("Flowsensor: " + to_string(l_pos) + " |type " + typeCheck 
                                     + "             |pin " + to_string(pin_num), INFO);
            
            cassettes[dispenserIdx]->setFlowsensor(pin_num, l_pos);
         }
         else
         {
            //wrong xml content found
            e_ret = ERROR_XMLFILE_NO_MATCH_CONTENT;
            return e_ret;
         }

         l_pSingleFlowsensor = l_pSingleFlowsensor -> NextSiblingElement(FLOWSENSOR_STRING);
      }
      else
      {
            debugOutput::sendMessage("done sorting dispense:" + string(dispenserId[dispenserIdx]), INFO);
      }
      l_pos++;
   }

   e_ret = OK;
   return e_ret;
}

DF_ERROR stateInit::setDispenserPump(TiXmlElement *dispenserEle, int dispenserIdx)
{
   DF_ERROR e_ret = ERROR_XMLFILE_NO_MATCH_CONTENT;
   TiXmlElement *l_pPump;

   if(dispenserEle->FirstChildElement(PUMP_STRING))
   {
      //get the node of solenoid
      l_pPump = dispenserEle->FirstChildElement(PUMP_STRING); 
   } 
   else
   {
      debugOutput::sendMessage("Pump element is null", INFO);
      //e_ret = ERROR_XMLFILE_NO_MATCH_CONTENT;
      e_ret = OK; //not all cassettes have pump element
      return e_ret;
   }
   
   int l_pos = 0;
   TiXmlElement *l_pSinglePump = l_pPump;

   while(nullptr != l_pSinglePump && l_pos < NUM_PUMP) //should loop through 3 times
   {
      string typeCheck = getXML(TYPE_STRING, l_pSinglePump);

      if("" != typeCheck) //set dispenser parameters accrodingly [mcp|x86|ard]
      {
         if(MCP_STRING == typeCheck) //ensures pump is control with mcp pins
         {
            int address_num = atoi(getXML(I2CADDRESS_STRING, l_pSinglePump));
            int pin_num = atoi(getXML(IO_STRING, l_pSinglePump));
            cassettes[dispenserIdx]->setPump(address_num, pin_num, l_pos);

            debugOutput::sendMessage("Pump:       " + to_string(l_pos) + " |type " + typeCheck 
                                     + " |address " + to_string(address_num) + " |pin " + to_string(pin_num), INFO);
         }
         else
         {
            //wrong xml content found
            e_ret = ERROR_XMLFILE_NO_MATCH_CONTENT;
            return e_ret;
         }

         l_pSinglePump = l_pSinglePump -> NextSiblingElement(PUMP_STRING);
      }
      else
      {
            debugOutput::sendMessage("done sorting dispense:" + string(dispenserId[dispenserIdx]), INFO);
            //finish with current dispenser
      }
      l_pos++;
   }

   e_ret = OK;
   return e_ret;
}

DF_ERROR stateInit::setButton(TiXmlElement *hardwareEle, int dispenserIdx)
{
   DF_ERROR e_ret = ERROR_XMLFILE_NO_MATCH_CONTENT;
   TiXmlElement *l_pButton;

   if(hardwareEle->FirstChildElement(BUTTON_STRING))
   {
      //get the node of flowsnesor
      l_pButton = hardwareEle->FirstChildElement(BUTTON_STRING); 
   } 
   else
   {
      debugOutput::sendMessage("Button element is null", INFO);
      e_ret = ERROR_XMLFILE_NO_MATCH_CONTENT; //no button should return error
      return e_ret;
   }
   
   int l_pos = 0;
   TiXmlElement *l_pSingleButton = l_pButton;

   while(nullptr != l_pSingleButton && l_pos < NUM_BUTTON) //should loop through once times
   {
      string typeCheck = getXML(TYPE_STRING, l_pSingleButton);

      if("" != typeCheck) //set dispenser parameters accrodingly [mcp|x86|ard]
      {
         if(MCP_STRING == typeCheck) //ensures button is control with mcp pin
         {
            int address_num = atoi(getXML(I2CADDRESS_STRING, l_pSingleButton));
            int pin_num = atoi(getXML(IO_STRING, l_pSingleButton));

            debugOutput::sendMessage("Button:     " + to_string(l_pos) + " |type " + typeCheck 
                                     + " |address " + to_string(address_num) + " |pin " + to_string(pin_num), INFO);                         
         
            if((X20 <= address_num &&  X22 >= address_num) && (MCP_PIN_START <= pin_num && MPC_PIN_END >= pin_num))
            {
               m_pButton[l_pos] = new mcpGPIO(address_num, pin_num);
               e_ret = OK;
               return e_ret;
            }
            else if(X20 > address_num || X22 < address_num)
            {
               e_ret = ERROR_WRONG_I2C_ADDRESS;
               return e_ret;
            }
            else if(MCP_PIN_START > pin_num || MPC_PIN_END < pin_num)
            {
               e_ret = ERROR_BAD_PARAMS;
               return e_ret;
            }
         }
         else
         {
            //wrong xml content found
            e_ret = ERROR_XMLFILE_NO_MATCH_CONTENT;
            return e_ret;
         }

         l_pSingleButton = l_pSingleButton -> NextSiblingElement(BUTTON_STRING);
      }
      else
      {
            debugOutput::sendMessage("done sorting dispense:" + string(dispenserId[dispenserIdx]), INFO);
      }
      l_pos++;
   }

   e_ret = OK;
   return e_ret;
}

//return the proper string regarding to solenoid from tinyXML
const char* stateInit::getXML(const char* subHeader, TiXmlElement *childEle)
{
   TiXmlElement *type = childEle->FirstChildElement(subHeader);

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

