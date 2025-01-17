//***************************************
//
// stateManualPump.cpp
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

#include "stateManualPump.h"
#include <ctime> // only precise to the second
#include <chrono>

#define STRING_STATE_MANUAL_PUMP "Manual Pump"

// Default CTOR
stateManualPump::stateManualPump()
{
   // printerr = new Adafruit_Thermal();
}

// CTOR Linked to IP Thread Socket Listener
stateManualPump::stateManualPump(messageMediator *message)
{
   isCyclicTesting = false;
   m_pMessaging = message;
}

// DTOR
stateManualPump::~stateManualPump()
{
   // delete stuff
   debugOutput::sendMessage("stateManualPump: ~stateManualPump", MSG_INFO);

}

// Overload for Debugger output
string stateManualPump::toString()
{
   return STRING_STATE_MANUAL_PUMP;
}

DF_ERROR stateManualPump::onEntry()
{
   m_state_requested = STATE_MANUAL_PUMP;
   m_state_auto_pump = AUTO_PUMP_STATE_IDLE;
   DF_ERROR e_ret = OK;
   // productDispensers = g_productDispensers;

   // default pump setting
   m_active_pump_index = 0; // pump 1 is activated by default
   debugOutput::sendMessage("Pump testing. Active dispenser pump: " + to_string(m_active_pump_index + 1), MSG_INFO);
   g_machine.m_productDispensers[m_active_pump_index].setPumpDirectionForward();
   g_machine.m_productDispensers[m_active_pump_index].setPumpPWM(255, true);


   g_machine.control_pcb->setDispenseButtonLightsAllOff();

   isFlowTest = false;
   isCyclicTesting = false;
   iscustomVolumeDispenseTest = false;
   isCyclicTestingPumpOn = false;
   triggerOutputData = false;

   using namespace std::chrono;
   uint64_t millis_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
   most_recent_data_output_epoch = millis_epoch;

   g_machine.pcb24VPowerSwitch(true);

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

      if (m_pMessaging->getAction() == ACTION_RESET)
      {
         m_pMessaging->sendMessageOverIP("Init Ready", true); // send to UI
         m_state_requested = STATE_IDLE;
      }
      else if ('0' == m_pMessaging->getAction() || ACTION_QUIT == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Exit pump test", MSG_INFO);
         g_machine.m_productDispensers[m_active_pump_index].setPumpsDisableAll();
            g_machine.control_pcb->setDispenseButtonLightsAllOff();
         // g_machine.m_productDispensers[m_active_pump_index].m_pcb->virtualButtonUnpressHack(m_active_pump_index + 1);

         m_state_requested = STATE_IDLE;
      }

      else if ('1' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Enable dispenser pump " + to_string(m_active_pump_index + 1) + "(press dispense button to make pump actually work)", MSG_INFO);
         g_machine.m_productDispensers[m_active_pump_index].initSelectedProductDispense(SIZE_CUSTOM_CHAR);
         g_machine.m_productDispensers[m_active_pump_index].setPumpEnable(); // POS is 1->4! index is 0->3
         // g_machine.m_productDispensers[m_active_pump_index].setMultiDispenseButtonLight(m_active_pump_index + 1, true);
         g_machine.control_pcb->setSingleDispenseButtonLight(m_active_pump_index + 1, true);
      }

      else if ('2' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Disable dispenser pump " + to_string(m_active_pump_index + 1), MSG_INFO);

         g_machine.m_productDispensers[m_active_pump_index].finishActivePNumberDispense();
         g_machine.m_productDispensers[m_active_pump_index].finishSelectedProductDispense();
      }

      else if ('3' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Direction forward dispenser pump " + to_string(m_active_pump_index + 1), MSG_INFO);
         g_machine.m_productDispensers[m_active_pump_index].setPumpDirectionForward();
      }
      else if ('4' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Direction backward pump " + to_string(m_active_pump_index + 1), MSG_INFO);
         g_machine.m_productDispensers[m_active_pump_index].setPumpDirectionReverse();
      }

      else if ('5' == m_pMessaging->getAction())
      {
         g_machine.m_productDispensers[m_active_pump_index].resetSelectedProductVolumeDispensed();
         dispenseButtonValueMemory = false;
         iscustomVolumeDispenseTest = !iscustomVolumeDispenseTest;
         debugOutput::sendMessage("Custom volume dispense pump model test active? : " + to_string(iscustomVolumeDispenseTest), MSG_INFO);
      }

      else if ('6' == m_pMessaging->getAction())
      {
         g_machine.m_productDispensers[m_active_pump_index].resetSelectedProductVolumeDispensed();
         isCyclicTesting = !isCyclicTesting;
         debugOutput::sendMessage("Toggle cyclic pump test. Enabled?: " + to_string(isCyclicTesting), MSG_INFO);

         if (isCyclicTesting)
         {
            g_machine.m_productDispensers[m_active_pump_index].m_pcb->flowSensorEnable(m_active_pump_index + 1);
            //   g_machine.m_productDispensers[m_active_pump_index].m_pcb->resetFlowSensorPulsesForDispenser(m_active_pump_index + 1);

            g_machine.m_productDispensers[m_active_pump_index].initProductFlowRateCalculation();
            isCyclicTestingPumpOn = false;
            using namespace std::chrono;
            uint64_t now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            cyclicTestPeriodStartEpochMillis = now - CYCLIC_PUMP_TEST_OFF_CYCLE_MILLIS - 10; // make sure it's expired.

            pumpCyclicTestCycleFinish();
         }
         else
         {
            g_machine.m_productDispensers[m_active_pump_index].m_pcb->flowSensorsDisableAll();

            g_machine.m_productDispensers[m_active_pump_index].setPumpsDisableAll();
         }
      }
      else if ('7' == m_pMessaging->getAction())
      {
         g_machine.m_productDispensers[m_active_pump_index].reversePumpForSetTimeMillis(500);
      }
      else if ('8' == m_pMessaging->getAction())
      {

         // Pump specific test
         // g_machine.m_productDispensers[m_active_pump_index].resetSelectedProductVolumeDispensed();
         // debugOutput::sendMessage("Do pump test", MSG_INFO);
         // pumpTest();

         // auto pump test
         g_machine.m_productDispensers[m_active_pump_index].resetSelectedProductVolumeDispensed();

         if (m_state_auto_pump == AUTO_PUMP_STATE_IDLE)
         {
            m_state_auto_pump = AUTO_PUMP_STATE_INIT;
         }
         else
         {
            m_state_auto_pump = AUTO_PUMP_STATE_FINISHED;
         }
      }

      else if ('9' == m_pMessaging->getAction())
      {
         isFlowTest = !isFlowTest;
         if (isFlowTest)
         {
            g_machine.m_productDispensers[m_active_pump_index].initProductFlowRateCalculation();
            using namespace std::chrono;
            startFlowTestMillis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            g_machine.m_productDispensers[m_active_pump_index].setPumpEnable();
            debugOutput::sendMessage("Flow measuring test enabled: True. Can be combined with cyclic test, if not, keep dispense button pressed. Will display test data in csv format.", MSG_INFO);
         }
         else
         {
            debugOutput::sendMessage("Flow measuring test enabled: False.", MSG_INFO);
            g_machine.m_productDispensers[m_active_pump_index].setPumpsDisableAll();
         }
         g_machine.m_productDispensers[m_active_pump_index].resetSelectedProductVolumeDispensed();
      }

      else if (ACTION_MANUAL_PUMP_SET == m_pMessaging->getAction())
      {
            g_machine.control_pcb->setDispenseButtonLightsAllOff();
         g_machine.m_productDispensers[m_active_pump_index].setPumpsDisableAll();
         // float PWM_value_byte = 3.12345;
         int val = m_pMessaging->getCommandValue();
         debugOutput::sendMessage("Value is pump number 1,2,3 or 4 :" + to_string((uint8_t)val), MSG_INFO);
         m_active_pump_index = ((uint8_t)val) - 1;

         g_machine.m_productDispensers[m_active_pump_index].resetSelectedProductVolumeDispensed();
      }
      else if (ACTION_MANUAL_PUMP_PWM_SET == m_pMessaging->getAction())
      {
         int val = m_pMessaging->getCommandValue();

         if (iscustomVolumeDispenseTest)
         {
            string value_str2 = std::to_string(val);
            string msg2 = "Rectract time in millis set to: " + value_str2;
            debugOutput::sendMessage(msg2, MSG_INFO);
            retract_time_millis = (uint64_t)val;
         }
         else
         {
            // float PWM_value_byte = 3.12345;
            debugOutput::sendMessage("PWM value to set (should be in range [0..255]): " + to_string((uint8_t)val), MSG_INFO);
            g_machine.m_productDispensers[m_active_pump_index].setPumpPWM((uint8_t)val, true);
         }
      }
      else
      {
         debugOutput::sendMessage(
             "---Pump test menu---\n"
             " active pump: " +
                 to_string(m_active_pump_index + 1) + "\n"
                                                      "Available commands:\n"
                                                      "0: Exit pump menu\n"
                                                      "nX: Set active pump, where X= 1,2,3 or 4\n"
                                                      "ixxx: Set active pump pwm [0..255] always 3 digits e.g. 050 = 50 \n"
                                                      "1: Active pump enable (additionally, press dispense button for motor to actually run)\n"
                                                      "2: Active pump disable\n"
                                                      "3: Active pump direction forward\n"
                                                      "4: Active pump direction reverse\n"
                                                      "5: Toggle custom volume dispense model test\n"
                                                      "   (please note: the machine will dispense some seconds and will then\n"
                                                      "   automatically revert for some time. Set the reverting time during the testing with\n"
                                                      "   ixxx;  where xxx is in milliseconds. e.g. i200;  will revert the pump for 200ms\n"
                                                      "   the pump speed cannot be set during this test)\n"
                                                      "6: Toggle endurance test. Active pump toggle cyclic ON/OFF (WARNING: Will pump autonomously WITHOUT dispense button pressed)\n"
                                                      "7: Reverse hack\n"
                                                      "8: Active enabled pump: Autofill 500ml . (WARNING: will pump without dispense button press. Choose 8 again to stop when pumping.)\n"
                                                      // "8: Run pump routine (hold dispense button for it to work)\n"
                                                      "9: Toggle Display flow measuring test values\n",
             MSG_INFO);
      }
   }

   using namespace std::chrono;
   uint64_t millis_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
   if (most_recent_data_output_epoch + MOTOR_TEST_DATA_OUTPUT_INTERVAL_MILLIS < millis_epoch)
   {
      triggerOutputData = true;
      most_recent_data_output_epoch = millis_epoch;
   }
   else
   {
      triggerOutputData = false;
   }

   if (m_state_auto_pump != AUTO_PUMP_STATE_IDLE)
   {
      autofillPresetQuantity();
   }
   else if (isCyclicTesting)
   {
      pumpCyclicTest();
      if (isFlowTest && isCyclicTestingPumpOn)
      {
         pumpFlowTest();
      }
   }
   else if (isFlowTest)
   {
      pumpFlowTest();
   }
   else if (iscustomVolumeDispenseTest)
   {
      customVolumeDispenseTest();
   }
   else
   {
      // manual
      if (g_machine.m_productDispensers[m_active_pump_index].getDispenseButtonValue())
      {
         double volume = g_machine.m_productDispensers[m_active_pump_index].getSelectedProductVolumeDispensed();

         // instant flow rate
        //  double flowRate = g_machine.m_productDispensers[m_active_pump_index].getProductFlowRateInstantaneous();

         // flow rate windowed avg
         g_machine.m_productDispensers[m_active_pump_index].updateActiveProductFlowRateRunningAverageWindow();
         Time_val avg_1s = g_machine.m_productDispensers[m_active_pump_index].getAveragedProductFlowRate(1000);

         if (triggerOutputData)
         {
            debugOutput::sendMessage("----------------Dispense button pressed.----------", MSG_INFO);
            debugOutput::sendMessage("Dispensed volume [total]: " + to_string(volume), MSG_INFO);
            // debugOutput::sendMessage("Dispense flowRate [V/s]: " + to_string(flowRate), MSG_INFO);
            debugOutput::sendMessage("Dispense flowRate 1s avg [V/s]: " + to_string(avg_1s.value), MSG_INFO);

            // usleep(500000);

            if (g_machine.m_productDispensers[m_active_pump_index].isSlotEnabled())
            {
               unsigned short speed = g_machine.m_productDispensers[m_active_pump_index].getPumpSpeed();
               string value = std::to_string(speed);
               string msg = "Pump speed: " + value;
               debugOutput::sendMessage(msg, MSG_INFO);

               using namespace std::chrono;
               uint64_t millis_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

               debugOutput::sendMessage("Millis since epoch: " + to_string(millis_since_epoch), MSG_INFO);

               // usleep(500000);
            }
         }
         // debugOutput::sendMessage("d", MSG_INFO);
      }
      if (g_machine.m_productDispensers[m_active_pump_index].getDispenseButtonEdgePositive())
      {
         if (g_machine.m_productDispensers[m_active_pump_index].m_pcb->get_pcb_version() == pcb::PcbVersion::EN134_4SLOTS)
         {
            g_machine.m_productDispensers[m_active_pump_index].m_pcb->startPump(m_active_pump_index + 1);
            g_machine.m_productDispensers[m_active_pump_index].setSpoutSolenoid(true);
         }
      }
      if (g_machine.m_productDispensers[m_active_pump_index].getDispenseButtonEdgeNegative())
      {
         if (g_machine.m_productDispensers[m_active_pump_index].m_pcb->get_pcb_version() == pcb::PcbVersion::EN134_4SLOTS)
         {
            g_machine.m_productDispensers[m_active_pump_index].m_pcb->stopPump(m_active_pump_index + 1);
            g_machine.m_productDispensers[m_active_pump_index].setSpoutSolenoid(false);
         }
      }
   }

   e_ret = OK;

   return e_ret;
}

void stateManualPump::customVolumeDispenseTest()
{

   // 1. pump with retraction at end
   // when button press has negative edge, retract motor. for time x
   dispenseButtonValue = g_machine.m_productDispensers[m_active_pump_index].getDispenseButtonValue();

   if (!dispenseButtonValue && dispenseButtonValueMemory)
   {
      debugOutput::sendMessage("Dispense Button negative Edge.", MSG_INFO);
   }

   if (dispenseButtonValue && !dispenseButtonValueMemory)
   {
      debugOutput::sendMessage("Start Dispensing", MSG_INFO);

      // g_machine.m_productDispensers[m_active_pump_index].setPumpPWM(125, true);

      isDispensing = true;
      using namespace std::chrono;
      startDispensingEpochMillis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

      g_machine.m_productDispensers[m_active_pump_index].setPumpDirectionForward();
      g_machine.m_productDispensers[m_active_pump_index].setPumpEnable();
   }

   using namespace std::chrono;
   uint64_t millis_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
   if (millis_epoch - startDispensingEpochMillis > 2000 && isDispensing)
   {
      debugOutput::sendMessage("End Dispensing. Activate retract millis: " + to_string(retract_time_millis), MSG_INFO);

      isDispensing = false;
      // g_machine.m_productDispensers[m_active_pump_index].setPumpsDisableAll();
      g_machine.m_productDispensers[m_active_pump_index].setPumpDirectionReverse();
      g_machine.m_productDispensers[m_active_pump_index].setPumpEnable();
      using namespace std::chrono;
      startRetractingEpochMillis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
      isRetracting = true;
   }

   millis_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
   if (millis_epoch - startRetractingEpochMillis >= retract_time_millis && isRetracting)
   {
      debugOutput::sendMessage("End retraction.", MSG_INFO);
      g_machine.m_productDispensers[m_active_pump_index].setPumpsDisableAll();
      isRetracting = false;
   }

   dispenseButtonValueMemory = dispenseButtonValue;

   // 2. detect container empty
   // if motor running and flow sensor dispensing.
}

void stateManualPump::pumpFlowTest()
{
   // instant flow rate
//    double flowRate = g_machine.m_productDispensers[m_active_pump_index].getProductFlowRateInstantaneous();

   // flow rate windowed avg
   g_machine.m_productDispensers[m_active_pump_index].updateActiveProductFlowRateRunningAverageWindow();
   Time_val avg_1s = g_machine.m_productDispensers[m_active_pump_index].getAveragedProductFlowRate(1000);
   Time_val avg_02s = g_machine.m_productDispensers[m_active_pump_index].getAveragedProductFlowRate(2000);
   Time_val avg_05s = g_machine.m_productDispensers[m_active_pump_index].getAveragedProductFlowRate(5000);

   double totalVolume = g_machine.m_productDispensers[m_active_pump_index].getSelectedProductVolumeDispensed();
   if (triggerOutputData)
   {

      debugOutput::sendMessage("millis/totalvol/avgSinceLastcall/avg1s/avg2s/avg5s, " +
                                   to_string(avg_1s.time_millis - startFlowTestMillis) + "," +
                                   to_string(totalVolume) + "," +
                                //    to_string(flowRate) + "," +
                                   to_string(avg_1s.value) + "," +
                                   to_string(avg_02s.value) + "," +
                                   to_string(avg_05s.value),
                               MSG_INFO);
   }
}

void stateManualPump::pumpCyclicTestCycleStart()
{
   pump_test_cycle_count++;

   int speed = g_machine.m_productDispensers[m_active_pump_index].getSelectedProduct()->getPWM();

   debugOutput::sendMessage("Pump speed for test: " + to_string(speed), MSG_INFO);

   // if (g_machine.m_productDispensers[m_active_pump_index].m_pcb->get_pcb_version() == pcb::PcbVersion::EN134_4SLOTS)
   // {
      g_machine.m_productDispensers[m_active_pump_index].m_pcb->startPump(m_active_pump_index + 1);
      g_machine.m_productDispensers[m_active_pump_index].setSpoutSolenoid(true);
   // }
   // else
   // {
   //    g_machine.m_productDispensers[m_active_pump_index].m_pcb->virtualButtonPressHack(m_active_pump_index + 1);
   //    g_machine.m_productDispensers[m_active_pump_index].pumpSlowStart(true);
   // }

   isCyclicTestingPumpOn = true;
}

void stateManualPump::pumpCyclicTestCycleFinish()
{

   // if (g_machine.m_productDispensers[m_active_pump_index].m_pcb->get_pcb_version() == pcb::PcbVersion::EN134_4SLOTS)
   // {
      g_machine.m_productDispensers[m_active_pump_index].m_pcb->stopPump(m_active_pump_index + 1);
      g_machine.m_productDispensers[m_active_pump_index].setSpoutSolenoid(false);
   // }
   // else
   // {
   //    // g_machine.m_productDispensers[m_active_pump_index].m_pcb->virtualButtonUnpressHack(m_active_pump_index + 1);
   //    g_machine.m_productDispensers[m_active_pump_index].pumpSlowStopBlocking();
   // }
   isCyclicTestingPumpOn = false;
}

void stateManualPump::pumpCyclicTest()
{

   using namespace std::chrono;
   uint64_t now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
   // }

   if (isCyclicTestingPumpOn)
   {
      // check for end of ON cycle
      if (cyclicTestPeriodStartEpochMillis + CYCLIC_PUMP_TEST_ON_CYCLE_MILLIS < now)
      {
         cyclicTestPeriodStartEpochMillis = now; //  + 2 * OFF_CYCLE_MILLIS
         // g_machine.m_productDispensers[m_active_pump_index].setPumpsDisableAll();
         pumpCyclicTestCycleFinish();
         cyclicTestPeriodStartEpochMillis = now;
      }
   }
   else
   {
      // check for end of OFF cycle
      if (cyclicTestPeriodStartEpochMillis + CYCLIC_PUMP_TEST_OFF_CYCLE_MILLIS < now)
      {

         debugOutput::sendMessage("\n******************************\n******PUMP CYCLING TESTING******\n*****************\n  cycle: " + to_string(pump_test_cycle_count), MSG_INFO);
         pumpCyclicTestCycleStart();
         cyclicTestPeriodStartEpochMillis = now;
      }
   }
}

// DF_ERROR stateManualPump::pumpCyclicTestOn()
// {

//    using namespace std::chrono;
//    uint64_t now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
//    // }

//    if (isCyclicTestingPumpOn)
//    {
//       // check for end of ON cycle
//       if (cyclicTestPeriodStartEpochMillis + CYCLIC_PUMP_TEST_ON_CYCLE_MILLIS < now)
//       {
//          cyclicTestPeriodStartEpochMillis = now; //  + 2 * OFF_CYCLE_MILLIS
//          // g_machine.m_productDispensers[m_active_pump_index].setPumpsDisableAll();
//          g_machine.m_productDispensers[m_active_pump_index].the_8344->virtualButtonUnpressHack();
//          g_machine.m_productDispensers[m_active_pump_index].pumpSlowStopBlocking();

//          isCyclicTestingPumpOn = false;
//          cyclicTestPeriodStartEpochMillis = now;
//       }
//    }
//    else
//    {
//       // check for end of OFF cycle
//       if (cyclicTestPeriodStartEpochMillis + CYCLIC_PUMP_TEST_OFF_CYCLE_MILLIS < now)
//       {

//          debugOutput::sendMessage("\n******************************\n******PUMP CYCLING TESTING******\n*****************\n  cycle: " + to_string(pump_test_cycle_count), MSG_INFO);
//          pump_test_cycle_count++;

//          g_machine.m_productDispensers[m_active_pump_index].the_8344->virtualButtonPressHack();

//          // g_machine.m_productDispensers[m_active_pump_index].setPumpDirectionForward();

//          int speed = g_machine.m_productDispensers[m_active_pump_index].getSelectedProduct()->getPWM();

//          debugOutput::sendMessage("Pump speed for test: " + to_string(speed), MSG_INFO);
//          // g_machine.m_productDispensers[m_active_pump_index].setPumpPWM(speed, true);
//          // g_machine.m_productDispensers[m_active_pump_index].setPumpEnable(); // POS is 1->4! index is 0->3

//          g_machine.m_productDispensers[m_active_pump_index].pumpSlowStart(true);

//          isCyclicTestingPumpOn = true;
//          cyclicTestPeriodStartEpochMillis = now;
//       }
//    }
// }

void stateManualPump::autofillPresetQuantity()
{
   using namespace std::chrono;
   uint64_t now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

   if (m_state_auto_pump == AUTO_PUMP_STATE_IDLE)
   {
   }
   else if (m_state_auto_pump == AUTO_PUMP_STATE_INIT)
   {
      // pump should be enabled first.
      

      int speed = g_machine.m_productDispensers[m_active_pump_index].getSelectedProduct()->getPWM();
      debugOutput::sendMessage("Pump auto start: " + to_string(speed), MSG_INFO);
      // if (g_machine.m_productDispensers[m_active_pump_index].m_pcb->get_pcb_version() == pcb::PcbVersion::EN134_4SLOTS)
      // {
         g_machine.m_productDispensers[m_active_pump_index].m_pcb->startPump(m_active_pump_index + 1);
         g_machine.m_productDispensers[m_active_pump_index].setSpoutSolenoid(true);
      // }
      // else
      // {
      //    g_machine.m_productDispensers[m_active_pump_index].m_pcb->virtualButtonPressHack(m_active_pump_index + 1);
      //    g_machine.m_productDispensers[m_active_pump_index].pumpSlowStart(true);
      // }

      cyclicTestPeriodStartEpochMillis = now;
      m_state_auto_pump = AUTO_PUMP_STATE_PUMPING;
   }
   else if (m_state_auto_pump == AUTO_PUMP_STATE_PUMPING)
   {
      double totalVolume = g_machine.m_productDispensers[m_active_pump_index].getSelectedProductVolumeDispensed();
      if (totalVolume > 500)
      {
         m_state_auto_pump = AUTO_PUMP_STATE_FINISHED;
         debugOutput::sendMessage("Pump auto requested volume reached.", MSG_INFO);
      }

      if (g_machine.m_productDispensers[m_active_pump_index].getDispenseButtonValue())
      {
         debugOutput::sendMessage("Interrupt auto fill process with button.", MSG_INFO);
         m_state_auto_pump = AUTO_PUMP_STATE_FINISHED;
      }
   }
   else if (m_state_auto_pump == AUTO_PUMP_STATE_FINISHED)
   {
      debugOutput::sendMessage("Pump auto finished.", MSG_INFO);

      // if (g_machine.m_productDispensers[m_active_pump_index].m_pcb->get_pcb_version() == pcb::PcbVersion::EN134_4SLOTS)
      // {
         g_machine.m_productDispensers[m_active_pump_index].m_pcb->stopPump(m_active_pump_index + 1);
         g_machine.m_productDispensers[m_active_pump_index].setSpoutSolenoid(false);
      // }
      // else
      // {

      //    g_machine.m_productDispensers[m_active_pump_index].pumpSlowStopBlocking();
      //    g_machine.m_productDispensers[m_active_pump_index].m_pcb->virtualButtonUnpressHack(m_active_pump_index + 1);
      // }

      m_state_auto_pump = AUTO_PUMP_STATE_IDLE;
   }
}

void stateManualPump::pumpTest()
{
   debugOutput::sendMessage("pump test", MSG_INFO);

   g_machine.m_productDispensers[m_active_pump_index].setPumpDirectionForward();
   g_machine.m_productDispensers[m_active_pump_index].setPumpPWM(125, true);
   g_machine.m_productDispensers[m_active_pump_index].setPumpEnable(); // POS is 1->4! index is 0->3
   usleep(1000000);                                        // press button to have the pump pumping.
   g_machine.m_productDispensers[m_active_pump_index].setPumpDirectionForward();
   g_machine.m_productDispensers[m_active_pump_index].setPumpPWM(255, true);
   g_machine.m_productDispensers[m_active_pump_index].setPumpEnable(); // POS is 1->4! index is 0->3
   usleep(1000000);                                        // press

   g_machine.m_productDispensers[m_active_pump_index].setPumpsDisableAll();

   g_machine.m_productDispensers[m_active_pump_index].setPumpDirectionReverse();
   g_machine.m_productDispensers[m_active_pump_index].setPumpPWM(125, true);
   g_machine.m_productDispensers[m_active_pump_index].setPumpEnable(); // POS is 1->4! index is 0->3
   usleep(1000000);                                        // press button to have the pump pumping.
   g_machine.m_productDispensers[m_active_pump_index].setPumpsDisableAll();
}

// Advances to Dispense Idle
DF_ERROR stateManualPump::onExit()
{
   DF_ERROR e_ret = OK;
   g_machine.m_productDispensers[m_active_pump_index].setPumpsDisableAll();
      g_machine.control_pcb->setDispenseButtonLightsAllOff();
   // g_machine.m_productDispensers[m_active_pump_index].m_pcb->virtualButtonUnpressHack(m_active_pump_index + 1);
   g_machine.pcb24VPowerSwitch(false);

   return e_ret;
}