//***************************************
//
// stateinit.h
// init state class
//
// Set Objects for FSM
// Check all Hardware is operational.
// Initialize threads for operation and
// communication in machine.
// Connect or create database.
//
// created: 01-2022
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "stateInit.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include "../fsm.h"

#define INIT_STRING "Init"

// Default Ctor
stateInit::stateInit()
{
}

// CTOR Linked to IPC
stateInit::stateInit(messageMediator *message)
{
}

// DTOR
stateInit::~stateInit()
{
    // delete stuff
}

// Overload for Debugger output
string stateInit::toString()
{
    return INIT_STRING;
}

/*
 * Initialize FSM State
 */
DF_ERROR stateInit::onEntry()
{
    m_state_requested = STATE_INIT;
    DF_ERROR e_ret = OK;
    return e_ret;
}

// Initialize all related hardware
DF_ERROR stateInit::onAction()
{
    DF_ERROR e_ret = ERROR_BAD_PARAMS;

    debugOutput::sendMessage("Use database at: " + std::to_string(1) + DB_PATH, MSG_INFO);

    e_ret = setProducts();
    if (OK == e_ret)
    {
        e_ret = dispenserSetup();
    }
    else
    {
        debugOutput::sendMessage("ERROR: Problems setting up the products.", MSG_INFO);
    }

    if (OK == e_ret)
    {
        m_state_requested = STATE_IDLE;

        // The UI program waits for this message to move from its initializing phase to its Idle phase:
        m_pMessaging->sendMessage("Init Ready");
    }
    else
    {

        debugOutput::sendMessage("ERROR: Problems setting up the dispenser.", MSG_INFO);
    }

    return e_ret;
}

DF_ERROR stateInit::onExit()
{
    DF_ERROR e_ret = OK;
    return e_ret;
}

// Initilization function for all dispensers...
DF_ERROR stateInit::dispenserSetup()
{
    int idx;
    dispenser *productDispensers = g_productDispensers;
    debugOutput::sendMessage("Setting up DS-ED-8344 hardware control board.\n", MSG_INFO);

    // We only need one flow sensor interrupt pin since only one pump
    // is ever active at a time.  The flow sensors are all connected
    // to the same pin in the hardware.
#ifndef __arm__
    for (idx = 0; idx < 4; idx++)
    {
        productDispensers[idx].setFlowsensor(IO_PIN_FLOW_SENSOR, idx);
    }
#else
    productDispensers[0].setFlowsensor(17, 0);
#endif

    // Set up the four pumps
    for (idx = 0; idx < 4; idx++)
    {
        productDispensers[idx].setPump(0, 0, idx);
    }

    productDispensers[0].setButtonsShutdownAndMaintenance(); // todo: this is a hack for the maintenance and power button. It should not be part of the dispenser class

    debugOutput::sendMessage("Dispenser intialized.", MSG_INFO);

    return OK;
} // End of dispenserSetup()

/**/
DF_ERROR stateInit::setProducts()
{

    for (int slot_index = 0; slot_index < PRODUCT_DISPENSERS_MAX; slot_index++)
    {
        debugOutput::sendMessage("Setup dispenser " + to_string(slot_index + 1), MSG_INFO);
        g_productDispensers[slot_index].setSlot(slot_index + 1);
        g_productDispensers[slot_index].setProduct(new product(slot_index + 1));
    }
    return OK;
}