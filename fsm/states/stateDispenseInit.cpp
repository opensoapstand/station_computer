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

    size = m_pMessaging->getRequestedSize();
    if (size == SIZE_DUMMY){
        debugOutput::sendMessage("ASSERT ERROR: Size not set. Will not continue.",  MSG_ERROR);
        m_state_requested = STATE_IDLE;
    }

    dispenser_index = m_pMessaging->getRequestedSlot() - 1;

    if (dispenser_index == PRODUCT_SLOT_DUMMY){
        debugOutput::sendMessage("ASSERT ERROR: Slot number not set. Will not continue.",  MSG_ERROR);
        m_state_requested = STATE_IDLE;
    }

    debugOutput::sendMessage("Dispense init: (re)load relevant parameters from database.", MSG_INFO);

    g_machine.loadGeneralProperties(false);
    g_machine.m_productDispensers[dispenser_index].loadGeneralProperties();

    debugOutput::sendMessage("Dispense init: Load selected product parameters: ", MSG_INFO);
    bool success = g_machine.m_productDispensers[dispenser_index].getSelectedProduct()->loadParameters();

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

    g_machine.m_productDispensers[dispenser_index].getSelectedProduct()->setTargetVolumeFromSize(size);

    debugOutput::sendMessage("Chosen dispenser slot: " +
                                 std::to_string(g_machine.m_productDispensers[dispenser_index].getSlot()) +
                                 " target volume: " +
                                 std::to_string(g_machine.m_productDispensers[dispenser_index].getSelectedProduct()->getTargetVolume()),
                             MSG_INFO);
    debugOutput::sendMessage(std::string("Dispenser slot state: ") + g_machine.m_productDispensers[dispenser_index].getSlotStateAsString(),
                             MSG_INFO);

    g_machine.m_productDispensers[dispenser_index].startSelectedProductDispense(
        size,
        g_machine.m_productDispensers[dispenser_index].getSelectedProduct()->getPrice(size)
        );

    
    
    m_state_requested = STATE_DISPENSE;
    return e_ret;
}

// Actions on leaving Dispense state
DF_ERROR stateDispenseInit::onExit()
{
    DF_ERROR e_ret = OK;

    return e_ret;
}
