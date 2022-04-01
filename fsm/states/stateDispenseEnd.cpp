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
#include <cmath>
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
    pos = m_pMessaging->getRequestedSlot();
    pos = pos - 1;
    // size = m_pMessaging->getRequestedSize();

    return e_ret;
}

/*
 *
 */
DF_ERROR stateDispenseEnd::onAction()
{
    debugOutput::sendMessage("onAction Dispense End...", MSG_STATE);

    DF_ERROR e_ret = OK;

    productDispensers[pos].stopDispense();

    // give time to settle down
    usleep(100000);

    // send dispensed volume to ui (will be used to write to portal)
    m_pMessaging->sendMessage(to_string(productDispensers[pos].getVolumeDispensed()));

    if (productDispensers[pos].getIsDispenseTargetReached())
    {
        m_pMessaging->sendMessage("Target Hit");
    }

    adjustSizeToDispensedVolume();

    bool is_valid_dispense = productDispensers[pos].getVolumeDispensed() >= MINIMUM_DISPENSE_VOLUME_ML;

    // SIZE_CUSTOM is sent during Maintenance Mode dispenses - we do not want to record these in the transaction database, or print receipts...
    if (m_pMessaging->getRequestedSize() == SIZE_TEST)
    {

        debugOutput::sendMessage("Test dispensing. (" + to_string(productDispensers[pos].getVolumeDispensed()) + "ml). No transaction will be made.", MSG_INFO);
        dispenseEndUpdateDB(true);
    }
    else if (!is_valid_dispense)
    {
        debugOutput::sendMessage("No minimum quantity of product dispensed (" + to_string(productDispensers[pos].getVolumeDispensed()) + "ml). No transaction will be made.", MSG_INFO);
    }
    else
    {
        e_ret = handleTransaction();
    }

    m_state_requested = STATE_IDLE;
    // TODO: Log events to DB

    // TODO: Send a complete ACK back to QT

    m_pMessaging->sendMessage("Transaction End");

    return e_ret;
}

void stateDispenseEnd::adjustSizeToDispensedVolume()
{
    // check requested volume versus dispensed volume.
    if (m_pMessaging->getRequestedSize() == SIZE_INVOLUNTARY_END)
    {
        // go down to next next allowed volume

        if (productDispensers[pos].getProduct()->getIsSizeEnabled(SIZE_CUSTOM_CHAR))
        {
            m_pMessaging->setRequestedSize(SIZE_CUSTOM_CHAR);
            debugOutput::sendMessage("Empty container detected, change to custom volume.", MSG_INFO);
        }
        else
        {
            char new_size = dispensedVolumeToSmallestFixedSize();
            m_pMessaging->setRequestedSize(new_size);
            debugOutput::sendMessage("Empty container detected, change to next lowest size: " + to_string(new_size), MSG_INFO);
        }
    }
    else if (m_pMessaging->getRequestedSize() == SIZE_CUSTOM_CHAR)
    {
        // debugOutput::sendMessage("custom volume dispensing.... ", MSG_INFO);
    }
    else
    {

// WARNING: if enabled, customers can cheat. e.g. they can dispense 1.4L for a 1.5 liter dispense request. And then only pay for the next smaller volume (e.g. medium =1L)
// --> I would advise against enabling it.
//#define ENABLE_CUSTOMERS_CAN_PAY_FOR_LESS_THAN_THEY_DISPENSE
#ifdef ENABLE_CUSTOMERS_CAN_PAY_FOR_LESS_THAN_THEY_DISPENSE
        debugOutput::sendMessage("Normal fixed price dispensing. Will take the lowest fixed size compared to the actual dispensed volume.", MSG_INFO);
        char new_size = dispensedVolumeToSmallestFixedSize();
        m_pMessaging->setRequestedSize(new_size);
#endif
    }
}

char stateDispenseEnd::dispensedVolumeToSmallestFixedSize()
{
    // check real dispensed volume compared to available fixed sizes.
    // if dispensed volume is lower than small size, will return "SIZE_SMALLER_THAN_SMALL"

    double dispensed_volume = productDispensers[pos].getVolumeDispensed();

    char sizes_big_to_small[3] = {SIZE_LARGE_CHAR, SIZE_MEDIUM_CHAR, SIZE_SMALL_CHAR};
    char fixed_size;
    double fixed_volume;
    char lowest_fixed_size = SIZE_SMALLER_THAN_SMALL;
    for (int i = 0; i < 3; i++)
    {
        fixed_size = sizes_big_to_small[i];
        if (productDispensers[pos].getProduct()->getIsSizeEnabled(fixed_size))
        {
            // debugOutput::sendMessage("check volume (dispensed vs target volume)" + to_string(fixed_size), MSG_INFO);
            // as long as the fixed volume is higher than the dispensed volume, go to next lowest size
            fixed_volume = productDispensers[pos].getProduct()->getTargetVolume(fixed_size);

            lowest_fixed_size = fixed_size;
            // debugOutput::sendMessage("lloooowest: " + to_string(lowest_fixed_size), MSG_INFO);
            if (dispensed_volume >= fixed_volume)
            {
                break;
            }
        }
    }
    if (lowest_fixed_size == SIZE_SMALLER_THAN_SMALL)
    {
        debugOutput::sendMessage("WARNING: SIZE SMALLER THAN SMALL DETECTED", MSG_INFO);
        // lowest_fixed_size =SIZE_SMALL_CHAR;
    }
    return lowest_fixed_size;
}

DF_ERROR stateDispenseEnd::handleTransaction()
{
    debugOutput::sendMessage("Update database:", MSG_INFO);
    dispenseEndUpdateDB(false);

#ifdef ENABLE_TRANSACTION_TO_CLOUD

    debugOutput::sendMessage("Send transaction to cloud:", MSG_INFO);
    sendTransactionToCloud();
#else

    debugOutput::sendMessage("NOT SENDING transaction to cloud:", MSG_INFO);
#endif

    debugOutput::sendMessage("Handle transaction.", MSG_INFO);
    DF_ERROR e_ret = OK;

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

    if (paymentMethod == "barcode" || paymentMethod == "plu")
    {
        debugOutput::sendMessage("Printing receipt:", MSG_INFO);
        print_receipt();
    }

    return e_ret;
}

// Actions on leaving Dispense state
DF_ERROR stateDispenseEnd::onExit()
{
    DF_ERROR e_ret = OK;

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
DF_ERROR stateDispenseEnd::sendTransactionToCloud()
{

    DF_ERROR e_ret = OK;

    std::string product = (productDispensers[pos].getProduct()->m_name);
    std::string target_volume = to_string(productDispensers[pos].getProduct()->getTargetVolume(m_pMessaging->getRequestedSize()));
    std::string price = to_string(productDispensers[pos].getProduct()->getPrice(m_pMessaging->getRequestedSize()));
    std::string start_time = productDispensers[pos].getDispenseStartTime();
    std::string machine_id = getMachineID();
    std::string pid = getProductID(pos + 1);
    std::string units = productDispensers[pos].getProduct()->getDisplayUnits();
    char EndTime[50];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(EndTime, 50, "%F %T", timeinfo);
    std::string readBuffer;
    std::string dispensed_volume_units_converted;

    if (productDispensers[pos].getVolumeDispensed() == productDispensers[pos].getProduct()->getVolumePerTick())
    {
        dispensed_volume_units_converted = "0";
    }
    else
    {
        double dv = productDispensers[pos].getVolumeDispensed();
        dv = productDispensers[pos].getProduct()->convertVolumeMetricToDisplayUnits(dv);
        dispensed_volume_units_converted = to_string(dv);
    }

    // todo Lode check with Ash
#ifdef USE_OLD_DATABASE
    std::string curl_param = "contents=" + product + "&quantity_requested=" + target_volume + "&quantity_dispensed=" + dispensed_volume_units_converted + "&units=" + units + "&price=" + price + "&productId=" + pid + "&start_time=" + start_time + "&end_time=" + EndTime + "&MachineSerialNumber=" + machine_id + "&paymentMethod=Printer";
#else
    std::string curl_param = "contents=" + product + "&quantity_requested=" + target_volume + "&quantity_dispensed=" + dispensed_volume_units_converted + "&size_unit=" + units + "&price=" + price + "&productId=" + pid + "&start_time=" + start_time + "&end_time=" + EndTime + "&MachineSerialNumber=" + machine_id + "&paymentMethod=Printer";
#endif
    char buffer[1080];
    strcpy(buffer, curl_param.c_str());

    curl = curl_easy_init();
    if (!curl)
    {
        debugOutput::sendMessage("cURL failed to init", MSG_INFO);
        return e_ret;
    }

    debugOutput::sendMessage("cURL init success", MSG_INFO);

    curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/pushPrinterOrder");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);

    res = curl_easy_perform(curl);
    if (res == CURLE_OPERATION_TIMEDOUT)
    {
        debugOutput::sendMessage("sendTransactionToCloud: CURL timed out (" + to_string(CURLOPT_TIMEOUT_MS) + "ms). err: " + to_string(res) + " Will buffer!", MSG_INFO);
        bufferCURL(curl_param);
        curl_easy_cleanup(curl);
    }
    else if (res != CURLE_OK)
    {
        debugOutput::sendMessage("sendTransactionToCloud: CURL fail. err: " + to_string(res) + " Will buffer!", MSG_INFO);
        bufferCURL(curl_param);
        curl_easy_cleanup(curl);
    }
    else
    {
        if (readBuffer == "true")
        {
            debugOutput::sendMessage("sendTransactionToCloud: CURL succes.", MSG_INFO);
        }
        else
        {
            debugOutput::sendMessage("sendTransactionToCloud: CURL readbuffer fail.", MSG_INFO);
            //                std::cout << "Curl didn't work, I need to buffer cURL" << endl;
            bufferCURL(curl_param);
        }
        readBuffer = "";
        curl_easy_cleanup(curl);
    }

    return e_ret;
}

// TODO: Figure out when/how to send all the bufferred curl data to the cloud (fixed with AWS IoT?)
void stateDispenseEnd::bufferCURL(std::string curl_params)
{
    char filetime[50];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(filetime, 50, "%F %T", timeinfo);
    std::string filelocation = "/home/df-admin/production/logging/transactions/";
    std::string filetype = ".txt";
    std::string filename = filelocation + filetime + filetype;
    std::ofstream out;
    out.open(filename);
    if (!out.is_open())
    {
        debugOutput::sendMessage("Cannot open output file at " + filename + " content: " + curl_params, MSG_INFO);
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

#ifdef USE_OLD_DATABASE
    std::string sql_string_pid = "SELECT product_id FROM products WHERE slot=" + std::to_string(slot) + ";";
#else
    std::string sql_string_pid = "SELECT productId FROM products WHERE slot=" + std::to_string(slot) + ";";

#endif

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

std::string stateDispenseEnd::getUnits(int slot)
{

    // does not work.... strings in C++ Lode needs a course!
    // return productDispensers[slot-1].getProduct()->getDisplayUnits();
    return "problemski";
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

#ifdef USE_OLD_DATABASE
    std::string sql_string_units = "SELECT units FROM products WHERE slot=" + std::to_string(slot) + ";";
#else
    std::string sql_string_units = "SELECT size_unit FROM products WHERE slot=" + std::to_string(slot) + ";";

#endif
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
DF_ERROR stateDispenseEnd::dispenseEndUpdateDB(bool test_transaction)
{
    char *zErrMsg = 0;

    // FIXME: DB needs fully qualified link to find...obscure with XML loading.
    rc = sqlite3_open(DB_PATH, &db);

    debugOutput::sendMessage("DB Update START", MSG_INFO);

    if (rc)
    {
        // TODO: Error handling here...
    }
    else
    {
        //       fprintf(stderr, "Opened database successfully\n");
    }
    std::string price;
    /* Create SQL statement for transactions */

    if (test_transaction)
    {
        price = "0";
    }
    else
    {
        price = to_string(productDispensers[pos].getProduct()->getPrice(m_pMessaging->getRequestedSize()));
    }

    std::string sql1;
    std::string product = (productDispensers[pos].getProduct()->m_name).c_str();
    std::string target_volume = to_string(productDispensers[pos].getProduct()->getTargetVolume(m_pMessaging->getRequestedSize()));
    std::string start_time = (productDispensers[pos].getDispenseStartTime());
    std::string dispensed_volume;

    debugOutput::sendMessage("update DB. dispense end: vol dispensed: " + to_string(productDispensers[pos].getVolumeDispensed()), MSG_INFO);

    if (productDispensers[pos].getVolumeDispensed() <= productDispensers[pos].getProduct()->getVolumePerTick())
    {
        dispensed_volume = "0";
    }
    else
    {
        dispensed_volume = to_string(productDispensers[pos].getVolumeDispensed());
    }
    debugOutput::sendMessage("Dispensed volume to be subtracted: " + dispensed_volume, MSG_INFO);

    sql1 = ("INSERT INTO transactions VALUES (NULL, '" + product + "', " + target_volume + ", " + price + ", '" + start_time + "', " + dispensed_volume + ", datetime('now', 'localtime'), '" + "0" + "', '" + "0" + "' );");
    // cout << sql1 << endl;

    char *sql_trans = new char[sql1.length() + 1];
    strcpy(sql_trans, sql1.c_str());

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql_trans, db_sql_callback, 0, &zErrMsg);

    if (rc != SQLITE_OK)
    {

        // fprintf(stderr, "SQL error: %s\n", zErrMsg);

        // INCORPORATE: zErrMsg !!!
        debugOutput::sendMessage("ERROR: SQL : (err number " + to_string(rc) + ") " + sql1, MSG_INFO);
        sqlite3_free(zErrMsg);
    }
    else
    {
        debugOutput::sendMessage("SUCCES: SQL1 : (" + to_string(rc) + ") " + sql1, MSG_INFO);
        //  fprintf(stdout, "Transaction Command Executed successfully\n");
    }

    /* Create SQL statement for product dispensed since restock */

#ifndef USE_OLD_DATABASE

    std::string sql21;
    sql21 = ("UPDATE products SET volume_dispensed_since_restock=volume_dispensed_since_restock+" + dispensed_volume + " WHERE name='" + product + "';");

    char *sql_prod21 = new char[sql21.length() + 1];
    strcpy(sql_prod21, sql21.c_str());

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql_prod21, db_sql_callback, 0, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        //        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        debugOutput::sendMessage("ERROR: SQL2 : (" + to_string(rc) + "):" + sql21, MSG_INFO);

        sqlite3_free(zErrMsg);
    }
    else
    {
        debugOutput::sendMessage("SUCCES: SQL2 : (" + to_string(rc) + ") " + sql21, MSG_INFO);
    }
#endif

    /* Create SQL statement for total product dispensed */
    std::string sql2;

#ifdef USE_OLD_DATABASE
    sql2 = ("UPDATE products SET total_dispensed=total_dispensed+" + dispensed_volume + " WHERE name='" + product + "';");
#else
    sql2 = ("UPDATE products SET volume_dispensed_total=volume_dispensed_total+" + dispensed_volume + " WHERE name='" + product + "';");

#endif
    char *sql_prod = new char[sql2.length() + 1];
    strcpy(sql_prod, sql2.c_str());

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql_prod, db_sql_callback, 0, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        //        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        debugOutput::sendMessage("ERROR: SQL2 : (" + to_string(rc) + "):" + sql2, MSG_INFO);

        sqlite3_free(zErrMsg);
    }
    else
    {
        debugOutput::sendMessage("SUCCES: SQL2 : (" + to_string(rc) + ") " + sql2, MSG_INFO);
    }

    /* Create SQL statement for product remaining */
    std::string sql3;
#ifdef USE_OLD_DATABASE
    sql3 = ("UPDATE products SET remaining_ml=full_ml-total_dispensed WHERE name='" + product + "';");
#else
    sql3 = ("UPDATE products SET volume_remaining=volume_full-volume_dispensed_since_restock WHERE name='" + product + "';");

#endif

    char *sql_prod2 = new char[sql3.length() + 1];
    strcpy(sql_prod2, sql3.c_str());

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql_prod2, db_sql_callback, 0, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        //        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        debugOutput::sendMessage("ERROR: SQL : " + sql3, MSG_INFO);
        sqlite3_free(zErrMsg);
    }
    else
    {
        debugOutput::sendMessage("SUCCES: SQL3 : (" + to_string(rc) + ") " + sql3, MSG_INFO);
    }

    sqlite3_close(db);
}

// This function prints the receipts by calling a system function (could be done better)
DF_ERROR stateDispenseEnd::print_receipt()
{
    // printerr.connectToPrinter();
    char chars_cost[MAX_BUF];
    char chars_volume_formatted[MAX_BUF];
    char chars_price_per_liter_formatted[MAX_BUF];
    char chars_plu_dynamic_formatted[MAX_BUF];
    string cost = (chars_cost);

    std::string name_receipt = (productDispensers[pos].getProduct()->m_name_receipt);
    std::string plu = productDispensers[pos].getProduct()->getPLU(m_pMessaging->getRequestedSize());
    std::string units = (productDispensers[pos].getProduct()->getDisplayUnits());
    double price = productDispensers[pos].getProduct()->getPrice(m_pMessaging->getRequestedSize());
    double price_per_liter;

    double volume_dispensed;

    debugOutput::sendMessage("aiisisixs  s=" + to_string(SIZE_SMALL_CHAR) + "l=" + to_string(SIZE_LARGE_CHAR) + "set: " + to_string(m_pMessaging->getRequestedSize()), MSG_INFO);

    if (m_pMessaging->getRequestedSize() == 's')
    {
        volume_dispensed = productDispensers[pos].getProduct()->m_nVolumeTarget_s;
    }
    else if (m_pMessaging->getRequestedSize() == 'm')
    {
        volume_dispensed = productDispensers[pos].getProduct()->m_nVolumeTarget_m;
    }
    else if (m_pMessaging->getRequestedSize() == 'l')
    {
        volume_dispensed = productDispensers[pos].getProduct()->m_nVolumeTarget_l;
    }
    else if (m_pMessaging->getRequestedSize() == 'c')
    {
        price_per_liter = productDispensers[pos].getProduct()->getPrice(m_pMessaging->getRequestedSize());
        volume_dispensed = productDispensers[pos].getVolumeDispensed();
        price = price_per_liter * volume_dispensed / 1000.0;
        //@Andi, how do we go higher than 9.99?

        // price = ceil(price * 100);
        // price=price/100;

        // string.Format("{0:f3}", price);
        // std::string test =std::format("{4:f3}", price);
        if (plu.size() != 8)
        {
            // debugOutput::sendMessage("Custom plu: " + plu, MSG_INFO);
            debugOutput::sendMessage("ERROR custom plu length must be of length eight. (standard drinkfill preamble(627987) + 2digit product code) : " + plu, MSG_INFO);
            string fake_plu = "66666666";
            plu = fake_plu;
        }

        snprintf(chars_plu_dynamic_formatted, sizeof(chars_plu_dynamic_formatted), "%5.2f", price);
        string plu_dynamic_price = (chars_plu_dynamic_formatted);
        string plu_dynamic_formatted = plu + plu_dynamic_price;

        // 3.14 --> " 3.14" --> " 314" --> "0314"
        std::string toReplace(".");
        size_t pos = plu_dynamic_formatted.find(toReplace);
        plu_dynamic_formatted.replace(pos, toReplace.length(), "");

        std::string toReplace2(" ");
        pos = plu_dynamic_formatted.find(toReplace2);
        plu_dynamic_formatted.replace(pos, toReplace2.length(), "0");

        plu = plu_dynamic_formatted;
    }
    else if (m_pMessaging->getRequestedSize() == 't')
    {
        price_per_liter = productDispensers[pos].getProduct()->getPrice(m_pMessaging->getRequestedSize());
        volume_dispensed = productDispensers[pos].getVolumeDispensed();
        price = price_per_liter * volume_dispensed / 1000.0;
    }
    else
    {
        debugOutput::sendMessage("invalid size provided" + m_pMessaging->getRequestedSize(), MSG_INFO);
    }

    // convert units
    if (units == "oz")
    {
        // volume_dispensed = ceil(volume_dispensed * ML_TO_OZ);
        volume_dispensed = ceil(productDispensers[pos].getProduct()->convertVolumeMetricToDisplayUnits(volume_dispensed));
        price_per_liter = price_per_liter / 1000 / ML_TO_OZ;
    }

    debugOutput::sendMessage("Volume dispensed for receipt:" + to_string(volume_dispensed), MSG_INFO);

    string base_unit = "ml";
    if (units == "ml")
    {
        base_unit = "l";
        snprintf(chars_volume_formatted, sizeof(chars_volume_formatted), "%.0f", volume_dispensed);
    }
    else if (units == "oz")
    {
        base_unit = "oz";
        snprintf(chars_volume_formatted, sizeof(chars_volume_formatted), "%.1f", volume_dispensed);
    }
    string receipt_volume_formatted = (chars_volume_formatted);

    debugOutput::sendMessage("Units for receipt2:" + units, MSG_INFO);
    debugOutput::sendMessage("Volume dispensed for receipt2:" + receipt_volume_formatted, MSG_INFO);

    snprintf(chars_cost, sizeof(chars_cost), "%.2f", price);
    string receipt_cost = (chars_cost);

    snprintf(chars_price_per_liter_formatted, sizeof(chars_volume_formatted), "%.2f", price_per_liter);
    string receipt_price_per_liter = (chars_price_per_liter_formatted);

    // add base price
    if (m_pMessaging->getRequestedSize() == 'c')
    {
        receipt_volume_formatted = receipt_volume_formatted + units + " @" + receipt_price_per_liter + "$/" + base_unit;
    }
    else if (m_pMessaging->getRequestedSize() == 't')
    {
        receipt_volume_formatted = receipt_volume_formatted + units + " @" + receipt_price_per_liter + "$/" + base_unit;
    }
    else
    {
        receipt_volume_formatted += units;
    }

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(now, 50, "%F %T", timeinfo);

    print_text(name_receipt + "\nPrice: $" + receipt_cost + " \nVolume: " + receipt_volume_formatted + "\nTime: " + now);

    if (plu.size() != 13 && plu.size() != 12)
    {
        // EAN13 codes need to be 13 digits, or else no barcode will be printed. If 12 dgits are provided, the last digit (checksum?!) is automatically generated
        debugOutput::sendMessage("ERROR: bar code invalid (" + plu + "). EAN13, Should be 13 digits" + to_string(plu.size()), MSG_INFO);
        print_text("\nPLU: " + plu + " (No barcode available)");
    }
    else
    {
        Adafruit_Thermal *printerr = new Adafruit_Thermal();
        printerr->connectToPrinter();
        printerr->setBarcodeHeight(100);
        printerr->printBarcode(plu.c_str(), EAN13);
        printerr->disconnectPrinter();
    }

    print_text("\n\n\n");
}

DF_ERROR stateDispenseEnd::print_text(string text)
{
    string printerstring = text;
    string printer_command_string = "echo '\n" + printerstring + "' > /dev/ttyS4";
    system(printer_command_string.c_str());
}