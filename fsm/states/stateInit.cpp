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
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
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
    m_pMessaging = message;
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

    debugOutput::sendMessage("Use database at: " + std::to_string(1) + CONFIG_DB_PATH, MSG_INFO);

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
        debugOutput::sendMessage("ERROR: Problems setting up the products.", MSG_ERROR);
    }

    if (OK == e_ret)
    {
        m_state_requested = STATE_IDLE;

        // The UI program waits for this message to move from its initializing phase to its Idle phase:
        m_pMessaging->sendMessageOverIP("Init Ready");
        debugOutput::sendMessage("Finished controller init.", MSG_INFO);
    }
    else
    {

        debugOutput::sendMessage("ERROR: Problems setting up the dispenser.", MSG_ERROR);
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
    debugOutput::sendMessage("Setting up control board.", MSG_INFO);

    // We only need one flow sensor interrupt pin since only one pump
    // is ever active at a time.  The flow sensors are all connected
    // to the same pin in the hardware.
#ifndef __arm__
    for (idx = 0; idx < 4; idx++)
    {
        productDispensers[idx].initGlobalFlowsensorIO(IO_PIN_FLOW_SENSOR, idx);
    }
#else
    productDispensers[0].initGlobalFlowsensorIO(17, 0);
#endif

    // Set up the four dispensers
    for (idx = 0; idx < 4; idx++)
    {
        productDispensers[idx].setPump(0, 0, idx);
        productDispensers[idx].loadGeneralProperties();
    }
    g_machine.loadGeneralProperties();

    // productDispensers[0].initButtonsShutdownAndMaintenance(); // todo: this is a hack for the maintenance and power button. It should not be part of the dispenser class

    // needs to be set up only once. Dispenser index is only important for the button 4 index.
    
    if (g_machine.control_pcb->get_pcb_version() == pcb::PcbVersion::DSED8344_PIC_MULTIBUTTON )  //&& this->slot == 4
    {
        if (g_machine.getMultiDispenseButtonEnabled())
        {
            productDispensers[3].initDispenseButton4Light(); // THE DISPENSER SLOT MUST REPRESENT THE BUTTON. It's dirty and I know it.
            productDispensers[3].setAllDispenseButtonLightsOff();
        }
    }
    else if (g_machine.control_pcb->get_pcb_version() == pcb::PcbVersion::EN134_4SLOTS || g_machine.control_pcb->get_pcb_version() == pcb::PcbVersion::EN134_8SLOTS  )
    {
        // debugOutput::sendMessage(" Enable 24V", MSG_INFO);
        // g_machine.pcb24VPowerSwitch(true);
        g_machine.pcb24VPowerSwitch(false);
        


    }else{
        debugOutput::sendMessage(" Unknown PCB (OLD?).", MSG_ERROR);
    }

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