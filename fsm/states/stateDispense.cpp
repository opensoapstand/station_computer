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
// by:Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
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
   // debugOutput::sendMessage("stateDispense(messageMediator * message)", MSG_INFO);
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
   pos = m_pMessaging->getRequestedSlot();
   size = m_pMessaging->getRequestedSize();
   pos = pos - 1;

   productDispensers[pos].getProduct()->productVolumeInfo();
   return e_ret;
}

/*
 * Checks state of FSM; Accepts incomming string to process for
 * Air, Water and Product.  Sends signal to Solenoids to Dispense,
 * Based on string command
 */
DF_ERROR stateDispense::onAction()
{
   productDispensers = g_productDispensers;
   DF_ERROR e_ret = ERROR_BAD_PARAMS;

   if (m_pMessaging->isCommandStringReadyToBeParsed())
   {
      m_pMessaging->parseCommandString();
   }

   // Send amount dispensed to UI (to show in Maintenance Mode, and/or animate filling)

   if (productDispensers[pos].getVolumeDispensed() >= MINIMUM_DISPENSE_VOLUME_ML)
   {
      m_pMessaging->sendMessage(to_string(productDispensers[pos].getVolumeDispensed()));
   }

   // Check if UI has sent a ACTION_DISPENSE_END to finish the transaction, or, if dispensing is complete
   if (m_pMessaging->getAction() == ACTION_DISPENSE_END)
   {
      debugOutput::sendMessage("Stop dispensing (stop command received)", MSG_INFO);
      m_state_requested = STATE_DISPENSE_END;
      return e_ret = OK;
   }

   if (productDispensers[pos].getIsDispenseTargetReached())
   {
      debugOutput::sendMessage("Stop dispensing. Requested volume reached.", MSG_INFO);

      // m_pMessaging->sendMessage("Target Hit");

      m_state_requested = STATE_DISPENSE_END;
      return e_ret = OK;
   }

   productDispensers[pos].setVolumeDispensedPreviously(productDispensers[pos].getVolumeDispensed());

   // #ifdef ENABLE_EMPTY_CONTAINER_DETECTION

   if (productDispensers[pos].getEmptyContainerDetectionEnabled())
   {

      Dispense_behaviour status = productDispensers[pos].getDispenseStatus();

      if (status == FLOW_STATE_CONTAINER_EMPTY)
      {

         m_pMessaging->sendMessage("No flow abort"); // send to UI
         debugOutput::sendMessage("******************* EMPTY CONTAINER DETECTED **********************", MSG_INFO);
         m_state_requested = STATE_DISPENSE_END;

         // experimental, convert to custom volume dispensing.

         m_pMessaging->setRequestedSize(SIZE_EMPTY_CONTAINER_DETECTED_CHAR);
      }
      else if (status == FLOW_STATE_DISPENSING)
      {
         debugOutput::sendMessage("debug. targets s,m,l,c_max:" +
                                      to_string(productDispensers[pos].getProduct()->m_nVolumeTarget_s) +
                                      "," + to_string(productDispensers[pos].getProduct()->m_nVolumeTarget_m) +
                                      "," + to_string(productDispensers[pos].getProduct()->m_nVolumeTarget_l) +
                                      "," + to_string(productDispensers[pos].getProduct()->m_nVolumeTarget_c_max) +
                                      ", Vol dispensed: " + to_string(productDispensers[pos].getVolumeDispensed()),
                                  MSG_INFO);
      }
      else if (status == FLOW_STATE_ATTEMTPING_TO_PRIME)
      {
         debugOutput::sendMessage("No flow during pumping. Priming? Vol dispensed: " + to_string(productDispensers[pos].getVolumeDispensed()),
                                  MSG_INFO);
      }
      else if (status == FLOW_STATE_PUMPING_NOT_DISPENSING)
      {
         debugOutput::sendMessage("No flow detected during pumping. Vol dispensed: " + to_string(productDispensers[pos].getVolumeDispensed()),
                                  MSG_INFO);
      }
      else if (status == FLOW_STATE_NOT_PUMPING_NOT_DISPENSING)
      {
         debugOutput::sendMessage("Wait for button press.           Vol dispensed: " + to_string(productDispensers[pos].getVolumeDispensed()),
                                  MSG_INFO);
      }
      else if (status == FLOW_STATE_UNAVAILABLE)
      {
         debugOutput::sendMessage("No flow data yet (init).         Vol dispensed: " + to_string(productDispensers[pos].getVolumeDispensed()),
                                  MSG_INFO);
      }
      else
      {
         debugOutput::sendMessage("Dispense status unknow: " + to_string(status), MSG_INFO);
      };
   }
   else
   {
      // TODO: Do a check if Pumps are operational
      // send IPC if pump fails
      debugOutput::sendMessage("debug. targets s,m,l,c_max:" +
                                   to_string(productDispensers[pos].getProduct()->m_nVolumeTarget_s) +
                                   "," + to_string(productDispensers[pos].getProduct()->m_nVolumeTarget_m) +
                                   "," + to_string(productDispensers[pos].getProduct()->m_nVolumeTarget_l) +
                                   "," + to_string(productDispensers[pos].getProduct()->m_nVolumeTarget_c_max) +
                                   ", Vol dispensed: " + to_string(productDispensers[pos].getVolumeDispensed()),
                               MSG_INFO);
   }

   usleep(500000);

   e_ret = OK;

   return e_ret;
}

// Actions on leaving Dispense state
DF_ERROR stateDispense::onExit()
{
   DF_ERROR e_ret = OK;
   return e_ret;
}
