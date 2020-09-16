//***************************************
//
// stateDispenseEnd.h
// dispense state class
//
// created: 26-06-2020
// by: Jason Wang & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef STATEDISPENSEEND__H_
#define STATEDISPENSEEND__H_

#include "../dftypes.h"
#include "../objects/messageMediator.h"

#include "stateVirtual.h"

class stateDispenseEnd : public stateVirtual
{
    public:
        stateDispenseEnd();
        stateDispenseEnd(messageMediator * message); 
        ~stateDispenseEnd();

        string toString();

        DF_ERROR onEntry();
        DF_ERROR onAction(dispenser* cassettes);
        DF_ERROR onExit();

        dispenser* dispenserSetup(){};
        
    private:    
};
#endif