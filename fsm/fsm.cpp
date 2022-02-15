//***************************************
//
// fsm.cpp
// main state class
//
// created: 01-2022
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
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
#include "states/stateManualPrinter.h"

#include "objects/dispenser.h"
#include "objects/messageMediator.h"

std::string stateStrings[FSM_MAX + 1] = {
    "DUMMY",
    "INIT",
    "IDLE",
    "PRODUCT_SELECT",
    "PAYMENT",
    "DISPENSE_IDLE",
    "DISPENSE",
    "DISPENSE_END",
    "MANUAL_PRINTER",
    "END",
    "FSM_MAX"};

messageMediator *g_pMessaging;                         //debug through local network
stateVirtual *g_stateArray[FSM_MAX + 1];               //an object for every state
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
    DF_FSM fsmState = INIT;
    DF_FSM previousState = DUMMY;

    while (OK == dfRet) //while no error has occurred
    {
        if (fsmState == DUMMY)
        {
            debugOutput::sendMessage("ERROR STATE " + fsmState, STATE_CHANGE);
        }

        // state change, deal with new state
        if (fsmState != previousState)
        {
            debugOutput::sendMessage("Enter state: " + stateStrings[fsmState], STATE_CHANGE);
            dfRet = g_stateArray[fsmState]->onEntry();
        }

        previousState = fsmState;

        // state not changed
        if (OK == dfRet)
        {
            dfRet = g_stateArray[fsmState]->onAction();
        }

        // deal with end of state if state changed
        if (OK == dfRet)
        {
            fsmState = g_stateArray[fsmState]->getRequestedState();

            if (fsmState != previousState)
            {
                debugOutput::sendMessage("Exit state: " + stateStrings[previousState], STATE_CHANGE);
                dfRet = g_stateArray[previousState]->onExit();
            }
        }

        if (fsmState == END){
            dfRet = ERROR_END;
        }
    }
    debugOutput::sendMessage("State machine ENDED. ", INFO);
    return dfRet;
}

// DF_ERROR stateLoop()
// {
//     DF_ERROR dfRet = OK;
//     DF_FSM fsmState = INIT;
//     // DF_FSM fsmRequestedState = INIT;
//     // DF_FSM fsmState = START;
//     // DF_FSM fsmRequestedState = INIT;
//     DF_FSM previousState = START;

//     while (OK == dfRet) //while no error has occurred
//     {

//         //if (fsmState != START){
//         //     fsmRequestedState = g_stateArray[fsmState]->getRequestedState();
//         // }else{
//         //     fsmRequestedState = INIT;
//         // }

//         // state change, deal with new state
//         if (fsmState != fsmRequestedState)
//         {
//             debugOutput::sendMessage("coming from: " + stateStrings[fsmState], STATE_CHANGE);
//             debugOutput::sendMessage("new state: " + stateStrings[fsmRequestedState], STATE_CHANGE);
//             fsmState = fsmRequestedState;
//             dfRet = g_stateArray[fsmState]->onEntry();
//         }

//         // state not changed
//         if (OK == dfRet)
//         {
//             dfRet = g_stateArray[fsmState]->onAction();
//             fsmRequestedState = g_stateArray[fsmState]->getRequestedState();
//             // debugOutput::sendMessage("tmplodee", INFO);
//             // debugOutput::sendMessage( std::to_string(fsmRequestedState), INFO);
//             // deal with end of state if state changed
//             if ((OK == dfRet) && (fsmRequestedState != fsmState))
//             {
//                 dfRet = g_stateArray[fsmState]->onExit();
//             }
//         }
//     }
//     debugOutput::sendMessage("Problem with state machine", INFO);
//     return dfRet;
// }

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
        g_stateArray[MANUAL_PRINTER] = new stateManualPrinter(g_pMessaging);
        dfRet = OK;
    }

    return dfRet;
}
