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

class stateVirtual
{
    public:
        stateVirtual();
        stateVirtual(messageMediator * message);
        stateVirtual(int state);
        ~stateVirtual();

        virtual DF_ERROR onEntry(){};
        virtual DF_ERROR onAction(DF_FSM * nextState){};
        virtual DF_ERROR onExit(){};


    protected:
        DF_FSM  m_state;
        messageMediator * m_pMessaging;
};


#endif