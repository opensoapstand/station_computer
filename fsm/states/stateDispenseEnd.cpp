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
   size = m_pMessaging->getnSize();
   pos = pos - 1;

   //cassettes[pos].getDrink()->stopDispense();
   cassettes[pos].stopDispense(pos);

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

   std::string paymentMethod = cassettes[pos].getDrink()->getPaymentMethod();

   // Currently only Drinkfill used the tap method of payment, so this checks if it is a tap payment system and runs the cleaning cycle if it is...
   // TODO: Change this to just check if the system is Soapstand or Drinkfill instead of payment system!
   if (paymentMethod == "tap"){
       sleep(5);

       debugOutput::sendMessage("Dispense OnEXIT", INFO);
       debugOutput::sendMessage("------Cleaning Mode------", INFO);
       debugOutput::sendMessage("Activating position -> " + to_string(pos + 1) + " solenoid -> WATER", INFO);
       debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(WATER)), INFO);
       debugOutput::sendMessage("Activating position -> " + to_string(pos + 1) + " solenoid -> WATER", INFO);
       debugOutput::sendMessage("Pin -> " + to_string(cassettes[pos].getI2CPin(DRINK)), INFO);

       // Function to clean the Drinkfill nozzle:
       cassettes[pos].cleanNozzle(WATER, AIR);
   }

   // TEST_CHAR is sent during Maintenance Mode dispenses - we do not want to record these in the transaction database, or print receipts...
   if (size != TEST_CHAR){
       updateDB();

       if (paymentMethod == "barcode" || paymentMethod == "plu"){
           debugOutput::sendMessage("Printing receipt", INFO);
           printer();
           sendDB();
       }
   }

   m_pMessaging->clearProcessString();
   m_pMessaging->clearCommandString();
   m_pMessaging->clearcCommand();

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

size_t WriteCallback(char* contents, size_t size, size_t nmemb, void *userp){
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// This function sends the transaction details to the cloud using libcurl, if it fails, it stores the data to be sent in the bufferCurl function
// TODO: This will be replaced with an AWS IoT method!
DF_ERROR stateDispenseEnd::sendDB(){

    std::string product = (cassettes[pos].getDrink()->m_name);
    std::string target_volume = to_string(cassettes[pos].getDrink()->getTargetVolume(size));
    std::string price = to_string(cassettes[pos].getDrink()->getPrice(size));
    std::string start_time = (cassettes[pos].getDrink()->m_nStartTime);
    std::string machine_id = getMachineID();
    std::string pid = getProductID(pos+1);
    char EndTime[50];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(EndTime, 50, "%F %T", timeinfo);
    std::string readBuffer;
    std::string dispensed_volume;

    if (cassettes[pos].getDrink()->m_nVolumeDispensed == cassettes[pos].getDrink()->m_nVolumePerTick){
        dispensed_volume = "0";
    }else{
        dispensed_volume = to_string(cassettes[pos].getDrink()->m_nVolumeDispensed);
    }

    std::string curl_param = "contents="+product+"&quantity_requested="+target_volume+"&quantity_dispensed="+dispensed_volume+"&size_unit=ml&price="+price+"&productId="+pid+"&start_time="+start_time+"&end_time="+EndTime+"&MachineSerialNumber="+machine_id+"&paymentMethod=Printer";
    char buffer[1080];
    strcpy(buffer, curl_param.c_str());

    curl = curl_easy_init();
    if (!curl){
        debugOutput::sendMessage("cURL failed to init", INFO);
    }else{
        debugOutput::sendMessage("cURL init success", INFO);

        curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/pushPrinterOrder");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK){
//            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
//            std::cout << "Curl didn't work, I need to buffer cURL" << endl;
            bufferCURL(curl_param);
            curl_easy_cleanup(curl);
        }else{
            debugOutput::sendMessage("CURL SUCCESS!", INFO);
            if (readBuffer == "true"){
//                std::cout << "Curl worked!" << endl;
            }else{
//                std::cout << "Curl didn't work, I need to buffer cURL" << endl;
                bufferCURL(curl_param);
            }
            readBuffer = "";
            curl_easy_cleanup(curl);
        }
    }
}

//TODO: Figure out when/how to send all the bufferred curl data to the cloud (fixed with AWS IoT?)
void stateDispenseEnd::bufferCURL(std::string curl_params){
    char filetime[50];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(filetime, 50, "%F %T", timeinfo);
    std::string filelocation = "/home/df-admin/curlBuffer/";
    std::string filetype = ".txt";
    std::string filename = filelocation+filetime+filetype;
    std::ofstream out;
    out.open(filename);
    if (!out.is_open()){
//        std::cout << "Cannot open output file!";
    }else{
        out << curl_params;
        out.close();
    }
}

std::string stateDispenseEnd::getProductID(int slot){
    rc = sqlite3_open(DB_PATH, &db);

    sqlite3_stmt * stmt;

    debugOutput::sendMessage("Product ID getter START", INFO);

    if( rc ) {
//       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
       // TODO: Error handling here...
    } else {
//       fprintf(stderr, "Opened database successfully\n");
    }

    std::string sql_string_pid = "SELECT product_id FROM products WHERE slot="+std::to_string(slot)+";";

    /* Create SQL statement for transactions */
     sqlite3_prepare(db, sql_string_pid.c_str(), -1, &stmt, NULL);
     sqlite3_step(stmt);
     std::string str = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));;
     sqlite3_finalize(stmt);
     sqlite3_close(db);
     return str;
}

std::string stateDispenseEnd::getMachineID(){

    rc = sqlite3_open(DB_PATH, &db);

    sqlite3_stmt * stmt;

    debugOutput::sendMessage("Machine ID getter START", INFO);

    if( rc ) {
//       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
       // TODO: Error handling here...
    } else {
//       fprintf(stderr, "Opened database successfully\n");
    }

     /* Create SQL statement for transactions */
     sqlite3_prepare(db, "SELECT machine_id FROM machine;", -1, &stmt, NULL);
     sqlite3_step(stmt);
     std::string str = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));;
     sqlite3_finalize(stmt);
     sqlite3_close(db);
     return str;

}

std::string stateDispenseEnd::getUnits(int slot){
    rc = sqlite3_open(DB_PATH, &db);

    sqlite3_stmt * stmt;

    debugOutput::sendMessage("Units getter START", INFO);

    if( rc ) {
//       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
       // TODO: Error handling here...
    } else {
//       fprintf(stderr, "Opened database successfully\n");
    }

    std::string sql_string_units = "SELECT units FROM products WHERE slot="+std::to_string(slot)+";";

    /* Create SQL statement for transactions */
     sqlite3_prepare(db, sql_string_units.c_str(), -1, &stmt, NULL);
     sqlite3_step(stmt);
     std::string str = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));;
     sqlite3_finalize(stmt);
     sqlite3_close(db);
//     cout << str << endl;
     return str;

}

// This function updates the local SQLite3 database with the transaction data, as well as updates the total product remaining locally
DF_ERROR stateDispenseEnd::updateDB(){
    char *zErrMsg = 0;

    // FIXME: DB needs fully qualified link to find...obscure with XML loading.
    rc = sqlite3_open(DB_PATH, &db);

    debugOutput::sendMessage("DB Update START", INFO);

    if( rc ) {
//       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
       // TODO: Error handling here...
    } else {
//       fprintf(stderr, "Opened database successfully\n");
    }

     /* Create SQL statement for transactions */
     std::string sql1;
     std::string product = (cassettes[pos].getDrink()->m_name).c_str();
     std::string target_volume = to_string(cassettes[pos].getDrink()->getTargetVolume(size));
     std::string price = to_string(cassettes[pos].getDrink()->getPrice(size));
     std::string start_time = (cassettes[pos].getDrink()->m_nStartTime);
     std::string dispensed_volume;
     if (cassettes[pos].getDrink()->m_nVolumeDispensed == cassettes[pos].getDrink()->m_nVolumePerTick){
         dispensed_volume = "0";
     }else{
         dispensed_volume = to_string(cassettes[pos].getDrink()->m_nVolumeDispensed);
     }

     sql1 = ("INSERT INTO transactions VALUES (NULL, '" + product + "', " + target_volume + ", " + price + ", '" + start_time + "', " + dispensed_volume + ", datetime('now', 'localtime'), '" + "0" + "', '" + "0" + "' );");
     //cout << sql1 << endl;

     char *sql_trans = new char[sql1.length() + 1];
     strcpy(sql_trans, sql1.c_str());

     /* Execute SQL statement */
     rc = sqlite3_exec(db, sql_trans, callback, 0, &zErrMsg);

     if( rc != SQLITE_OK ){
//        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
     } else {
//        fprintf(stdout, "Transaction Command Executed successfully\n");
     }

     /* Create SQL statement for total product dispensed */
     std::string sql2;
     sql2 = ("UPDATE products SET total_dispensed=total_dispensed+"+dispensed_volume+" WHERE name='"+product+"';");

     char *sql_prod = new char[sql2.length() + 1];
     strcpy(sql_prod, sql2.c_str());

     /* Execute SQL statement */
     rc = sqlite3_exec(db, sql_prod, callback, 0, &zErrMsg);

     if( rc != SQLITE_OK ){
//        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
     } else {
//        fprintf(stdout, "Product Command Executed successfully\n");
     }

     /* Create SQL statement for product remaining */
     std::string sql3;
     sql3 = ("UPDATE products SET remaining_ml=full_ml-total_dispensed WHERE name='"+product+"';");

     char *sql_prod2 = new char[sql3.length() + 1];
     strcpy(sql_prod2, sql3.c_str());

     /* Execute SQL statement */
     rc = sqlite3_exec(db, sql_prod2, callback, 0, &zErrMsg);

     if( rc != SQLITE_OK ){
//        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
     } else {
//        fprintf(stdout, "Product Remaining Command Executed successfully\n");
     }

    sqlite3_close(db);

}

// This function prints the receipts by calling a system function (could be done better)
DF_ERROR stateDispenseEnd::printer(){
    
    char cost2[MAX_BUF];
    char volume2[MAX_BUF];
    //char name2[MAX_BUF];

    snprintf(cost2, sizeof(cost2), "%.2f", cassettes[pos].getDrink()->getPrice(size));

    string cost = (cost2);

    std::string name = (cassettes[pos].getDrink()->m_name_receipt);
    std::string plu;
    std::string units = getUnits(pos+1);

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

    string printerstring = name +"\nPrice: $" + cost + " \nVolume: " + volume + units + "\nTime: " + now + "\nPLU: " + plu;
    string sysstring = "echo '\n---------------------------\n\n\n"+printerstring+"' > /dev/ttyS4";

    Adafruit_Thermal* printerr = new Adafruit_Thermal();

    system(sysstring.c_str());
    printerr->setBarcodeHeight(100);
    printerr->printBarcode(plu.c_str(), EAN13);
    system("echo '\n---------------------------\n\n\n' > /dev/ttyS4");


}

