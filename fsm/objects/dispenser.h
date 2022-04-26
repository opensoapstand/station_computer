//***************************************
//
// dispenser.cpp
// dispenser class owns the package for a dispense.
//
// Links to circuit board addresses and
// holds instructions for dispensing.
//
// created: 01-2022
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef _DISPENSER__H_
#define _DISPENSER__H_

#include "../dftypes.h"
#include "../components/gpio.h"
#include "../components/dsed8344.h"
#include "../components/odysseyx86gpio.h"
#include "product.h"
#include <sqlite3.h>

// #include <stdint.h>
// Total Number of Devices
#define NUM_SOLENOID 3 // 12v for drink,water, and air
#define NUM_PUMP 2     // forward and reverse pin
#define NUM_FLOWSENSOR 1
#define NUM_BUTTON 1

// Range of Dispenser Still Option positions that use pumps
#define PUMP_OPTION_START_POSITION 5
#define PUMP_OPTION_STOP_POSITION 8

#define PRODUCT 0
#define WATER 1
#define AIR 2

#define FORWARD 0
#define REVERSE 1

#define FLOW 0

#define X20 20
#define X21 21
#define X22 22

#define MCP_PIN_START 0
#define MPC_PIN_END 15

class dispenser
{
public:
      dispenser();
      dispenser(gpio *buttonReference);
      ~dispenser();

      DF_ERROR setup();
      // void initDispenser(int slot);

      // private variable setters
      //  DF_ERROR setSolenoid(int mcpAddress, int pin, int pos);
      DF_ERROR setPump(int mcpAddress, int pin, int position);
      DF_ERROR setFlowsensor(int pinint, int pos);

      DF_ERROR startDispense();
      // DF_ERROR stopDispense();

      unsigned short getPumpSpeed();
      bool isPumpEnabled();
      DF_ERROR setPumpDirectionForward();
      DF_ERROR setPumpDirectionReverse();
      DF_ERROR setPumpsDisableAll();
      DF_ERROR setPumpEnable(int pos);
      DF_ERROR setPumpPWM(uint8_t value);
      bool getDispenseButtonValue();
      double getVolumeDispensed();
      void resetVolumeDispensed();

      double getVolumeDeltaAndReset();
      // DF_ERROR updateVolumeDelta();
      double getInstantFlowRate();

      Time_val getVolumeDispensedNow();
      DF_ERROR updateRunningAverageWindow();

      Time_val getAveragedFlowRate(uint64_t window_length_millis);

      bool getIsDispenseTargetReached();
      Dispense_behaviour getDispenseStatus();

      void dispenseButtonTimingreset();
      void dispenseButtonTimingUpdate();
      uint64_t getButtonPressedTotalMillis();
      uint64_t getButtonPressedCurrentPressMillis();

      // void setm_pIsDispenseDone() { *m_pIsDispensing = false; }
      // void setm_pIsDispensing() { *m_pIsDispensing = true; }
      // void setm_pRestartDispense() { *m_pIsDispensing = false; }

      product *getProduct();

      DF_ERROR setProduct(product *product);

      int getI2CAddress(int pos);
      int getI2CPin(int pos);

      DF_ERROR setButtonsShutdownAndMaintenance();
      DF_ERROR setSlot(int slot);
      int getSlot();
      //      DF_ERROR setButtonPress(int address_num, int pin_num);
      bool reader = true;

      // double getVolumeDispensed();
      double getVolumeDispensedPreviously();
      void setVolumeDispensedPreviously(double volume);
      // Interrupt Helpers

      DF_ERROR initDispense(int nVolumeToDispense, double nPrice);
      DF_ERROR stopDispense();

      // double getVolumeSinceLastPoll();
      bool isDispenseTargetVolumeReached();
      // bool registerFlowSensorTick();

      string getDispenseStartTime();
      void loadEmptyContainerDetectionEnabledFromDb();
      bool getEmptyContainerDetectionEnabled();

      //      double getButtonPressDuration();
      //      int getButtonPressTimes();
      //      void resetButtonPressTimes();
      //      void resetButtonPressDuration();

private:
      bool isDispenseFinished;
      double m_nVolumeDispensedSinceLastPoll;
      double m_nTickCount;
      double m_nVolumeTarget;
      char m_nStartTime[50];

      double m_price;

      time_t rawtime;
      struct tm *timeinfo;

      // double m_nVolumeDispensed; // how much has been dispensed in this sale
      double m_nVolumeDispensedPreviously;

      // We only want to create one instance of the class that controls
      // the actual hardware, so declare this static.

      static dsed8344 *the_8344;

      uint64_t dispense_cycle_pump_running_time_millis;
      uint64_t dispense_start_timestamp_epoch;

      uint64_t dispense_button_time_at_last_check_epoch;
      uint64_t dispense_button_total_pressed_millis; // culmination of all button press times
      uint64_t dispense_button_current_press_millis; // time of this single press

      Dispense_behaviour previous_dispense_state;

      int slot;
      Time_val flowRateBuffer[RUNNING_AVERAGE_WINDOW_LENGTH];
      int flowRateBufferIndex;

      uint64_t millisAtLastCheck;
      double previousDispensedVolume;
      unsigned char pump_position;

      bool m_isEmptyContainerDetectionEnabled;


      bool m_isDispenseDone; // XXX: Remove later.
      bool m_isStill;

      bool m_isPumpEnabled;
      bool *m_pIsDispensing;

      bool m_isDispenseNew;

      sqlite3 *db;
        int rc;


      product *m_pDispensedProduct;

      DF_ERROR *m_pthreadError;

      // Pointers to Addresses set in State Init

      gpio *m_pSolenoid[NUM_SOLENOID]; // air,product, and water solenoid control
      gpio *m_pFlowsenor[NUM_FLOWSENSOR];
      gpio *m_pPump[NUM_PUMP]; // forward and reverse pin control
      gpio *m_pPowerOff[1];
      gpio *m_pMM[1];
      gpio *m_pPWRorMM[1];

      // Button reference m_pButton[1] in stateVirtual; IPC shared due to Arduino!
      gpio *m_pButton[NUM_BUTTON];
};

#endif
