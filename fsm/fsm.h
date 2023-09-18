//***************************************
//
// fsm.h
// holds some globals for sharing
//
// created: 22-09-2020
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef FSM__H_
#define FSM__H_
#include "objects/dispenser.h"
#include "objects/machine.h"

extern dispenser g_productDispensers[PRODUCT_DISPENSERS_MAX];
extern machine g_machine;

#endif