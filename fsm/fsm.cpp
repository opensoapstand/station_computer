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
    debugOutput debugInfo;
    DF_ERROR dfRet = OK;
    DF_FSM fsmState = INIT; //!!!change to start once implement
    DF_FSM fsmNewState = DISPESE_IDLE; //!!!change once other state is implemented

    while (OK == dfRet) //while no error has occur
    {
        debugInfo.sendMessage("Current state [" + to_string(fsmState) + "]", INFO);

        if (fsmState != fsmNewState) //state change
        {
            debugInfo.sendMessage("State change", INFO);
            dfRet = g_stateArray[fsmNewState]->onEntry();
            
            fsmState = fsmNewState;
        }

        if (OK == dfRet) //
        {
            debugInfo.sendMessage("New state set [" + to_string(fsmState) + "]", INFO);


            dfRet = g_stateArray[fsmState]->onAction(&fsmNewState);
            if ((OK == dfRet) && (fsmNewState != fsmState))
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

    char inputChar[50];
    //g_pMessaging = new messageMediator();
    do{
        cout << "Enter \"s\" to start: ";
        cin >> inputChar;
        dfRet = createStateArray(inputChar);
    }while(OK != dfRet);

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
        g_stateArray[DF_FSM::INIT] = new stateInit();
        g_stateArray[DF_FSM::DISPESE_IDLE] = new stateDispenseIdle();
        g_stateArray[DF_FSM::DISPENSE] = new stateDispense();

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