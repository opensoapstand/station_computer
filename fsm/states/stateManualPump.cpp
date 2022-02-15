//***************************************
//
// stateManualPump.cpp
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

#include "stateManualPump.h"

#define STRING_STATE_MANUAL_PUMP "Manual Pump"

// Default CTOR
stateManualPump::stateManualPump()
{
   // printerr = new Adafruit_Thermal();
}

// CTOR Linked to IP Thread Socket Listener
stateManualPump::stateManualPump(messageMediator *message)
{
}

// DTOR
stateManualPump::~stateManualPump()
{
   // delete stuff
}

// Overload for Debugger output
string stateManualPump::toString()
{
   return STRING_STATE_MANUAL_PUMP;
}

// Sets a looped Idle state
DF_ERROR stateManualPump::onEntry()
{
   m_state_requested = STATE_MANUAL_PUMP;
   DF_ERROR e_ret = OK;
   debugOutput::sendMessage("Test pumps manually.", MSG_INFO);
   productDispensers = g_productDispensers;

   // default pump setting
   productDispensers[0].forwardPump();
   productDispensers[0].setPumpPWM(255);


   return e_ret;
}

DF_ERROR stateManualPump::onAction()
{
   DF_ERROR e_ret = OK;

   // Check if Command String is ready
   if (m_pMessaging->isCommandStringReadyToBeParsed())
   {
      DF_ERROR ret_msg;
      ret_msg = m_pMessaging->parseCommandString();

      if (ACTION_QUIT == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Exit pump test", MSG_INFO);

         m_state_requested = STATE_IDLE;
      }

      if (ACTION_MANUAL_PUMP_TEST == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Do pump test", MSG_INFO);
         pumpTest();
      }

      if (ACTION_MANUAL_PUMP_ENABLE == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Enable dispenser pump at pos 1 (needs button to be pressed to actually work)", MSG_INFO);
         productDispensers[0].enablePump(1); // POS is 1->4! index is 0->3
      }

      if (ACTION_MANUAL_PUMP_DISABLE == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Disable dispenser pump at pos 1", MSG_INFO);
         productDispensers[0].disableAllPumps();
      }
      if (ACTION_MANUAL_PUMP_DIRECTION_FORWARD == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Direction forward dispenser pump at pos 1", MSG_INFO);
         productDispensers[0].forwardPump();
      }
      if (ACTION_MANUAL_PUMP_DIRECTION_REVERSE == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Direction backward pump at pos 1", MSG_INFO);
         productDispensers[0].reversePump();
      }
      if (ACTION_MANUAL_PUMP_PWM_SET == m_pMessaging->getAction())
      {
         // debugOutput::sendMessage("PWM Speed pump at pos 1 to " + m_pMessaging->getCommandValue(), MSG_INFO);
         productDispensers[0].setPumpPWM(m_pMessaging->getCommandValue());
      }

      if (ACTION_HELP == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("help\nAvailable printer test commands: \n t: test pump1 \n e: enable pump1 \n d: disable pump1\n f: direction forward pump1 \n r: direction reverse pump1\n ixxx: set pwm pump1 [0..255]\nq: quit ", MSG_INFO);
      }
   }

   e_ret = OK;

   return e_ret;
}

DF_ERROR stateManualPump::pumpTest()
{
   debugOutput::sendMessage("pump pump", MSG_INFO);

   productDispensers[0].forwardPump();
   productDispensers[0].setPumpPWM(125);
   productDispensers[0].enablePump(1); // POS is 1->4! index is 0->3
   usleep(1000000);                     // press button to have the pump pumping.
   productDispensers[0].disableAllPumps();

   productDispensers[0].reversePump();
   productDispensers[0].setPumpPWM(125);
   productDispensers[0].enablePump(1); // POS is 1->4! index is 0->3
   usleep(1000000);                     // press button to have the pump pumping.
   productDispensers[0].disableAllPumps();
}

// Advances to Dispense Idle
DF_ERROR stateManualPump::onExit()
{
   DF_ERROR e_ret = OK;
   return e_ret;
}