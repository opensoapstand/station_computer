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

#include "fsm.h"

#include "states/stateVirtual.h"

#include "states/stateInit.h"
#include "states/stateIdle.h"
#include "states/stateDispense.h"
#include "states/stateDispenseIdle.h"
#include "states/stateDispenseEnd.h"

#include "objects/dispenser.h"
#include "objects/messageMediator.h"

std::string stateStrings[10] = {
    "START",
    "INIT",
    "IDLE",
    "PRODUCT_SELECT",
    "PAYMENT",
    "DISPENSE_IDLE",
    "DISPENSE",
    "DISPENSE_END",
    "CLEANING",
    "FSM_MAX"};

messageMediator *g_pMessaging;                         //debug through local network
stateVirtual *g_stateArray[FSM_MAX];                   //an object for every state
dispenser g_productDispensers[PRODUCT_DISPENSERS_MAX]; //replace the magic number

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

        if (OK == dfRet)
        {
            dfRet = stateLoop();
        }
    }

    return dfRet;
}

/*
 * Poll for State changes: States outlined in DF_FSM
*/
DF_ERROR stateLoop()
{
    DF_ERROR dfRet = OK;
    DF_FSM fsmState = START;
    DF_FSM fsmNewState = INIT;

    while (OK == dfRet) //while no error has occurred
    {

        // state change, deal with new state
        if (fsmState != fsmNewState)
        {

            // debugOutput::sendMessage( "coming from:" + g_stateArray[fsmState]->toString(), STATE_CHANGE);
            // debugOutput::sendMessage( "new state:" + g_stateArray[fsmNewState]->toString(), STATE_CHANGE);
            debugOutput::sendMessage("coming from: " + stateStrings[fsmState], STATE_CHANGE);
            debugOutput::sendMessage("new state: " + stateStrings[fsmNewState], STATE_CHANGE);
            //debugOutput::sendMessage( "coming from:" + g_stateArray[fsmState]->toString() + "onEntry() new state:  [" + g_stateArray[fsmNewState]->toString() + "]", STATE_CHANGE);

            // debugOutput::sendMessage( "coming from: onEntry() new state:  [" + g_stateArray[fsmNewState]->toString() + "]", STATE_CHANGE);
            fsmState = fsmNewState;
            dfRet = g_stateArray[fsmState]->onEntry();
        }

        // Should Poll for Idle state change.  This triggers FSM to go forward.
        if (OK == dfRet)
        {
            dfRet = g_stateArray[fsmState]->onAction();
            // TODO if this isn't OK deal with bad things

            fsmNewState = g_stateArray[fsmState]->getNextState();

            // deal with end of state if state changed
            if ((OK == dfRet) && (fsmNewState != fsmState))
            {
                // debugOutput::sendMessage("coming from:" + g_stateArray[fsmState]->toString() + "onExit() going to [" + g_stateArray[fsmNewState]->toString() + "]", STATE_CHANGE);
                dfRet = g_stateArray[fsmState]->onExit();
                //TODO if this isn't ok deal with bad things
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
 * XXX: Reminder to instantiate new states here!
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
        g_stateArray[DISPENSE_END] = new stateDispenseEnd(g_pMessaging);
        dfRet = OK;
    }

    return dfRet;
}
