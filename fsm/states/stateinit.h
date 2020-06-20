//***************************************
//
// stateinit.h
// init state class
//
// created: 12-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef STATEINIT__H_
#define STATEINIT__H_

#include "../dftypes.h"
#include "../objects/messagemediator.h"
#include "statevirtual.h"

class stateInit : public stateVirtual
{
    public:
        stateInit(messageMediator * message);
        ~stateInit();

        DF_ERROR onEntry();
        DF_ERROR onAction(DF_FSM * nextState);
        DF_ERROR onExit();


    private:
        
};


#endif