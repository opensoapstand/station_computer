//
// statedispenseidle.h
// dispense idle state class
//
// created: 26-06-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef STATEDISPENSEIDLE__H_
#define STATEDISPENSEIDLE__H_

#include "../dftypes.h"
#include "../objects/messagemediator.h"
#include "statevirtual.h"

#include "../objects/dispenser.h"

class stateDispenseIdle : public stateVirtual
{
    public:
        stateDispenseIdle();
        stateDispenseIdle(messageMediator * message); //debug through local network 
        stateDispenseIdle(int inputDigit); //debug through terminal
        ~stateDispenseIdle();

        DF_ERROR onEntry();
        
        DF_ERROR onAction();
        DF_FSM onAction(dispenser* dispenseObj);
        
        DF_ERROR onExit();
        DF_FSM onExit(dispenser* dispenseObj);

    private:
};


#endif