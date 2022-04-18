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

    productDispensers = g_productDispensers;

    size = m_pMessaging->getRequestedSize();
    dispenser_index = m_pMessaging->getRequestedSlot() - 1;

    debugOutput::sendMessage("dispense init, we (re)load parameters from database.", MSG_INFO);

    bool success = this->productDispensers[dispenser_index].getProduct()->reloadParametersFromDb();
    if (!success)
    {
        debugOutput::sendMessage("Did not reload parameters from database", MSG_INFO);
    }
    
    return e_ret;
}

/*
 *
 */
DF_ERROR stateDispenseInit::onAction()
{

    DF_ERROR e_ret = OK;
debugOutput::sendMessage("..............111111111111", MSG_INFO);
    this->productDispensers[dispenser_index].loadEmptyContainerDetectionEnabledFromDb();

    debugOutput::sendMessage("Chosen dispenser slot: " +
                                 std::to_string(productDispensers[dispenser_index].getSlot()) +
                                 " target volume: " +
                                 std::to_string(productDispensers[dispenser_index].getProduct()->getTargetVolume(size)),
                             MSG_INFO);
                             debugOutput::sendMessage("22222222222", MSG_INFO);
    // debugOutput::sendMessage("+++++++++++++++++++++++++++++++++++ " + std::to_string( m_selectedOrder->size) + "sloottt: " + std::to_string( m_selectedOrder->slot), MSG_INFO);
    //debugOutput::sendMessage("Chosen dispenser slot: " + std::to_string(  productDispensers[dispenser_index].getProduct()->getTargetVolume(size)), MSG_INFO);
    // debugOutput::sendMessage("Chosen dispenser slot: " + std::to_string(  productDispensers[dispenser_index].getProduct()->getPrice(size)), MSG_INFO);

    productDispensers[dispenser_index].initDispense(
        productDispensers[dispenser_index].getProduct()->getTargetVolume(size)
        ,productDispensers[dispenser_index].getProduct()->getPrice(size)
        );

    productDispensers[dispenser_index].getProduct()->productInfo();
    productDispensers[dispenser_index].getProduct()->productVolumeInfo();

    productDispensers[dispenser_index].startDispense();
    
    m_state_requested = STATE_DISPENSE;
    return e_ret;
}

// Actions on leaving Dispense state
DF_ERROR stateDispenseInit::onExit()
{
    DF_ERROR e_ret = OK;

    return e_ret;
}
