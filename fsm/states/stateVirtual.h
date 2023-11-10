//***************************************
//
// statevirtual.h
// virtual state class
//
// created: 01-2022
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef STATEVIRTUAL__H_
#define STATEVIRTUAL__H_

#include <string>
#include "../dftypes.h"
#include "../objects/messageMediator.h"

#include "../objects/debugOutput.h"
#include "../objects/dispenser.h"


using namespace std;
class stateVirtual
{
public:
    stateVirtual();
    // stateVirtual(messageMediator *message);
    ~stateVirtual();

    virtual string toString() = 0;

    DF_FSM getRequestedState();
    // DF_FSM getCurrentState();

    virtual DF_ERROR onEntry() = 0;
    virtual DF_ERROR onAction() = 0;
    virtual DF_ERROR onExit() = 0;

    // gpio *getSolenoid(int pos, int type);
    // gpio *getDispenseButtonState();

    //static product *testProducts[PRODUCT_DISPENSERS_MAX];
    
    messageMediator *m_pMessaging;

protected:
    // DF_FSM m_state;
    DF_FSM m_state_requested;
    gpio *m_pButton[NUM_BUTTON];


    // product_order* m_selectedOrder;

    // Unique Reference to selected Product
    unique_ptr<product> selectedProductPtr;
    // Unique Reference to currently selected Dispenser
    unique_ptr<dispenser> selectedDispenserPtr;
};

#endif
