//***************************************
//
// statedispense.h
// dispense state class
//
// Recieves and interprets string command
// from messageMediator in FSM.
// Routes dispense instruction to GPIO's
//
// created: 01-2022
// by:Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "stateDispense.h"
#include <cstring>

#define DISPENSE_STRING "Dispense"

// CTOR
stateDispense::stateDispense()
{
}

// CTOR Linked to IPC
stateDispense::stateDispense(messageMediator *message)
{
   m_pMessaging = message;
}

// DTOR
stateDispense::~stateDispense()
{
}

// Overload for Debugger output
string stateDispense::toString()
{
   return DISPENSE_STRING;
}

/*
 * Called from FSM loop to check state before inAction execution
 * TODO: Create State Class to hold this function definition
 */
DF_ERROR stateDispense::onEntry()
{
   m_state_requested = STATE_DISPENSE;
   // productDispensers = g_productDispensers;
   DF_ERROR e_ret = OK;
   slot = m_pMessaging->getRequestedSlot();
   slot_index = slot - 1;

   if (m_pMessaging->getAction() == ACTION_AUTOFILL)
   {
      startPumping();
   }

   return e_ret;
}


DF_ERROR stateDispense::onAction()
{
   // productDispensers = g_productDispensers;
   DF_ERROR e_ret = ERROR_BAD_PARAMS;

   if (m_pMessaging->isCommandStringReadyToBeParsed())
   {
      m_pMessaging->parseCommandString();
   }

   if (m_pMessaging->getAction() != ACTION_AUTOFILL)
   {
      if (g_machine.m_productDispensers[slot_index].getDispenseButtonEdgePositive())
      {
         debugOutput::sendMessage("Dispense button pressed edge", MSG_INFO);
         m_pMessaging->sendMessageOverIP("Dispense Button Pos Edge", true); // send to UI

         startPumping();
         g_machine.m_productDispensers[slot_index].addDispenseButtonPress();
      }

      if (g_machine.m_productDispensers[slot_index].getDispenseButtonEdgeNegative())
      {
         debugOutput::sendMessage("Dispense button released edge", MSG_INFO);
         m_pMessaging->sendMessageOverIP("Dispense Button Neg Edge", true); // send to UI

         stopPumping();
      }
   }

   // Send amount dispensed to UI (to show in Maintenance Mode, and/or animate filling)

   g_machine.m_productDispensers[slot_index].updateActiveProductFlowRateRunningAverageWindow();
   g_machine.m_productDispensers[slot_index].updateDispenseStatus();
   g_machine.m_productDispensers[slot_index].updateSlotState();

   if (g_machine.m_productDispensers[slot_index].getIsStatusUpdateAllowed())
   {
      double volume = g_machine.m_productDispensers[slot_index].getSelectedProductVolumeDispensed();

      Time_val avg_02s = g_machine.m_productDispensers[slot_index].getAveragedProductFlowRate(1000);
      double flowrate = avg_02s.value;
      // const char *statusStringChar = g_machine.m_productDispensers[slot_index].getDispenseStatusAsString();
      // std::string statusString(statusStringChar);
      // std::string message = "dispenseupdate|" + std::to_string(volume) + "|" + std::to_string(flowrate) + "|" + statusString;
      const char *statusStringChar = g_machine.m_productDispensers[slot_index].getSlotStateAsString();
      std::string statusString(statusStringChar);
      std::string message = "dispenseupdate|" + std::to_string(volume) + "|" + std::to_string(flowrate) + "|" + statusString;
#define PRINT_STATUS

#ifdef PRINT_STATUS
      m_pMessaging->sendMessageOverIP(message, true); // send to UI

      // update of the actual dispense
      const char *dispenseStatusStr = g_machine.m_productDispensers[slot_index].getDispenseStatusAsString();
      debugOutput::sendMessage(dispenseStatusStr, MSG_INFO);
      debugOutput::sendMessage(to_string( g_machine.m_productDispensers[slot_index].m_pcb->getFlowSensorPulsesSinceEnabling(slot)), MSG_INFO);
#endif
   }

   if (m_pMessaging->getAction() == ACTION_RESET)
   {
      m_pMessaging->sendMessageOverIP("Init Ready", true); // send to UI
      m_state_requested = STATE_IDLE;
      stopPumping();
      return e_ret = OK;
   }

   // Check if UI has sent a ACTION_DISPENSE_END to finish the transaction, or, if dispensing is complete
   if (m_pMessaging->getAction() == ACTION_DISPENSE_END)
   {
      debugOutput::sendMessage("Stop dispensing (stop command received)", MSG_INFO);
      m_state_requested = STATE_DISPENSE_END;
      stopPumping();

      return e_ret = OK;
   }

   if (m_pMessaging->getAction() == ACTION_REPAIR_PCA)
   {
      if (g_machine.m_productDispensers[slot_index].m_pcb->get_pcb_version() == pcb::PcbVersion::EN134_4SLOTS || g_machine.m_productDispensers[slot_index].m_pcb->get_pcb_version() == pcb::PcbVersion::EN134_8SLOTS)
      {
         g_machine.m_productDispensers[slot_index].m_pcb->sendEN134DefaultConfigurationToPCA9534(slot, true);
         m_pMessaging->resetAction();
         g_machine.control_pcb->setSingleDispenseButtonLight(slot, true);

         g_machine.m_productDispensers[slot_index].m_pcb->flowSensorEnable(slot);
      }
   }

   if (g_machine.m_productDispensers[slot_index].isSelectedProductVolumeTargetReached())
   {
      debugOutput::sendMessage("Stop dispensing. Requested volume reached. " + to_string(g_machine.m_productDispensers[slot_index].getSelectedProductVolumeDispensed()), MSG_INFO);
      m_state_requested = STATE_DISPENSE_END;
      stopPumping();
      return e_ret = OK;
   }

   e_ret = OK;

   return e_ret;
}

void stateDispense::startPumping()
{

   switch (g_machine.getHardwareVersion())
   {
   case (machine::HardwareVersion::AP1):
   case (machine::HardwareVersion::SS1):
   case (machine::HardwareVersion::SS2):
   {
      debugOutput::sendMessage("start pumping SS2.", MSG_INFO);
      g_machine.m_productDispensers[slot_index].m_pcb->setPumpSpeedPercentage(0); // pump speed is inverted!
      g_machine.m_productDispensers[slot_index].m_pcb->setPumpDirection(slot, true);

      g_machine.m_productDispensers[slot_index].m_pcb->startPump(slot);
      g_machine.m_productDispensers[slot_index].m_pcb->setSpoutSolenoid(slot, true);
   }
   break;
   case (machine::HardwareVersion::SS09):
   {
      debugOutput::sendMessage("start pumping SS09.", MSG_INFO);
      g_machine.m_productDispensers[slot_index].pumpSlowStart(true);
   }
   break;
   case (machine::HardwareVersion::AP2):
   {
      debugOutput::sendMessage("start pumping AP2.", MSG_INFO);
      g_machine.m_productDispensers[slot_index].m_pcb->startPump(slot);
      g_machine.m_productDispensers[slot_index].m_pcb->setSpoutSolenoid(slot, true);
   }
   break;
   default:
   {
      debugOutput::sendMessage("UNKNOWN HARDWARE version", MSG_ERROR);
   }
   break;
   }
}

void stateDispense::stopPumping()
{

   switch (g_machine.getHardwareVersion())
   {
   case (machine::HardwareVersion::AP1):
   case (machine::HardwareVersion::SS1):
   case (machine::HardwareVersion::SS2):
   {
      if (g_machine.m_productDispensers[slot_index].m_pcb->get_pcb_version() == pcb::PcbVersion::EN134_4SLOTS)
      {
         g_machine.m_productDispensers[slot_index].m_pcb->stopPump(slot);
         g_machine.m_productDispensers[slot_index].m_pcb->setSpoutSolenoid(slot, false);
      }
      else
      {
         debugOutput::sendMessage("pcb not compatible", MSG_ERROR);
      }
   }
   break;
   case (machine::HardwareVersion::SS09):
   {
      g_machine.m_productDispensers[slot_index].pumpSlowStopBlocking();
      rectractProductBlocking();
   }
   break;
   case (machine::HardwareVersion::AP2):
   {
      if (g_machine.m_productDispensers[slot_index].m_pcb->get_pcb_version() == pcb::PcbVersion::EN258_4SLOTS)
      {
         g_machine.m_productDispensers[slot_index].m_pcb->stopPump(slot);
         g_machine.m_productDispensers[slot_index].m_pcb->setSpoutSolenoid(slot, false);
      }
      else
      {
         debugOutput::sendMessage("pcb not compatiblevnot en258 4 slots", MSG_ERROR);
      }
   }
   break;
   default:
   {
      debugOutput::sendMessage("UNKNOWN HARDWARE version", MSG_ERROR);
   }
   break;
   }
}

DF_ERROR stateDispense::rectractProductBlocking()
{
   DF_ERROR e_ret = OK;
   g_machine.m_productDispensers[slot_index].reversePumpForSetTimeMillis(g_machine.m_productDispensers[slot_index].getSelectedProduct()->getRetractionTimeMillis());
   return e_ret;
}

// Actions on leaving Dispense state
DF_ERROR stateDispense::onExit()
{
   g_machine.m_productDispensers[slot_index].setPumpsDisableAll();
   // g_machine.m_productDispensers[slot_index].m_pcb->virtualButtonUnpressHack(this->slot);
   g_machine.m_productDispensers[slot_index].m_pcb->setSingleDispenseButtonLight(this->slot, false);

   DF_ERROR e_ret = OK;
   return e_ret;
}
