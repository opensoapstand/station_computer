//***************************************
//
// statevirtual.cpp
// virtual state class
//
// created: 01-2022
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "stateVirtual.h"
#include <iostream>

stateVirtual::stateVirtual()
{
   //m_state = START;
   //m_state_requested = START; // should error if ever on "START", this is only virtual!
}

// stateVirtual::stateVirtual(messageMediator *message)
// {
//    if (nullptr == message)
//    {
//       //bad things have occured
//    }
//    //m_state = START;

//    m_pMessaging = message; // linking of the message sender (TCP/IP)

// }

stateVirtual::~stateVirtual()
{
   //clean up
   debugOutput::sendMessage("stateVirtual: ~stateVirtual", MSG_INFO);

}

//getter for next state from fsm
DF_FSM stateVirtual::getRequestedState()
{
   return m_state_requested;
}

// //getter for current state from fsm
// DF_FSM stateVirtual::getCurrentState()
// {
//    return m_state;
// }

// gpio *stateVirtual::getDispenseButtonState()
// {
//    return m_pButton[NUM_BUTTON];
// }
