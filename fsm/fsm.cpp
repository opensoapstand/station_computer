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

#include "fsm.h"

#include <string.h>
#include "dftypes.h"

#include "states/stateVirtual.h"

#include "states/stateInit.h"
#include "states/stateIdle.h"
#include "states/stateDispenseInit.h"
#include "states/stateDispense.h"
#include "states/stateDispenseIdle.h"
#include "states/stateDispenseEnd.h"
#include "states/stateManualPrinter.h"
#include "states/stateManualPump.h"
#include "states/stateEnd.h"

#include "objects/dispenser.h"
#include "objects/messageMediator.h"

#include "objects/machine.h"

std::string stateStrings[FSM_MAX + 1] = {
    "STATE_DUMMY",
    "STATE_INIT",
    "STATE_IDLE",
    "STATE_DISPENSE_INIT",
    "STATE_DISPENSE_IDLE",
    "STATE_DISPENSE",
    "STATE_DISPENSE_END",
    "STATE_MANUAL_PRINTER",
    "STATE_MANUAL_PUMP",
    "STATE_END",
    "FSM_MAX"};

messageMediator *g_pMessaging;           // debug through local network
stateVirtual *g_stateArray[FSM_MAX + 1]; // an object for every state

dispenser g_productDispensers[PRODUCT_DISPENSERS_MAX];

DF_ERROR initObjects();
DF_ERROR createStateArray();
DF_ERROR stateLoop();

/*
 * Instantiate Array to hold objects specfic to states (DF_FSM)
 * XXX: Reminder to instantiate new states here!
 */
DF_ERROR createStateArray()
{
    DF_ERROR dfRet = ERROR_PTHREADS;

    if (NULL != g_pMessaging)
    {
        g_stateArray[STATE_INIT] = new stateInit(g_pMessaging);
        g_stateArray[STATE_IDLE] = new stateIdle(g_pMessaging);
        g_stateArray[STATE_DISPENSE_INIT] = new stateDispenseInit(g_pMessaging);
        g_stateArray[STATE_DISPENSE_IDLE] = new stateDispenseIdle(g_pMessaging);
        g_stateArray[STATE_DISPENSE] = new stateDispense(g_pMessaging);
        g_stateArray[STATE_DISPENSE_END] = new stateDispenseEnd(g_pMessaging);
        g_stateArray[STATE_MANUAL_PRINTER] = new stateManualPrinter(g_pMessaging);
        g_stateArray[STATE_MANUAL_PUMP] = new stateManualPump(g_pMessaging);
        g_stateArray[STATE_END] = new stateEnd(g_pMessaging);
        dfRet = OK;
    }

    return dfRet;
}

int main()
{
    pthread_t ipThread, kbThread;

    DF_ERROR dfRet = OK;

    // __cplusplus  // uncomment and hover over it to check the cpp version used. https://stackoverflow.com/questions/2324658/how-to-determine-the-version-of-the-c-standard-used-by-the-compiler
    
    std::string version = CONTROLLER_VERSION;
    debugOutput::sendMessage("***************************************************************************", MSG_INFO);
    debugOutput::sendMessage("****** SOAPSTAND CONTROLLER v" + version + " ***********************************", MSG_INFO);
    debugOutput::sendMessage("***************************************************************************", MSG_INFO);

    machine test;
    test.testtest();

    if (OK == initObjects())
    {
        debugOutput::sendMessage("Init objects done", MSG_INFO);
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
    DF_FSM fsmState = STATE_INIT;
    DF_FSM previousState = STATE_DUMMY;

    while (OK == dfRet) // while no error has occurred
    {
        if (fsmState == STATE_DUMMY)
        {
            debugOutput::sendMessage("ERROR STATE " + fsmState, MSG_STATE);
        }

        // state change, deal with new state
        if (fsmState != previousState)
        {
            debugOutput::sendMessage("Enter state: " + stateStrings[fsmState], MSG_STATE);
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
                debugOutput::sendMessage("Exit state: " + stateStrings[previousState], MSG_STATE);
                dfRet = g_stateArray[previousState]->onExit();
            }
        }

        if (fsmState == STATE_END)
        {
            dfRet = ERROR_END;
        }

        
        usleep(1000); // micros! 1 ms delay to slow down refresh rate
    }
    debugOutput::sendMessage("State machine ENDED. ", MSG_INFO);
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

    for (int i = 0; i < PRODUCT_DISPENSERS_MAX; i++)
    {
        g_productDispensers[i].setup();
    }
    debugOutput::sendMessage("Dispensers set up. ", MSG_INFO);

    dfRet = createStateArray();
    if (OK != dfRet)
    {
        debugOutput::sendMessage("Error at set up.", MSG_ERROR);
        // TODO: DB function to check/create DB
        // next
    }
    return dfRet;
}
