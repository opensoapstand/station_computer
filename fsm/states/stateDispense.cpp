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
   DF_ERROR e_ret = OK;

   if (g_machine.getSelectedDispenser().getAutoDispense())
   {
      g_machine.getSelectedDispenser().startActiveDispensing(); // make part of dispenser.
   }

   return e_ret;
}

DF_ERROR stateDispense::onAction()
{
   DF_ERROR e_ret = ERROR_BAD_PARAMS;

   // button press behaviour
   if (!g_machine.getSelectedDispenser().getAutoDispense())
   {
      if (g_machine.getSelectedDispenser().getDispenseButtonEdgePositive())
      {
         debugOutput::sendMessage("Dispense button pressed edge", MSG_INFO);
         m_pMessaging->sendMessageOverIP("Dispense Button Pos Edge", true); // send to UI

         g_machine.getSelectedDispenser().startActiveDispensing();
         g_machine.getSelectedDispenser().addDispenseButtonPress();
      }

      if (g_machine.getSelectedDispenser().getDispenseButtonEdgeNegative())
      {
         debugOutput::sendMessage("Dispense button released edge", MSG_INFO);
         m_pMessaging->sendMessageOverIP("Dispense Button Neg Edge", true); // send to UI

         g_machine.getSelectedDispenser().stopActiveDispensing();
      }
   }

   // Send amount dispensed to UI (to show in Maintenance Mode, and/or animate filling)
   g_machine.getSelectedDispenser().updateActiveProductFlowRateRunningAverageWindow();
   g_machine.getSelectedDispenser().updateDispenseStatus();
   // g_machine.getSelectedDispenser().updateActiveProductState();

   // do logging
   statusUpdateLoggingAndOverIP(true);
#ifdef ENABLE_PARALLEL_MIX

   // parallel mixing, we only look at the base and mix in additives as we go.
   if (g_machine.getSelectedDispenser().getIsStatusUpdateAllowed())
   {
      g_machine.getSelectedDispenser().setParallelSolenoids();
   }

   // Is target volume reached?
   if (g_machine.getSelectedDispenser().isSelectedProductVolumeTargetReached())
   {
      g_machine.getSelectedDispenser().finishActivePNumberDispense();
      g_machine.getSelectedDispenser().finishSelectedProductDispense();
      m_state_requested = STATE_DISPENSE_END;
      debugOutput::sendMessage("Stop dispensing selected product. Requested volume reached. Dispensed: " + to_string(g_machine.getSelectedDispenser().getSelectedProductVolumeDispensed()), MSG_INFO);
   }

#else
   // sequential mixing: first additives than base

   // Is target volume reached?
   if (g_machine.getSelectedDispenser().isActiveProductVolumeTargetReached())
   {
      std::string activePNumber = to_string(g_machine.getSelectedDispenser().getActivePNumber());
      double activeProductVolumeDispensed = g_machine.getSelectedDispenser().getActiveProductVolumeDispensed();
      double volume_remaining = g_machine.getSelectedDispenser().getActiveProduct()->getVolumeRemaining() - activeProductVolumeDispensed;
      g_machine.getSelectedDispenser().setMixDispenseReport(activePNumber, activeProductVolumeDispensed, volume_remaining);

      debugOutput::sendMessage("Active product. Requested volume reached. Stop and next.   P-" + activePNumber + ":  " +
                                   to_string(activeProductVolumeDispensed) + "/" +
                                   to_string(g_machine.getSelectedDispenser().getActiveProduct()->getTargetVolume()) + "ml",
                               MSG_INFO);

      bool isAllPartsOfDispenseProductDispensed = g_machine.getSelectedDispenser().handleMixComponentTargetVolumeReached();
      if (isAllPartsOfDispenseProductDispensed)
      {
         // check for next mixing product to activate to dispense.   additive n -> .... ->  additive 1 -> base. (end with base)
         m_state_requested = STATE_DISPENSE_END;
         debugOutput::sendMessage("Stop dispensing selected product. Requested volume reached. Dispensed: " + to_string(g_machine.getSelectedDispenser().getSelectedProductVolumeDispensed()), MSG_INFO);
      }
      else
      {
         if (g_machine.getSelectedDispenser().getDispenseButtonValue())
         {
            debugOutput::sendMessage("Dispense button is pressed, so restart next phase automatically. ", MSG_INFO);
            g_machine.getSelectedDispenser().startActiveDispensing();
         }
      }
   }

#endif

   // Check received commands from UI
   if (m_pMessaging->isCommandStringReadyToBeParsed())
   {
      m_pMessaging->parseCommandString();
   }

   if (m_pMessaging->getAction() == ACTION_RESET)
   {
      debugOutput::sendMessage("Stop dispensing (stop command received)", MSG_INFO);
      m_state_requested = STATE_IDLE;
      g_machine.getSelectedDispenser().finishActivePNumberDispense();
      g_machine.getSelectedDispenser().finishSelectedProductDispense();

      m_pMessaging->sendMessageOverIP("Init Ready", true); // send to UI
      return e_ret = OK;
   }

   if (m_pMessaging->getAction() == ACTION_DISPENSE_END)
   {
      debugOutput::sendMessage("Stop dispensing (stop command received)", MSG_INFO);
      m_state_requested = STATE_DISPENSE_END;

      g_machine.getSelectedDispenser().finishActivePNumberDispense();
      g_machine.getSelectedDispenser().finishSelectedProductDispense();

      std::string activePNumber = to_string(g_machine.getSelectedDispenser().getActivePNumber());
      double activeProductVolumeDispensed = g_machine.getSelectedDispenser().getActiveProductVolumeDispensed();
      double volume_remaining = g_machine.getSelectedDispenser().getActiveProduct()->getVolumeRemaining() - activeProductVolumeDispensed;
      g_machine.getSelectedDispenser().setMixDispenseReport(activePNumber, activeProductVolumeDispensed, volume_remaining);

      return e_ret = OK;
   }

   if (m_pMessaging->getAction() == ACTION_REPAIR_PCA)
   {
      if (g_machine.getPcb()->get_pcb_version() == pcb::PcbVersion::EN134_4SLOTS || g_machine.getPcb()->get_pcb_version() == pcb::PcbVersion::EN134_8SLOTS)
      {
         g_machine.getPcb()->sendEN134DefaultConfigurationToPCA9534(g_machine.getSelectedDispenserNumber(), true);
         m_pMessaging->resetAction();
         g_machine.control_pcb->setSingleDispenseButtonLight(g_machine.getSelectedDispenserNumber(), true);

         g_machine.getPcb()->flowSensorEnable(g_machine.getSelectedDispenserNumber());
      }
   }
   e_ret = OK;
   return e_ret;
}

DF_ERROR stateDispense::statusUpdateLoggingAndOverIP(bool onlyIfAllowed)
{
   DF_ERROR e_ret = OK;
   if (g_machine.getSelectedDispenser().getIsStatusUpdateAllowed() || !onlyIfAllowed)
   {
      double volume = g_machine.getSelectedDispenser().getActiveProductVolumeDispensed();

      Time_val avg_01s = g_machine.getSelectedDispenser().getAveragedProductFlowRate(1000);
      double flowrate = avg_01s.value;
      const char *statusStringChar = g_machine.getSelectedDispenser().getDispenseStatusAsString();
      std::string statusString(statusStringChar);

      std::string message = "dispenseupdate|" + std::to_string(volume) + "|" + std::to_string(flowrate) + "|" + statusString;
      m_pMessaging->sendMessageOverIP(message, true); // send to UI

#define PRINT_STATUS
#ifdef PRINT_STATUS

      // update of the actual dispense
      debugOutput::sendMessage("-----------Dispense update. State: " + std::string(g_machine.getSelectedDispenser().getDispenseStatusAsString()), MSG_INFO);
      debugOutput::sendMessage(g_machine.getSelectedDispenser().getDispenseUpdateString(), MSG_INFO);
      const char *dispenseStatusStr = g_machine.getSelectedDispenser().getDispenseStatusAsString();
      debugOutput::sendMessage("Flow Pulses since enabled: " + to_string(g_machine.getPcb()->getFlowSensorPulsesSinceEnabling(g_machine.getSelectedDispenserNumber())), MSG_INFO);
      g_machine.getPcb()->displayMCP23017IORegisters(g_machine.getSelectedDispenserNumber());
#endif
   }
   return e_ret;
}

// DF_ERROR stateDispense::rectractProductBlocking()
// {
//    DF_ERROR e_ret = OK;
//    g_machine.getSelectedDispenser().reversePumpForSetTimeMillis(g_machine.getSelectedDispenser().getSelectedProduct()->getRetractionTimeMillis());
//    return e_ret;
// }

// Actions on leaving Dispense state
DF_ERROR stateDispense::onExit()
{
   g_machine.getSelectedDispenser().setPumpsDisableAll();
   g_machine.getPcb()->setSingleDispenseButtonLight(g_machine.getSelectedDispenserNumber(), false);
   statusUpdateLoggingAndOverIP(false); // final status update of dispensing

   DF_ERROR e_ret = OK;
   return e_ret;
}
