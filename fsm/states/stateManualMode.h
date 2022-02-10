//***************************************
//
// stateManualMode.h
// idle state class
//
// created: 29-06-2020
// by: Jason Wang & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef STATEMANUALMODE__H_
#define STATEMANUALMODE__H_

#include "../dftypes.h"
#include "../objects/messageMediator.h"
#include "stateVirtual.h"
#include "../fsm.h"
#include "../objects/product.h"

#include "../../library/printer/Adafruit_Thermal.h"
#include <string>

class stateManualMode : public stateVirtual
{
public:
    stateManualMode();
    stateManualMode(messageMediator *message); //debug through local network
    ~stateManualMode();

    string toString();
    DF_ERROR printTest();

    DF_ERROR onEntry();
    DF_ERROR onAction();
    DF_ERROR onExit();
   

private:
    // Adafruit_Thermal *printerr;
   
};

#endif
