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

#include "states/stateVirtual.h"

#include "states/stateInit.h"
#include "states/stateIdle.h"
#include "states/stateDispense.h"
#include "states/stateDispenseIdle.h"

#include "objects/dispenser.h"
#include "objects/messageMediator.h"

messageMediator *g_pMessaging;       //debug through local network
stateVirtual *g_stateArray[FSM_MAX]; //an object for every state
dispenser *g_dispense;               //replace the magic number

DF_ERROR initObjects();
DF_ERROR createStateArray();
DF_ERROR stateLoop();

int main()
{

    pthread_t ipThread, kbThread;

    DF_ERROR dfRet = OK;

    if (OK == initObjects())
    {
        dfRet = g_pMessaging->createThreads(kbThread, ipThread);
        if (OK == dfRet){
            dfRet = stateLoop();
        }
    }

    return dfRet;
}

/*
 * Poll for State changes: States oulined in DF_FSM
*/
DF_ERROR stateLoop()
{
    DF_ERROR dfRet = OK;
    DF_FSM fsmState = START; 
    DF_FSM fsmNewState = INIT; 

    while (OK == dfRet) //while no error has occurred
    {
        if (fsmState != fsmNewState) //state change
        {
            // debugOutput::sendMessage("onEntry()  [" + g_stateArray[fsmNewState]->toString() + "]", STATE_CHANGE);
            dfRet = g_stateArray[fsmNewState]->onEntry();
            fsmState = g_stateArray[fsmNewState]->getCurrentState();
        }

        if (OK == dfRet) 
        {
            // debugOutput::sendMessage("onAction() [" + g_stateArray[fsmState]->toString() + "]", STATE_CHANGE);
            dfRet = g_stateArray[fsmState]->onAction(g_dispense);

            // FIXME: Move to initialization within init state...
            if(INIT == fsmState)
            {                
                g_dispense = g_stateArray[INIT]->dispenserSetup();
            }

            fsmNewState = g_stateArray[fsmState]->getNextState();

            if ((OK == dfRet) && (fsmNewState != fsmState))
            {
                debugOutput::sendMessage("onExit()   [" + g_stateArray[fsmState]->toString() + "]", STATE_CHANGE);
                dfRet = g_stateArray[fsmState]->onExit();

                fsmNewState = g_stateArray[fsmState]->getNextState(); //update the state
            }

        }

    }

    return dfRet;
}

/*
 * Mutex Setting; Spin up Threads
*/
DF_ERROR initObjects()
{
    DF_ERROR dfRet = OK;

    g_pMessaging = NULL;
    g_pMessaging = new messageMediator();
        
    dfRet = createStateArray();
    if (OK != dfRet)
    {
        // TODO: DB function to check/create DB
        //next 
    }
    return dfRet;
}

/*
 * Instantiate Array to hold objects specfic to states (DF_FSM)
*/
DF_ERROR createStateArray()
{
    DF_ERROR dfRet = ERROR_PTHREADS;

    if (NULL != g_pMessaging)
    {
        g_stateArray[INIT] = new stateInit(g_pMessaging);
        g_stateArray[IDLE] = new stateIdle(g_pMessaging);
        g_stateArray[DISPENSE_IDLE] = new stateDispenseIdle(g_pMessaging);
        g_stateArray[DISPENSE] = new stateDispense(g_pMessaging);
        dfRet = OK;
    }

    return dfRet;
}
