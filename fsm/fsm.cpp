//***************************************
//
// fsm.cpp
// main state class
//
// created: 12-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "dftypes.h"

#include "states\statevirtual.h"
#include "states\stateinit.h"

#include "objects\dispenser.h"

messageMediator * g_pMessaging;
stateVirtual * g_stateArray[FSM_MAX];
dispenser * g_dispenseArray[9];   //replace the magic number

int main()
{
    DF_ERROR dfRet = OK;

    if (OK != createStateArray())
    {
        dfRet = stateLoop();
    }

    return dfRet;
}

DF_ERROR stateLoop()
{
    DF_ERROR dfRet = OK;
    DF_FSM fsmState = START;
    DF_FSM fsmNewState = IDLE;

    while (OK == dfRet)
    {
        if (fsmState != fsmNewState)
        {
            dfRet = g_stateArray[fsmNewState]->onEntry();
            fsmState = fsmNewState;
        }

        if (OK == dfRet)
        {
            dfRet = g_stateArray[fsmState]->onAction(&fsmNewState);
            if ((OK == dfRet) &&
                (fsmNewState != fsmState))
            {
                dfRet = g_stateArray[fsmState]->onExit();
            }
        }

    }

    return dfRet;
}

DF_ERROR initObjects()
{
    DF_ERROR dfRet = OK;

    dfRet = createStateArray();
    if (OK != dfRet)
    {
        //next 
    }

    return dfRet;
}

DF_ERROR createStateArray()
{
    DF_ERROR dfRet = OK;

    g_stateArray[INIT] = new stateInit(g_pMessaging);

    return dfRet;
}