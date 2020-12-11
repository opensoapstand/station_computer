//***************************************
//
// stateDispenseEnd.h
// Dispense End State; Reset for Idle
//
// created: 26-06-2020
// by: Jason Wang & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "stateDispenseEnd.h"

#define DISPENSE_END_STRING "Dispense End"

#define MAX_BUF 64

// CTOR
stateDispenseEnd::stateDispenseEnd()
{

}

// CTOR Linked to IPC
stateDispenseEnd::stateDispenseEnd(messageMediator * message){
   debugOutput::sendMessage("stateDispenseEnd(messageMediator * message)", INFO);
}

// DTOR
stateDispenseEnd::~stateDispenseEnd()
{

}

// Overload for Debugger output
string stateDispenseEnd::toString()
{
   return DISPENSE_END_STRING;
}

/*
 * Called from FSM loop to check state before inAction execution
 * TODO: Create State Class to hold this function definition
 */ 
DF_ERROR stateDispenseEnd::onEntry()
{
   DF_ERROR e_ret  = OK;

   debugOutput::sendMessage("Entering Dispense End...", STATE_CHANGE);

   cassettes = g_cassettes;
   pos = m_pMessaging->getnOption();
   pos = pos - 1;

   updateDB();

   //cassettes[pos].getDrink()->stopDispense();
   cassettes[pos].stopDispense(DRINK);

      
   return e_ret;
}

/*
 * 
 */
DF_ERROR stateDispenseEnd::onAction()
{
   DF_ERROR e_ret  = ERROR_BAD_PARAMS;
   m_pMessaging->getPositionReady();
   command = m_pMessaging->getcCommand();
   if (nullptr != &m_nextState)
   {
      switch (command)
      {
      case DRINK_CHAR:
         /* code */
         m_pMessaging->clearCommandString();
         m_pMessaging->clearcCommand();
         m_nextState = DISPENSE_END;
         sleep(2);
         break;

      case DISPENSE_END_CHAR:
         debugOutput::sendMessage("Exit", INFO);
         m_nextState = IDLE;
         break;
      
      default:
         break;
      }

      // TODO: Log events to DB

      // TODO: Send a complete ACK back to QT
      // m_pMessaging->sendMessage("!");


      e_ret = OK;
   }

   return e_ret;
}

// Actions on leaving Dispense state
DF_ERROR stateDispenseEnd::onExit()
{
   cassettes = g_cassettes;
   DF_ERROR e_ret = OK;
   pos = m_pMessaging->getnOption();
   pos = pos - 1;

//   sleep(5);
//   debugOutput::sendMessage("Dispense OnEXIT", INFO);
//   debugOutput::sendMessage("------Cleaning Mode------", INFO);
//   debugOutput::sendMessage("Activating position -> " + to_string(pos + 1) + " solenoid -> WATER", INFO);
//   debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(WATER)), INFO);
//   debugOutput::sendMessage("Activating position -> " + to_string(pos + 1) + " solenoid -> WATER", INFO);
//   debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(DRINK)), INFO);

   //cassettes[pos].cleanNozzle(WATER, AIR);


   updateDB();

   m_pMessaging->clearProcessString();
   m_pMessaging->clearCommandString();
   m_pMessaging->clearcCommand();

   printer();

   cassettes[pos].getDrink()->stopDispense();
   cassettes[pos].stopDispense(DRINK);



   debugOutput::sendMessage("Exiting Dispensing END[" + toString() + "]", INFO);

   // TODO: Does not seem to advance to Idle again...
   m_state = DISPENSE_END;
   m_nextState = IDLE; //go back for now



   return e_ret;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

DF_ERROR stateDispenseEnd::updateDB(){
    char *zErrMsg = 0;

    // FIXME: DB needs fully qualified link to find...obscure with XML loading.
    rc = sqlite3_open("/home/df-admin/drinkfill/db/sqlite/drinkfill-sqlite.db", &db);

    debugOutput::sendMessage("DB Update START", INFO);

    if( rc ) {
       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
       // TODO: Error handling here...
    } else {
       fprintf(stderr, "Opened database successfully\n");
    }

     /* Create SQL statement */
     std::string sql1;
     sql1 = ("INSERT INTO transactions VALUES (NULL, " + to_string(pos+1) + ", " + to_string(cassettes[pos].getDrink()->getTargetVolume()) + ", " + to_string(cassettes[pos].getDrink()->m_price) + ", '" + (cassettes[pos].getDrink()->m_nStartTime) + "', " + to_string(cassettes[pos].getDrink()->m_nVolumeDispensed) + ", datetime('now', 'localtime'));");

     char *sql = new char[sql1.length() + 1];
     strcpy(sql, sql1.c_str());

     /* Execute SQL statement */
     rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

     if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
     } else {
        fprintf(stdout, "Command Executed successfully\n");
     }

    sqlite3_close(db);
}

DF_ERROR stateDispenseEnd::printer(){

    char cost2[MAX_BUF];
    char volume2[MAX_BUF];

    snprintf(cost2, sizeof(cost2), "%.2f", cassettes[pos].getDrink()->m_price);
    snprintf(volume2, sizeof(volume2), "%.2f", cassettes[pos].getDrink()->m_nVolumeDispensed);

    string cost = (cost2);
    string volume = (volume2);

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(now, 50, "%F %T", timeinfo);

    string printerstring = "Price: $" + cost + " Volume: " + volume + "ml Time: " + now;

    debugOutput::sendMessage(printerstring, INFO);

}


