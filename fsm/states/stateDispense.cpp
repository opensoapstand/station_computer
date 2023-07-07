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
// by:Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
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
   productDispensers = g_productDispensers;
   DF_ERROR e_ret = OK;
   slot = m_pMessaging->getRequestedSlot();
   size = m_pMessaging->getRequestedSize();
   slot_index = slot - 1;

   productDispensers[slot_index].resetDispenseButton();

   productDispensers[slot_index].getProduct()->productVolumeInfo();

   if (m_pMessaging->getAction() == ACTION_AUTOFILL)
   {
      if (productDispensers[slot_index].the_pcb->get_pcb_version() == pcb::PcbVersion::EN134_4SLOTS)
      {
      }
      else
      {
         productDispensers[slot_index].the_pcb->virtualButtonPressHack(this->slot);
      }
      startPumping();
   }

   return e_ret;
}

DF_ERROR stateDispense::rectractProductBlocking()
{
   DF_ERROR e_ret = OK;
   productDispensers[slot_index].reversePumpForSetTimeMillis(productDispensers[slot_index].getProduct()->getRetractionTimeMillis());
   return e_ret;
}

DF_ERROR stateDispense::onAction()
{
   productDispensers = g_productDispensers;
   DF_ERROR e_ret = ERROR_BAD_PARAMS;

   if (m_pMessaging->isCommandStringReadyToBeParsed())
   {
      m_pMessaging->parseCommandString();
   }

   if (m_pMessaging->getAction() != ACTION_AUTOFILL)
   {
      if (productDispensers[slot_index].getDispenseButtonEdgePositive())
      {
         debugOutput::sendMessage("Dispense button pressed edge", MSG_INFO);
         m_pMessaging->sendMessageOverIP("Dispense Button Pos Edge"); // send to UI

         startPumping();
         productDispensers[slot_index].addDispenseButtonPress();
      }

      if (productDispensers[slot_index].getDispenseButtonEdgeNegative())
      {
         debugOutput::sendMessage("Dispense button released edge", MSG_INFO);
         m_pMessaging->sendMessageOverIP("Dispense Button Neg Edge"); // send to UI

         stopPumping();
      }
   }

   // Send amount dispensed to UI (to show in Maintenance Mode, and/or animate filling)

   productDispensers[slot_index].updateRunningAverageWindow();
   productDispensers[slot_index].updateDispenseStatus();
   productDispensers[slot_index].updateSlotState();

   if (productDispensers[slot_index].getIsStatusUpdateAllowed())
   {
      double volume = productDispensers[slot_index].getVolumeDispensed();

      Time_val avg_02s = productDispensers[slot_index].getAveragedFlowRate(1000);
      double flowrate = avg_02s.value;
      // const char *statusStringChar = productDispensers[slot_index].getDispenseStatusAsString();
      // std::string statusString(statusStringChar);
      // std::string message = "dispenseupdate|" + std::to_string(volume) + "|" + std::to_string(flowrate) + "|" + statusString;
      const char *statusStringChar = productDispensers[slot_index].getSlotStateAsString();
      std::string statusString(statusStringChar);
      std::string message = "dispenseupdate|" + std::to_string(volume) + "|" + std::to_string(flowrate) + "|" + statusString;
      m_pMessaging->sendMessageOverIP(message);

      // update of the actual dispense
      const char *dispenseStatusStr = productDispensers[slot_index].getDispenseStatusAsString();
      debugOutput::sendMessage(dispenseStatusStr, MSG_INFO);
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
      if (productDispensers[slot_index].the_pcb->get_pcb_version() == pcb::PcbVersion::EN134_4SLOTS || productDispensers[slot_index].the_pcb->get_pcb_version() == pcb::PcbVersion::EN134_8SLOTS)
      {
         productDispensers[slot_index].the_pcb->sendEN134DefaultConfigurationToPCA9534(slot, true);
         m_pMessaging->resetAction();
         productDispensers[slot_index].setMultiDispenseButtonLight(slot, true);
         productDispensers[slot_index].the_pcb->flowSensorEnable(slot);
      }
   }

   if (productDispensers[slot_index].getIsDispenseTargetReached())
   {
      debugOutput::sendMessage("Stop dispensing. Requested volume reached. " + to_string(productDispensers[slot_index].getVolumeDispensed()), MSG_INFO);
      m_state_requested = STATE_DISPENSE_END;
      stopPumping();
      return e_ret = OK;
   }

   // if (productDispensers[slot_index].getEmptyContainerDetectionEnabled())
   // {

   //    Dispense_behaviour status = productDispensers[slot_index].getDispenseStatus();

   //    if (status == FLOW_STATE_EMPTY)
   //    {

   //       debugOutput::sendMessage("******************* EMPTY CONTAINER DETECTED **********************", MSG_INFO);
   //       usleep(100000);

   //       send message delay (pause from previous message) desperate attempt to prevent crashes
   //       m_pMessaging->sendMessageOverIP("No flow abort"); // send to UI
   //       stopPumping();
   //       m_state_requested = STATE_DISPENSE_END;

   //       m_pMessaging->setRequestedSize(SIZE_EMPTY_CONTAINER_DETECTED_CHAR);
   //    }
   //    else if (status == FLOW_STATE_DISPENSING)
   //    {
   //       productDispensers[slot_index].logUpdateIfAllowed("debug. targets s,m,l,c_max:" +
   //                                                       to_string(productDispensers[slot_index].getProduct()->m_nVolumeTarget_s) +
   //                                                       "," + to_string(productDispensers[slot_index].getProduct()->m_nVolumeTarget_m) +
   //                                                       "," + to_string(productDispensers[slot_index].getProduct()->m_nVolumeTarget_l) +
   //                                                       "," + to_string(productDispensers[slot_index].getProduct()->m_nVolumeTarget_c_max) +
   //                                                       ", Vol dispensed: " + to_string(productDispensers[slot_index].getVolumeDispensed()));
   //    }
   //    else if (status == FLOW_STATE_PRIMING_OR_EMPTY)
   //    {
   //       productDispensers[slot_index].logUpdateIfAllowed("No flow during pumping. Priming? Vol dispensed: " + to_string(productDispensers[slot_index].getVolumeDispensed()));
   //    }
   //    else if (status == FLOW_STATE_PUMPING_NOT_DISPENSING)
   //    {
   //       productDispensers[slot_index].logUpdateIfAllowed("No flow detected during pumping. Vol dispensed: " + to_string(productDispensers[slot_index].getVolumeDispensed()));
   //    }
   //    else if (status == FLOW_STATE_NOT_PUMPING_NOT_DISPENSING)
   //    {
   //       productDispensers[slot_index].logUpdateIfAllowed("Wait for button press.           Vol dispensed: " + to_string(productDispensers[slot_index].getVolumeDispensed()));
   //    }
   //    else if (status == FLOW_STATE_UNAVAILABLE)
   //    {
   //       productDispensers[slot_index].logUpdateIfAllowed("No flow data                     Vol dispensed: " + to_string(productDispensers[slot_index].getVolumeDispensed()));
   //    }
   //    else if (status == FLOW_STATE_RAMP_UP)
   //    {
   //       productDispensers[slot_index].logUpdateIfAllowed("Flow ramping up                  Vol dispensed: " + to_string(productDispensers[slot_index].getVolumeDispensed()));
   //    }
   //    else
   //    {
   //       debugOutput::sendMessage("Dispense status unknow: " + to_string(status), MSG_INFO);
   //    };
   // }
   // else
   // {
   // // TODO: Do a check if Pumps are operational
   // // send IPC if pump fails
   // productDispensers[slot_index].logUpdateIfAllowed("Vol dispensed: " + to_string(productDispensers[slot_index].getVolumeDispensed()));

   // productDispensers[slot_index].updateRunningAverageWindow();
   // Time_val flowavg = productDispensers[slot_index].getAveragedFlowRate(2000);
   // productDispensers[slot_index].logUpdateIfAllowed("Flow rate 2s: " + to_string(flowavg.value));
   // }

   e_ret = OK;

   return e_ret;
}

void stateDispense::startPumping()
{
   if (productDispensers[slot_index].the_pcb->get_pcb_version() == pcb::PcbVersion::EN134_4SLOTS)
   {
      productDispensers[slot_index].the_pcb->setPumpSpeedPercentage(0);
      productDispensers[slot_index].the_pcb->setPumpDirection(slot, true);

      productDispensers[slot_index].the_pcb->startPump(slot);
      productDispensers[slot_index].the_pcb->setSolenoid(slot, true);
   }
   else
   {
      productDispensers[slot_index].pumpSlowStart(true);
   }
}

void stateDispense::stopPumping()
{
   if (productDispensers[slot_index].the_pcb->get_pcb_version() == pcb::PcbVersion::EN134_4SLOTS)
   {
      productDispensers[slot_index].the_pcb->stopPump(slot);
      productDispensers[slot_index].the_pcb->setSolenoid(slot, false);
   }
   else
   {
      productDispensers[slot_index].pumpSlowStopBlocking();
      rectractProductBlocking();
   }
}

// Actions on leaving Dispense state
DF_ERROR stateDispense::onExit()
{
   productDispensers[slot_index].setPumpsDisableAll();
   productDispensers[slot_index].the_pcb->virtualButtonUnpressHack(this->slot);
   productDispensers[slot_index].the_pcb->setSingleDispenseButtonLight(this->slot, false);

   DF_ERROR e_ret = OK;
   return e_ret;
}
