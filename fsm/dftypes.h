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

#define XML_SETTINGS  "df_settings.xml"

typedef enum DF_FSM {
   START,
   INIT,
   IDLE,
   PRODUCT_SELECT, //retrieve info from ui which cassettes need to be set
   PAYMENT,        //retrieve info from ui whether or not payment is finished proccessing
   DISPENSE_IDLE,
   DISPENSE,
   DISPENSE_END,
   CLEANING,
   FSM_MAX,
} DF_FSM;


typedef enum DF_ERROR {
   OK = 0,
   ERROR_BAD_PARAMS,
   ERROR_DRINK_FAULT,
   ERROR_WATER_FAULT,
   ERROR_AIR_FAULT,
   ERROR_FPUMP_FAULT,
   ERROR_RPUMP_FAULT,
   ERROR_FS_FAULT,
   ERROR_I2C_ADDRESS_NOT_FOUND,
   ERROR_WRONG_I2C_ADDRESS,
   ERROR_XMLFILE_NOT_FOUND,
   ERROR_XMLFILE_NO_MATCH_CONTENT,
   ERROR_PAYMENT_NOT_FOUND,
   ERROR_PTHREADS
} DF_ERROR;

#endif