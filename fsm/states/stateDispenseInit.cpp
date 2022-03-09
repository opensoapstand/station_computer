//***************************************
//
// stateDispenseInit.h
// Dispense Init State
//
// created: 01-2022
// by:Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
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

    size = m_pMessaging->getRequestedVolume();
    productDispensers = g_productDispensers;

    dispenser_index = m_pMessaging->getProductNumber() - 1;

    return e_ret;
}

/*
 * 
 */
DF_ERROR stateDispenseInit::onAction()
{

    DF_ERROR e_ret = OK;

    debugOutput::sendMessage("Chosen dispenser slot: " + std::to_string(  productDispensers[dispenser_index].getSlot()), MSG_INFO);
    // debugOutput::sendMessage("Chosen dispenser slot: " + std::to_string(  productDispensers[dispenser_index].getProduct()->getTargetVolume(size)), MSG_INFO);
    // debugOutput::sendMessage("Chosen dispenser slot: " + std::to_string(  productDispensers[dispenser_index].getProduct()->getPrice(size)), MSG_INFO);

    productDispensers[dispenser_index].getProduct()->initDispense(
        productDispensers[dispenser_index].getProduct()->getTargetVolume(size),
        productDispensers[dispenser_index].getProduct()->getPrice(size));

    productDispensers[dispenser_index].getProduct()->productInfo();
    productDispensers[dispenser_index].getProduct()->productVolumeInfo();

    productDispensers[dispenser_index].startDispense();
    m_state_requested = STATE_DISPENSE_IDLE;

    return e_ret;
}

// Actions on leaving Dispense state
DF_ERROR stateDispenseInit::onExit()
{
    DF_ERROR e_ret = OK;

    return e_ret;
}
