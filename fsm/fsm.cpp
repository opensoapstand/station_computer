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
dispenser *g_dispenseArray[9];   //replace the magic number

DF_ERROR initObjects();
DF_ERROR createStateArray();
DF_ERROR createStateArray(char* inputChar); 
DF_ERROR stateLoop();

int main()
{
    DF_ERROR dfRet = OK;

    if (OK == initObjects())
    {
        dfRet = stateLoop();
    }

    return dfRet;
}

DF_ERROR stateLoop()
{
    DF_ERROR dfRet = OK;
    DF_FSM fsmState = START; 
    DF_FSM fsmNewState = INIT; 

    while (OK == dfRet) //while no error has occur
    {
        if (fsmState != fsmNewState) //state change
        {
            debugOutput::sendMessage("onEntry() [" + to_string(fsmNewState) + "]", INFO);
            dfRet = g_stateArray[fsmNewState]->onEntry();
            fsmState = g_stateArray[fsmNewState]->getCurrentState();
        }

        if (OK == dfRet) 
        {
           debugOutput::sendMessage("onAction() [" + to_string(fsmState) + "]", INFO);
           dfRet = g_stateArray[fsmState]->onAction();

            fsmNewState = g_stateArray[fsmState]->getNextState();

            if ((OK == dfRet) && (fsmNewState != fsmState))
            {
                debugOutput::sendMessage("onExit() [" + to_string(fsmState) + "]", INFO);
                dfRet = g_stateArray[fsmState]->onExit();

                fsmNewState = g_stateArray[fsmState]->getNextState(); //update tge state
            }
        }

    }

    return dfRet;
}

DF_ERROR initObjects()
{
    DF_ERROR dfRet = OK;

    char inputChar[] = {'s'};
    //g_pMessaging = new messageMediator();
    //do{
    //    cout << "Enter \"s\" to start: ";
    //    cin >> inputChar;
        dfRet = createStateArray(inputChar);
    //}while(OK != dfRet);

    //dfRet = createStateArray();
    //if (OK != dfRet)
    //{
        //next 
    //}

    return dfRet;
}

DF_ERROR createStateArray()
{
    DF_ERROR dfRet = OK;

    //g_stateArray[INIT] = new stateInit(g_pMessaging);

    return dfRet;
}

DF_ERROR createStateArray(char* inputChar)
{
    debugOutput debugInfo;
    DF_ERROR dfRet = OK;

    if('s' == inputChar[0])
    {
        /*for(int i = DF_FSM::START; i < DF_FSM::FSM_MAX; i++)
        {
            //g_stateArray[i] = new stateVirtual(i);
            g_stateArray[i] = new stateInit(i);
        }*/

        //creating an object for each state
        g_stateArray[INIT] = new stateInit(); //1
        g_stateArray[IDLE] = new stateIdle(); //2
        g_stateArray[DISPENSE_IDLE] = new stateDispenseIdle();
        g_stateArray[DISPENSE] = new stateDispense(); //6

        g_dispenseArray[0] = new dispenser();
        return dfRet;
    }
    else
    {
        dfRet = ERROR_BAD_PARAMS;

        debugInfo.sendMessage("Invalid input", MESSAGE_LEVEL::ERROR);
    }

    return dfRet;
}