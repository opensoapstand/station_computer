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

#define AIR_CHAR 'a'
#define WATER_CHAR 'w'
#define DRINK_CHAR 'd'

class stateDispense : public stateVirtual
{
    public:
        stateDispense();
        stateDispense(messageMediator * message); 
        ~stateDispense();

        string toString();

        DF_ERROR onEntry();
        DF_ERROR onAction(dispenser* cassettes[]);
        DF_ERROR onExit();

    private:
    
};


#endif