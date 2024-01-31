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
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
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
    debugOutput::sendMessage("stateInit: ~stateInit", MSG_INFO);
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
    // DF_ERROR e_ret = ERROR_BAD_PARAMS;

    DF_ERROR e_ret = OK;

    debugOutput::sendMessage("Use database at: " + std::to_string(1) + CONFIG_DB_PATH, MSG_INFO);

    // // setup PIC programmer pins. Set to input as we will not use them. (this replaces setting them manually in BIOS)
    // pin_vpp = new FSModdyseyx86GPIO(PIC_PROGRAMMER_PIN_VPP);
    // pin_vpp->setPinAsInputElseOutput(true);
    // pin_pgc = new FSModdyseyx86GPIO(PIC_PROGRAMMER_PIN_PGC);
    // pin_pgc->setPinAsInputElseOutput(true);
    // pin_pgd = new FSModdyseyx86GPIO(PIC_PROGRAMMER_PIN_PGD);
    // pin_pgd->setPinAsInputElseOutput(true);

    dispenserSetup();

    if (OK == e_ret)
    {
        m_state_requested = STATE_IDLE;

        // The UI program waits for this message to move from its initializing phase to its Idle phase:
        m_pMessaging->sendMessageOverIP("Init Ready", true); // send to UI
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
    // dispenser *productDispensers = g_productDispensers;

    debugOutput::sendMessage("Setting up control board.", MSG_INFO);

    // We only need one flow sensor interrupt pin since only one pump
    // is ever active at a time.  The flow sensors are all connected
    // to the same pin in the hardware.

    g_machine.loadGeneralProperties(true);

    // g_machine.m_productDispensers[0].initButtonsShutdownAndMaintenance(); // todo: this is a hack for the maintenance and power button. It should not be part of the dispenser class

    // needs to be set up only once. Dispenser index is only important for the button 4 index.
    switch (g_machine.control_pcb->get_pcb_version())
    {

    case (pcb::PcbVersion::DSED8344_NO_PIC):
    {
    }

    break;
    case pcb::PcbVersion::DSED8344_PIC_MULTIBUTTON:
    {
    }
    break;
    case (pcb::PcbVersion::EN134_4SLOTS):
    case (pcb::PcbVersion::EN134_8SLOTS):
    {
        // Set up the four dispensers
    }
    break;
    case (pcb::PcbVersion::EN258_4SLOTS):
    case (pcb::PcbVersion::EN258_8SLOTS):
    {

        g_machine.pcb24VPowerSwitch(false);
        for (int slot = 1; slot <= g_machine.getPcb()->getSlotCountByPcbType(); slot++)
        {
            g_machine.getPcb()->setFlowSensorType(slot, pcb::FlowSensorType::DIGMESA);
            //g_machine.getPcb()->setFlowSensorType(slot, pcb::FlowSensorType::AICHI);
        }
    }
    break;
    default:
    {
        debugOutput::sendMessage("stateInit: Pcb not comptible. ", MSG_WARNING);
    }
    break;
    }

    debugOutput::sendMessage("Dispenser intialized.", MSG_INFO);

    return OK;
} // End of dispenserSetup()

/**/
DF_ERROR stateInit::setProducts()
{

    return OK;
}