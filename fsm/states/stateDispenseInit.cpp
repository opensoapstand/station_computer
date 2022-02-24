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

    pos = m_pMessaging->getProductNumber();
    pos = pos - 1;

    return e_ret;
}

/*
 * 
 */
DF_ERROR stateDispenseInit::onAction()
{

    DF_ERROR e_ret = OK;

    debugOutput::sendMessage("Chosen product: " + std::to_string(pos), MSG_INFO);

    // DO THIS IN A STATE DISPENSE_INIT
    // TODO this should be a separate state (dispense_init)

    productDispensers[pos].getProduct()->initDispense(
        productDispensers[pos].getProduct()->getTargetVolume(size),
        productDispensers[pos].getProduct()->getPrice(size));

    // productDispensers[pos].setIsDispenseComplete(false);

    productDispensers[pos].getProduct()->productInfo();
    productDispensers[pos].getProduct()->productVolumeInfo();

    // productDispensers[pos].startDispense(
    //     productDispensers[pos].getProduct()->getProductOption());
    productDispensers[pos].startDispense();
    m_state_requested = STATE_DISPENSE_IDLE;

    return e_ret;
}

// Actions on leaving Dispense state
DF_ERROR stateDispenseInit::onExit()
{
    DF_ERROR e_ret = OK;

    return e_ret;
}
