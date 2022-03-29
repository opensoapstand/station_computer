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
    // m_state_requested = STATE_DISPENSE_IDLE;
    // DF_ERROR e_ret = OK;

    // productDispensers = g_productDispensers;

    // pos = m_pMessaging->getRequestedSlot();
    // pos = pos - 1;
    // size = m_pMessaging->getRequestedSize();
    // return e_ret;
}

DF_ERROR stateDispenseIdle::onAction()
{
    // DF_ERROR df_ret = ERROR_BAD_PARAMS;

    // // Idles after proper initilization;  Waits for a command from messageMediator or dispense action.

    // if (m_pMessaging->isCommandStringReadyToBeParsed())
    // {
    //     m_pMessaging->parseCommandString();

    //     // Check if UI has sent a ACTION_DISPENSE_END to compelte the transaction, or, the taget has been hit, to enter into the DispenseEnd state
    //     if (ACTION_DISPENSE_END == m_pMessaging->getAction())
    //     {
    //         // let's do things in one state!
    //         m_state_requested = STATE_DISPENSE;
    //         return df_ret = OK;
    //     }
    // }

    // if (productDispensers[pos].getIsDispenseTargetReached())
    // {
    //     // let's do things in one state!
    //     debugOutput::sendMessage("Weird that this happened here.", MSG_INFO);
    //     m_state_requested = STATE_DISPENSE;
    //     return df_ret = OK;
    // }

    // // If volume has not changed, stay in Idle state, else, volume is changing, go to Dispense state...
    // if (productDispensers[pos].getProduct()->getVolumeDispensed() == productDispensers[pos].getProduct()->getVolumeDispensedPreviously())
    // {
    //     debugOutput::sendMessage("Wait for volume to change to go to dispensing state" + to_string(productDispensers[pos].getProduct()->getVolumeDispensedPreviously()) + " --- " +
    //                                  to_string(productDispensers[pos].getProduct()->getVolumeDispensed()),
    //                              MSG_INFO);
    // }
    // else
    // {
    //     debugOutput::sendMessage("DO STATE CHANGE!!!!volume change detected, go to dispense state." + to_string(productDispensers[pos].getProduct()->getVolumeDispensedPreviously()) + " --- " +
    //                                  to_string(productDispensers[pos].getProduct()->getVolumeDispensed()),
    //                              MSG_INFO);

    //     productDispensers[pos].getProduct()->m_nVolumeDispensedPreviously = productDispensers[pos].getProduct()->getVolumeDispensed();

    //     m_state_requested = STATE_DISPENSE;
    // }

    // usleep(500000);
    // df_ret = OK;

    // return df_ret;
}

// Advances to Dispense End with completed Dispense
DF_ERROR stateDispenseIdle::onExit()
{
    // // TODO: NO STATE CHANGES ON EXIT PLEAAAASE
    // DF_ERROR e_ret = OK;
    // return e_ret;
}

// Overload for Debugger output
string stateDispenseIdle::toString()
{
    return DISPENSE_IDLE_STRING;
}
