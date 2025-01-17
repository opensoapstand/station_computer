//***************************************
//
// stateDispenseInit.h
// Dispense Init State
//
// created: 01-2022
// by:Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "stateDispenseInit.h"

#define DISPENSE_INIT_STRING "Dispense Init"

// CTOR
stateDispenseInit::stateDispenseInit()
{
}

// CTOR Linked to IPC
stateDispenseInit::stateDispenseInit(messageMediator *message)
{
    m_pMessaging = message;
}

// DTOR
stateDispenseInit::~stateDispenseInit()
{
    debugOutput::sendMessage("stateDispenseInit: ~stateDispenseInit", MSG_INFO);
}

// Overload for Debugger output
string stateDispenseInit::toString()
{
    return DISPENSE_INIT_STRING;
}

/*
 * Called from FSM loop to check state before inAction execution
 * TODO: Create State Class to hold this function definition
 */
DF_ERROR stateDispenseInit::onEntry()
{
    m_state_requested = STATE_DISPENSE_INIT;
    DF_ERROR e_ret = OK;

    // size = m_pMessaging->getRequestedSize();
    size = g_machine.getSelectedDispenser().getSelectedProduct()->getTargetVolumeAsChar();

    if (size == SIZE_DUMMY)
    {
        debugOutput::sendMessage("ASSERT ERROR: Size not set. Will not continue.", MSG_ERROR);
        m_state_requested = STATE_IDLE;
    }

    // dispenser_index = m_pMessaging->getRequestedSlot() - 1;

    if (g_machine.getSelectedDispenserNumber() == PRODUCT_SLOT_DUMMY)
    {
        debugOutput::sendMessage("ASSERT ERROR: Slot number not set. Will not continue.", MSG_ERROR);
        m_state_requested = STATE_IDLE;
    }

    debugOutput::sendMessage("Dispense init: ", MSG_INFO);

    // debugOutput::sendMessage("Dispense init: (re)load relevant parameters from database.", MSG_INFO);
    // g_machine.loadGeneralMachineProperties(false);
    // g_machine.getSelectedDispenser().loadGeneralProperties();

    debugOutput::sendMessage("Dispense init: Load selected product parameters. Slot: " + to_string(g_machine.getSelectedDispenserNumber()) + " Product: " + to_string(g_machine.getSelectedDispenser().getSelectedPNumber()), MSG_INFO);
    // debugOutput::sendMessage("Dispense init: Load selected product parameters66666. Slot: " + to_string(g_machine.getSelectedDispenserNumber()) + " Product: " + to_string(g_machine.getSelectedDispenser().getSelectedProduct()->getPNumber()), MSG_INFO);
    bool success = g_machine.getSelectedDispenser().getSelectedProduct()->loadParameters(false);
    g_machine.getSelectedDispenser().resetMixDispenseReport();
    if (!success)
    {
        debugOutput::sendMessage("Did not reload parameters from database", MSG_INFO);
    }

    g_machine.pcb24VPowerSwitch(true);

    return e_ret;
}

DF_ERROR stateDispenseInit::onAction()
{

    DF_ERROR e_ret = OK;

    // g_machine.getSelectedDispenser().getSelectedProduct()->setTargetVolumeFromSize(size);

    g_machine.getSelectedDispenser().initSelectedProductDispense(size);

    debugOutput::sendMessage("Chosen dispenser slot: " + std::to_string(g_machine.getSelectedDispenserNumber()) +
                                 " target volume: " + std::to_string(g_machine.getSelectedDispenser().getSelectedProduct()->getTargetVolume()),
                             MSG_INFO);

    m_state_requested = STATE_DISPENSE;
    return e_ret;
}

// Actions on leaving Dispense state
DF_ERROR stateDispenseInit::onExit()
{
    DF_ERROR e_ret = OK;

    return e_ret;
}
