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
        productDispensers[idx].setFlowsensor(364, idx);
    }
#else
    productDispensers[0].setFlowsensor(17, 0);
#endif

    // Set up the four pumps
    for (idx = 0; idx < 4; idx++)
    {
        productDispensers[idx].setPump(0, 0, idx);
    }

    productDispensers[0].setPowerOffListener(); // is this the power off button?

    debugOutput::sendMessage("Dispenser intialized.", MSG_INFO);

    return OK;
} // End of dispenserSetup()

// This function (called in SetDrinks) converts the data that is in the product database to variables,
//which are then passed to the SetDrink function to create product objects for each product.
static int db_sql_product_callback(void *data, int argc, char **argv, char **azColName)
{
    int i;
    int slot;
    string name;
    double volume_dispensed;
    double volume_target_l;
    double volume_target_s;
    double calibration_const;
    double price_l;
    double price_s;
    int is_still;
    double volume_per_tick;
    string paymentMethod;
    string plu_l;
    string plu_s;
    string name_receipt;

    //   printf("\n----------\n");

    for (i = 0; i < argc; i++)
    {
        //printf("%s = %s\n", azColName[i], argv[i]);
        std::string colname = azColName[i];

        if (colname == "slot")
        {
            slot = atoi(argv[i]);
        }
        else if (colname == "name")
        {
            name = argv[i];
        }
        else if (colname == "name_receipt")
        {
            name_receipt = argv[i];
        }
        else if (colname == "volume_dispensed")
        {
            volume_dispensed = atof(argv[i]);
        }
        else if (colname == "volume_target_l")
        {
            volume_target_l = atof(argv[i]);
        }
        else if (colname == "volume_target_s")
        {
            volume_target_s = atof(argv[i]);
        }
        else if (colname == "calibration_const")
        {
            calibration_const = atof(argv[i]);
        }
        else if (colname == "price_l")
        {
            price_l = atof(argv[i]);
        }
        else if (colname == "price_s")
        {
            price_s = atof(argv[i]);
        }
        else if (colname == "is_still")
        {
            is_still = atoi(argv[i]);
        }
        else if (colname == "volume_per_tick")
        {
            volume_per_tick = atof(argv[i]);
        }
        else if (colname == "PLU_l")
        {
            plu_l = argv[i];
        }
        else if (colname == "PLU_s")
        {
            plu_s = argv[i];
        }
        else if (colname == "payment")
        {
            paymentMethod = argv[i];
        }

        g_productDispensers[slot - 1].setProduct(
            new product(slot,
                        name,
                        volume_dispensed,
                        volume_target_l,
                        volume_target_s,
                        calibration_const,
                        price_l,
                        price_s,
                        false,
                        volume_per_tick,
                        plu_l,
                        plu_s,
                        paymentMethod,
                        name_receipt));
    }

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
