//***************************************
//
// statedispenseidle.h
// dispense idle state class
//
// Dispense Idle state while waiting for user to
// Press button to dispense - Flips between
// Dispense and Dispense Idle.
// Receives and interprets string command from FSM.
//
// created: 01-2022
// by:Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "stateDispenseIdle.h"

#define DISPENSE_IDLE_STRING "Dispense Idle"

// Default CTOR
stateDispenseIdle::stateDispenseIdle()
{
}

// CTOR Linked to IPC
stateDispenseIdle::stateDispenseIdle(messageMediator *message)
{
    m_pMessaging = message;
}

// DTOR
stateDispenseIdle::~stateDispenseIdle()
{
    debugOutput::sendMessage("stateDispenseIdle: ~stateDispenseIdle", MSG_INFO);
}

// FIXME: See state dispense function header
DF_ERROR stateDispenseIdle::onEntry()
{
}

DF_ERROR stateDispenseIdle::onAction()
{
   
   
}

// Advances to Dispense End with completed Dispense
DF_ERROR stateDispenseIdle::onExit()
{
}

// Overload for Debugger output
string stateDispenseIdle::toString()
{
    return DISPENSE_IDLE_STRING;
}
