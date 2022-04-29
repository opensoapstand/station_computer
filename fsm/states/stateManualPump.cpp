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
   productDispensers = g_productDispensers;

   // default pump setting
   m_active_pump_index = 0; // pump 1 is activated by default
   debugOutput::sendMessage("Pump testing. Active dispenser pump: " + to_string(m_active_pump_index + 1), MSG_INFO);
   productDispensers[m_active_pump_index].setPumpDirectionForward();
   productDispensers[m_active_pump_index].setPumpPWM(255);

   isFlowTest = false;
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

      if ('0' == m_pMessaging->getAction() || ACTION_QUIT == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Exit pump test", MSG_INFO);

         m_state_requested = STATE_IDLE;
      }

      else if ('1' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Enable dispenser pump " + to_string(m_active_pump_index + 1) + "(press dispense button to make pump actually work)", MSG_INFO);
         productDispensers[m_active_pump_index].setPumpEnable(); // POS is 1->4! index is 0->3
      }

      else if ('2' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Disable dispenser pump " + to_string(m_active_pump_index + 1), MSG_INFO);
         productDispensers[m_active_pump_index].setPumpsDisableAll();
      }
      else if ('3' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Direction forward dispenser pump " + to_string(m_active_pump_index + 1), MSG_INFO);
         productDispensers[m_active_pump_index].setPumpDirectionForward();
      }
      else if ('4' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Direction backward pump " + to_string(m_active_pump_index + 1), MSG_INFO);
         productDispensers[m_active_pump_index].setPumpDirectionReverse();
      }

      else if (ACTION_MANUAL_PUMP_SET == m_pMessaging->getAction())
      {
         productDispensers[m_active_pump_index].setPumpsDisableAll();
         // float PWM_value_byte = 3.12345;
         int val = m_pMessaging->getCommandValue();
         debugOutput::sendMessage("Value is pump number 1,2,3 or 4 :" + to_string((uint8_t)val), MSG_INFO);
         m_active_pump_index = ((uint8_t)val) - 1;
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
            productDispensers[m_active_pump_index].setPumpPWM((uint8_t)val);
         }
      }

      else if ('5' == m_pMessaging->getAction())
      {
         dispenseButtonValueMemory = false;
         iscustomVolumeDispenseTest = !iscustomVolumeDispenseTest;
         debugOutput::sendMessage("Custom volume dispense pump model test active? : " + to_string(iscustomVolumeDispenseTest), MSG_INFO);
      }

      else if ('6' == m_pMessaging->getAction())
      {
         isCyclicTesting = !isCyclicTesting;
         debugOutput::sendMessage("Toggle cyclic pump test" + to_string(isCyclicTesting), MSG_INFO);

         if (isCyclicTesting)
         {
            isCyclicTestingPumpOn = false;
            using namespace std::chrono;
            uint64_t now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            cyclicTestPeriodStartEpochMillis = now - CYCLIC_PUMP_TEST_OFF_CYCLE_MILLIS - 10; // make sure it's expired.
         }
         else
         {
            productDispensers[m_active_pump_index].setPumpsDisableAll();
         }
      }
      else if ('8' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Do pump test", MSG_INFO);
         pumpTest();
      }
      else if ('7' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("HACKHACK HAKC", MSG_INFO);
         productDispensers[m_active_pump_index].reversePumpForSetTimeMillis(500);
         
      }
      else if ('9' == m_pMessaging->getAction())
      {
         isFlowTest = !isFlowTest;
         if (isFlowTest){
            using namespace std::chrono;
            startFlowTestMillis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            productDispensers[m_active_pump_index].setPumpEnable();
            debugOutput::sendMessage("Flow measuring test. Keep dispense button pushed during test. Will display test data in csv format.", MSG_INFO);

         }else{
            debugOutput::sendMessage("End of flow measuring.", MSG_INFO);
            productDispensers[m_active_pump_index].setPumpsDisableAll();
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
                                                      "sX: Set active pump, where X= 1,2,3 or 4\n"
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
                                                      "6: Toggle endurance test. Active pump toggle cyclic ON/OFF (keep button pressed)\n"
                                                      "7: Reverse hack\n"
                                                      "8: Run pump routine (hold dispense button for it to work)\n"
                                                      "9: Toggle flow measuring test\n",
             MSG_INFO);
      }
   }
   if (isCyclicTesting)
   {
      pumpCyclicTest();
      if(isFlowTest){
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

      if (productDispensers[m_active_pump_index].getDispenseButtonValue())
      {
         debugOutput::sendMessage("----------------Dispense button pressed.----------", MSG_INFO);

         double volume = productDispensers[m_active_pump_index].getVolumeDispensed();
         debugOutput::sendMessage("Dispensed volume [total]: " + to_string(volume), MSG_INFO);

         // instant flow rate
         double flowRate = productDispensers[m_active_pump_index].getInstantFlowRate();
         debugOutput::sendMessage("Dispense flowRate [V/s]: " + to_string(flowRate), MSG_INFO);

         // flow rate windowed avg
         productDispensers[m_active_pump_index].updateRunningAverageWindow();
         Time_val avg_1s = productDispensers[m_active_pump_index].getAveragedFlowRate(1000);
         debugOutput::sendMessage("Dispense flowRate 1s avg [V/s]: " + to_string(avg_1s.value), MSG_INFO);
         usleep(500000);
      }

      if (productDispensers[m_active_pump_index].isPumpEnabled() && productDispensers[m_active_pump_index].getDispenseButtonValue())
      {
         unsigned short speed = productDispensers[m_active_pump_index].getPumpSpeed();
         string value = std::to_string(speed);
         string msg = "Pump speed: " + value;
         debugOutput::sendMessage(msg, MSG_INFO);

         using namespace std::chrono;
         uint64_t millis_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

         debugOutput::sendMessage("Millis since epoch: " + to_string(millis_since_epoch), MSG_INFO);

         usleep(500000);
      }
   }

   e_ret = OK;

   return e_ret;
}

DF_ERROR stateManualPump::customVolumeDispenseTest()
{

   // 1. pump with retraction at end
   // when button press has negative edge, retract motor. for time x
   dispenseButtonValue = productDispensers[m_active_pump_index].getDispenseButtonValue();

   if (!dispenseButtonValue && dispenseButtonValueMemory)
   {
      debugOutput::sendMessage("Dispense Button negative Edge.", MSG_INFO);
   }

   if (dispenseButtonValue && !dispenseButtonValueMemory)
   {
      debugOutput::sendMessage("Start Dispensing", MSG_INFO);

      // productDispensers[m_active_pump_index].setPumpPWM(125);

      isDispensing = true;
      using namespace std::chrono;
      startDispensingEpochMillis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

      productDispensers[m_active_pump_index].setPumpDirectionForward();
      productDispensers[m_active_pump_index].setPumpEnable();
   }

   using namespace std::chrono;
   uint64_t millis_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
   if (millis_epoch - startDispensingEpochMillis > 2000 && isDispensing)
   {
      debugOutput::sendMessage("End Dispensing. Activate retract millis: " + to_string(retract_time_millis), MSG_INFO);

      isDispensing = false;
      // productDispensers[m_active_pump_index].setPumpsDisableAll();
      productDispensers[m_active_pump_index].setPumpDirectionReverse();
      productDispensers[m_active_pump_index].setPumpEnable();
      using namespace std::chrono;
      startRetractingEpochMillis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
      isRetracting = true;
   }

   millis_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
   if (millis_epoch - startRetractingEpochMillis >= retract_time_millis && isRetracting)
   {
      debugOutput::sendMessage("End retraction.", MSG_INFO);
      productDispensers[m_active_pump_index].setPumpsDisableAll();
      isRetracting = false;
   }

   dispenseButtonValueMemory = dispenseButtonValue;

   // 2. detect container empty
   // if motor running and flow sensor dispensing.
}

DF_ERROR stateManualPump::pumpFlowTest()
{
   // double volume = productDispensers[m_active_pump_index].getVolumeDispensed();
   // debugOutput::sendMessage("Dispense flowRate test. millis/totalvolumne/avgSinceLastcall/02s avg/05s avg/1s avg, " + to_string(volume),MSG_INFO);
   // debugOutput::sendMessage("volume per tick " + to_string(productDispensers[m_active_pump_index].getProduct()->getVolumePerTick() ), MSG_INFO);

   if (productDispensers[m_active_pump_index].getDispenseButtonValue() & productDispensers[m_active_pump_index].isPumpEnabled())
   {

      // instant flow rate
      double flowRate = productDispensers[m_active_pump_index].getInstantFlowRate();

      // flow rate windowed avg

      productDispensers[m_active_pump_index].updateRunningAverageWindow();
      Time_val avg_02s = productDispensers[m_active_pump_index].getAveragedFlowRate(200);
      Time_val avg_05s = productDispensers[m_active_pump_index].getAveragedFlowRate(500);
      Time_val avg_1s = productDispensers[m_active_pump_index].getAveragedFlowRate(1000);

      double totalVolume = productDispensers[m_active_pump_index].getVolumeDispensed();
      debugOutput::sendMessage("Dispense flowRate test. millis/totalvolumne/avgSinceLastcall/02s avg/05s avg/1s avg, " +
                                   // debugOutput::sendMessage("Dispense flowRate test. millis/instant/1s/5s avg, " +
                                   to_string(avg_1s.time_millis - startFlowTestMillis) + "," +
                                   to_string(totalVolume) + "," +
                                   to_string(flowRate) + "," +
                                   to_string(avg_02s.value) + "," +
                                   to_string(avg_05s.value) + "," +
                                   to_string(avg_1s.value)
                               //+ "," + to_string(avg_5s.value)
                               ,
                               MSG_INFO);

      // debugOutput::sendMessage("Dispense flowRate test. millis/totalvolumne/avgSinceLastcall/1s avg, " +
      //                              // debugOutput::sendMessage("Dispense flowRate test. millis/instant/1s/5s avg, " +
      //                              to_string(avg_1s.time_millis - startFlowTestMillis) + "," +
      //                              to_string(totalVolume) + "," +
      //                              to_string(flowRate) + "," +
      //                              to_string(avg_1s.value)
      //                          //+ "," + to_string(avg_5s.value)
      //                          ,
      //                          MSG_INFO);
   }
   usleep(50000);
}

DF_ERROR stateManualPump::pumpCyclicTest()
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
         productDispensers[m_active_pump_index].setPumpsDisableAll();
         isCyclicTestingPumpOn = false;
         cyclicTestPeriodStartEpochMillis = now;
      }
   }
   else
   {
      // check for end of OFF cycle
      if (cyclicTestPeriodStartEpochMillis + CYCLIC_PUMP_TEST_OFF_CYCLE_MILLIS < now)
      {
         debugOutput::sendMessage("\n******************************\n******PUMP CYCLING TESTING******\n*****************\n  cycle: " + to_string(pump_test_cycle_count), MSG_INFO);
         pump_test_cycle_count++;
         productDispensers[m_active_pump_index].setPumpDirectionForward();
         // this->productDispensers[m_active_pump_index].getPumpSpeed();
         // productDispensers[m_active_pump_index].setPumpPWM(255);
         int speed = productDispensers[m_active_pump_index].getProduct()->getPWMFromDB();

         debugOutput::sendMessage("Pump speed for test: " + to_string(speed), MSG_INFO);
         productDispensers[m_active_pump_index].setPumpEnable(); // POS is 1->4! index is 0->3
         isCyclicTestingPumpOn = true;
         cyclicTestPeriodStartEpochMillis = now;
      }
   }
}

DF_ERROR stateManualPump::pumpTest()
{
   debugOutput::sendMessage("pump pump", MSG_INFO);

   productDispensers[m_active_pump_index].setPumpDirectionForward();
   productDispensers[m_active_pump_index].setPumpPWM(125);
   productDispensers[m_active_pump_index].setPumpEnable(); // POS is 1->4! index is 0->3
   usleep(1000000);                                         // press button to have the pump pumping.
   productDispensers[m_active_pump_index].setPumpDirectionForward();
   productDispensers[m_active_pump_index].setPumpPWM(255);
   productDispensers[m_active_pump_index].setPumpEnable(); // POS is 1->4! index is 0->3
   usleep(1000000);                                         // press

   productDispensers[m_active_pump_index].setPumpsDisableAll();

   productDispensers[m_active_pump_index].setPumpDirectionReverse();
   productDispensers[m_active_pump_index].setPumpPWM(125);
   productDispensers[m_active_pump_index].setPumpEnable(); // POS is 1->4! index is 0->3
   usleep(1000000);                                         // press button to have the pump pumping.
   productDispensers[m_active_pump_index].setPumpsDisableAll();
}

// Advances to Dispense Idle
DF_ERROR stateManualPump::onExit()
{
   DF_ERROR e_ret = OK;
   productDispensers[m_active_pump_index].setPumpsDisableAll();

   return e_ret;
}