//***************************************
//
// dispenser.cpp
// dispenser class owns the package for a dispense.
//
// Links to circuit board addresses and
// holds instructions for dispensing.
//
// created: 01-2022
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef _DISPENSER__H_
#define _DISPENSER__H_

#include "../dftypes.h"
#include "../components/gpio.h"
#include "../components/pcb.h"
#include "../components/fsmodysseyx86gpio.h"
// #include "machine.h"
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

// class machine;

class dispenser
{
public:
      dispenser();
      dispenser(gpio *buttonReference);
      ~dispenser();

      // DF_ERROR setup(machine *machine, product *pnumber);
      DF_ERROR setup(pcb *pcb, product *pnumbers);
      // DF_ERROR setup(pcb* pcb, machine* machine);
      void refresh();
      DF_ERROR loadGeneralProperties();
      bool loadDispenserParametersFromDb();
      void sendToUiIfAllowed(string message);
      void logUpdateIfAllowed(string message);
      bool getIsStatusUpdateAllowed();
      bool isSlotEnabled();

      // product setting
      // selected product
      product *getProductFromPNumber(int pnumber);

      void setBasePNumberAsSingleDispenseSelectedProduct();
      product *getSelectedProduct();
      bool setSelectedProduct(int pnumber);
      void setSelectedSizeAsChar(char size);
      char getSelectedSizeAsChar();
      double getSelectedSizeAsVolume();

      void setActiveProduct(int pnumber);

      void setPNumberAsSingleDispenseSelectedProduct(int pnumber);
      void setAdditiveFromPositionAsSingleDispenseSelectedProduct(int position);

      bool isPNumberValidInThisDispenser(int pnumber, bool mustBeAdditiveOrBase);

      int getMixPNumberFromMixIndex(int mixIndex);

      // active product
      product *getActiveProduct();
      void linkActiveProductVolumeUpdate();

      // DF_ERROR initButtonsShutdownAndMaintenance();
      DF_ERROR setSlot(int slot);
      int getSlot();

      DF_ERROR initGlobalFlowsensorIO(int pinint);

      void setPumpReversalEnabled(bool isEnabled);
      void setPumpSlowStartStopEnabled(bool isEnabled);
      unsigned short getPumpSpeed();
      DF_ERROR setPumpDirectionForward();
      DF_ERROR setPumpDirectionReverse();
      DF_ERROR setPumpsDisableAll();
      DF_ERROR setPumpEnable();
      DF_ERROR setPumpPWM(uint8_t value, bool enableLog);
      DF_ERROR pumpSlowStart(bool forwardElseReverse);
      DF_ERROR pumpSlowStartHandler();
      DF_ERROR pumpSlowStopBlocking();
      DF_ERROR preparePumpForDispenseTrigger();
      void reversePumpForSetTimeMillis(int millis);

      const char *getDispenseStatusAsString();
      void updateDispenseStatus();
      const char *getSlotStateAsString();
      Dispense_behaviour getDispenseStatus();
      Slot_state getSlotState();
      void setSlotState(Slot_state state);
      void updateSlotState();
      void analyseSlotState();

      DF_ERROR initActivePNumberDispense(double volume);
      DF_ERROR startActivePNumberDispense();
      DF_ERROR stopActivePNumberDispense();

      DF_ERROR initSelectedProductDispense(char size, double nPrice);
      DF_ERROR startSelectedProductDispense();
      DF_ERROR stopSelectedProductDispense();
      string getSelectedProductDispenseStartTime();
      string getSelectedProductDispenseEndTime();

      int getBasePNumber();
      int getActivePNumber();
      int getSelectedPNumber();
      int getAdditivePNumber(int position);

      // dispenser flow
      // void resetDispenserVolumeDispensed();
      // double getDispenserVolumeDispensed();
      // bool isDispenserVolumeTargetReached();

      // product flow
      bool isActiveProductVolumeTargetReached();
      double getActiveProductVolumeDispensed();
      double getActiveProductVolumeRemaining();
      void resetActiveProductVolumeDispensed();
      void subtractActiveFromProductVolumeDispensed(double volume_to_distract);

      bool isSelectedProductVolumeTargetReached();
      double getSelectedProductVolumeDispensed();
      double getSelectedProductVolumeRemaining();
      void resetSelectedProductVolumeDispensed();
      void subtractSelectedFromProductVolumeDispensed(double volume_to_distract);

      bool isProductVolumeTargetReached(int pnumber);
      double getProductTargetVolume(int pnumber);
      double getProductVolumeDispensed(int pnumber);
      double getProductVolumeRemaining(int pnumber);
      void resetProductVolumeDispensed(int pnumber);
      void subtractFromProductVolumeDispensed(int pnumber, double volume_to_distract);

      void initProductFlowRateCalculation();
      Time_val createAndGetActiveProductVolumeDispensedDatapoint();
      double getProductVolumeDeltaAndReset();
      double getProductFlowRateInstantaneous();
      DF_ERROR updateActiveProductFlowRateRunningAverageWindow();
      Time_val getAveragedProductFlowRate(uint64_t window_length_millis);

      void setDispenseButtonLight(bool onElseOff);
      bool getDispenseButtonValue();
      bool getDispenseButtonEdgeNegative();
      bool getDispenseButtonEdgePositive();
      void dispenseButtonTimingreset();
      void dispenseButtonTimingUpdate();
      int getDispenseButtonPressesDuringDispensing();
      uint64_t getButtonPressedTotalMillis();
      uint64_t getButtonPressedCurrentPressMillis();
      void addDispenseButtonPress();

      void setSpoutSolenoid(bool openElseClosed);
      void setActiveProductSolenoid(bool openElseClosed);
      void setProductSolenoid(int pnumber, bool openElseClosed);

      pcb *m_pcb;
      product *m_pnumbers;

      static void parseIntCsvString(const std::string &csvString, int* intArray, int &size);
      static void parseDoubleCsvString(const std::string &csvString, double * doubleArray,int &size);

      void setCustomMixParametersAsSelectedProduct(string pnumbers, string pnumberRatios);

private:
      int m_slot;

      bool m_isPumpReversalEnabled;
      bool m_isPumpSlowStartStopEnabled;
      bool isPumpSoftStarting;
      uint64_t slowStartMostRecentIncreaseEpoch;
      uint8_t pwm_actual_set_speed;

      char m_selectedSizeAsChar;

      int m_base_pnumber;
      int m_selected_pnumber;
      int m_active_pnumber;
      int m_dispense_pnumbers [DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX];
      int m_additive_pnumbers [DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX];
      int m_custom_mix_pnumbers [DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX];
      int m_custom_mix_pnumbers_count;
      double m_custom_mix_ratios [DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX];
      int m_custom_mix_ratios_count;

      int m_dispense_pnumbers_count;
      int m_additive_pnumbers_count;
      string dispense_numbers_str;
      string m_additive_pnumbers_str;
      bool m_is_slot_enabled;
      string m_status_text;

      double m_dispenser_volume_dispensed;
      double m_dispenserVolumeTarget;

      char m_nStartTime[50];
      char m_nEndTime[50];

      double m_price;

      time_t rawtime;
      struct tm *timeinfo;

      uint64_t dispense_cycle_pump_running_time_millis;
      uint64_t dispense_start_timestamp_epoch;

      uint64_t dispense_button_time_at_last_check_epoch;
      uint64_t dispense_button_total_pressed_millis; // culmination of all button press times
      uint64_t dispense_button_current_press_millis; // time of this single press

      int dispense_button_press_count_during_dispensing;

      uint64_t previous_status_update_allowed_epoch;
      bool isStatusUpdateSendAndPrintAllowed;
      Dispense_behaviour previous_dispense_state;
      Dispense_behaviour dispense_state;
      Slot_state slot_state;

      Time_val flowRateBuffer[RUNNING_AVERAGE_WINDOW_LENGTH];
      int flowRateBufferIndex;
      uint64_t millisAtLastCheck;
      double previousDispensedVolume;

      bool m_isSlotEnabled;

      sqlite3 *db;
      int rc;

      gpio *m_pFlowsensor;

      // gpio *m_pButtonPowerOff[1];
      // gpio *m_pButtonDisplayMaintenanceMode[1];
      // gpio *m_pPowerOffOrMaintenanceModeButtonPressed[1];
      // gpio *m_pDispenseButton4[1];

      // Button reference m_pButton[1] in stateVirtual; IPC shared due to Arduino!
      // gpio *m_pButton[NUM_BUTTON];
};

#endif
