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

    // setup PIC programmer pins. Set to input as we will not use them. (this replaces setting them manually in BIOS)
    pin_vpp = new oddyseyx86GPIO(PIC_PROGRAMMER_PIN_VPP);
    pin_vpp->setPinAsInputElseOutput(true);
    pin_pgc = new oddyseyx86GPIO(PIC_PROGRAMMER_PIN_PGC);
    pin_pgc->setPinAsInputElseOutput(true);
    pin_pgd = new oddyseyx86GPIO(PIC_PROGRAMMER_PIN_PGD);
    pin_pgd->setPinAsInputElseOutput(true);

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
        m_pMessaging->sendMessageOverIP("Init Ready");
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
        productDispensers[idx].initFlowsensorIO(IO_PIN_FLOW_SENSOR, idx);
    }
#else
    productDispensers[0].initFlowsensorIO(17, 0);
#endif

    // Set up the four dispensers
    for (idx = 0; idx < 4; idx++)
    {
        productDispensers[idx].setPump(0, 0, idx);
        productDispensers[idx].loadGeneralProperties();
    }

    productDispensers[0].initButtonsShutdownAndMaintenance(); // todo: this is a hack for the maintenance and power button. It should not be part of the dispenser class

    // #ifdef ENABLE_MULTI_BUTTON

    // needs to be set up only once. Dispener index is only important for the button 4 index.
    if (productDispensers[3].getMultiDispenseButtonEnabled())
    {
        productDispensers[3].initDispenseButton4Light(); // THE DISPENSER SLOT MUST REPRESENT THE BUTTON. It's dirty and I know it.
        productDispensers[3].setAllDispenseButtonLightsOff();
    }
    // #endif

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