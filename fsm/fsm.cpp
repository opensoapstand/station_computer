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
#include <string.h>
#include "dftypes.h"

#include "states/statevirtual.h"

#include "states/stateinit.h"
#include "states/stateidle.h"
#include "states/statedispense.h"
#include "states/statedispenseidle.h"

#include "objects/dispenser.h"
#include "objects/messagemediator.h"

messageMediator *g_pMessaging; //debug through local network
stateVirtual *g_stateArray[FSM_MAX]; //an object for every state
dispenser *g_dispense;   //replace the magic number

DF_ERROR initObjects();
DF_ERROR createStateArray();
DF_ERROR stateLoop();

int main()
{
    DF_ERROR dfRet = OK;

    if (OK == initObjects())
    {
        dfRet = g_pMessaging->createThreads();
        if (OK == dfRet){
            dfRet = stateLoop();
        }
    }

    return dfRet;
}

DF_ERROR stateLoop()
{
    DF_ERROR dfRet = OK;
    DF_FSM fsmState = START; 
    DF_FSM fsmNewState = INIT; 

    while (OK == dfRet) //while no error has occurred
    {
        if (fsmState != fsmNewState) //state change
        {
            debugOutput::sendMessage("onEntry() [" + g_stateArray[fsmNewState]->toString() + "]", INFO);
            dfRet = g_stateArray[fsmNewState]->onEntry();
            fsmState = g_stateArray[fsmNewState]->getCurrentState();
        }

        if (OK == dfRet) 
        {
           debugOutput::sendMessage("onAction() [" + g_stateArray[fsmState]->toString() + "]", INFO);
           dfRet = g_stateArray[fsmState]->onAction();

            fsmNewState = g_stateArray[fsmState]->getNextState();

            if ((OK == dfRet) && (fsmNewState != fsmState))
            {
                debugOutput::sendMessage("onExit() [" + g_stateArray[fsmState]->toString() + "]", INFO);
                dfRet = g_stateArray[fsmState]->onExit();

                fsmNewState = g_stateArray[fsmState]->getNextState(); //update the state
            }
        }

    }

    return dfRet;
}

DF_ERROR initObjects()
{
    DF_ERROR dfRet = OK;

    g_pMessaging = NULL;
    g_pMessaging = new messageMediator();
        
    dfRet = createStateArray();
    if (OK != dfRet)
    {
        //next 
    }

    return dfRet;
}

DF_ERROR createStateArray()
{
    DF_ERROR dfRet = ERROR_PTHREADS;

    if (null != g_pMessaging)
    {
        g_stateArray[INIT] = new stateInit(g_pMessaging);
        g_stateArray[IDLE] = new stateIdle(g_pMessaging);
        g_stateArray[DISPENSE_IDLE] = new stateDispenseIdle(g_pMessaging);
        g_stateArray[DISPENSE] = new stateDispense(g_pMessaging);
    }

    return dfRet;
}
