//***************************************
//
// stateinit.h
// init state class
//
// Set Objects for FSM and Addresses for 
// GPIO's through XML Reference.
// Check all Hardware is operational.
// Initialize threads for operation and 
// communication in machine.
// Connect or create database.
//
// created: 12-06-2020
// by: Denis Londry & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "stateInit.h"
#include <iostream>
#include <string>
#include "../fsm.h"

#define INIT_STRING "Init"

// Default Ctor
stateInit::stateInit()
{
   //m_stateNext = static_cast<DF_FSM>(DF_FSM::INIT+1);
   // for(int i = 0; i < CASSETTES_MAX; i++)
   // {
   //    dispenserId[i] = nullptr;
   //    debugOutput::sendMessage("Object "+ to_string(i) + " has been created", INFO);
   // }
}

// CTOR Linked to IPC
stateInit::stateInit(messageMediator * message)
{

}

// DTOR
stateInit::~stateInit()
{
   //delete stuff
}

// Overload for Debugger output
string stateInit::toString()
{
   return INIT_STRING;
}

/*
 * Initialize FSM State and set XML file reference with 
 * TinyXML library
 */
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
      
      //setting up elements for proccessing xml file
      m_pRoot = m_pXMLSettings->FirstChildElement(DRINKFILL_STRING);
      m_pHardware = m_pRoot->FirstChildElement(HARDWARE_STRING);
      m_pDispenser = m_pHardware->FirstChildElement(DISPENSER_STRING);
      // TODO: Insert node reference for sensors
      e_ret = OK;
   }
   else
   {
      debugOutput::sendMessage("Problem loading XML file: " + string(XML_SETTINGS), ERROR);
      e_ret = ERROR_SECU_XMLFILE_NOT_FOUND;
   }
   return e_ret;
}

// Initialize all related hardware
DF_ERROR stateInit::onAction()
{
   //need to check with tinyXML for hardware info
   DF_ERROR e_ret  = ERROR_BAD_PARAMS;
   m_state = INIT; //ensure the current state is INIT

   // Initialization do e_rets
   
   /*
   * Note: Button set up in state virtual.
   * Set up Dispenser Register; 
   * Dispenser Array to hold Register values;
   * Setup drink Inventory Array;
   */
   e_ret = setDispenserId();

   if(OK != e_ret) {
      debugOutput::sendMessage("setDispenserId did not return OK", INFO);
   }
   // TODO: Seperate buttonSetup() atomic function.

   if(OK == e_ret)
   {
      e_ret = setDrinks();
   }

   if(OK == e_ret) 
   {
      e_ret = dispenserSetup();// Do error check per state
   }


   if (nullptr != &m_nextState && OK == e_ret)
   {
      if(OK == e_ret)
      {
         m_nextState = IDLE;
      }
   }
   else
   {
      e_ret = ERROR_SECU_XMLFILE_NO_MATCH_CONTENT;

   }
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

// Set Dispenser Cassettes:
// Extract addressing id from XML and map to dispenser
DF_ERROR stateInit::setDispenserId()
{
   DF_ERROR e_ret = ERROR_SECU_XMLFILE_NO_MATCH_CONTENT;
   int l_counter = 0;

   if (m_pDispenser == NULL){
      e_ret = ERROR_SECU_XMLFILE_NO_MATCH_CONTENT;
   }
   else
   {
      TiXmlElement *l_p;
      const char * att;
      l_counter = 0;
      int position = 0;
      // for(l_p = m_pDispenser; l_p->ValueTStr() !=  "dispenser"; l_p = l_p->NextSiblingElement())// != l_pDispenser->NextSiblingElement("dispenser"))
      for(l_p = m_pDispenser; l_p !=  NULL; l_p = l_p->NextSiblingElement())// != l_pDispenser->NextSiblingElement("dispenser"))
      {
         // FIXME: UGLY way to iterate through XML.  Need to find better way!
         cout << "accessing element" << endl;
         string szTemp = l_p->Value();        
         if (0 == szTemp.compare("dispenser")) {  
            cout << l_p->Value() << endl;
            att = l_p->Attribute("id");
            position = *att - '0';
            cout << position << endl;
            dispenserId[position] = l_p->Attribute("id");
         }
      }
      debugOutput::sendMessage("Done getting dispense ID!", INFO);
      e_ret = OK;
   }

   return e_ret;
}

// Set Solenoids on Dispenser Cassettes
// Extract addressing id from XML and map to Solenoid
DF_ERROR stateInit::setDispenserSolenoid(TiXmlElement *dispenserEle, int dispenserIdx, dispenser* cassettes)
{
   DF_ERROR e_ret = ERROR_SECU_XMLFILE_NO_MATCH_CONTENT;
   TiXmlElement *l_pSolenoid;

   if(dispenserEle->FirstChildElement(SOLENOID_STRING))
   {
      //get the node of solenoid
      l_pSolenoid = dispenserEle->FirstChildElement(SOLENOID_STRING); 
   } else {
      debugOutput::sendMessage("Solenoid element is null", INFO);
      e_ret = ERROR_SECU_XMLFILE_NO_MATCH_CONTENT;
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
            cassettes[dispenserIdx].setSolenoid(address_num, pin_num, l_pos);

            //debugOutput::sendMessage("Solenoid:   " + to_string(l_pos) + " |type " + typeCheck 
            //                         + " |address " + to_string(cassettes[dispenserIdx]->getI2CAddress(l_pos)) + " |pin " + to_string(pin_num), INFO);
            debugOutput::sendMessage("Solenoid:   " + to_string(l_pos) + " |type " + typeCheck 
                                     + " |address " + to_string(address_num) + " |pin " + to_string(pin_num), INFO);
         }
         else
         {
            //wrong xml content found
            e_ret = ERROR_SECU_XMLFILE_NO_MATCH_CONTENT;
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

// Set Flow Sensors
// Extract addressing id from XML and map to Flowsensor
DF_ERROR stateInit::setDispenserFlowSensor(TiXmlElement *dispenserEle, int dispenserIdx, dispenser* cassettes)
{
   DF_ERROR e_ret = ERROR_SECU_XMLFILE_NO_MATCH_CONTENT;
   TiXmlElement *l_pFlowsensor;

   if(dispenserEle->FirstChildElement(FLOWSENSOR_STRING))
   {
      //get the node of flowsnesor
      l_pFlowsensor = dispenserEle->FirstChildElement(FLOWSENSOR_STRING); 
   } 
   else
   {
      debugOutput::sendMessage("Flowsensor element is null", INFO);
      e_ret = ERROR_SECU_XMLFILE_NO_MATCH_CONTENT;
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
            
            cassettes[dispenserIdx].setFlowsensor(pin_num, l_pos);
         }
         else
         {
            //wrong xml content found
            e_ret = ERROR_SECU_XMLFILE_NO_MATCH_CONTENT;
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

// Set Dispenser Pumps
// Extract addressing id from XML and map to Still Pumps
DF_ERROR stateInit::setDispenserPump(TiXmlElement *dispenserEle, int dispenserIdx, dispenser* cassettes)
{
   DF_ERROR e_ret = ERROR_SECU_XMLFILE_NO_MATCH_CONTENT;
   TiXmlElement *l_pPump;

   if(dispenserEle->FirstChildElement(PUMP_STRING))
   {
      //get the node of solenoid
      l_pPump = dispenserEle->FirstChildElement(PUMP_STRING); 
   } 
   else
   {
      debugOutput::sendMessage("Pump element is null", INFO);
      //e_ret = ERROR_SECU_XMLFILE_NO_MATCH_CONTENT
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
            cassettes[dispenserIdx].setPump(address_num, pin_num, l_pos);

            debugOutput::sendMessage("Pump:       " + to_string(l_pos) + " |type " + typeCheck 
                                     + " |address " + to_string(address_num) + " |pin " + to_string(pin_num), INFO);
         }
         else
         {
            //wrong xml content found
            e_ret = ERROR_SECU_XMLFILE_NO_MATCH_CONTENT;
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

// Set Dispenser Button
// Extract addressing id from XML and map to Dispensing Button
// BUTTON REFERENCE in State Virtual Required due to LED Arduino control.
DF_ERROR stateInit::setButton(TiXmlElement *hardwareEle, int dispenserIdx)
{
   DF_ERROR e_ret = ERROR_SECU_XMLFILE_NO_MATCH_CONTENT;
   TiXmlElement *l_pButton;

   if(hardwareEle->FirstChildElement(BUTTON_STRING))
   {
      // Get the button node
      debugOutput::sendMessage("Found Button Element", INFO);
      l_pButton = hardwareEle->FirstChildElement(BUTTON_STRING); 
   } else
   {
      debugOutput::sendMessage("Button element is null", ERROR);
      e_ret = ERROR_SECU_XMLFILE_NO_MATCH_CONTENT; // XML missing button
      return e_ret;
   }
   
   int l_pos = 0;
   TiXmlElement *l_pSingleButton = l_pButton;

   while(nullptr != l_pSingleButton && l_pos < NUM_BUTTON) //should loop through once times
   {
      string typeCheck = getXML(TYPE_STRING, l_pSingleButton);

      debugOutput::sendMessage("Button typeCheck Result:" + typeCheck, INFO);

      // if("" != typeCheck) //set dispenser parameters accrodingly [mcp|x86|ard]
      // {
         if(MCP_STRING == typeCheck) //ensures button is control with mcp pin
         {
            int address_num = atoi(getXML(I2CADDRESS_STRING, l_pSingleButton));
            int pin_num = atoi(getXML(IO_STRING, l_pSingleButton));

            debugOutput::sendMessage("Button:     " + to_string(l_pos) + " |type " + typeCheck 
                                     + " |address " + to_string(address_num) + " |pin " + to_string(pin_num), INFO);                         
         
            if((X20 <= address_num &&  X22 >= address_num) && (MCP_PIN_START <= pin_num && MPC_PIN_END >= pin_num))
            {
               m_pButton[l_pos] = new mcpGPIO(address_num, pin_num);
               debugOutput::sendMessage("Set button High", PIN_CHANGE);
               m_pButton[l_pos]->writePin(HIGH);
               e_ret = OK;
               return e_ret;
            }
            else if(X20 > address_num || X22 < address_num)
            {
               e_ret = ERROR_ELEC_WRONG_I2C_ADDRESS;
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
            e_ret = ERROR_SECU_XMLFILE_NO_MATCH_CONTENT;
            return e_ret;
         }

         l_pSingleButton = l_pSingleButton -> NextSiblingElement(BUTTON_STRING);
      // }
      // else
      // {
      //       debugOutput::sendMessage("done sorting dispense:" + string(dispenserId[dispenserIdx]), INFO);
      // }
      l_pos++;
   }

   e_ret = OK;
   return e_ret;
}

// Return the proper string regarding to solenoid from tinyXML
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

// Initilization function for all dispensers...mainly function calls
// XML and GPIO mapping.
DF_ERROR stateInit::dispenserSetup()
{
   //need to check with tinyXML for hardware info
   DF_ERROR e_ret  = ERROR_BAD_PARAMS;

   dispenser* cassettes = g_cassettes;

   TiXmlElement *l_pDispenser;

   // XML Node (ideally through 0-2 Slots for Cassettes)
   int idx = 0;

   // Move this to a Function for setup button...Atomic INIT on action
   debugOutput::sendMessage("Set up button: ------------------" , INFO);\
   e_ret = setButton(m_pHardware, idx);


   // Move this to state virtual
   // Protected
   // static dispenser cassettes[CASSETTES_MAX];

   if(nullptr == m_pDispenser){
       debugOutput::sendMessage("m_pDispenser is null", INFO);
       e_ret = ERROR_SECU_XMLFILE_NO_MATCH_CONTENT;
   }
   else{
      l_pDispenser = m_pDispenser;
   }

   if(OK != e_ret) //if solenoid not set properly, return error
   {
      debugOutput::sendMessage("setButton did not return OK", INFO);
      return e_ret;
   }

   // while(nullptr != dispenserId[idx])
   while(idx < 9 )
   {

      if(nullptr != dispenserId[idx]) {
      debugOutput::sendMessage("Sort for dispenser:" + to_string(idx), INFO);

      e_ret = setDispenserSolenoid(l_pDispenser, idx, cassettes);
         
      if(OK != e_ret) //if solenoid not set properly, return error
      {
         debugOutput::sendMessage("setDispenserSolenoid did not return OK", INFO);
         return e_ret;
      }

      // NO PUMPS FOR NOW NEED LOGIC CHECK
      /*
      e_ret  = ERROR_BAD_PARAMS; //reset e_ret
      if(idx > PUMP_OPTION_START_POSITION && idx < PUMP_OPTION_STOP_POSITION )
      {
         // FIXME: PUMP LOGIC
         // e_ret = setDispenserPump(l_pDispenser, idx, cassettes);
         setDispenserPump(l_pDispenser, idx, cassettes);
      } else {
         debugOutput::sendMessage("Not a still drink; Out of Pump Range", ERROR);
      }
      */


      // if(OK != e_ret) //if flowsensor not set properly, return error
      // {
      //    debugOutput::sendMessage("setDispenserPump did not return OK", INFO);
      //    // XXX: REMOVE THIS AFTER!!!
      //    e_ret = OK;
      //    return e_ret;
      // }

      e_ret  = ERROR_BAD_PARAMS; //reset e_ret
      // e_ret = setDispenserFlowSensor(l_pDispenser, idx);
      e_ret = setDispenserFlowSensor(l_pDispenser, idx, cassettes);

      // XXX: REMOVE THIS AFTER TESTING
      e_ret = OK;

      if(OK != e_ret) //if flowsensor not set properly, return error
      {
         debugOutput::sendMessage("setDispenserFlowsensor did not return OK", INFO);
         //return e_ret;
         //return;
      }
        

      //check for nullptr
      if(nullptr != l_pDispenser->NextSiblingElement(DISPENSER_STRING))
      {
         l_pDispenser = l_pDispenser->NextSiblingElement(DISPENSER_STRING);
      }
      }
      idx++;
   }
   debugOutput::sendMessage("Hardware initialized...", INFO);

   return e_ret;
}


DF_ERROR stateInit::setDrinks(){

   // Drink Setup
   // load the SQLITE manager

   // FIXME: Hardcode for now.
   

   // for(int i = 0; i < MAX_CASSETTES; i++) {
   //    g_cassettes[i]->setDrink()
   // }
   // Hardcoded drink class for testing

   g_cassettes[0].setDrink(new drink(1, "Drink1", 0, 355, 1.3, 4.00, false, 25));
   g_cassettes[1].setDrink(new drink(2, "Drink2", 0, 355, 1.3, 4.00, false, 25));
   g_cassettes[2].setDrink(new drink(3, "Drink3", 0, 355, 1.3, 4.00, false, 25));
   g_cassettes[3].setDrink(new drink(4, "Drink4", 0, 355, 1.3, 4.00, false, 25));
   g_cassettes[4].setDrink(new drink(5, "Drink5", 0, 355, 1.3, 4.00, false, 25));
   g_cassettes[5].setDrink(new drink(6, "Drink6", 0, 355, 1.3, 4.00, false, 25));
   g_cassettes[6].setDrink(new drink(7, "Drink7", 0, 355, 1.3, 4.00, false, 25));
   g_cassettes[7].setDrink(new drink(8, "Drink8", 0, 355, 1.3, 4.00, false, 25));
   g_cassettes[8].setDrink(new drink(9, "Drink9", 0, 355, 1.3, 4.00, false, 25));

   return OK;
}