//***************************************
//
// stateDispenseEnd.h
// Dispense End State; Reset for Idle
//
// created: 01-2022
// by:Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
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
    m_pMessaging = message;
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
    slot = m_pMessaging->getRequestedSlot();
    slot_index = slot - 1;

    return e_ret;
}

DF_ERROR stateDispenseEnd::onAction()
{
    debugOutput::sendMessage("onAction Dispense End...", MSG_STATE);

    DF_ERROR e_ret = OK;

    productDispensers[slot_index].stopDispense();

    // handle minimum dispensing
    bool is_valid_dispense = productDispensers[slot_index].getVolumeDispensed() >= MINIMUM_DISPENSE_VOLUME_ML;

    // send dispensed volume to ui (will be used to write to portal)
    usleep(100000); // send message delay (pause from previous message) desperate attempt to prevent crashes

    if (productDispensers[slot_index].getIsDispenseTargetReached())
    {
        usleep(100000); // send message delay (pause from previous message) desperate attempt to prevent crashes
        m_pMessaging->sendMessageOverIP("Target Hit");
    }

    // bool empty_stock_detected = false;
    // // handle empty container detection
    // if (m_pMessaging->getRequestedSize() == SIZE_EMPTY_CONTAINER_DETECTED_CHAR)
    // {
    //     empty_stock_detected = true;
    //     debugOutput::sendMessage("WARNING: Empty container detected. Will set remaining volume to zero.", MSG_INFO);
    // }

    // adjust to nearest lower fixed volume if less dispensed than requested
    adjustSizeToDispensedVolume();

    // SIZE_TEST_CHAR is sent during Maintenance Mode dispenses - we do not want to record these in the transaction database, or print receipts...
    if (m_pMessaging->getRequestedSize() == SIZE_TEST_CHAR)
    {
        debugOutput::sendMessage("Not a transaction: Test dispensing. (" + to_string(productDispensers[slot_index].getVolumeDispensed()) + "ml).", MSG_INFO);
        dispenseEndUpdateDB(false); // update the db dispense statistics
    }
    else if (!is_valid_dispense)
    {
        debugOutput::sendMessage("Not a transaction: No minimum quantity of product dispensed (" + to_string(productDispensers[slot_index].getVolumeDispensed()) + "ml). ", MSG_INFO);

        // check for technical problems
        dispenseEndUpdateDB(false); // update the db dispense statistics
    }
    else
    {
        e_ret = handleTransactionPayment();

        debugOutput::sendMessage("Normal transaction.", MSG_INFO);
        dispenseEndUpdateDB(true);

        bool success = false;
#ifdef ENABLE_TRANSACTION_TO_CLOUD

        std::string paymentMethod = productDispensers[slot_index].getProduct()->getPaymentMethod();

        if (paymentMethod == "qr")
        {
            // these transactions are dealt with in the UI
        }
        else
        {
            // make sure to do this after dispenseEndUpdateDB
            // at that point remaining product volume is already updated in db, and in Product
            success = sendTransactionToCloud(productDispensers[slot_index].getProduct()->getVolumeRemaining());
        }
#else
        debugOutput::sendMessage("NOT SENDING transaction to cloud.", MSG_INFO);
#endif
    }

    m_state_requested = STATE_IDLE;
    m_pMessaging->sendMessageOverIP("Transaction End"); // send to UI

    return e_ret;
}

void stateDispenseEnd::adjustSizeToDispensedVolume()
{
    if (
        (m_pMessaging->getRequestedSize() == SIZE_SMALL_CHAR ||
         m_pMessaging->getRequestedSize() == SIZE_MEDIUM_CHAR ||
         m_pMessaging->getRequestedSize() == SIZE_LARGE_CHAR) &&
        productDispensers[slot_index].getProduct()->getIsSizeEnabled(SIZE_CUSTOM_CHAR))
    {
        // if custom volume is enabled, always revert back to the actual volume that's dispensed.
        m_pMessaging->setRequestedSize(SIZE_CUSTOM_CHAR);
        debugOutput::sendMessage("Custom volume is enabled, so we will always revert to it to calculate actual price.", MSG_INFO);
    }
    else if (m_pMessaging->getRequestedSize() == SIZE_EMPTY_CONTAINER_DETECTED_CHAR)
    {
        // check requested volume versus dispensed volume.
        // go down to next next allowed volume

        if (productDispensers[slot_index].getProduct()->getIsSizeEnabled(SIZE_CUSTOM_CHAR))
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

    else
    {
        debugOutput::sendMessage("No size adjustment", MSG_INFO);

// WARNING: if enabled, customers can cheat. e.g. they can dispense 1.4L for a 1.5 liter dispense request. And then only pay for the next smaller volume (e.g. medium =1L)
// --> Lode would advise against enabling it.
// #define ENABLE_CUSTOMERS_CAN_PAY_FOR_LESS_THAN_THEY_DISPENSE
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

    double dispensed_volume = productDispensers[slot_index].getVolumeDispensed();

    char sizes_big_to_small[3] = {SIZE_LARGE_CHAR, SIZE_MEDIUM_CHAR, SIZE_SMALL_CHAR};
    char fixed_size;
    double fixed_volume;
    char lowest_fixed_size = SIZE_SMALLER_THAN_SMALL;
    for (int i = 0; i < 3; i++)
    {
        fixed_size = sizes_big_to_small[i];
        if (productDispensers[slot_index].getProduct()->getIsSizeEnabled(fixed_size))
        {
            // debugOutput::sendMessage("check volume (dispensed vs target volume)" + to_string(fixed_size), MSG_INFO);
            // as long as the fixed volume is higher than the dispensed volume, go to next lowest size
            fixed_volume = productDispensers[slot_index].getProduct()->getTargetVolume(fixed_size);

            lowest_fixed_size = fixed_size;
            if (dispensed_volume >= fixed_volume)
            {
                break;
            }
        }
    }
    if (lowest_fixed_size == SIZE_SMALLER_THAN_SMALL)
    {
        debugOutput::sendMessage("WARNING: SIZE SMALLER THAN SMALL DETECTED", MSG_INFO);
    }
    return lowest_fixed_size;
}

DF_ERROR stateDispenseEnd::handleTransactionPayment()
{

    debugOutput::sendMessage("Transaction payment", MSG_INFO);
    DF_ERROR e_ret = OK;

    std::string paymentMethod = productDispensers[slot_index].getProduct()->getPaymentMethod();

    // Currently only Drinkfill used the tap method of payment, so this checks if it is a tap payment system and runs the cleaning cycle if it is...
    // TODO: Change this to just check if the system is Soapstand or Drinkfill instead of payment system!
    if (paymentMethod == "qr")
    {
        debugOutput::sendMessage("QR payment. No payment action in controller", MSG_INFO);
    }
    else if (paymentMethod == "tap")
    {
        // sleep(5);

        // debugOutput::sendMessage("Dispense OnEXIT", MSG_INFO);
        // debugOutput::sendMessage("------Cleaning Mode------", MSG_INFO);
        // debugOutput::sendMessage("Activating position -> " + to_string(pos + 1) + " solenoid -> WATER", MSG_INFO);
        // debugOutput::sendMessage("Pin -> " + to_string(productDispensers[slot_index].getI2CPin(WATER)), MSG_INFO);
        // debugOutput::sendMessage("Activating position -> " + to_string(pos + 1) + " solenoid -> WATER", MSG_INFO);
        // debugOutput::sendMessage("Pin -> " + to_string(productDispensers[slot_index].getI2CPin(PRODUCT)), MSG_INFO);
    }
    else if (paymentMethod == "barcode" || paymentMethod == "barcode_EAN-13" || paymentMethod == "barcode_EAN-2" || paymentMethod == "plu")
    {
        debugOutput::sendMessage("Printing receipt:", MSG_INFO);
        setup_and_print_receipt();
    }
    else
    {
        debugOutput::sendMessage("WARNING: No payment method detected.", MSG_INFO);
    }
    return e_ret;
}

// Actions on leaving Dispense state
DF_ERROR stateDispenseEnd::onExit()
{
    DF_ERROR e_ret = OK;
    m_pMessaging->setDispenseCommandToDummy();

    g_machine.pcb24VPowerSwitch(false);
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

// This function sends the transaction details to the cloud using libcurl, if it fails, it stores the data to be sent in the write_curl_to_file function
// TODO: This will be replaced with an AWS IoT method!
bool stateDispenseEnd::sendTransactionToCloud(double volume_remaining)
{
    DF_ERROR e_ret = OK;

    // std::string volume_remaining = to_string(productDispensers[slot_index].getVolumeRemaining());
    // char EndTime[50];
    // time(&rawtime);
    // timeinfo = localtime(&rawtime);
    // strftime(EndTime, 50, "%F %T", timeinfo);

    std::string start_time = productDispensers[slot_index].getDispenseStartTime();
    std::string end_time = productDispensers[slot_index].getDispenseEndTime();
    double price = getFinalPrice();
    std::string price_string = to_string(price);
    debugOutput::sendMessage("Final price" + price_string, MSG_INFO);
    std::string target_volume = to_string(productDispensers[slot_index].getProduct()->getTargetVolume(m_pMessaging->getRequestedSize()));
    std::string product = (productDispensers[slot_index].getProduct()->m_name);
    std::string machine_id = getMachineID();
    std::string pid = getProductID(slot);
    std::string units = productDispensers[slot_index].getProduct()->getDisplayUnits();
    std::string readBuffer;
    std::string volume_remaining_units_converted_string;
    std::string coupon = m_pMessaging->getPromoCode();
    std::string button_press_duration = to_string(productDispensers[slot_index].getButtonPressedTotalMillis());
    std::string dispense_button_count = to_string(productDispensers[slot_index].getDispenseButtonPressesDuringDispensing());
    std::string soapstand_product_serial = (productDispensers[slot_index].getProduct()->getSoapstandProductSerial());
    double volume_remaining_converted;
    std::string dispensed_volume_units_converted;
    double dispensed_volume = ceil(productDispensers[slot_index].getVolumeDispensed());
    if (dispensed_volume <= productDispensers[slot_index].getProduct()->getVolumePerTick())
    {
        dispensed_volume_units_converted = "0";
    }
    else
    {
        double dv = productDispensers[slot_index].getProduct()->convertVolumeMetricToDisplayUnits(dispensed_volume);
        dispensed_volume_units_converted = to_string(dv);
    }

    volume_remaining_converted = productDispensers[slot_index].getProduct()->convertVolumeMetricToDisplayUnits(volume_remaining);
    volume_remaining_units_converted_string = to_string(volume_remaining_converted);

    std::string curl_param = "contents=" + product + "&quantity_requested=" + target_volume + "&quantity_dispensed=" + dispensed_volume_units_converted + "&size_unit=" + units + "&price=" + price_string + "&productId=" + pid + "&start_time=" + start_time + "&end_time=" + end_time + "&MachineSerialNumber=" + machine_id + "&paymentMethod=Printer&volume_remaining_ml=" + to_string(volume_remaining) + "&quantity_dispensed_ml=" + to_string(productDispensers[slot_index].getVolumeDispensed()) + "&volume_remaining=" + volume_remaining_units_converted_string + "&coupon=" + coupon + "&buttonDuration=" + button_press_duration + "&buttonTimes=" + dispense_button_count + "&soapstand_product_serial=" + soapstand_product_serial;
    char buffer[1080];
    strcpy(buffer, curl_param.c_str());

    curl = curl_easy_init();
    if (!curl)
    {
        debugOutput::sendMessage("ERROR: cURL failed to init", MSG_INFO);
        return e_ret;
    }

    debugOutput::sendMessage("cURL init success. Will send: https://soapstandportal.com/api/machine_data/pushPrinterOrder?" + curl_param, MSG_INFO);

    curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/pushPrinterOrder");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);

    res = curl_easy_perform(curl);
    if (res == CURLE_OPERATION_TIMEDOUT)
    {
        debugOutput::sendMessage("sendTransactionToCloud: CURL timed out (" + to_string(CURLOPT_TIMEOUT_MS) + "ms). err: " + to_string(res) + " Will buffer!", MSG_INFO);
        write_curl_to_file(curl_param);
    }
    else if (res != CURLE_OK)
    {
        debugOutput::sendMessage("sendTransactionToCloud: CURL fail. err: " + to_string(res) + " Will buffer!", MSG_INFO);
        write_curl_to_file(curl_param);
    }
    else
    {
        if (readBuffer == "true")
        {
            debugOutput::sendMessage("sendTransactionToCloud: CURL succes.", MSG_INFO);

            // set transaction as processed in database.
            std::string sql;
            std::string start_time = productDispensers[slot_index].getDispenseStartTime();
            sql = ("UPDATE transactions SET processed_by_backend=1 WHERE start_time='" + start_time + "';");
            databaseUpdateSql(sql, USAGE_DB_PATH);
        }
        else
        {
            debugOutput::sendMessage("sendTransactionToCloud: CURL readbuffer fail.", MSG_INFO);
            write_curl_to_file(curl_param);
        }
        readBuffer = "";
    }

    curl_easy_cleanup(curl);

    return e_ret;
}

// TODO: Figure out when/how to send all the bufferred curl data to the cloud (fixed with AWS IoT?)

void stateDispenseEnd::write_curl_to_file(std::string curl_params)
{
    char filetime[50];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(filetime, 50, "%F %T", timeinfo);
    std::string filelocation = "/home/df-admin/production/logging/transactions/";
    std::string filetype = ".txt";
    // std::string filename = filelocation + filetime + filetype;
    std::string filename = filelocation + "missingTransactions" + filetype;

    std::ofstream out;
    out.open(filename, std::ios_base::app);
    if (!out.is_open())
    {
        debugOutput::sendMessage("Cannot open output file at " + filename + " content: " + curl_params, MSG_INFO);
        //        std::cout << "Cannot open output file!";
    }
    else
    {
        out << curl_params << '\n';
        out.close();
    }
}

std::string stateDispenseEnd::getProductID(int slot)
{
    rc = sqlite3_open(CONFIG_DB_PATH, &db);

    sqlite3_stmt *stmt;

    debugOutput::sendMessage("Product ID getter START", MSG_INFO);

    std::string sql_string_pid = "SELECT productId FROM products WHERE slot=" + std::to_string(slot) + ";";

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

    rc = sqlite3_open(CONFIG_DB_PATH, &db);

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

    sqlite3_prepare(db, "SELECT machine_id FROM machine;", -1, &stmt, NULL);
    sqlite3_step(stmt);
    std::string str = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
    ;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return str;
}

DF_ERROR stateDispenseEnd::databaseUpdateSql(string sqlStatement, string dbPath)
{

    // FIXME: DB needs fully qualified link to find...obscure with XML loading.
    char *zErrMsg = 0;
    if(dbPath == USAGE_DB_PATH){
        rc = sqlite3_open(USAGE_DB_PATH, &db);
    }
    else{
        rc = sqlite3_open(CONFIG_DB_PATH, &db);
    }

    if (rc)
    {
        // TODO: Error handling here...
        debugOutput::sendMessage("ERROR: DB could not be opened. " + sqlStatement, MSG_INFO);
    }
    else
    {
    }

    char *sql_trans = new char[sqlStatement.length() + 1];
    strcpy(sql_trans, sqlStatement.c_str());

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql_trans, db_sql_callback, 0, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        // INCORPORATE: zErrMsg !!!
        debugOutput::sendMessage("ERROR: SQL : (err number " + to_string(rc) + ") " + sqlStatement, MSG_INFO);
        sqlite3_free(zErrMsg);
    }
    else
    {
        debugOutput::sendMessage("SUCCESS: SQL : (" + to_string(rc) + ") " + sqlStatement, MSG_INFO);
    }

    sqlite3_close(db);
}

// This function updates the local SQLite3 database with the transaction data, as well as updates the total product remaining locally

DF_ERROR stateDispenseEnd::dispenseEndUpdateDB(bool isValidTransaction)
{
    char *zErrMsg = 0;
    std::string product_name = (productDispensers[slot_index].getProduct()->m_name).c_str();
    std::string target_volume;
    std::string button_press_duration = to_string(productDispensers[slot_index].getButtonPressedTotalMillis());
    std::string dispense_button_count = to_string(productDispensers[slot_index].getDispenseButtonPressesDuringDispensing());
    std::string start_time = (productDispensers[slot_index].getDispenseStartTime());
    std::string end_time = (productDispensers[slot_index].getDispenseEndTime());
    std::string soapstand_product_serial = (productDispensers[slot_index].getProduct()->getSoapstandProductSerial());
    double price;
    std::string price_string;
    std::string product_status;
    std::string slot_status_text;

    price = getFinalPrice();
    price_string = to_string(price);

    target_volume = to_string(ceil(productDispensers[slot_index].getProduct()->getTargetVolume(m_pMessaging->getRequestedSize())));
    char size = m_pMessaging->getRequestedSize();

    // everything rounded to the ml.
    double volume_dispensed_since_restock = ceil(productDispensers[slot_index].getProduct()->getVolumeDispensedSinceLastRestock());
    double volume_dispensed_total_ever = ceil(productDispensers[slot_index].getProduct()->getVolumeDispensedTotalEver());
    double volume_remaining = ceil(productDispensers[slot_index].getProduct()->getVolumeRemaining());

    double dispensed_volume = ceil(productDispensers[slot_index].getVolumeDispensed());

    double updated_volume_remaining;
    double updated_volume_dispensed_since_restock;
    double updated_volume_dispensed_total_ever;

    if (dispensed_volume <= productDispensers[slot_index].getProduct()->getVolumePerTick())
    {
        dispensed_volume = 0;
    }
    updated_volume_dispensed_total_ever = volume_dispensed_total_ever + dispensed_volume;
    updated_volume_remaining = volume_remaining - dispensed_volume;
    updated_volume_dispensed_since_restock = volume_dispensed_since_restock + dispensed_volume;

    // update slot state
    if (productDispensers[slot_index].getSlotState() == SLOT_STATE_PROBLEM_EMPTY)
    {
        updated_volume_remaining = 0;
    }
    else if (productDispensers[slot_index].getSlotState() == SLOT_STATE_PROBLEM_NEEDS_ATTENTION)
    {
        // do nothing
    }
    else if (productDispensers[slot_index].getSlotState() == SLOT_STATE_DISABLED_COMING_SOON || productDispensers[slot_index].getSlotState() == SLOT_STATE_DISABLED)
    {
        // do nothing
    }
    else if (updated_volume_remaining < CONTAINER_EMPTY_THRESHOLD_ML)
    {
        // disabled states are only manually changeable.
        if (productDispensers[slot_index].getSlotState() != SLOT_STATE_DISABLED_COMING_SOON && productDispensers[slot_index].getSlotState() != SLOT_STATE_DISABLED)
        {
            productDispensers[slot_index].setSlotState(SLOT_STATE_AVAILABLE_LOW_STOCK);
        }
        else
        {
            debugOutput::sendMessage("State will not be set, as it's disabled", MSG_INFO);
        }
        debugOutput::sendMessage("Almost empty warning: " + to_string(updated_volume_remaining), MSG_INFO);
    }

    std::string dispensed_volume_str = to_string(dispensed_volume);
    std::string updated_volume_remaining_str = to_string(updated_volume_remaining);
    std::string updated_volume_dispensed_total_ever_str = to_string(updated_volume_dispensed_total_ever);
    std::string updated_volume_dispensed_since_restock_str = to_string(updated_volume_dispensed_since_restock);
    std::string slot_state_str = productDispensers[slot_index].getSlotStateAsString();
    std::string product_id = getProductID(slot);

    // FIXME: DB needs fully qualified link to find...obscure with XML loading.
    debugOutput::sendMessage("Update DB at dispense end: Vol dispensed: " + dispensed_volume_str, MSG_INFO);

    // std::string s = std::format("{:.2f}", 3.14159265359); // s == "3.14"

    if (isValidTransaction)
    {
        std::string sql1;
        sql1 = ("INSERT INTO transactions (product,quantity_requested,price,start_time,quantity_dispensed,end_time,volume_remaining,slot,button_duration,button_times,processed_by_backend,product_id, soapstand_product_serial) VALUES ('" + product_name + "'," + target_volume + "," + price_string + ",'" + start_time + "'," + dispensed_volume_str + ",'" + end_time + "'," + updated_volume_remaining_str + "," + to_string(slot) + "," + button_press_duration + "," + dispense_button_count + "," + to_string(false) + ",'" + product_id + "','" + soapstand_product_serial + "');");
        databaseUpdateSql(sql1, USAGE_DB_PATH);
    }
    // update transactions table

    // update product table
    std::string sql2;
    sql2 = ("UPDATE products SET volume_dispensed_total=" + updated_volume_dispensed_total_ever_str + ", volume_remaining=" + updated_volume_remaining_str + ", volume_dispensed_since_restock=" + updated_volume_dispensed_since_restock_str + " WHERE slot='" + to_string(slot) + "';");
    databaseUpdateSql(sql2, CONFIG_DB_PATH);

    // update machine table
    std::string slot_status_field_name = "status_text_slot_" + to_string(slot);

    std::string sql3;
    sql3 = ("UPDATE machine SET " + slot_status_field_name + "='" + slot_state_str + "';");
    databaseUpdateSql(sql3,CONFIG_DB_PATH);

    // reload (changed) db values
    productDispensers[slot_index].getProduct()->reloadParametersFromDb();
}

double stateDispenseEnd::getFinalPrice()
{
    // for fixed prices: will return fixed price
    // for custom volumes: will return price calculated from dispensed volume
    char size = m_pMessaging->getRequestedSize();
    double price_per_ml;
    double volume_dispensed;
    double price;

    if (size == SIZE_CUSTOM_CHAR)
    {

        bool isDiscountEnabled;
        double discountVolume;
        double discountPrice;
        volume_dispensed = productDispensers[slot_index].getVolumeDispensed();
        productDispensers[slot_index].getProduct()->customDispenseDiscountData(&isDiscountEnabled, &discountVolume, &discountPrice);

        if (isDiscountEnabled && (volume_dispensed > discountVolume))
        {
            // with price reduction at larger quantities
            price_per_ml = discountPrice;
            debugOutput::sendMessage("Custom volume discount price will be applied. ", MSG_INFO);
        }
        else
        {
            debugOutput::sendMessage("No custom volume discount will be applied.", MSG_INFO);
            price_per_ml = productDispensers[slot_index].getProduct()->getPrice(SIZE_CUSTOM_CHAR);
        }

        price = price_per_ml * volume_dispensed;
    }
    else if (size == SIZE_TEST_CHAR)
    {
        price = 0;
    }
    else
    {
        price = m_pMessaging->getRequestedPrice();
    }
    debugOutput::sendMessage("Post dispense final price: " + to_string(price), MSG_INFO);
    double volume = productDispensers[slot_index].getVolumeDispensed();
    std::string message = "finalVolumeDispensed|" + std::to_string(volume) + "|";
    m_pMessaging->sendMessageOverIP(message);
    return price;
}

// This function prints the receipts by calling a system function (could be done better)
DF_ERROR stateDispenseEnd::setup_and_print_receipt()
{

    // printerr.connectToPrinter();
    char chars_cost[MAX_BUF];
    char chars_volume_formatted[MAX_BUF];
    char chars_price_per_ml_formatted[MAX_BUF];

    string cost = (chars_cost);

    std::string paymentMethod = productDispensers[slot_index].getProduct()->getPaymentMethod();
    std::string name_receipt = (productDispensers[slot_index].getProduct()->getProductName());

    std::string units = (productDispensers[slot_index].getProduct()->getDisplayUnits());
    double price = getFinalPrice();
    double price_per_ml;
    debugOutput::sendMessage("Price final for receipt:" + to_string(price), MSG_INFO);

    double volume_dispensed;

    if (m_pMessaging->getRequestedSize() == 's')
    {
        volume_dispensed = productDispensers[slot_index].getProduct()->m_nVolumeTarget_s;
    }
    else if (m_pMessaging->getRequestedSize() == 'm')
    {
        volume_dispensed = productDispensers[slot_index].getProduct()->m_nVolumeTarget_m;
    }
    else if (m_pMessaging->getRequestedSize() == 'l')
    {
        volume_dispensed = productDispensers[slot_index].getProduct()->m_nVolumeTarget_l;
    }
    else if (m_pMessaging->getRequestedSize() == 'c')
    {
        bool isDiscountEnabled;
        double discountVolume;
        double discountPrice;
        productDispensers[slot_index].getProduct()->customDispenseDiscountData(&isDiscountEnabled, &discountVolume, &discountPrice);

        volume_dispensed = productDispensers[slot_index].getVolumeDispensed();
        if (isDiscountEnabled && (volume_dispensed > discountVolume))
        {
            // with price reduction at larger quantities
            price_per_ml = discountPrice;
        }
        else
        {
            price_per_ml = productDispensers[slot_index].getProduct()->getPrice(SIZE_CUSTOM_CHAR);
        }
    }
    else if (m_pMessaging->getRequestedSize() == 't')
    {
        volume_dispensed = productDispensers[slot_index].getVolumeDispensed();
        // normal
        // price_per_ml = productDispensers[slot_index].getProduct()->getPrice(m_pMessaging->getRequestedSize());
        // with reduction for larger quantities
        price_per_ml = 0;
    }
    else
    {
        debugOutput::sendMessage("invalid size provided" + m_pMessaging->getRequestedSize(), MSG_INFO);
    }

    std::string plu = productDispensers[slot_index].getFinalPLU(m_pMessaging->getRequestedSize(), price);

    // convert units
    if (units == "oz")
    {
        // volume_dispensed = ceil(volume_dispensed * ML_TO_OZ);
        volume_dispensed = ceil(productDispensers[slot_index].getProduct()->convertVolumeMetricToDisplayUnits(volume_dispensed));
        price_per_ml = price_per_ml / ML_TO_OZ;
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
    else if (units == "g")
    {
        base_unit = "100g";
        snprintf(chars_volume_formatted, sizeof(chars_volume_formatted), "%.0f", volume_dispensed);
    }
    string receipt_volume_formatted = (chars_volume_formatted);

    debugOutput::sendMessage("Units for receipt2:" + units, MSG_INFO);
    debugOutput::sendMessage("Volume dispensed for receipt2:" + receipt_volume_formatted, MSG_INFO);

    snprintf(chars_cost, sizeof(chars_cost), "%.2f", price);
    string receipt_cost = (chars_cost);

    double price_per_unit;
    // add base price
    if (m_pMessaging->getRequestedSize() == 'c' || m_pMessaging->getRequestedSize() == 't')
    {
        if (base_unit == "l")
        {
            price_per_unit = price_per_ml * 1000;
        }
        else if (base_unit == "100g")
        {
            price_per_unit = price_per_ml * 100;
            // receipt_price_per_ml = receipt_price_per_ml * 1000;
        }

        snprintf(chars_price_per_ml_formatted, sizeof(chars_volume_formatted), "%.2f", price_per_unit);
        string receipt_price_per_unit = (chars_price_per_ml_formatted);

        receipt_volume_formatted = receipt_volume_formatted + units + " @$" + receipt_price_per_unit + "/" + base_unit;
    }
    // else if (m_pMessaging->getRequestedSize() == 't')
    // {
    //     receipt_volume_formatted = receipt_volume_formatted + units + " @" + receipt_price_per_ml + "$/" + base_unit;
    // }
    else
    {

        receipt_volume_formatted += units;
    }

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(now, 50, "%F %T", timeinfo);

    machine tmp;
    // receipt_cost = m_pMessaging->getRequestedPrice();
    string promoCode = m_pMessaging->getPromoCode();
    debugOutput::sendMessage("Price changed to " + receipt_cost, MSG_INFO);
    tmp.print_receipt(name_receipt, receipt_cost, receipt_volume_formatted, now, units, paymentMethod, plu, promoCode, true);
}

// DF_ERROR stateDispenseEnd::print_text(string text)
// {
//     string printerstring = text;
//     string printer_command_string = "echo '\n" + printerstring + "' > /dev/ttyS4";
//     system(printer_command_string.c_str());
// }