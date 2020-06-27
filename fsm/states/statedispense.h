//***************************************
//
// statedispense.h
// dispense state class
//
// created: 26-06-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef STATEDISPENSE__H_
#define STATEDISPENSE__H_

#include "../dftypes.h"
#include "../objects/messagemediator.h"

#include "statevirtual.h"

class stateDispense : public stateVirtual
{
    public:
        stateDispense();
        stateDispense(messageMediator * message); //debug through local network 
        stateDispense(int inputDigit); //debug through terminal
        ~stateDispense();

        DF_ERROR onEntry();
        DF_ERROR onAction(DF_FSM * nextState);
        DF_ERROR onExit();

    private:
        DF_FSM m_stateCurrent;
    
};


#endif