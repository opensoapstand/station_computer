//***************************************
//
// stateManualConfig.cpp
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

#include "stateManualConfig.h"

#define STRING_STATE_MANUAL_CONFIG "Manual Config"

#define MAX_BUF 64

// Default CTOR
stateManualConfig::stateManualConfig()
{
   // printerr = new Adafruit_Thermal();
}

// CTOR Linked to IP Thread Socket Listener
stateManualConfig::stateManualConfig(messageMediator *message)
{
   m_pMessaging = message;
}

// DTOR
stateManualConfig::~stateManualConfig()
{
   // delete stuff
}

// Overload for Debugger output
string stateManualConfig::toString()
{
   return STRING_STATE_MANUAL_CONFIG;
}

// Sets a looped Idle state
DF_ERROR stateManualConfig::onEntry()
{
   m_state_requested = STATE_MANUAL_CONFIG;
   DF_ERROR e_ret = OK;
   debugOutput::sendMessage("Config manually.", MSG_INFO);
   m_active_pump_index = 0;
   // productDispensers = g_productDispensers;
   return e_ret;
}

DF_ERROR stateManualConfig::onAction()
{
   DF_ERROR e_ret = OK;

   // Check if Command String is ready
   if (m_pMessaging->isCommandStringReadyToBeParsed())
   {
      DF_ERROR ret_msg;
      ret_msg = m_pMessaging->parseCommandString();

      
      if (m_pMessaging->getAction() == ACTION_RESET)
      {
         m_pMessaging->sendMessageOverIP("Init Ready", true); // send to UI
         m_state_requested = STATE_IDLE;
      }else if ('0' == m_pMessaging->getAction() || ACTION_QUIT == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Exit Manual Config State", MSG_INFO);

         m_state_requested = STATE_IDLE;
      }
      else if ('1' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Get active slot pca9534 info (active slot: " + to_string((uint8_t)m_active_pump_index+1) + ")", MSG_INFO);
         readRelevantRegisters();
      }
      else if (ACTION_MANUAL_PUMP_SET == m_pMessaging->getAction())
      {
         int val = m_pMessaging->getCommandValue();
         debugOutput::sendMessage("Active slot(1,2,3 or 4) is set to: " + to_string((uint8_t)val), MSG_INFO);
         m_active_pump_index = ((uint8_t)val) - 1;
      }
      else if ('2' == m_pMessaging->getAction())
      {
         g_machine.m_productDDDDDispensers[m_active_pump_index].m_pcb->PCA9534SendByteToSlot(m_active_pump_index + 1, 0x01, 0b00100000); // Output pin values register (has no influence on input values)
         g_machine.m_productDDDDDispensers[m_active_pump_index].m_pcb->PCA9534SendByteToSlot(m_active_pump_index + 1, 0x03, 0b01011000); // Config register 0 = output, 1 = input (https://www.nxp.com/docs/en/data-sheet/PCA9534.pdf)
         debugOutput::sendMessage("Sent EN-134 default configuration to PCA9534 of slot: " + to_string(m_active_pump_index+1), MSG_INFO);
         readRelevantRegisters();
      }
      else if ('3' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Get Printer display status", MSG_INFO);
      }
      else if ('4' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Is Printer reachable?", MSG_INFO);
      }
      else if ('5' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Print transaction?", MSG_INFO);
      }
      else if ('6' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Test send OK to UI", MSG_INFO);
      }
      else
      {
         debugOutput::sendMessage("---Manual config menu---"
                                  "Available commands: \n"
                                  "nX: Set active slot, where X= 1,2,3 or 4\n"
                                  " 0: Exit manual config menu \n"
                                  " 1: Get active slot register info\n"
                                  " 2: Write active slot default register values\n"
                                  " h: Display this help menu\n"
                                  "Register 0x01: value:(76543210) (if output: 1=ON)\n"
                                  "Register 0x03 pin direction (IN(=1) or OUT(=0))\n"
                                  "sequence(76543210):\n"
                                  "PCA9534_EN134_PIN_OUT_SOLENOID 0 \n"
                                  "PCA9534_EN134_PIN_OUT_PUMP_ENABLE 1 \n"
                                  "PCA9534_EN134_PIN_OUT_PUMP_DIR 2 \n"
                                  "PCA9534_EN134_PIN_IN_PUMP_FAULT 3 \n"
                                  "PCA9534_EN134_PIN_IN_BUTTON 4 \n"
                                  "PCA9534_EN134_PIN_OUT_BUTTON_LED_LOW_IS_ON 5 \n"
                                  "PCA9534_EN134_PIN_IN_FLOW_SENSOR_TICKS 6 \n"
                                  "PCA9534_EN134_PIN_OUT_FLOW_SENSOR_ENABLE 7 \n",
                                  MSG_INFO);
      }
   }

   e_ret = OK;

   return e_ret;
}

std::string uint8ToBinaryString(uint8_t x)
{
   std::string result;
   for (int i = 7; i >= 0; i--)
   {
      result += ((x >> i) & 1) ? '1' : '0';
   }
   return result;
}

// Advances to Dispense Idle

void stateManualConfig::readRelevantRegisters()
{
   uint8_t val;
   val = g_machine.m_productDDDDDispensers[m_active_pump_index].m_pcb->PCA9534ReadRegisterFromSlot(m_active_pump_index + 1, 0x03);
   debugOutput::sendMessage("Pin direction register 0x03: " + to_string(val) + "(" + uint8ToBinaryString(val) + ")", MSG_INFO);
   val = g_machine.m_productDDDDDispensers[m_active_pump_index].m_pcb->PCA9534ReadRegisterFromSlot(m_active_pump_index + 1, 0x01);
   debugOutput::sendMessage("Pin output val reg     0x01: " + to_string(val) + "(" + uint8ToBinaryString(val) + ")", MSG_INFO);
}

DF_ERROR stateManualConfig::onExit()
{
   DF_ERROR e_ret = OK;
   return e_ret;
}
// g_machine.print_receipt(name_receipt, receipt_cost, receipt_volume_formatted, time_stamp, units, paymentMethod, plu, "");
