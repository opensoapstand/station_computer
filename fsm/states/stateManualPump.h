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

class stateManualPump : public stateVirtual
{
public:
    stateManualPump();
    stateManualPump(messageMediator *message); //debug through local network
    ~stateManualPump();

    string toString();
    DF_ERROR pumpTest();

    DF_ERROR onEntry();
    DF_ERROR onAction();
    DF_ERROR onExit();
   

private:
    dispenser *productDispensers;
   
};

#endif
