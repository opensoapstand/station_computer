//
// statedispenseidle.h
// dispense idle state class
//
// Recieves and interprets string command from FSM.
// Routes dispense instruction to GPIO's
// created: 01-2022
// by:Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef STATEDISPENSEIDLE__H_
#define STATEDISPENSEIDLE__H_

#include "../dftypes.h"
#include "../objects/messageMediator.h"
#include "stateVirtual.h"
#include "../fsm.h"

#include "../objects/dispenser.h"

class stateDispenseIdle : public stateVirtual
{
public:
    stateDispenseIdle();
    stateDispenseIdle(messageMediator *message); //debug through local network
    ~stateDispenseIdle();

    string toString();

    DF_ERROR onEntry();
    DF_ERROR onAction();
    DF_ERROR onExit();

private:
    int pos;
    char size;
    dispenser *productDispensers;
    bool flushCommand;
};

#endif
