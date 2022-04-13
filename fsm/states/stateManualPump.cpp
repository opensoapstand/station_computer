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
   debugOutput::sendMessage("Test pumps manually.", MSG_INFO);
   productDispensers = g_productDispensers;

   // default pump setting
   productDispensers[0].setPumpDirectionForward();
   productDispensers[0].setPumpPWM(255);

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
      if (ACTION_TOGGLE_CYCLIC_PUMP_TEST == m_pMessaging->getAction())
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
            productDispensers[0].setPumpsDisableAll();
         }
      }

      if (ACTION_MANUAL_PUMP_ENABLE == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Enable dispenser pump at pos 1 (needs button to be pressed to actually work)", MSG_INFO);
         productDispensers[0].setPumpEnable(1); // POS is 1->4! index is 0->3
      }

      if (ACTION_MANUAL_PUMP_DISABLE == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Disable dispenser pump at pos 1", MSG_INFO);
         productDispensers[0].setPumpsDisableAll();
      }
      if (ACTION_MANUAL_PUMP_DIRECTION_FORWARD == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Direction forward dispenser pump at pos 1", MSG_INFO);
         productDispensers[0].setPumpDirectionForward();
      }
      if (ACTION_MANUAL_PUMP_DIRECTION_REVERSE == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Direction backward pump at pos 1", MSG_INFO);
         productDispensers[0].setPumpDirectionReverse();
      }
      if (ACTION_MANUAL_PUMP_PWM_SET == m_pMessaging->getAction())
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
            string value_str = std::to_string(val);
            string msg = "PWM value to set (should be in range [0..255]): " + value_str;
            debugOutput::sendMessage(msg, MSG_INFO);
            productDispensers[0].setPumpPWM((uint8_t)val);
         }
      }

      if (ACTION_MANUAL_PUMP_CUSTOM_VOLUME_TEST_TOGGLE == m_pMessaging->getAction())
      {
         dispenseButtonValueMemory = false;
         iscustomVolumeDispenseTest = !iscustomVolumeDispenseTest;
         debugOutput::sendMessage("Custom volume dispense pump model test active? : " + to_string(iscustomVolumeDispenseTest), MSG_INFO);
      }

      if (ACTION_MANUAL_PUMP_FLOW_TEST_TOGGLE == m_pMessaging->getAction())
      {
         isFlowTest = !isFlowTest;
         using namespace std::chrono;
         startFlowTestMillis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
         productDispensers[0].setPumpEnable(1);
         debugOutput::sendMessage("Will display test data in csv format. Keep dispense button pushed during test", MSG_INFO);
      }

      if (ACTION_HELP == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("help\nAvailable printer test commands: \n \
            t: test pump1 \n \
            a: flow measuring test \n \
            g: custom volume dispense model test \n \
               (please note: the machine will dispense some seconds and will then \n \
               automatically revert for some time. Set the reverting time during the testing with \n \
               ixxx;  where xxx is in milliseconds. e.g. i200;  will revert the pump for 200ms \n \
               the pump speed cannot be set during this test) \n \
            e: enable pump1 \n \
            d: disable pump1\n \
            c: toggle cyclic ON/OFF pump1 (keep button pressed)\n \
            f: direction forward pump1 \n r: direction reverse pump1\n ixxx: set pwm pump1 [0..255]\nq: quit ",
                                  MSG_INFO);
      }
   }
   if (isCyclicTesting)
   {
      pumpCyclicTest();
      pumpFlowTest();
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

      if (productDispensers[0].getDispenseButtonValue())
      {
         debugOutput::sendMessage("----------------Dispense button pressed.----------", MSG_INFO);

         double volume = productDispensers[0].getVolumeDispensed();
         debugOutput::sendMessage("Dispensed volume [total]: " + to_string(volume), MSG_INFO);

         // instant flow rate
         double flowRate = productDispensers[0].getInstantFlowRate();
         debugOutput::sendMessage("Dispense flowRate [V/s]: " + to_string(flowRate), MSG_INFO);

         // flow rate windowed avg
         productDispensers[0].updateRunningAverageWindow();
         Time_val avg_1s = productDispensers[0].getAveragedFlowRate(1000);
         debugOutput::sendMessage("Dispense flowRate 1s avg [V/s]: " + to_string(avg_1s.value), MSG_INFO);
         usleep(500000);
      }

      if (productDispensers[0].isPumpEnabled() && productDispensers[0].getDispenseButtonValue())
      {
         unsigned short speed = productDispensers[0].getPumpSpeed();
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
   dispenseButtonValue = productDispensers[0].getDispenseButtonValue();

   if (!dispenseButtonValue && dispenseButtonValueMemory)
   {
      debugOutput::sendMessage("Dispense Button negative Edge.", MSG_INFO);
   }

   if (dispenseButtonValue && !dispenseButtonValueMemory)
   {
      debugOutput::sendMessage("Start Dispensing", MSG_INFO);

      // productDispensers[0].setPumpPWM(125);

      isDispensing = true;
      using namespace std::chrono;
      startDispensingEpochMillis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

      productDispensers[0].setPumpDirectionForward();
      productDispensers[0].setPumpEnable(1);
   }

   using namespace std::chrono;
   uint64_t millis_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
   if (millis_epoch - startDispensingEpochMillis > 2000 && isDispensing)
   {
      debugOutput::sendMessage("End Dispensing. Activate retract millis: " + to_string(retract_time_millis), MSG_INFO);

      isDispensing = false;
      // productDispensers[0].setPumpsDisableAll();
      productDispensers[0].setPumpDirectionReverse();
      productDispensers[0].setPumpEnable(1);
      using namespace std::chrono;
      startRetractingEpochMillis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
      isRetracting = true;
   }

   millis_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
   if (millis_epoch - startRetractingEpochMillis >= retract_time_millis && isRetracting)
   {
      debugOutput::sendMessage("End retraction.", MSG_INFO);
      productDispensers[0].setPumpsDisableAll();
      isRetracting = false;
   }

   dispenseButtonValueMemory = dispenseButtonValue;

   // 2. detect container empty
   // if motor running and flow sensor dispensing.
}

DF_ERROR stateManualPump::pumpFlowTest()
{
   // double volume = productDispensers[0].getVolumeDispensed();
   // debugOutput::sendMessage("Dispense flowRate test. millis/totalvolumne/avgSinceLastcall/02s avg/05s avg/1s avg, " + to_string(volume),MSG_INFO);
   // debugOutput::sendMessage("volume per tick " + to_string(productDispensers[0].getProduct()->getVolumePerTick() ), MSG_INFO);

   if (productDispensers[0].getDispenseButtonValue() & productDispensers[0].isPumpEnabled())
   {

      // instant flow rate
      double flowRate = productDispensers[0].getInstantFlowRate();

      // flow rate windowed avg

      productDispensers[0].updateRunningAverageWindow();
      Time_val avg_02s = productDispensers[0].getAveragedFlowRate(200);
      Time_val avg_05s = productDispensers[0].getAveragedFlowRate(500);
      Time_val avg_1s = productDispensers[0].getAveragedFlowRate(1000);

      double totalVolume = productDispensers[0].getVolumeDispensed();
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
         productDispensers[0].setPumpsDisableAll();
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
         productDispensers[0].setPumpDirectionForward();
         // this->productDispensers[0].getPumpSpeed();
         // productDispensers[0].setPumpPWM(255);
         int speed = productDispensers[0].getProduct()->getPWMFromDB();

         debugOutput::sendMessage("Pump speed for test: " + to_string(speed), MSG_INFO);
         productDispensers[0].setPumpEnable(1); // POS is 1->4! index is 0->3
         isCyclicTestingPumpOn = true;
         cyclicTestPeriodStartEpochMillis = now;
      }
   }
}

DF_ERROR stateManualPump::pumpTest()
{
   debugOutput::sendMessage("pump pump", MSG_INFO);

   productDispensers[0].setPumpDirectionForward();
   productDispensers[0].setPumpPWM(125);
   productDispensers[0].setPumpEnable(1); // POS is 1->4! index is 0->3
   usleep(1000000);                       // press button to have the pump pumping.
   productDispensers[0].setPumpsDisableAll();

   productDispensers[0].setPumpDirectionReverse();
   productDispensers[0].setPumpPWM(125);
   productDispensers[0].setPumpEnable(1); // POS is 1->4! index is 0->3
   usleep(1000000);                       // press button to have the pump pumping.
   productDispensers[0].setPumpsDisableAll();
}

// Advances to Dispense Idle
DF_ERROR stateManualPump::onExit()
{
   DF_ERROR e_ret = OK;
   productDispensers[0].setPumpsDisableAll();

   return e_ret;
}