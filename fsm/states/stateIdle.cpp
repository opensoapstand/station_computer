//***************************************
//
// stateidle.cpp
// idle state class
//
// After stateInit: Waits for a dispense
// command from IPC QT Socket
//
// created: 01-2022
// by:Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "stateIdle.h"

#define IDLE_STRING "Idle"

// Default CTOR
stateIdle::stateIdle()
{
}

// CTOR Linked to IP Thread Socket Listener
stateIdle::stateIdle(messageMediator *message)
{
}

// DTOR
stateIdle::~stateIdle()
{
   // delete stuff
}

// Overload for Debugger output
string stateIdle::toString()
{
   return IDLE_STRING;
}

// Sets a looped Idle state
DF_ERROR stateIdle::onEntry()
{
   m_state_requested = STATE_IDLE;
   DF_ERROR e_ret = OK;

   return e_ret;
}

DF_ERROR stateIdle::onAction()
{
   DF_ERROR e_ret = ERROR_BAD_PARAMS;

   // if (nullptr != &m_state_requested)
   // {

   // Check if Command String is ready
   if (m_pMessaging->isCommandStringReadyToBeParsed())
   {
      DF_ERROR ret_msg;
      ret_msg = m_pMessaging->parseCommandString();

      if (ACTION_DISPENSE == m_pMessaging->getAction() || ACTION_AUTOFILL == m_pMessaging->getAction())
      {
         m_state_requested = STATE_DISPENSE_INIT;
      }
      else if ('0' == m_pMessaging->getAction() || ACTION_QUIT == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Request application exit.", MSG_INFO);
         m_state_requested = STATE_END;
      }
      else if ('1' == m_pMessaging->getAction() || ACTION_UI_COMMAND_PRINTER_MENU == m_pMessaging->getAction())
      {
         m_state_requested = STATE_MANUAL_PRINTER;
      }
      else if ('2' == m_pMessaging->getAction())
      {
         m_state_requested = STATE_MANUAL_PUMP;
      }
      //  else if (GET_TEMPERATURE == m_pMessaging->getAction()) //temperature code
      //  {
      //     int temperature  = g_machine.getTemperature();
      //     m_pMessaging->sendMessageOverIP("|temperature|" + to_int(temperature));

      //  }
      else if ('3' == m_pMessaging->getAction())
      {
         m_state_requested = STATE_MANUAL_CONFIG;
      }
      else if ('4' == m_pMessaging->getAction())
      {
         productDispensers = g_productDispensers;
         debugOutput::sendMessage("beffooore reload parameters from product1", MSG_INFO);
         bool success = this->productDispensers[0].getProduct()->reloadParametersFromDb();
         debugOutput::sendMessage("after" + to_string(success), MSG_INFO);
      }
      else if ('5' == m_pMessaging->getAction())
      {

         double temperature = this->productDispensers[0].the_pcb->getTemperature();
         // double temperature = this->productDispensers[0].the_pcb->getTemperatureConfigure();
         debugOutput::sendMessage("Temperature in Celsius: " + std::to_string(temperature), MSG_INFO);


         printf("Temperature polling from MCP9808: %.3f Celcius \n", temperature);
         //  m_pMessaging->sendMessageOverIP("|temperature|" + to_int(temperature));
          m_pMessaging->sendMessageOverIP("|temperature|" + (temperature));
      }
      else
      {
         debugOutput::sendMessage("---Main Menu---\n"
                                  " Controller Idle State. Available Commands (enter command digit followed by a semicolon e.g. h;[ENTER] )\n"
                                  " 0: Quit(in independent mode)\n"
                                  " 1: Test printer\n"
                                  " 2: Test pumps\n"
                                  " 3: Manual Config \n"
                                  " 4: Get slot 1 properties loaded from db \n"
                                  " 5: Get temperature from MCP9808 \n"
                                  " [1..4]d[l,s,t]: Enter dispense mode. [product number]d[size] e.g. 1ld;\n"
                                  " [1..4]f[l,s,t]: If dispensing: to End Dispensing [product number]f[size] e.g. 1lf;\n"
                                  " h: Display this help message"
                                  "",
                                  MSG_INFO);
      }
   }
   else
   {
      m_state_requested = STATE_IDLE;
   }
   e_ret = OK;
   // }
   // usleep(1000000);
   return e_ret;
}

// Advances to Dispense Idle
DF_ERROR stateIdle::onExit()
{
   DF_ERROR e_ret = OK;
   return e_ret;
}
