//***************************************
//
// fsm.h
// holds some globals for sharing
//
// created: 22-09-2020
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef FSM__H_
#define FSM__H_

#include "objects/product.h"
#include "objects/machine.h"

// extern dispenser g_g_machine.m_productDDDDDispensers[PRODUCT_DISPENSERS_MAX];
extern product g_pnumbers[PNUMBERS_COUNT];
extern machine g_machine;

#endif