//***************************************
//
// statedispenseidle.h
// dispense idle state class
//
// Dispense Idle state while waiting for user to
// Press button to dispense - Flips between
// Dispense and Dispense Idle.
// Receives and interprets string command from FSM.
//
// created: 01-2022
// by:Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "stateDispenseIdle.h"

#define DISPENSE_IDLE_STRING "Dispense Idle"

// Default CTOR
stateDispenseIdle::stateDispenseIdle()
{
}

// CTOR Linked to IPC
stateDispenseIdle::stateDispenseIdle(messageMediator *message)
{
}

// DTOR
stateDispenseIdle::~stateDispenseIdle()
{
}

// FIXME: See state dispense function header
DF_ERROR stateDispenseIdle::onEntry()
{
    m_state_requested = DISPENSE_IDLE;
    DF_ERROR e_ret = OK;

    productDispensers = g_productDispensers;
    pos = m_pMessaging->getProductNumber();
    pos = pos - 1;
    size = m_pMessaging->getRequestedVolume();

    productDispensers[pos].setIsDispenseComplete(false);

    if (productDispensers[pos].getProduct()->getVolumeDispensed() == 0)
    {
        // TODO this should be a separate state (dispense_init)

        productDispensers[pos].getProduct()->startDispense(productDispensers[pos].getProduct()->getTargetVolume(size), productDispensers[pos].getProduct()->getPrice(size));
        productDispensers[pos].setIsDispenseComplete(false);
        productDispensers[pos].getProduct()->productInfo();
        productDispensers[pos].getProduct()->productVolumeInfo();
        productDispensers[pos].startDispense(productDispensers[pos].getProduct()->getProductOption());
    }

    return e_ret;
}

// Idles after proper initilization;  Waits for a command from messageMediator
DF_ERROR stateDispenseIdle::onAction()
{
    DF_ERROR df_ret = ERROR_BAD_PARAMS;

    if (nullptr != &m_state_requested)
    {

        if (m_pMessaging->isCommandStringReadyToBeParsed())
        {
            m_pMessaging->parseCommandString();

            // Check if UI has sent a ACTION_DISPENSE_END to compelte the transaction, or, the taget has been hit, to enter into the DispenseEnd state
            if (ACTION_DISPENSE_END == m_pMessaging->getAction())
            {

                m_state_requested = DISPENSE_END;
                return df_ret = OK;
            }
        }

        if (productDispensers[pos].getIsDispenseComplete())
        {

            m_state_requested = DISPENSE_END;
            return df_ret = OK;
        }

        productDispensers[pos].getProduct()->productVolumeInfo();

        // If volume has not changed, stay in Idle state, else, volume is changing, go to Dispense state...
        if (productDispensers[pos].getProduct()->getVolumeDispensed() == productDispensers[pos].getProduct()->getVolumeDispensedPreviously())
        {
            debugOutput::sendMessage("Wait for volume to change to go to dispensing state", INFO);
            m_state_requested = DISPENSE_IDLE;
        }
        else
        {
            productDispensers[pos].getProduct()->m_nVolumeDispensedPreviously = productDispensers[pos].getProduct()->getVolumeDispensed();
            m_state_requested = DISPENSE;
        }
        usleep(500000);
        df_ret = OK;
    }

    return df_ret;
}

// Advances to Dispense End with completed Dispense
DF_ERROR stateDispenseIdle::onExit()
{

    // TODO: NO STATE CHANGES ON EXIT PLEAAAASE
    DF_ERROR e_ret = OK;

    if ((m_pMessaging->getAction() == ACTION_DISPENSE_END) || (productDispensers[pos].getIsDispenseComplete()))
    {
        debugOutput::sendMessage("Dispensing done.", INFO);
        m_state_requested = DISPENSE_END;
    }

    return e_ret;
}

// Overload for Debugger output
string stateDispenseIdle::toString()
{
    return DISPENSE_IDLE_STRING;
}
