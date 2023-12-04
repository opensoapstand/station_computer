//***************************************
//
// stateidle.cpp
// idle state class
//
// After stateInit: Waits for a dispense
// command from IPC QT Socket
//
// created: 01-2022
// by:Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "stateIdle.h"

#define IDLE_STRING "Idle"
#include <chrono>
#include <iostream>
#include <thread>

// Default CTOR
stateIdle::stateIdle()
{
   // productDispensers = g_productDispensers;
}

// CTOR Linked to IP Thread Socket Listener
stateIdle::stateIdle(messageMediator *message)
{
   m_pMessaging = message;
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
   g_machine.resetButtonLightAnimation();

   return e_ret;
}

// sendTemperature() {
//    temperatureRefresh=1;
// }

// int timer() {
//     std::chrono::seconds interval(5);

//     auto timerCallback = []() {
//         sendTemperature();
//     };

//     while (true) {
//         auto start = std::chrono::steady_clock::now();

//         timerCallback();

//         auto end = std::chrono::steady_clock::now();
//         auto elapsed = end - start;

//         auto sleepDuration = interval - std::chrono::duration_cast<std::chrono::seconds>(elapsed);

//         if (sleepDuration > std::chrono::seconds(0)) {
//             std::this_thread::sleep_for(sleepDuration);
//         }
//     }

//     return 0;
// }

DF_ERROR stateIdle::onAction()
{
   g_machine.refreshButtonLightAnimation();

   DF_ERROR e_ret = ERROR_BAD_PARAMS;

   if (m_pMessaging->isCommandStringReadyToBeParsed())
   {
      DF_ERROR ret_msg;
      ret_msg = m_pMessaging->parseCommandString();

      if (ACTION_DISPENSE == m_pMessaging->getAction() || ACTION_AUTOFILL == m_pMessaging->getAction())
      {
         if (m_pMessaging->getRequestedSlot() == PRODUCT_SLOT_DUMMY || m_pMessaging->getRequestedSize() == SIZE_DUMMY)
         {
            debugOutput::sendMessage("Invalid dispenser command received. ", MSG_INFO);
         }
         else
         {
            m_state_requested = STATE_DISPENSE_INIT;
         }
      }
      else if (m_pMessaging->getAction() == ACTION_NO_ACTION){

      }
      else if (m_pMessaging->getAction() == ACTION_RESET)
      {
         m_pMessaging->sendMessageOverIP("Init Ready", true); // send to UI
         debugOutput::sendMessage("Send msg to ui: Controller is ready.", MSG_INFO);
      }
      else if ('0' == m_pMessaging->getAction() || ACTION_QUIT == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Request application exit.", MSG_INFO);
         m_state_requested = STATE_END;
      }
      else if ('1' == m_pMessaging->getAction() || ACTION_UI_COMMAND_PRINTER_MENU == m_pMessaging->getAction())
      {
         m_state_requested = STATE_MANUAL_PRINTER;
         // g_stateArray[STATE_MANUAL_PRINTER].sendPrinterStatus();
      }
      else if ('2' == m_pMessaging->getAction())
      {
         m_state_requested = STATE_MANUAL_PUMP;
      }
      else if ('3' == m_pMessaging->getAction())
      {
         m_state_requested = STATE_MANUAL_CONFIG;
      }
      else if ('4' == m_pMessaging->getAction())
      {
         
         debugOutput::sendMessage("Before reload parameters from product", MSG_INFO);
         bool success = g_machine.m_productDDDDDispensers[0].getSelectedProduct()->loadParameters();
         g_machine.m_productDDDDDispensers[0].loadGeneralProperties();
         g_machine.loadGeneralProperties();

         debugOutput::sendMessage("After" + to_string(success), MSG_INFO);
      }
      else if ('5' == m_pMessaging->getAction())
      {
        m_pMessaging->sendTemperatureData();
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
   g_machine.resetButtonLightAnimation();
   DF_ERROR e_ret = OK;
   return e_ret;
}
