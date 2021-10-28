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
// created: 12-06-2020
// by: Denis Londry & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
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
stateInit::stateInit(messageMediator * message)
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
   DF_ERROR e_ret  = OK;

   m_state = INIT; //ensure the current state is INIT
   m_nextState = INIT;

   return e_ret;
}

// Initialize all related hardware
DF_ERROR stateInit::onAction()
{
   DF_ERROR e_ret  = ERROR_BAD_PARAMS;
   m_state = INIT; //ensure the current state is INIT

   e_ret = setDrinks();

   if(OK == e_ret) 
   {
      e_ret = dispenserSetup();
   }

   if (nullptr != &m_nextState && OK == e_ret)
   {
      if(OK == e_ret)
      {
         m_nextState = IDLE;

         m_pMessaging->sendMessage("Init Ready");
      }
   }

   return e_ret;
}

DF_ERROR stateInit::onExit()
{
   DF_ERROR e_ret  = OK;

   m_state = INIT;
   m_nextState = IDLE; //once everything is good, move to idle state

   return e_ret;
}


// Initilization function for all dispensers...
DF_ERROR stateInit::dispenserSetup()
{
    int idx;
    dispenser* cassettes = g_cassettes;

    debugOutput::sendMessage("Setting up DS-ED-8344 hardware control board.\n", INFO);
    
    // We only need one flow sensor interrupt pin since only one pump
    // is ever active at a time.  The flow sensors are all connected
    // to the same pin in the hardware.
#ifndef __arm__
//    cassettes[0].setFlowsensor(364, 0);
    for (idx=0; idx<4; idx++)
    {
        cassettes[idx].setFlowsensor(364, idx);
//    cassettes[0].setFlowsensor(364, 0);
    }
#else
    cassettes[0].setFlowsensor(17, 0);
#endif

    // Set up the four pumps
    for (idx=0; idx<4; idx++)
    {
	cassettes[idx].setPump(0, 0, idx);
    }

    cassettes[0].setPowerOffListener();

    debugOutput::sendMessage("Hardware initialized...", INFO);

    return OK;
}   // End of dispenserSetup()


static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   //fprintf(stderr, "%s: ", (const char*)data);

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

   for(i = 0; i<argc; i++){
      //printf("%s = %s\n", azColName[i], argv[i]);
      std::string colname = azColName[i];

      if (colname == "slot"){
          //printf("setting slot \n");
          slot = atoi(argv[i]);
//          printf("Slot: %d \n", slot);
      }
      else if (colname == "name"){
          //printf("setting name \n");
          name = argv[i];
//          printf("Name: %s \n", name.c_str());
      }
      else if (colname == "name_receipt"){
        name_receipt = argv[i];
      }
      else if (colname == "volume_dispensed"){
          //printf("setting vol disp \n");
          volume_dispensed = atof(argv[i]);
//          printf("Volume Dispensed: %.2f \n", volume_dispensed);
      }
      else if (colname == "volume_target_l"){
          //printf("setting vol tar \n");
          volume_target_l = atof(argv[i]);
//          printf("Volume Target L: %.2f \n", volume_target_l);
      }
      else if (colname == "volume_target_s"){
          //printf("setting vol tar \n");
          volume_target_s = atof(argv[i]);
//          printf("Volume Target S: %.2f \n", volume_target_s);
      }
      else if (colname == "calibration_const"){
          //printf("setting cal con \n");
          calibration_const = atof(argv[i]);
//          printf("Calibration Const: %.2f \n", calibration_const);
      }
      else if (colname == "price_l"){
          //printf("setting price \n");
          price_l = atof(argv[i]);
//          printf("Price L: %.2f \n", price_l);
      }
      else if (colname == "price_s"){
          //printf("setting price \n");
          price_s = atof(argv[i]);
//          printf("Price S: %.2f \n", price_s);
      }
      else if (colname == "is_still"){
          //printf("setting is still \n");
          is_still = atoi(argv[i]);
          //printf("Is Still: %d \n", is_still);
      }
      else if (colname == "volume_per_tick"){
          //printf("setting vol per tick \n");
          volume_per_tick = atof(argv[i]);
//          printf("Volume per Tick: %.2f \n", volume_per_tick);
      }
      else if (colname == "PLU_l"){
          plu_l = argv[i];
//          printf("PLU L: %s \n", plu_l.c_str());
      }
      else if (colname == "PLU_s"){
          plu_s = argv[i];
//          printf("PLU S: %s \n", plu_s.c_str());
      }
      else if (colname == "payment"){
          paymentMethod = argv[i];
//          printf("Payment Method: %s \n", paymentMethod.c_str());
      }

//      printf("\n");

      g_cassettes[slot-1].setDrink(new drink(slot, name, volume_dispensed, volume_target_l, volume_target_s , calibration_const, price_l, price_s, false, volume_per_tick, plu_l, plu_s, paymentMethod, name_receipt));
   }

   return 0;
}

DF_ERROR stateInit::setDrinks(){

   // Drink Setup
   // load the SQLITE manager


    char *zErrMsg = 0;
    int rc;
    //char *sql;
    const char* data = "Callback function called";

    rc = sqlite3_open(DB_PATH, &db);

    debugOutput::sendMessage("DB GETTER START", INFO);

    if( rc ) {
//       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
       // TODO: Error handling here...
    } else {
//       fprintf(stderr, "Opened database successfully\n\n");
    }

    /* Create SQL statement */
    std::string sql11 = "SELECT * from products";
    char *sql = new char[sql11.length() + 1];
    strcpy(sql, sql11.c_str());

    //printf("\n______________PRODUCTS_______________\n\n");


    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);

    if( rc != SQLITE_OK ) {
//       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
    } else {
//       fprintf(stdout, "Operation done successfully\n");
    }
    sqlite3_close(db);

//    printf("\n_____________________________________\n\n");


   // FIXME: Hardcode for now.
   

   // for(int i = 0; i < MAX_CASSETTES; i++) {
   //    g_cassettes[i]->setDrink()
   // }
   // Hardcoded drink class for testing


                            // drink(int slot, string name, double nVolumeDispensed, double nVolumeTarget, double calibration_const, double price, bool isStillDrink, double nVolumePerTick)
//   g_cassettes[0].setDrink(new drink(1, "Drink1", 0, 1500, 1.3, 3.75, false, 1));
//   g_cassettes[1].setDrink(new drink(2, "Drink2", 0, 400, 1.3, 2.20, false, 1));
//   g_cassettes[2].setDrink(new drink(3, "Drink3", 0, 200, 1.3, 2.00, false, 1));
//   g_cassettes[3].setDrink(new drink(4, "Drink4", 0, 500, 1.3, 1.10, false, 1));
//   g_cassettes[4].setDrink(new drink(5, "Drink5", 0, 355, 1.3, 4.00, false, 10));
//   g_cassettes[5].setDrink(new drink(6, "Drink6", 0, 355, 1.3, 4.00, false, 25));
//   g_cassettes[6].setDrink(new drink(7, "Drink7", 0, 355, 1.3, 4.00, false, 10));
//   g_cassettes[7].setDrink(new drink(8, "Drink8", 0, 355, 1.3, 4.00, false, 25));
//   g_cassettes[8].setDrink(new drink(9, "Drink9", 0, 355, 1.3, 4.00, false, 10));

   return OK;
}


