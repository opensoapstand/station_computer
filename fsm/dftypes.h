//***************************************
//
// dftypes.h
// types specific to drinkfill
//
// created: 12-06-2020
// by: Denis Londry
//
// copyright 2020 by Productfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef DFTYPES__H_
#define DFTYPES__H_

#define XML_SETTINGS "/release/df_settings.xml"

#include <sqlite3.h>

#define PRODUCT_DISPENSERS_MAX 4
#define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"

typedef enum DF_FSM
{
   START,
   INIT,
   IDLE,
   PRODUCT_SELECT, //retrieve info from ui which productDispensers need to be set
   PAYMENT,        //retrieve info from ui whether or not payment is finished proccessing
   DISPENSE_IDLE,
   DISPENSE,
   DISPENSE_END,
   CLEANING,
   FSM_MAX,
} DF_FSM;

typedef enum DF_ERROR
{
   OK = 0,
   ERROR_BAD_PARAMS,
   ERROR_MECH = 10,
   ERROR_MECH_PRODUCT_FAULT,
   ERROR_MECH_WATER_FAULT,
   ERROR_MECH_AIR_FAULT,
   ERROR_MECH_FPUMP_FAULT,
   ERROR_MECH_RPUMP_FAULT,
   ERROR_MECH_FS_FAULT,
   ERROR_ELEC = 20,
   ERROR_ELEC_I2C_ADDRESS_NOT_FOUND,
   ERROR_COMMS,
   ERROR_ELEC_BUTTON_ADDRESS,
   ERROR_ELEC_SOLENOID_ADDRESS,
   ERROR_ELEC_WRONG_I2C_ADDRESS,
   ERROR_ELEC_PIN_DISPENSE,
   ERROR_ELEC_PIN_CLEAN,
   ERROR_SECU = 30,
   ERROR_SECU_XMLFILE_NOT_FOUND,
   ERROR_SECU_XMLFILE_NO_MATCH_CONTENT,
   ERROR_NETW = 40,
   ERROR_NETW_NO_COMMAND,
   ERROR_NETW_NO_OPTION,
   ERROR_NETW_NO_POSITION,
   ERROR_PAYM = 50,
   ERROR_PAYM_NOT_FOUND,
   ERROR_PTHREADS = 60,
   ERROR_PTHREADS_KBTHREAD,
   ERROR_PTHREADS_IPTHREAD,
   ERROR_PTHREADS_IPTHREAD_NAK
} DF_ERROR;

#endif
