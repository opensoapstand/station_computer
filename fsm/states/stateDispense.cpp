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
   debugOutput::sendMessage("stateDispense: ~stateDispense", MSG_INFO);
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
      g_machine.m_productDispensers[slot_index].startActiveDispensing(); // make part of dispenser.
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

         g_machine.m_productDispensers[slot_index].startActiveDispensing();
         g_machine.m_productDispensers[slot_index].addDispenseButtonPress();
      }

      if (g_machine.m_productDispensers[slot_index].getDispenseButtonEdgeNegative())
      {
         debugOutput::sendMessage("Dispense button released edge", MSG_INFO);
         m_pMessaging->sendMessageOverIP("Dispense Button Neg Edge", true); // send to UI

         g_machine.m_productDispensers[slot_index].stopActiveDispensing();
      }
   }

   // Send amount dispensed to UI (to show in Maintenance Mode, and/or animate filling)

   g_machine.m_productDispensers[slot_index].updateActiveProductFlowRateRunningAverageWindow();
   g_machine.m_productDispensers[slot_index].updateDispenseStatus();
   g_machine.m_productDispensers[slot_index].updateSlotState();

   if (g_machine.m_productDispensers[slot_index].getIsStatusUpdateAllowed())
   {
      double volume = g_machine.m_productDispensers[slot_index].getActiveProductVolumeDispensed();

      Time_val avg_02s = g_machine.m_productDispensers[slot_index].getAveragedProductFlowRate(1000);
      double flowrate = avg_02s.value;
      const char *statusStringChar = g_machine.m_productDispensers[slot_index].getSlotStateAsString();
      std::string statusString(statusStringChar);


      std::string message = "dispenseupdate|" + std::to_string(volume) + "|" + std::to_string(flowrate) + "|" + statusString;
      m_pMessaging->sendMessageOverIP(message, true); // send to UI
      
#define PRINT_STATUS
#ifdef PRINT_STATUS

      // update of the actual dispense
      debugOutput::sendMessage("-----------Dispense update. State: " + std::string(g_machine.m_productDispensers[slot_index].getDispenseStatusAsString()), MSG_INFO);
      debugOutput::sendMessage(g_machine.m_productDispensers[slot_index].getDispenseUpdateString(), MSG_INFO);
      const char *dispenseStatusStr = g_machine.m_productDispensers[slot_index].getDispenseStatusAsString();
      debugOutput::sendMessage("Flow Pulses since enabled: " + to_string(g_machine.getPcb()->getFlowSensorPulsesSinceEnabling(slot)), MSG_INFO);
      g_machine.getPcb()->displayMCP23017IORegisters(slot);
#endif
   }

   if (m_pMessaging->getAction() == ACTION_RESET)
   {
      m_state_requested = STATE_IDLE;
      g_machine.m_productDispensers[slot_index].finishActivePNumberDispense();

      m_pMessaging->sendMessageOverIP("Init Ready", true); // send to UI
      return e_ret = OK;
   }

   // Check if UI has sent a ACTION_DISPENSE_END to finish the transaction, or, if dispensing is complete
   if (m_pMessaging->getAction() == ACTION_DISPENSE_END)
   {
      debugOutput::sendMessage("Stop dispensing (stop command received)", MSG_INFO);
      m_state_requested = STATE_DISPENSE_END;
      g_machine.m_productDispensers[slot_index].finishActivePNumberDispense();
      std::string activePNumber = to_string(g_machine.m_productDispensers[slot_index].getActivePNumber());
      double activeProductVolumeDispensed = g_machine.m_productDispensers[slot_index].getActiveProductVolumeDispensed();
      double volume_remaining =g_machine.m_productDispensers[slot_index].getActiveProduct()->getVolumeRemaining() - activeProductVolumeDispensed;
      g_machine.m_productDispensers[slot_index].setMixProductsDispenseInfo(activePNumber, activeProductVolumeDispensed, volume_remaining);

      return e_ret = OK;
   }

   if (m_pMessaging->getAction() == ACTION_REPAIR_PCA)
   {
      if (g_machine.getPcb()->get_pcb_version() == pcb::PcbVersion::EN134_4SLOTS || g_machine.getPcb()->get_pcb_version() == pcb::PcbVersion::EN134_8SLOTS)
      {
         g_machine.getPcb()->sendEN134DefaultConfigurationToPCA9534(slot, true);
         m_pMessaging->resetAction();
         g_machine.control_pcb->setSingleDispenseButtonLight(slot, true);

         g_machine.getPcb()->flowSensorEnable(slot);
      }
   }

   if (g_machine.m_productDispensers[slot_index].isActiveProductVolumeTargetReached())
   {

      debugOutput::sendMessage("Active product. Requested volume reached. Stop and next.   P-" +
                                   to_string(g_machine.m_productDispensers[slot_index].getActivePNumber()) + ":  " +
                                   to_string(g_machine.m_productDispensers[slot_index].getActiveProductVolumeDispensed()) + "/" +
                                   to_string(g_machine.m_productDispensers[slot_index].getActiveProduct()->getTargetVolume()) + "ml",
                               MSG_INFO);

      std::string activePNumber = to_string(g_machine.m_productDispensers[slot_index].getActivePNumber());
      double activeProductVolumeDispensed = g_machine.m_productDispensers[slot_index].getActiveProductVolumeDispensed();
      double volume_remaining =g_machine.m_productDispensers[slot_index].getActiveProduct()->getVolumeRemaining() - activeProductVolumeDispensed;
      g_machine.m_productDispensers[slot_index].setMixProductsDispenseInfo(activePNumber, activeProductVolumeDispensed, volume_remaining);


      bool isAllPartsOfDispenseProductDispensed = g_machine.m_productDispensers[slot_index].setNextActiveProductAsPartOfSelectedProduct();
      if (isAllPartsOfDispenseProductDispensed)
      {
         // check for next mixing product to activate to dispense.   additive n -> .... ->  additive 1 -> base. (end with base)
         m_state_requested = STATE_DISPENSE_END;
         debugOutput::sendMessage("Stop dispensing selected product. Requested volume reached. " + to_string(g_machine.m_productDispensers[slot_index].getSelectedProductVolumeDispensed()), MSG_INFO);
      }
      else
      {
         if (g_machine.m_productDispensers[slot_index].getDispenseButtonValue())
         {
            debugOutput::sendMessage("Dispense button is pressed, so restart next phase automatically. ", MSG_INFO);
            g_machine.m_productDispensers[slot_index].startActiveDispensing();
         }
      }
   }

   e_ret = OK;

   return e_ret;
}

// void stateDispense::startActiveDispensing()
// {

//    switch (g_machine.getHardwareVersion())
//    {
//    case (machine::HardwareVersion::AP1):
//    case (machine::HardwareVersion::SS1):
//    case (machine::HardwareVersion::SS2):
//    {
//       debugOutput::sendMessage("start pumping SS2.", MSG_INFO);
//       g_machine.getPcb()->setPumpSpeedPercentage(0); // pump speed is inverted!
//       g_machine.getPcb()->setPumpDirection(slot, true);

//       g_machine.getPcb()->startPump(slot);
//       g_machine.getPcb()->setSpoutSolenoid(slot, true);
//    }
//    break;
//    case (machine::HardwareVersion::SS09):
//    {
//       debugOutput::sendMessage("start pumping SS09.", MSG_INFO);
//       g_machine.m_productDispensers[slot_index].pumpSlowStart(true);
//    }
//    break;
//    case (machine::HardwareVersion::AP2):
//    {
//       debugOutput::sendMessage("start pumping AP2.", MSG_INFO);
//       g_machine.getPcb()->startPump(slot);
//       g_machine.getPcb()->setSpoutSolenoid(slot, true);
//    }
//    break;
//    default:
//    {
//       debugOutput::sendMessage("UNKNOWN HARDWARE version", MSG_ERROR);
//    }
//    break;
//    }
// }

// void stateDispense::stopActiveDispensing()
// {

//    switch (g_machine.getHardwareVersion())
//    {
//    case (machine::HardwareVersion::AP1):
//    case (machine::HardwareVersion::SS1):
//    case (machine::HardwareVersion::SS2):
//    case (machine::HardwareVersion::AP2):
//    {
//       // g_machine.getPcb()->stopPump(slot);
//       // g_machine.getPcb()->setSpoutSolenoid(slot, false);
//       g_machine.m_productDispensers[slot_index].finishActivePNumberDispense();
//    }
//    break;
//    case (machine::HardwareVersion::SS09):
//    {
//       g_machine.m_productDispensers[slot_index].pumpSlowStopBlocking();
//       rectractProductBlocking();
//    }
//    break;
//    default:
//    {
//       debugOutput::sendMessage("UNKNOWN HARDWARE version", MSG_ERROR);
//    }
//    break;
//    }
// }

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
   // g_machine.getPcb()->virtualButtonUnpressHack(this->slot);
   g_machine.getPcb()->setSingleDispenseButtonLight(this->slot, false);

   DF_ERROR e_ret = OK;
   return e_ret;
}
