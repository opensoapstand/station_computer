//***************************************
//
// statedispense.h
// dispense state class
//
// created: 01-2022
// by:Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef STATEDISPENSE__H_
#define STATEDISPENSE__H_

#include "../dftypes.h"
#include "../objects/messageMediator.h"
#include "../fsm.h"

#include "stateVirtual.h"

class stateDispense : public stateVirtual
{
public:
    stateDispense();
    stateDispense(messageMediator *message);
    ~stateDispense();

    string toString();

    DF_ERROR rectractProductBlocking();

    DF_ERROR onEntry();
    DF_ERROR onAction();
    DF_ERROR onExit();

private:
    int slot;
    int pos_index;

    dispenser *productDispensers;
    bool flushCommand;
    char size;
};
#endif
