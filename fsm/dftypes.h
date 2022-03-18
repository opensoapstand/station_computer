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

// #define XML_SETTINGS "/release/df_settings.xml"

#include <sqlite3.h>
#include <stdint.h>

#define PRODUCT_DISPENSERS_MAX 4
#define MINIMUM_DISPENSE_VOLUME_ML 10
// #define USE_OLD_DATABASE

#ifdef USE_OLD_DATABASE
   #define DB_PATH "/home/df-admin/production/db/drinkfill-sqlite.db"
   // #define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"
   //#define DB_PATH "/home/df-admin/drinkfill/db/sqlite/drinkfill-sqlite.db"
#else

   #define DB_PATH "/home/df-admin/production/db/drinkfill-sqlite_newlayout.db"
#endif

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

#ifdef USE_OLD_DATABASE
   #define DB_PRODUCTS_SLOT 0
   #define DB_PRODUCTS_NAME 1
   #define DB_PRODUCTS_VOLUME_DISPENSED 2
   #define DB_PRODUCTS_VOLUME_TARGET_L 3
   #define DB_PRODUCTS_VOLUME_TARGET_S 4
   #define DB_PRODUCTS_CALIBRATION_CONST 5
   #define DB_PRODUCTS_PRICE_L 6
   #define DB_PRODUCTS_PRICE_S 7
   #define DB_PRODUCTS_IS_STILL 8
   #define DB_PRODUCTS_VOLUME_PER_TICK 9
   #define DB_PRODUCTS_PLU_L 10
   #define DB_PRODUCTS_PLU_S 11
   #define DB_PRODUCTS_FULL_ML 12
   #define DB_PRODUCTS_REMAINING_ML 13
   #define DB_PRODUCTS_TOTAL_DISPENSED 14
   #define DB_PRODUCTS_NAME_RECEIPT 15
   #define DB_PRODUCTS_PAYMENT 16
   #define DB_PRODUCTS_LAST_REFILL 17
   #define DB_PRODUCTS_PWM 18
   #define DB_PRODUCTS_BUFFER 19
   #define DB_PRODUCTS_PRODUCT_ID 20
   #define DB_PRODUCTS_UNITS 21

#else

   #define DB_PRODUCTS_PRODUCTID 0
   #define DB_PRODUCTS_SOAPSTAND_PRODUCT_SERIAL     1
   #define DB_PRODUCTS_SLOT    2
   #define DB_PRODUCTS_NAME    3
   #define DB_PRODUCTS_SIZE_UNIT 4   
   #define DB_PRODUCTS_CURRENCY   5 
   #define DB_PRODUCTS_PAYMENT    6
   #define DB_PRODUCTS_NAME_RECEIPT 7    
   #define DB_PRODUCTS_DISPENSE_SPEED 8    
   #define DB_PRODUCTS_CALIBRATION_CONST 9    
   #define DB_PRODUCTS_VOLUME_PER_TICK    10
   #define DB_PRODUCTS_LAST_RESTOCK    11
   #define DB_PRODUCTS_VOLUME_FULL    12
   #define DB_PRODUCTS_VOLUME_REMAINING 13   
   #define DB_PRODUCTS_VOLUME_DISPENSED_SINCE_RESTOCK    14
   #define DB_PRODUCTS_VOLUME_DISPENSED_TOTAL    15
   #define DB_PRODUCTS_IS_ENABLED_SMALL    16
   #define DB_PRODUCTS_IS_ENABLED_MEDIUM    17
   #define DB_PRODUCTS_IS_ENABLED_LARGE    18
   #define DB_PRODUCTS_IS_ENABLED_CUSTOM    19
   #define DB_PRODUCTS_SIZE_SMALL    20
   #define DB_PRODUCTS_SIZE_MEDIUM    21
   #define DB_PRODUCTS_SIZE_LARGE    22
   #define DB_PRODUCTS_SIZE_CUSTOM_MIN 23   
   #define DB_PRODUCTS_SIZE_CUSTOM_MAX   24 
   #define DB_PRODUCTS_PRICE_SMALL    25
   #define DB_PRODUCTS_PRICE_MEDIUM    26
   #define DB_PRODUCTS_PRICE_LARGE    27
   #define DB_PRODUCTS_PRICE_CUSTOM    28
   #define DB_PRODUCTS_PLU_SMALL    29
   #define DB_PRODUCTS_PLU_MEDIUM    30
   #define DB_PRODUCTS_PLU_LARGE    31
   #define DB_PRODUCTS_PLU_CUSTOM    32
   #define DB_PRODUCTS_PID_SMALL    33
   #define DB_PRODUCTS_PID_MEDIUM    34
   #define DB_PRODUCTS_PID_LARGE    35
   #define DB_PRODUCTS_PID_CUSTOM    36
   #define DB_PRODUCTS_FLAVOUR    37
   #define DB_PRODUCTS_IMAGE_URL    38
   #define DB_PRODUCTS_TYPE    39
   #define DB_PRODUCTS_INGREDIENTS 40    
   #define DB_PRODUCTS_FEATURES    41

#endif

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
