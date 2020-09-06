//***************************************
//
// drink.cpp
// Drink (Model) class Implementation:
//
// Owns current beverage use/transaction.
// Holds and coordinates values from database
// to dispense classes.
//
// created: 26-06-2020
// by: Jason Wang & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************
#include "drink.h"

// Default CTOR
drink::drink()
{

}

// CTOR with Option Slot
// TODO: Should call this from Message Mediator
//       And figure out storage/reference location...
drink::drink(int slot)
{

}

// DTOR
drink::~drink()
{

}

// Set the Drink option slot
void drink::setSlot(int slot)
{
    m_nSlot = slot;
}

// Set the Drink Name
// TODO: Redefine function prototype, no argument.
void drink::setDrinkName(string drinkName)
{
    // TODO: SQLite database Query could be better option.
}

// Get the remaining volume
int drink::getVolumeRemaining()
{
    // TODO: SQLite database Query could be better option.
}

// TODO: Function name is inaccurate...deduct sale would be better
void drink::recordSale(int volume)
{
    // TODO: SQLite database Update.
}

// TODO: This function could live somewhere else...linked to future maintenance.
void drink::refill(int volume)
{
    // TODO: SQLite database Update.
}