//***************************************
//
// stateinit.h
// init state class
//
// Set Objects for FSM
// Check all Hardware is operational.
// Initialize threads for operation and
// communication in machine.
// Connect or create database.
//
// created: 01-2022
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "stateInit.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include "../fsm.h"

#define INIT_STRING "Init"

// Default Ctor
stateInit::stateInit()
{
}

// CTOR Linked to IPC
stateInit::stateInit(messageMediator *message)
{
}

// DTOR
stateInit::~stateInit()
{
    //delete stuff
}

// Overload for Debugger output
string stateInit::toString()
{
    return INIT_STRING;
}

/*
 * Initialize FSM State
*/
DF_ERROR stateInit::onEntry()
{
    m_state_requested = STATE_INIT;
    DF_ERROR e_ret = OK;
    return e_ret;
}

// Initialize all related hardware
DF_ERROR stateInit::onAction()
{
    DF_ERROR e_ret = ERROR_BAD_PARAMS;

    e_ret = setProducts();

    if (OK == e_ret)
    {
        e_ret = dispenserSetup();
    }

    if (nullptr != &m_state_requested && OK == e_ret)
    {
        if (OK == e_ret)
        {
            m_state_requested = STATE_IDLE;

            // The UI program waits for this message to move from its initializing phase to its Idle phase:
            m_pMessaging->sendMessage("Init Ready");
        }
    }

    return e_ret;
}

DF_ERROR stateInit::onExit()
{
    DF_ERROR e_ret = OK;
    return e_ret;
}

// Initilization function for all dispensers...
DF_ERROR stateInit::dispenserSetup()
{
    int idx;
    dispenser *productDispensers = g_productDispensers;

    debugOutput::sendMessage("Setting up DS-ED-8344 hardware control board.\n", MSG_INFO);

    // We only need one flow sensor interrupt pin since only one pump
    // is ever active at a time.  The flow sensors are all connected
    // to the same pin in the hardware.
#ifndef __arm__
    for (idx = 0; idx < 4; idx++)
    {
        productDispensers[idx].setFlowsensor(IO_PIN_FLOW_SENSOR, idx);
    }
#else
    productDispensers[0].setFlowsensor(17, 0);
#endif

    // Set up the four pumps
    for (idx = 0; idx < 4; idx++)
    {
        productDispensers[idx].setPump(0, 0, idx);
    }

    productDispensers[0].setButtonsShutdownAndMaintenance(); // todo: this is a hack for the maintenance and power button. It should not be part of the dispenser class

    debugOutput::sendMessage("Dispenser intialized.", MSG_INFO);

    return OK;
} // End of dispenserSetup()

// This function (called in SetDrinks) converts the data that is in the product database to variables,
//which are then passed to the SetDrink function to create product objects for each product.
static int db_sql_product_callback(void *data, int argc, char **argv, char **azColName)
{
    // callback is called for every record.
    int i;
    int slot;
    string name;
    string name_receipt;
    double volume_dispensed;
    double volume_large;
    double volume_medium;
    double volume_small;
    double volume_target_custom_min;
    double volume_target_custom_max;
    double calibration_const;
    double price_large;
    double price_small;
    double price_medium;
    double price_custom_per_liter;
    int is_still;
    double volume_per_tick;
    int dispense_speed_pwm;
    string paymentMethod;
    string display_unit;

    string plu_large;
    string plu_medium;
    string plu_small;
    string plu_custom;

    // debugOutput::sendMessage("Check product values from database:", MSG_INFO);
    // for (i = 0; i < argc; i++)
    // {
    //     std::string colname = azColName[i];
    //     if(argv[i]){
    //     debugOutput::sendMessage("colname,value :" + colname + "," + argv[i], MSG_INFO); //+ std::string to_string(colname)
    //     }else{
    //         debugOutput::sendMessage("colname,NULL :" + colname , MSG_INFO); //+ std::string to_string(colname)
    //     }
    // }

    debugOutput::sendMessage("-------------------Set product values from database:", MSG_INFO);
    for (i = 0; i < argc; i++)
    {
        //printf("%s = %s\n", azColName[i], argv[i]);
        std::string colname = azColName[i];

        char *value = argv[i];
        char dummy = '0';
        char *dummy_pointer = &dummy;
        if (!value)
        {
            debugOutput::sendMessage("colname, NULL --> set to 0 :" + colname, MSG_INFO);
            value = dummy_pointer;
        }

        debugOutput::sendMessage("colname,value :" + colname + "," + value, MSG_INFO); //+ std::string to_string(colname)

        if (colname == "slot")
        {
            slot = atoi(value);
        }
        else if (colname == "name")
        {
            name = value;
        }
        else if (colname == "display_unit")
        {
            display_unit = value;
        }
        else if (colname == "name_receipt")
        {
            name_receipt = value;
        }
        else if (colname == "volume_remaining")
        {
            volume_dispensed = atof(value);
        }
        else if (colname == "size_custom_min")
        {
            volume_target_custom_min = atof(value);
        }
        else if (colname == "size_custom_max")
        {
            volume_target_custom_max = atof(value);
        }
        else if (colname == "size_small")
        {
            volume_small = atof(value);
        }
        else if (colname == "size_medium")
        {
            volume_medium = atof(value);
        }
        else if (colname == "size_large")
        {
            volume_large = atof(value);
        }
        else if (colname == "calibration_const")
        {
            calibration_const = atof(value);
        }
        else if (colname == "price_small")
        {
            price_small = atof(value);
        }
        else if (colname == "price_medium")
        {
            price_medium = atof(value);
        }
        else if (colname == "price_large")
        {
            price_large = atof(value);
        }
        else if (colname == "price_custom")
        {
            price_custom_per_liter = atof(value);
        }
        else if (colname == "volume_per_tick")
        {
            volume_per_tick = atof(value);
        }
        else if (colname == "dispense_speed")
        {
            dispense_speed_pwm = atof(value);
        }
        else if (colname == "plu_small")
        {
            plu_small = value;
        }
        else if (colname == "plu_medium")
        {
            plu_large = value;
        }
        else if (colname == "plu_large")
        {
            plu_large = value;
        }
        else if (colname == "plu_custom")
        {
            plu_large = value;
        }
        else if (colname == "payment")
        {
            paymentMethod = value;
        }
        else
        {
            debugOutput::sendMessage("unprocessed colname: " + colname, MSG_INFO); //+ std::string to_string(colname)
        }
    }
    g_productDispensers[slot - 1].setSlot(slot);
    g_productDispensers[slot - 1].setProduct(
        new product(slot, name, calibration_const, volume_per_tick, dispense_speed_pwm,
                    volume_small, volume_medium, volume_large, volume_target_custom_min, volume_target_custom_max,
                    price_small, price_medium, price_large, price_custom_per_liter,
                    plu_small, plu_medium, plu_large, plu_custom,
                    paymentMethod, name_receipt, display_unit));

    return 0;
}

DF_ERROR stateInit::setProducts()
{

    // Product Setup
    // load the SQLITE manager

    char *zErrMsg = 0;
    int rc;
    const char *data = "Callback function called";

    rc = sqlite3_open(DB_PATH, &db);

    debugOutput::sendMessage("DB GETTER START", MSG_INFO);

    if (rc)
    {
        // fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        // TODO: Error handling here...

        debugOutput::sendMessage("Database opening error", MSG_INFO);
    }
    else
    {
        //       fprintf(stderr, "Opened database successfully\n\n");
    }

    /* Create SQL statement */
    std::string sql11 = "SELECT * from products";
    char *sql = new char[sql11.length() + 1];
    strcpy(sql, sql11.c_str());

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, db_sql_product_callback, (void *)data, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        debugOutput::sendMessage("Product info SQL error (OR DB PATH opening ERROR!!)", MSG_INFO);
        //       fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        //       fprintf(stdout, "Operation done successfully\n");
    }
    sqlite3_close(db);

    debugOutput::sendMessage("Products intialized.", MSG_INFO);

    return OK;
}
