//***************************************
//
// stateManualPump.h
// idle state class
//
// created: 01-2022
// by:Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef STATEMANUALPUMP__H_
#define STATEMANUALPUMP__H_

#include "../dftypes.h"
#include "../objects/messageMediator.h"
#include "stateVirtual.h"
#include "../fsm.h"
#include "../objects/product.h"

#include <string>
typedef enum state_auto_pump
{
   AUTO_PUMP_STATE_IDLE,
   AUTO_PUMP_STATE_INIT,
   AUTO_PUMP_STATE_PUMPING,
   AUTO_PUMP_STATE_FINISHED
} state_auto_pump;
class stateManualPump : public stateVirtual
{
public:
    stateManualPump();
    stateManualPump(messageMediator *message); //debug through local network
    ~stateManualPump();

    string toString();
    DF_ERROR pumpTest();
    DF_ERROR autoPumpSetQuantityTest();
    DF_ERROR pumpCyclicTest();
    DF_ERROR pumpFlowTest();
    DF_ERROR customVolumeDispenseTest();

    DF_ERROR onEntry();
    DF_ERROR onAction();
    DF_ERROR onExit();

private:
    dispenser *productDispensers;
    bool isFlowTest=false;
    bool iscustomVolumeDispenseTest=false;
    bool dispenseButtonValueMemory=false;
    bool dispenseButtonValue=false;

    uint64_t startFlowTestMillis;

    uint64_t most_recent_data_output_epoch;
    uint64_t startDispensingEpochMillis;
    uint64_t startRetractingEpochMillis;
    bool isDispensing=false;
    bool isRetracting=false;

    int pump_test_cycle_count;
    bool isCyclicTesting;
    bool isCyclicTestingPumpOn;

    bool triggerOutputData;

    int m_active_pump_index;
    uint64_t cyclicTestPeriodStartEpochMillis;
    uint64_t logSampleStartEpochMillis;

    state_auto_pump m_state_auto_pump;

    uint64_t retract_time_millis;
};

#endif
