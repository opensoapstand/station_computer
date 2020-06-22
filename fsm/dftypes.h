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
   PRODUCT_SELECT,
   PAYMENT,
   DISPENSE,
   CLEANING,
   FSM_MAX,
} DF_FSM;


typedef enum DF_ERROR {
   OK = 0,
   ERROR_BAD_PARAMS,
   ERROR_FS_FAULT,
   ERROR_I2C_ADDRESS_NOT_FOUND,
   ERROR_PAYMENT_NOT_FOUND
} DF_ERROR;

#endif