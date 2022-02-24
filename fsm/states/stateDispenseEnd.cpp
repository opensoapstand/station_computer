//***************************************
//
// stateDispenseEnd.h
// Dispense End State; Reset for Idle
//
// created: 01-2022
// by:Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
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
stateDispenseEnd::stateDispenseEnd(messageMediator *message)
{
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
    m_state_requested = STATE_DISPENSE_END;
    DF_ERROR e_ret = OK;

    productDispensers = g_productDispensers;
    pos = m_pMessaging->getProductNumber();
    pos = pos - 1;
    size = m_pMessaging->getRequestedVolume();

    productDispensers[pos].stopDispense();

    return e_ret;
}

/*
 * 
 */
DF_ERROR stateDispenseEnd::onAction()
{
    debugOutput::sendMessage("onAction Dispense End...", MSG_STATE);
    m_state_requested = STATE_IDLE;
    // TODO: Log events to DB

    // TODO: Send a complete ACK back to QT
    // m_pMessaging->sendMessage("!");

    DF_ERROR e_ret = OK;

    return e_ret;
}

// Actions on leaving Dispense state
DF_ERROR stateDispenseEnd::onExit()
{
    productDispensers = g_productDispensers;
    DF_ERROR e_ret = OK;
    pos = m_pMessaging->getProductNumber();
    pos = pos - 1;

    std::string paymentMethod = productDispensers[pos].getProduct()->getPaymentMethod();

    // Currently only Drinkfill used the tap method of payment, so this checks if it is a tap payment system and runs the cleaning cycle if it is...
    // TODO: Change this to just check if the system is Soapstand or Drinkfill instead of payment system!
    if (paymentMethod == "tap")
    {
        // sleep(5);

        // debugOutput::sendMessage("Dispense OnEXIT", MSG_INFO);
        // debugOutput::sendMessage("------Cleaning Mode------", MSG_INFO);
        // debugOutput::sendMessage("Activating position -> " + to_string(pos + 1) + " solenoid -> WATER", MSG_INFO);
        // debugOutput::sendMessage("Pin -> " + to_string(productDispensers[pos].getI2CPin(WATER)), MSG_INFO);
        // debugOutput::sendMessage("Activating position -> " + to_string(pos + 1) + " solenoid -> WATER", MSG_INFO);
        // debugOutput::sendMessage("Pin -> " + to_string(productDispensers[pos].getI2CPin(PRODUCT)), MSG_INFO);
    }

    // REQUESTED_VOLUME_CUSTOM is sent during Maintenance Mode dispenses - we do not want to record these in the transaction database, or print receipts...
    // if (size != REQUESTED_VOLUME_CUSTOM)
    // {
    debugOutput::sendMessage("Update database:", MSG_INFO);
    updateDB();

    if (paymentMethod == "barcode" || paymentMethod == "plu")
    {
        debugOutput::sendMessage("Printing receipt:", MSG_INFO);
        print_receipt();
        debugOutput::sendMessage("Send db to cloud:", MSG_INFO);
        sendDbToCloud();
    }
    // }
    return e_ret;
}

static int db_sql_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    // int i;
    // for (i = 0; i < argc; i++)
    // {
    //     printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    // }
    // printf("\n");
    // return 0;
}

size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

// This function sends the transaction details to the cloud using libcurl, if it fails, it stores the data to be sent in the bufferCurl function
// TODO: This will be replaced with an AWS IoT method!
DF_ERROR stateDispenseEnd::sendDbToCloud()
{

    std::string product = (productDispensers[pos].getProduct()->m_name);
    std::string target_volume = to_string(productDispensers[pos].getProduct()->getTargetVolume(size));
    std::string price = to_string(productDispensers[pos].getProduct()->getPrice(size));
    std::string start_time = (productDispensers[pos].getProduct()->m_nStartTime);
    std::string machine_id = getMachineID();
    std::string pid = getProductID(pos + 1);
    char EndTime[50];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(EndTime, 50, "%F %T", timeinfo);
    std::string readBuffer;
    std::string dispensed_volume;

    if (productDispensers[pos].getProduct()->m_nVolumeDispensed == productDispensers[pos].getProduct()->m_nVolumePerTick)
    {
        dispensed_volume = "0";
    }
    else
    {
        dispensed_volume = to_string(productDispensers[pos].getProduct()->m_nVolumeDispensed);
    }

    // todo Lode check with Ash
    std::string curl_param = "contents=" + product + "&quantity_requested=" + target_volume + "&quantity_dispensed=" + dispensed_volume + "&display_unit=ml&price=" + price + "&productId=" + pid + "&start_time=" + start_time + "&end_time=" + EndTime + "&MachineSerialNumber=" + machine_id + "&paymentMethod=Printer";
    char buffer[1080];
    strcpy(buffer, curl_param.c_str());

    curl = curl_easy_init();
    if (!curl)
    {
        debugOutput::sendMessage("cURL failed to init", MSG_INFO);
    }
    else
    {
        debugOutput::sendMessage("cURL init success", MSG_INFO);

        curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/pushPrinterOrder");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            //            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            //            std::cout << "Curl didn't work, I need to buffer cURL" << endl;
            bufferCURL(curl_param);
            curl_easy_cleanup(curl);
        }
        else
        {
            debugOutput::sendMessage("CURL SUCCESS!", MSG_INFO);
            if (readBuffer == "true")
            {
                //                std::cout << "Curl worked!" << endl;
            }
            else
            {
                //                std::cout << "Curl didn't work, I need to buffer cURL" << endl;
                bufferCURL(curl_param);
            }
            readBuffer = "";
            curl_easy_cleanup(curl);
        }
    }
}

//TODO: Figure out when/how to send all the bufferred curl data to the cloud (fixed with AWS IoT?)
void stateDispenseEnd::bufferCURL(std::string curl_params)
{
    char filetime[50];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(filetime, 50, "%F %T", timeinfo);
    std::string filelocation = "/home/df-admin/curlBuffer/";
    std::string filetype = ".txt";
    std::string filename = filelocation + filetime + filetype;
    std::ofstream out;
    out.open(filename);
    if (!out.is_open())
    {
        //        std::cout << "Cannot open output file!";
    }
    else
    {
        out << curl_params;
        out.close();
    }
}

std::string stateDispenseEnd::getProductID(int slot)
{
    rc = sqlite3_open(DB_PATH, &db);

    sqlite3_stmt *stmt;

    debugOutput::sendMessage("Product ID getter START", MSG_INFO);

    std::string sql_string_pid = "SELECT product_id FROM products WHERE slot=" + std::to_string(slot) + ";";

    /* Create SQL statement for transactions */
    sqlite3_prepare(db, sql_string_pid.c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);
    std::string str = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
    ;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return str;
}

std::string stateDispenseEnd::getMachineID()
{

    rc = sqlite3_open(DB_PATH, &db);

    sqlite3_stmt *stmt;

    debugOutput::sendMessage("Machine ID getter START", MSG_INFO);

    if (rc)
    {
        //       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        // TODO: Error handling here...
    }
    else
    {
        //       fprintf(stderr, "Opened database successfully\n");
    }

    /* Create SQL statement for transactions */
    sqlite3_prepare(db, "SELECT machine_id FROM machine;", -1, &stmt, NULL);
    sqlite3_step(stmt);
    std::string str = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
    ;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return str;
}

std::string stateDispenseEnd::getUnits(int slot){
    productDispensers[slot-1].getProduct()->getDisplayUnits();
}

std::string stateDispenseEnd::getUnitsFromDb(int slot)
{
    rc = sqlite3_open(DB_PATH, &db);

    sqlite3_stmt *stmt;

    debugOutput::sendMessage("Units getter START", MSG_INFO);

    if (rc)
    {
        //       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        // TODO: Error handling here...
    }
    else
    {
        //       fprintf(stderr, "Opened database successfully\n");
    }

    std::string sql_string_units = "SELECT units FROM products WHERE slot=" + std::to_string(slot) + ";";

    /* Create SQL statement for transactions */
    sqlite3_prepare(db, sql_string_units.c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);
    std::string str = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
    ;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    //     cout << str << endl;
    return str;
}

// This function updates the local SQLite3 database with the transaction data, as well as updates the total product remaining locally
DF_ERROR stateDispenseEnd::updateDB()
{
    char *zErrMsg = 0;

    // FIXME: DB needs fully qualified link to find...obscure with XML loading.
    rc = sqlite3_open(DB_PATH, &db);

    debugOutput::sendMessage("DB Update START", MSG_INFO);

    if (rc)
    {
        //       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        // TODO: Error handling here...
    }
    else
    {
        //       fprintf(stderr, "Opened database successfully\n");
    }

    /* Create SQL statement for transactions */
    std::string sql1;
    std::string product = (productDispensers[pos].getProduct()->m_name).c_str();
    std::string target_volume = to_string(productDispensers[pos].getProduct()->getTargetVolume(size));
    std::string price = to_string(productDispensers[pos].getProduct()->getPrice(size));
    std::string start_time = (productDispensers[pos].getProduct()->m_nStartTime);
    std::string dispensed_volume;
    debugOutput::sendMessage("dispenseenddbadd: vol dispensed: " + to_string(productDispensers[pos].getProduct()->getVolumeDispensed()), MSG_INFO);
    
    if (productDispensers[pos].getProduct()->m_nVolumeDispensed <= productDispensers[pos].getProduct()->m_nVolumePerTick)
    {
        dispensed_volume = "0";
    }
    else
    {
        dispensed_volume = to_string(productDispensers[pos].getProduct()->m_nVolumeDispensed);
    }

    sql1 = ("INSERT INTO transactions VALUES (NULL, '" + product + "', " + target_volume + ", " + price + ", '" + start_time + "', " + dispensed_volume + ", datetime('now', 'localtime'), '" + "0" + "', '" + "0" + "' );");
    //cout << sql1 << endl;

    char *sql_trans = new char[sql1.length() + 1];
    strcpy(sql_trans, sql1.c_str());

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql_trans, db_sql_callback, 0, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        // fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        //        fprintf(stdout, "Transaction Command Executed successfully\n");
    }

    /* Create SQL statement for total product dispensed */
    std::string sql2;
    sql2 = ("UPDATE products SET volume_dispensed_total=volume_dispensed_total+" + dispensed_volume + " WHERE name='" + product + "';");

    char *sql_prod = new char[sql2.length() + 1];
    strcpy(sql_prod, sql2.c_str());

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql_prod, db_sql_callback, 0, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        //        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        //        fprintf(stdout, "Product Command Executed successfully\n");
    }

    /* Create SQL statement for product remaining */
    std::string sql3;
    sql3 = ("UPDATE products SET volume_remaining=volume_full-volume_dispensed_total WHERE name='" + product + "';");

    char *sql_prod2 = new char[sql3.length() + 1];
    strcpy(sql_prod2, sql3.c_str());

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql_prod2, db_sql_callback, 0, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        //        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        //        fprintf(stdout, "Product Remaining Command Executed successfully\n");
    }

    sqlite3_close(db);
}

// This function prints the receipts by calling a system function (could be done better)
DF_ERROR stateDispenseEnd::print_receipt()
{
    // printerr.connectToPrinter();
    debugOutput::sendMessage("-- 1", MSG_INFO);
    char cost2[MAX_BUF];
    char chars_volume_formatted[MAX_BUF];
    //char name2[MAX_BUF];

    debugOutput::sendMessage("-- 2", MSG_INFO);
    snprintf(cost2, sizeof(cost2), "%.2f", productDispensers[pos].getProduct()->getPrice(size));

    string cost = (cost2);

    debugOutput::sendMessage("-- 3", MSG_INFO);
    std::string name = (productDispensers[pos].getProduct()->m_name_receipt);
    std::string plu;
    debugOutput::sendMessage("-- 4", MSG_INFO);
    std::string units = getUnits(pos + 1);

    debugOutput::sendMessage("-- 5", MSG_INFO);
    size = m_pMessaging->getRequestedVolume();
    debugOutput::sendMessage("-- 6", MSG_INFO);

    if (size == 's')
    {
        plu = (productDispensers[pos].getProduct()->m_nPLU_small);
        snprintf(chars_volume_formatted, sizeof(chars_volume_formatted), "%.0f", productDispensers[pos].getProduct()->m_nVolumeTarget_s);
    }
    else if (size == 'm')
    {
        plu = (productDispensers[pos].getProduct()->m_nPLU_medium);
        snprintf(chars_volume_formatted, sizeof(chars_volume_formatted), "%.0f", productDispensers[pos].getProduct()->m_nVolumeTarget_m);
    }
    if (size == 'l')
    {
        plu = (productDispensers[pos].getProduct()->m_nPLU_large);
        snprintf(chars_volume_formatted, sizeof(chars_volume_formatted), "%.0f", productDispensers[pos].getProduct()->m_nVolumeTarget_l);
    }
    if (size == 't')
    {
        plu = (productDispensers[pos].getProduct()->m_nPLU_large);
        snprintf(chars_volume_formatted, sizeof(chars_volume_formatted), "%.0f", productDispensers[pos].getDispensedVolume());
    }

    debugOutput::sendMessage("-- 7", MSG_INFO);
    string receipt_volume_formatted = (chars_volume_formatted);

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(now, 50, "%F %T", timeinfo);

    string printerstring = name + "\nPrice: $" + cost + " \nVolume: " + receipt_volume_formatted + units + "\nTime: " + now + "\nPLU: " + plu;
    string sysstring = "echo '\n---------------------------\n\n\n" + printerstring + "' > /dev/ttyS4";


    Adafruit_Thermal *printerr = new Adafruit_Thermal();
    printerr->connectToPrinter();

    system(sysstring.c_str());
    printerr->setBarcodeHeight(100);

    printerr->printBarcode(plu.c_str(), EAN13);

    system("echo '\n---------------------------\n\n\n' > /dev/ttyS4");
    printerr->disconnectPrinter();
}
