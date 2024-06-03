//***************************************
//
// statedispenseinit.h
// idle state class
//
// created: 01-2022
// by:Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef STATEDISPENSEINIT__H_
#define STATEDISPENSEINIT__H_

#include "../dftypes.h"
#include "../objects/messageMediator.h"
#include "stateVirtual.h"
#include "../fsm.h"
#include "../objects/product.h"

class stateDispenseInit : public stateVirtual
{
public:
    stateDispenseInit();
    stateDispenseInit(messageMediator *message); //debug through local network
    ~stateDispenseInit();

    string toString();

    DF_ERROR onEntry();
    DF_ERROR onAction();
    DF_ERROR onExit();
    
    // product *productOrder;

private:
    // int dispenser_index;
    char size;
    // dispenser *productDispensers;
};

#endif
