//***************************************
//
// stateEnd.cpp
// end state class
//
// After stateInit: Waits for a dispense
// command from IPC QT Socket
//
// created: 01-2022
// by:Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "stateEnd.h"

#define END_STRING "End"

// Default CTOR
stateEnd::stateEnd()
{
}

// CTOR Linked to IP Thread Socket Listener
stateEnd::stateEnd(messageMediator *message)
{
}

// DTOR
stateEnd::~stateEnd()
{
   // delete stuff
}

// Overload for Debugger output
string stateEnd::toString()
{
   return END_STRING;
}

DF_ERROR stateEnd::onEntry()
{
   // dummy, will never be reached
   m_state_requested = STATE_END;
   DF_ERROR e_ret = OK;
   return e_ret;
}

DF_ERROR stateEnd::onAction()
{
   // dummy, will never be reached
   DF_ERROR e_ret = OK;
   return e_ret;
}

DF_ERROR stateEnd::onExit()
{
   // dummy, will never be reached
   DF_ERROR e_ret = OK;
   return e_ret;
}
