//***************************************
//
// statevirtual.h
// virtual state class
//
// created: 12-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef STATEVIRTUAL__H_
#define STATEVIRTUAL__H_

#include <string>
#include "../dftypes.h"
#include "../objects/messagemediator.h"

#include "../objects/debugoutput.h"
#include "../objects/dispenser.h"

#define CASSETTES_MAX 9

using namespace std;
class stateVirtual
{
    public:
        stateVirtual();
        stateVirtual(messageMediator * message);
        ~stateVirtual();

        virtual string toString() = 0;

        DF_FSM getNextState();
        DF_FSM getCurrentState();

        virtual DF_ERROR onEntry() = 0;
        virtual DF_ERROR onAction(dispenser* cassettes) = 0;
        virtual DF_ERROR onExit() = 0;
        virtual dispenser* dispenserSetup() = 0;

        gpio* getSolenoid(int pos, int type);
        gpio* getButton();

    protected:
        DF_FSM  m_state;
        DF_FSM  m_nextState;
        //dispenser* cassettes[CASSETTES_MAX];
        gpio *m_pButton[NUM_BUTTON];
        messageMediator * m_pMessaging;
};


#endif