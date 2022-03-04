//***************************************
//
// dftypes.h
// types specific to drinkfill
//
// created: 01-2022
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Productfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef DFTYPES__H_
#define DFTYPES__H_

#define XML_SETTINGS "/release/df_settings.xml"

#include <sqlite3.h>
#include <stdint.h>

#define USE_OLD_DATABASE

#define PRODUCT_DISPENSERS_MAX 4
#define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"
//#define DB_PATH "/home/df-admin/drinkfill/db/sqlite/drinkfill-sqlite.db"

#define MILLIS_INIT_DUMMY 0
#define IO_PIN_FLOW_SENSOR 364
#define IO_PIN_FLOW_SENSOR_STRING "364"
#define RUNNING_AVERAGE_WINDOW_LENGTH 100
#define DISPENSE_BUTTON_DEBOUNCE_MILLIS 50

#define ML_TO_OZ 0.033814

struct Time_val{
   uint64_t time_millis;
   double value;
};
typedef struct Time_val Time_val;


typedef enum DF_FSM
{
   STATE_DUMMY,
   STATE_INIT,
   STATE_IDLE,
   STATE_DISPENSE_INIT,
   STATE_DISPENSE_IDLE,
   STATE_DISPENSE,
   STATE_DISPENSE_END,
   STATE_MANUAL_PRINTER,
   STATE_MANUAL_PUMP,
   STATE_END,
   FSM_MAX,
} DF_FSM;

typedef enum DF_ERROR
{
   OK = 0,
   ERROR_END,
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
