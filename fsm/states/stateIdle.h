//***************************************
//
// stateidle.h
// idle state class
//
// created: 29-06-2020
// by: Jason Wang & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef STATEIDLE__H_
#define STATEIDLE__H_

#include "../dftypes.h"
#include "../objects/messageMediator.h"
#include "stateVirtual.h"
#include "../fsm.h"
#include "../objects/drink.h"

class stateIdle : public stateVirtual
{
public:
    stateIdle();
    stateIdle(messageMediator *message); //debug through local network
    ~stateIdle();

    string toString();

    DF_ERROR onEntry();
    DF_ERROR onAction();
    DF_ERROR onExit();
    // DF_ERROR dispenserSetup(){};

    // messageMediator * g_pMessaging;

    drink *drinkOrder;

private:
    int pos;
    dispenser *productDispensers;
};

#endif
