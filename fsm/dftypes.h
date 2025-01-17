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

#define CONTROLLER_VERSION "3.10"

#define PRODUCT_DETAILS_TSV_PATH "/home/df-admin/production/references/products/product_details.tsv" // https://docs.google.com/spreadsheets/d/17WR2gRyPIDIlGKBy1YKFAqN-Hyw_3VOJ6JCmfcAtjVk/edit#gid=169583479 download as .tsv file
#define PRODUCT_DETAILS_FIELD_COUNT 100  // take some extra

#define ENABLE_PARALLEL_MIX

#define CSV_PRODUCT_COL_ID 0
#define CSV_PRODUCT_COL_NAME 1
#define CSV_PRODUCT_COL_TYPE 2
#define CSV_PRODUCT_COL_MIX_PNUMBERS 3
#define CSV_PRODUCT_COL_MIX_RATIOS_LOW 4
#define CSV_PRODUCT_COL_MIX_RATIOS_DEFAULT 5
#define CSV_PRODUCT_COL_MIX_RATIOS_HIGH 6
#define CSV_PRODUCT_COL_SUPPLIER 7
#define CSV_PRODUCT_COL_BRAND 8
#define CSV_PRODUCT_COL_LOCATION 9
#define CSV_PRODUCT_COL_INGREDIENTS 10
#define CSV_PRODUCT_COL_PRODUCT_NAME_SHORT 11
#define CSV_PRODUCT_COL_PRODUCT_TITLE 12
#define CSV_PRODUCT_COL_PRODUCT_TAG_LINE 13
#define CSV_PRODUCT_COL_PRODUCT_NAME_RECEIPT 14
#define CSV_PRODUCT_COL_NAME_UI 15
#define CSV_PRODUCT_COL_DESCRIPTION_UI 16
#define CSV_PRODUCT_COL_FEATURES_UI 17
#define CSV_PRODUCT_COL_INGREDIENTS_UI 18
#define CSV_PRODUCT_COL_CALIBRATION 19
#define CSV_PRODUCT_COL_NOTES 20
#define CSV_PRODUCT_COL_URL 21

// #define CSV_PRODUCT_COL_NAME_UI 11
// #define CSV_PRODUCT_COL_DESCRIPTION_UI 12
// #define CSV_PRODUCT_COL_FEATURES_UI 13
// #define CSV_PRODUCT_COL_INGREDIENTS_UI 14
// #define CSV_PRODUCT_COL_NOTES 15


#define DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX 6

#define PRODUCT_DISPENSERS_MAX 4

#define DUMMY_PNUMBER 2
#define CUSTOM_MIX_PNUMBER 1


#define ADDITIVES_PER_SLOT_COUNT_MAX 5
#define PNUMBERS_COUNT 200
#define MINIMUM_DISPENSE_VOLUME_ML 10

// #define ENABLE_MULTI_BUTTON

#define CONFIG_DB_PATH "/home/df-admin/production/db/configuration.db"
#define USAGE_DB_PATH "/home/df-admin/production/db/usage.db"


#define MOTOR_TEST_DATA_OUTPUT_INTERVAL_MILLIS 1000
// #define MOTOR_TEST_DATA_OUTPUT_INTERVAL_MILLIS 1000
#define CYCLIC_PUMP_TEST_ON_CYCLE_MILLIS 30000
#define CYCLIC_PUMP_TEST_OFF_CYCLE_MILLIS 30000

#define MILLIS_INIT_DUMMY 0
// #define IO_PIN_BUTTON_MAINTENANCE 340 // connector pin 38
// #define IO_PIN_BUTTON_SHUTDOWN 341 //  connector pin 40 deprecated 
// #define IO_PIN_BUTTON_MAINTENANCE_SHUTDOWN_EDGE_DETECTOR 391 // connector pin 32 deprecated.
#define IO_PIN_FLOW_SENSOR 364 // deprecated, try to get through i2c chip for consistency. connector pin 11

#define FLOWSENSOR_DEJITTER_MICROS 700ULL  //digmesa flowsensor
//#define FLOWSENSOR_DEJITTER_MICROS 10000ULL  // AICHI flow sensor
// #define IO_PIN_BUTTON_4 410   // connector pin 36 for EN-29 pcb

#define IO_PIN_ENABLE_3point3V_BEFORE_SYSFS_DEPRECATED 389 // connector pin 28 for EN258 pcb
#define IO_PIN_ENABLE_5V_BEFORE_SYSFS_DEPRECATED 338       // connector pin 12 for EN258 pcb
#define IO_PIN_ENABLE_24V_BEFORE_SYSFS_DEPRECATED 410 // connector pin 36 for EN-134 pcb
 
// sysfs deprecated --> still works if all pins have 512 added to it...
// https://forum.seeedstudio.com/t/gpio-pins-not-responding-in-code/252187/2
#define IO_PIN_ENABLE_3point3V_AFTER_SYSFS_DEPRECATED 901 // connector pin 28 for EN258 pcb
#define IO_PIN_ENABLE_5V_AFTER_SYSFS_DEPRECATED 850       // connector pin 12 for EN258 pcb
#define IO_PIN_ENABLE_24V_AFTER_SYSFS_DEPRECATED 922 // connector pin 36 for EN-134 pcb

// #define INTERRUPT_DRIVE_FLOW_SENSOR_TICKS   // WARNING: if enabled: check pin for ubuntu 22 and up!!!!  (deprecated. We're using polling now)
#define power_cycle_attempt_AT_INVALID_PCB 10

// #define PIC_PROGRAMMER_PIN_VPP 337
// #define PIC_PROGRAMMER_PIN_PGC 412
// #define PIC_PROGRAMMER_PIN_PGD 413

// #define IO_PIN_FLOW_SENSOR_STRING "364"
#define RUNNING_AVERAGE_WINDOW_LENGTH 10000
#define DISPENSE_BUTTON_DEBOUNCE_MILLIS 80
#define FLOW_SENSOR_DEBOUNCE_MILLIS 1

#define SLOW_START_INCREASE_PERIOD_MILLIS 2 // set to 0 for instant start
#define SLOW_STOP_PERIOD_MILLIS 1           // set to 0 for instant stop

#define DISPENSE_STATUS_UPDATE_DELTA_MILLIS 1000 // period of which controller can send status to ui and logging and terminal

#define SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS 3000
// #define EMPTY_CONTAINER_DETECTION_FLOW_THRESHOLD_ML_PER_S 15 // reference: at 2l/min we have 33ml/s
#define EMPTY_CONTAINER_DETECTION_FLOW_AVERAGE_WINDOW_MILLIS 1000
#define EMPTY_CONTAINER_DETECTION_MAXIMUM_PRIME_TIME_MILLIS 10000

#define END_OF_DISPENSE_SPOUT_SHUTOFF_DELAY_MICROS 250000 // only applies to parallel mixing
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
#define SIZE_SAMPLE_CHAR 'f'
#define SIZE_DUMMY 'x'
#define SIZE_SMALLER_THAN_SMALL '0'
#define SIZE_EMPTY_CONTAINER_DETECTED_CHAR 'i' // hack to accomodate for empty container or dispense timeout

#define ACTION_REPAIR_PCA 'r'
#define ACTION_DISPENSE 'd'
#define ACTION_RESET 'b'
#define ACTION_AUTOFILL 'a'

#define ACTION_NO_ACTION '-'
#define ACTION_DISPENSE_END 'f'
#define ACTION_DUMMY 'x'
#define ACTION_QUIT 'q'
#define ACTION_MANUAL_PUMP_PWM_SET 'i'
#define ACTION_MANUAL_PUMP_SET 'n'
#define ACTION_PRINT_TRANSACTION '@'
#define ACTION_UI_COMMAND_PRINT_TRANSACTION '+'
#define ACTION_UI_COMMAND_TEST_PRINT '='

#define ACTION_UI_COMMAND_PRINTER_SEND_STATUS '$'
#define ACTION_UI_COMMAND_PRINTER_MENU 'p'
#define ACTION_HELP 'h'

#define PRODUCT_DUMMY_CHAR 'x'
#define PRODUCT_SLOT_DUMMY 666

#define PWM_CHAR 'P'


#define SIZE_INDEX_TO_CHAR_ARRAY                                           \
   {                                                                       \
      SIZE_SMALL_CHAR, SIZE_MEDIUM_CHAR, SIZE_LARGE_CHAR, SIZE_CUSTOM_CHAR \
   }



typedef enum Product_state
{
   PRODUCT_STATE_AVAILABLE = 0,
   PRODUCT_STATE_AVAILABLE_LOW_STOCK,
   PRODUCT_STATE_NOT_PRIMED,
   PRODUCT_STATE_PROBLEM_NEEDS_ATTENTION,
   PRODUCT_STATE_PROBLEM_EMPTY,
   PRODUCT_STATE_DISABLED_COMING_SOON,
   PRODUCT_STATE_DISABLED,
   PRODUCT_STATE_INVALID,
   // PRODUCT_STATE_AVAILABLE = 0,
   // PRODUCT_STATE_AVAILABLE_LOW_STOCK,
   // PRODUCT_STATE_EMPTY
} Product_state;

// slot state basic states for the dispenser. 
typedef enum SlotState
{
   SLOT_STATE_AVAILABLE = 0,
   SLOT_STATE_NOT_PRIMED,
   SLOT_STATE_PROBLEM_NEEDS_ATTENTION,
   SLOT_STATE_DISABLED,
   SLOT_STATE_INVALID
}Slot_state;

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



// typedef enum ProductState
// {
//    PRODUCT_STATE_AVAILABLE = 0,
//    PRODUCT_STATE_AVAILABLE_LOW_STOCK,
//    PRODUCT_STATE_PROBLEM_NEEDS_ATTENTION,
//    PRODUCT_STATE_PROBLEM_EMPTY,
//    PRODUCT_STATE_DISABLED,
//    PRODUCT_STATE_INVALID,
// } ProductState;

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

#define TABLE_PRODUCTS_COLUMN_COUNT 56

typedef enum MIX_RATIO_SETTING
{
   LOW,
   DEFAULT,
   HIGH
} MIX_RATIO_SETTING;

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
