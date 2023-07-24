//***************************************
//
// stateManualConfig.h
// idle state class
//
// created: 01-2022
// by:Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef STATEMANUALCONFIG__H_
#define STATEMANUALCONFIG__H_

#include "../dftypes.h"
#include "../objects/messageMediator.h"
#include "stateVirtual.h"
#include "../fsm.h"
#include "../objects/product.h"
#include "../objects/machine.h"

#include <string>
std::string uint8ToBinaryString(uint8_t x);

class stateManualConfig : public stateVirtual
{
public:
    stateManualConfig();
    stateManualConfig(messageMediator *message); // debug through local network
    ~stateManualConfig();

    string toString();
    DF_ERROR onEntry();
    DF_ERROR onAction();
    DF_ERROR onExit();

    void readRelevantRegisters();

private:
    int m_active_pump_index;
    dispenser *productDispensers;
};

#endif
