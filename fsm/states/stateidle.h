//***************************************
//
// stateidle.h
// idle state class
//
// created: 29-06-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef STATEIDLE__H_
#define STATEIDLE__H_

#include "../dftypes.h"
#include "../objects/messagemediator.h"
#include "statevirtual.h"

class stateIdle : public stateVirtual
{
    public:
        stateIdle();
        stateIdle (messageMediator * message); //debug through local network 
        stateIdle (int inputDigit); //debug through terminal
        ~stateIdle ();

        DF_ERROR onEntry();
        DF_ERROR onAction();
        DF_ERROR onExit();

    private:    
};


#endif