//***************************************
//
// statedispense.h
// dispense state class
//
// created: 26-06-2020
// by: Jason Wang & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef STATEDISPENSE__H_
#define STATEDISPENSE__H_

#include "../dftypes.h"
#include "../objects/messageMediator.h"

#include "stateVirtual.h"

#define AIR_CHAR 'a'
#define WATER_CHAR 'w'
#define DRINK_CHAR 'd'
#define CLEAN_CHAR 'c'
#define DISPENSE_END_CHAR 'f'

class stateDispense : public stateVirtual
{
    public:
        stateDispense();
        stateDispense(messageMediator * message); 
        ~stateDispense();

        string toString();

        DF_ERROR onEntry();
        DF_ERROR onAction();
        DF_ERROR onExit();
        
        // FIXME: Issues with Initializing with Button...
        // dispenser* dispenserSetup(getButton()) {};
        // DF_ERROR dispenserSetup();


    private:
        // XXX: Remove when drink instantiated
        int pos;

        // present read Volume
        // last read Volume
        // max read volume
    
};
#endif