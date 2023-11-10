//***************************************
//
// stateEnd.h
// end state class
//
// created: 01-2022
// by:Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef STATEEND__H_
#define STATEEND__H_

#include "../dftypes.h"
#include "../objects/messageMediator.h"
#include "stateVirtual.h"
#include "../fsm.h"


class stateEnd : public stateVirtual
{
public:
    stateEnd();
    stateEnd(messageMediator *message); //debug through local network
    ~stateEnd();

    string toString();

    DF_ERROR onEntry();
    DF_ERROR onAction();
    DF_ERROR onExit();
    
    product *productOrder;

private:
    int pos;
    dispenser *productDispensers;
};

#endif
