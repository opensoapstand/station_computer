//***************************************
//
// dftypes.h
// types specific to drinkfill
//
// created: 01-2022
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2022 by Productfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef DFTYPES__H_
#define DFTYPES__H_

// #define XML_SETTINGS "/release/df_settings.xml"

#include <sqlite3.h>
#include <stdint.h>

#define CONTROLLER_VERSION "3.0"

#define PRODUCT_DETAILS_TSV_PATH "/home/df-admin/production/references/products/product_details.tsv" // https://docs.google.com/spreadsheets/d/17WR2gRyPIDIlGKBy1YKFAqN-Hyw_3VOJ6JCmfcAtjVk/edit#gid=169583479 download as .tsv file
#define CSV_PRODUCT_COL_ID 0
#define CSV_PRODUCT_COL_NAME 1
#define CSV_PRODUCT_COL_TYPE 2
#define CSV_PRODUCT_COL_SUPPLIER 3
#define CSV_PRODUCT_COL_BRAND 4
#define CSV_PRODUCT_COL_INGREDIENTS 5
#define CSV_PRODUCT_COL_LOCATION 6
#define CSV_PRODUCT_COL_NAME_UI 7
#define CSV_PRODUCT_COL_DESCRIPTION_UI 8
#define CSV_PRODUCT_COL_FEATURES_UI 9
#define CSV_PRODUCT_COL_INGREDIENTS_UI 10
#define CSV_PRODUCT_COL_NOTES 11

#define DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX 6

#define PRODUCT_DISPENSERS_MAX 4
#define DUMMY_PNUMBER 666
#define ADDITIVES_PER_SLOT_COUNT_MAX 5
#define PNUMBERS_COUNT 100
#define MINIMUM_DISPENSE_VOLUME_ML 10

// #define ENABLE_MULTI_BUTTON

#define CONFIG_DB_PATH "/home/df-admin/production/db/configuration.db"
#define USAGE_DB_PATH "/home/df-admin/production/db/usage.db"


#define MOTOR_TEST_DATA_OUTPUT_INTERVAL_MILLIS 1000
// #define MOTOR_TEST_DATA_OUTPUT_INTERVAL_MILLIS 1000
#define CYCLIC_PUMP_TEST_ON_CYCLE_MILLIS 30000
#define CYCLIC_PUMP_TEST_OFF_CYCLE_MILLIS 30000

#define MILLIS_INIT_DUMMY 0
#define IO_PIN_BUTTON_MAINTENANCE 340 // connector pin 38
#define IO_PIN_BUTTON_SHUTDOWN 341 //  connector pin 40 deprecated 
#define IO_PIN_BUTTON_MAINTENANCE_SHUTDOWN_EDGE_DETECTOR 391 // connector pin 32 deprecated.
#define IO_PIN_FLOW_SENSOR 364 // connector pin 11

#define FLOWSENSOR_DEJITTER_MICROS 700ULL  //digmesa flowsensor
//#define FLOWSENSOR_DEJITTER_MICROS 10000ULL  // AICHI flow sensor
#define IO_PIN_BUTTON_4 410   // connector pin 36 for EN-29 pcb
#define IO_PIN_ENABLE_24V 410 // connector pin 36 for EN-134 pcb
#define IO_PIN_ENABLE_3point3V 389 // connector pin 28 for EN258 pcb
#define IO_PIN_ENABLE_5V 338       // connector pin 12 for EN258 pcb

#define PIC_PROGRAMMER_PIN_VPP 337
#define PIC_PROGRAMMER_PIN_PGC 412
#define PIC_PROGRAMMER_PIN_PGD 413

// #define IO_PIN_FLOW_SENSOR_STRING "364"
#define RUNNING_AVERAGE_WINDOW_LENGTH 10000
#define DISPENSE_BUTTON_DEBOUNCE_MILLIS 80
#define FLOW_SENSOR_DEBOUNCE_MILLIS 1

#define SLOW_START_INCREASE_PERIOD_MILLIS 2 // set to 0 for instant start
#define SLOW_STOP_PERIOD_MILLIS 1           // set to 0 for instant stop

#define DISPENSE_STATUS_UPDATE_DELTA_MILLIS 500 // period of which controller can send status to ui and logging and terminal

#define SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS 3000
// #define EMPTY_CONTAINER_DETECTION_FLOW_THRESHOLD_ML_PER_S 15 // reference: at 2l/min we have 33ml/s
#define EMPTY_CONTAINER_DETECTION_FLOW_AVERAGE_WINDOW_MILLIS 1000
#define EMPTY_CONTAINER_DETECTION_MAXIMUM_PRIME_TIME_MILLIS 10000

#define TEST_DISPENSE_TARGET_VOLUME 5000

#define ML_TO_OZ 0.033814

#define DEFAULT_PUMP_PWM 0x80

#define CONTAINER_EMPTY_THRESHOLD_ML 1500

#define SIZE_INDEX_SMALL 0
#define SIZE_INDEX_MEDIUM 1
#define SIZE_INDEX_LARGE 2
#define SIZE_INDEX_CUSTOM 3

#define SIZE_SMALL_CHAR 's'
#define SIZE_MEDIUM_CHAR 'm'
#define SIZE_LARGE_CHAR 'l'
#define SIZE_CUSTOM_CHAR 'c'
#define SIZE_TEST_CHAR 't'
// #define SIZE_AUTO_CHAR 'a'
#define SIZE_DUMMY 'x'
#define SIZE_SMALLER_THAN_SMALL '0'
#define SIZE_EMPTY_CONTAINER_DETECTED_CHAR 'i' // hack to accomodate for empty container or dispense timeout

#define ACTION_REPAIR_PCA 'r'
#define ACTION_DISPENSE 'd'
#define ACTION_RESET 'b'
#define ACTION_AUTOFILL 'a'
// #define CLEAN_CHAR 'c'

#define ACTION_NO_ACTION '-'
#define ACTION_DISPENSE_END 'f'
#define ACTION_DUMMY 'x'
#define ACTION_QUIT 'q'
#define ACTION_MANUAL_PUMP_PWM_SET 'i'
#define ACTION_MANUAL_PUMP_SET 'n'
#define ACTION_PRINT_TRANSACTION 't'

// #define ACTION_UI_COMMAND_PRINTER_SEND_STATUS 'a'
#define ACTION_UI_COMMAND_PRINTER_MENU 'p'
#define ACTION_HELP 'h'

#define PRODUCT_DUMMY 'x'
#define PRODUCT_SLOT_DUMMY 666

#define PWM_CHAR 'P'


#define SIZE_INDEX_TO_CHAR_ARRAY                                           \
   {                                                                       \
      SIZE_SMALL_CHAR, SIZE_MEDIUM_CHAR, SIZE_LARGE_CHAR, SIZE_CUSTOM_CHAR \
   }

typedef enum Dispense_behaviour
{
   FLOW_STATE_UNAVAILABLE = 0,
   FLOW_STATE_RAMP_UP,
   FLOW_STATE_DISPENSING,
   FLOW_STATE_PUMPING_NOT_DISPENSING,
   FLOW_STATE_NOT_PUMPING_NOT_DISPENSING,
   FLOW_STATE_PRIMING_OR_EMPTY,
   FLOW_STATE_PRIME_FAIL_OR_EMPTY,
   FLOW_STATE_EMPTY
} Dispense_behaviour;

struct product_order
{
   int slot;
   char size;
};
typedef struct product_order product_order;

typedef enum Slot_state
{
   SLOT_STATE_AVAILABLE = 0,
   SLOT_STATE_AVAILABLE_LOW_STOCK,
   SLOT_STATE_WARNING_PRIMING,
   SLOT_STATE_PROBLEM_NEEDS_ATTENTION,
   SLOT_STATE_PROBLEM_EMPTY,
   SLOT_STATE_DISABLED_COMING_SOON,
   SLOT_STATE_DISABLED

} Slot_state;

typedef enum Button_lights_behaviour
{
   IDLE_OFF = 0,
   IDLE_ANIMATION_FROM_DB,
   IDLE_ANIMATION_PING_PONG,
   IDLE_ANIMATION_FILL,

} Button_lights_behaviour;

typedef struct product_order product_order;

struct Time_val
{
   uint64_t time_millis;
   double value;
};
typedef struct Time_val Time_val;

#define TABLE_PRODUCTS_COLUMN_COUNT 54

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
   STATE_MANUAL_CONFIG,
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
