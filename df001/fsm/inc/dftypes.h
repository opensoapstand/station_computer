//***************************************
//
// dftypes.h
// types specific to drinkfill
//
// created: 12-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef DFTYPES__H_
#define DFTYPES__H_

typedef enum DF_FSM {
   START,
   INIT,
   IDLE,
   FSM_MAX,
} DF_FSM;


typedef enum DF_ERROR {
   OK = 0,
   ERROR_BAD_PARAMS,
} DF_ERROR;

#endif