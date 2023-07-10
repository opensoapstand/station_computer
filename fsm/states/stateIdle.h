//***************************************
//
// stateidle.h
// idle state class
//
// created: 01-2022
// by:Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef STATEIDLE__H_
#define STATEIDLE__H_

#include "../dftypes.h"
#include "../objects/messageMediator.h"
#include "stateVirtual.h"
#include "../fsm.h"
#include "../objects/product.h"

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
    product *productOrder;
    int temperatureRefresh;

private:
    int pos;
    dispenser *productDispensers;
};

#endif
