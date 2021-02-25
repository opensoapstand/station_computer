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
#include "../components/mcpgpio.h"

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
   size = m_pMessaging->getnSize();
   pos = pos - 1;

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

   cassettes[pos].resetButtonPressTimes();
   cassettes[pos].resetButtonPressDuration();

   debugOutput::sendMessage("START backing up DB", INFO);
   system("/release/dbbackup.sh");
   system("heroku restart -a soapstand");
   debugOutput::sendMessage("END backing up DB", INFO);

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
    rc = sqlite3_open(DB_PATH, &db);

    debugOutput::sendMessage("DB Update START", INFO);

    if( rc ) {
       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
       // TODO: Error handling here...
    } else {
       fprintf(stderr, "Opened database successfully\n");
    }

     /* Create SQL statement for transactions */
     std::string sql1;
     std::string product = (cassettes[pos].getDrink()->m_name).c_str();
     std::string target_volume = to_string(cassettes[pos].getDrink()->getTargetVolume(size));
     std::string price = to_string(cassettes[pos].getDrink()->getPrice(size));
     std::string start_time = (cassettes[pos].getDrink()->m_nStartTime);
     std::string dispensed_volume = to_string(cassettes[pos].getDrink()->m_nVolumeDispensed);
     std::string buttonPressDuration = to_string(cassettes[pos].getButtonPressDuration());
     std::string buttonPressTimes = to_string(cassettes[pos].getButtonPressTimes());

     sql1 = ("INSERT INTO transactions VALUES (NULL, '" + product + "', " + target_volume + ", " + price + ", '" + start_time + "', " + dispensed_volume + ", datetime('now', 'localtime'), '" + buttonPressDuration + "', '" + buttonPressTimes + "' );");
     //cout << sql1 << endl;

     char *sql_trans = new char[sql1.length() + 1];
     strcpy(sql_trans, sql1.c_str());

     /* Execute SQL statement */
     rc = sqlite3_exec(db, sql_trans, callback, 0, &zErrMsg);

     if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
     } else {
        fprintf(stdout, "Transaction Command Executed successfully\n");
     }

     /* Create SQL statement for total product dispensed */
     std::string sql2;
     sql2 = ("UPDATE products SET total_dispensed=total_dispensed+"+dispensed_volume+" WHERE name='"+product+"';");

     char *sql_prod = new char[sql2.length() + 1];
     strcpy(sql_prod, sql2.c_str());

     /* Execute SQL statement */
     rc = sqlite3_exec(db, sql_prod, callback, 0, &zErrMsg);

     if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
     } else {
        fprintf(stdout, "Product Command Executed successfully\n");
     }

     /* Create SQL statement for product remaining */
     std::string sql3;
     sql3 = ("UPDATE products SET remaining_ml=full_ml-total_dispensed WHERE name='"+product+"';");

     char *sql_prod2 = new char[sql3.length() + 1];
     strcpy(sql_prod2, sql3.c_str());

     /* Execute SQL statement */
     rc = sqlite3_exec(db, sql_prod2, callback, 0, &zErrMsg);

     if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
     } else {
        fprintf(stdout, "Product Remaining Command Executed successfully\n");
     }

    sqlite3_close(db);

}

DF_ERROR stateDispenseEnd::printer(){

    char cost2[MAX_BUF];
    char volume2[MAX_BUF];
    //char name2[MAX_BUF];

    snprintf(cost2, sizeof(cost2), "%.2f", cassettes[pos].getDrink()->getPrice(size));

    string cost = (cost2);

    std::string name = (cassettes[pos].getDrink()->m_name);
    std::string plu;

    size = m_pMessaging->getnSize();

    if (size == 'l'){
        plu = (cassettes[pos].getDrink()->m_nPLU_l);
        snprintf(volume2, sizeof(volume2), "%.0f", cassettes[pos].getDrink()->m_nVolumeTarget_l);
    }
    else{
        plu = (cassettes[pos].getDrink()->m_nPLU_s);
        snprintf(volume2, sizeof(volume2), "%.0f", cassettes[pos].getDrink()->m_nVolumeTarget_s);
    }

    string volume = (volume2);

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(now, 50, "%F %T", timeinfo);

    string printerstring = name +"\nPrice: $" + cost + " \nVolume: " + volume + "ml \nTime: " + now;
    string sysstring = "echo '\n---------------------------\n\n\n"+printerstring+"' > /dev/ttyS4";

    Adafruit_Thermal* printerr = new Adafruit_Thermal();

    system(sysstring.c_str());
    printerr->setBarcodeHeight(100);
    printerr->printBarcode(plu.c_str(), EAN13);
    system("echo '\n\n\n---------------------------\n\n\n' > /dev/ttyS4");


}


