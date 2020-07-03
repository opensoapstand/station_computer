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

#include "../dftypes.h"
#include "../objects/messagemediator.h"

#include "../objects/debugoutput.h"
#include "../objects/dispenser.h"

#define CASSETTES_MAX 9


class stateVirtual
{
    public:
        stateVirtual();
        stateVirtual(messageMediator * message);
        stateVirtual(int state);
        ~stateVirtual();

        DF_FSM getNextState();
        DF_FSM getCurrentState();

        //void setNextState(DF_FSM *nextState);

        virtual DF_ERROR onEntry(){};
        virtual DF_ERROR onAction(){};
        virtual DF_ERROR onExit(){};

    protected:
        DF_FSM  m_state;
        DF_FSM  m_nextState;
        dispenser* cassettes[CASSETTES_MAX];
        gpio *m_pButton[NUM_BUTTON];
        messageMediator * m_pMessaging;
};


#endif