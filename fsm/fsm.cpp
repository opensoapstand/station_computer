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

#include "objects/dispenser.h"
#include "objects/messagemediator.h"

messageMediator *g_pMessaging; //debug through local network
stateVirtual *g_stateArray[FSM_MAX];
dispenser *g_dispenseArray[9];   //replace the magic number

DF_ERROR initObjects();
DF_ERROR createStateArray();
DF_ERROR createStateArray(char* inputChar);
DF_ERROR stateLoop();

int main()
{
    DF_ERROR dfRet = OK;

    if (OK != initObjects())
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

    while (OK == dfRet) //while no error has occur
    {
        if (fsmState != fsmNewState) //state change
        {
            //dfRet = g_stateArray[fsmNewState]->onEntry();
            fsmState = fsmNewState;
        }

        if (OK == dfRet) //
        {
            //dfRet = g_stateArray[fsmState]->onAction(&fsmNewState);
            if ((OK == dfRet) &&
                (fsmNewState != fsmState))
            {
                //dfRet = g_stateArray[fsmState]->onExit();
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
    DF_ERROR dfRet = OK;

    if('s' == inputChar[0])
    {
        //int pos = atoi(inputChar);
        for(int i = DF_FSM::START; i < DF_FSM::FSM_MAX; i++)
        {
            g_stateArray[i] = new stateVirtual(i);
        }
        return dfRet;
    }
    else
    {
        dfRet = ERROR_BAD_PARAMS;
        cout << "Invalid input \n";
    }

    return dfRet;
}