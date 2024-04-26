//***************************************
//
// stateDispenseEnd.h
// Dispense End State; Reset for Idle
//
// created: 01-2022
// by:Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
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
    debugOutput::sendMessage("stateDispenseEnd: ~stateDispenseEnd", MSG_INFO);
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

    // productDispensers = g_productDispensers;
    // m_slot = m_pMessaging->getRequestedSlot();
    // m_slot_index = m_slot - 1;

    return e_ret;
}

DF_ERROR stateDispenseEnd::onAction()
{
    DF_ERROR e_ret = OK;
    debugOutput::sendMessage("onAction Dispense End...", MSG_STATE);
    double price = getFinalPrice();
    double volume_dispensed = g_machine.getSelectedDispenser().getSelectedProductVolumeDispensed();

    //
    // replaced by sendEndTransactionMessageToUI! std::string message = "finalVolumeDispensed|" + std::to_string(volume_dispensed) + "|";
    // usleep(100000); // send message delay
    // m_pMessaging->sendMessageOverIP(message, true); // send to UI

    g_machine.getPcb()->displayMCP23017IORegisters(g_machine.getSelectedDispenserNumber());

    // handle minimum dispensing
    bool is_valid_dispense = volume_dispensed >= MINIMUM_DISPENSE_VOLUME_ML;

    if (!is_valid_dispense)
    {
        g_machine.getSelectedDispenser().resetSelectedProductVolumeDispensed();

        debugOutput::sendMessage("Not a valid dispense. Volume set to zero. " + std::to_string(volume_dispensed), MSG_STATE);
    }

    // send dispensed volume to ui (will be used to write to portal)

    if (g_machine.getSelectedDispenser().isSelectedProductVolumeTargetReached())
    {
        usleep(100000);                                      // send message delay (pause from previous message) desperate attempt to prevent crashes
        m_pMessaging->sendMessageOverIP("Target Hit", true); // send to UI
    }

    // bool empty_stock_detected = false;
    // // handle empty container detection
    // if (size == SIZE_EMPTY_CONTAINER_DETECTED_CHAR)
    // {
    //     empty_stock_detected = true;
    //     debugOutput::sendMessage("WARNING: Empty container detected. Will set remaining volume to zero.", MSG_INFO);
    // }
    // adjust to nearest lower fixed volume if less dispensed than requested
    adjustSizeToDispensedVolume();

    // SIZE_TEST_CHAR is sent during Maintenance Mode dispenses - we do not want to record these in the transaction database, or print receipts...
    if (g_machine.getSelectedDispenser().getSelectedSizeAsChar() == SIZE_TEST_CHAR)
    {
        debugOutput::sendMessage("Not a transaction: Test dispensing. (" + to_string(volume_dispensed) + "ml).", MSG_INFO);
        sendEndTransactionMessageToUI(false);
    }
    else if (!is_valid_dispense)
    {
        debugOutput::sendMessage("Not a transaction: No minimum quantity of product dispensed (" + to_string(volume_dispensed) + "ml). ", MSG_INFO);
        // dispenseEndUpdateDB(false); // update the db dispense statistics
        sendEndTransactionMessageToUI(false);
    }
    else
    {
        debugOutput::sendMessage("Normal transaction.", MSG_INFO);
        sendEndTransactionMessageToUI(true);
        e_ret = handleTransactionPayment();

        // dispenseEndUpdateDB(true);

        // #define ENABLE_TRANSACTION_TO_CLOUD
        // #ifdef ENABLE_TRANSACTION_TO_CLOUD

        //         std::string paymentMethod = g_machine.getPaymentMethod();

        //         if (paymentMethod == "qr" && price != 0.0)
        //         {
        //             // these transactions are dealt with in the UI
        //         }
        //         else
        //         {
        //             // bool success = false;
        //             // make sure to do this after dispenseEndUpdateDB
        //             // at that point remaining product volume is already updated in db, and in Product
        //             // success = sendTransactionToCloud(g_machine.getSelectedDispenser().getSelectedProductVolumeRemaining());
        //         }
        // #else
        //         debugOutput::sendMessage("NOT SENDING transaction to cloud.", MSG_INFO);
        // #endif
    }

    debugOutput::sendMessage("Post dispense final price: " + to_string(price), MSG_INFO);

    // send dispensed volume to ui (will be used to write to portal)
    m_state_requested = STATE_IDLE;
    usleep(1000000);                                          // give UI the chance to catch up (and display page end)
    m_pMessaging->sendMessageOverIP("Transaction End", true); // send to UI

    return e_ret;
}

void stateDispenseEnd::adjustSizeToDispensedVolume()
{
    if (
        (g_machine.getSelectedDispenser().getSelectedSizeAsChar() == SIZE_SMALL_CHAR ||
         g_machine.getSelectedDispenser().getSelectedSizeAsChar() == SIZE_MEDIUM_CHAR ||
         g_machine.getSelectedDispenser().getSelectedSizeAsChar() == SIZE_LARGE_CHAR) &&
        g_machine.getSelectedDispenser().getSelectedProduct()->getIsSizeEnabled(SIZE_CUSTOM_CHAR))
    {
        // if custom volume is enabled, always revert back to the actual volume that's dispensed.
        // Bug alert: If setting the size to custom, quantity_requested also change to custom size and database transaction shows that customer chose custom size
        // m_pMessaging->setRequestedSize(SIZE_CUSTOM_CHAR);
        debugOutput::sendMessage("Custom volume is enabled, so we will always revert to it to calculate actual price.", MSG_INFO);
    }
    else if (g_machine.getSelectedDispenser().getSelectedSizeAsChar() == SIZE_EMPTY_CONTAINER_DETECTED_CHAR)
    {
        // check requested volume versus dispensed volume.
        // go down to next next allowed volume

        if (g_machine.getSelectedDispenser().getSelectedProduct()->getIsSizeEnabled(SIZE_CUSTOM_CHAR))
        {
            // m_pMessaging->setRequestedSize(SIZE_CUSTOM_CHAR);
            g_machine.getSelectedDispenser().getSelectedProduct()->setTargetVolumeFromSize(SIZE_CUSTOM_CHAR);
            debugOutput::sendMessage("Empty container detected, change to custom volume.", MSG_INFO);
        }
        else
        {
            char new_size = dispensedVolumeToSmallestFixedSize();
            g_machine.getSelectedDispenser().getSelectedProduct()->setTargetVolumeFromSize(new_size);
            // m_pMessaging->setRequestedSize(new_size);
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
        // m_pMessaging->setRequestedSize(new_size);
        g_machine.getSelectedDispenser().getSelectedProduct()->setTargetVolumeFromSize(new_size);
#endif
    }
}

char stateDispenseEnd::dispensedVolumeToSmallestFixedSize()
{
    // check real dispensed volume compared to available fixed sizes.
    // if dispensed volume is lower than small size, will return "SIZE_SMALLER_THAN_SMALL"

    double dispensed_volume = g_machine.getSelectedDispenser().getSelectedProductVolumeDispensed();

    char sizes_big_to_small[3] = {SIZE_LARGE_CHAR, SIZE_MEDIUM_CHAR, SIZE_SMALL_CHAR};
    char fixed_size;
    double fixed_volume;
    char lowest_fixed_size = SIZE_SMALLER_THAN_SMALL;
    for (int i = 0; i < 3; i++)
    {
        fixed_size = sizes_big_to_small[i];
        if (g_machine.getSelectedDispenser().getSelectedProduct()->getIsSizeEnabled(fixed_size))
        {
            // debugOutput::sendMessage("check volume (dispensed vs target volume)" + to_string(fixed_size), MSG_INFO);
            // as long as the fixed volume is higher than the dispensed volume, go to next lowest size
            fixed_volume = g_machine.getSelectedDispenser().getSelectedProduct()->getVolumeFromSize(fixed_size);

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

    std::string paymentMethod = g_machine.getPaymentMethod();

    // Currently only Drinkfill used the tap method of payment, so this checks if it is a tap payment system and runs the cleaning cycle if it is...
    // TODO: Change this to just check if the system is Soapstand or Drinkfill instead of payment system!
    if (paymentMethod == "qr")
    {
        debugOutput::sendMessage("QR payment. No payment action in controller", MSG_INFO);
    }
    else if (paymentMethod == "tap")
    {
        debugOutput::sendMessage("Tap payment. No payment action in controller", MSG_INFO);
        // sleep(5);
        // debugOutput::sendMessage("Dispense OnEXIT", MSG_INFO);
        // debugOutput::sendMessage("------Cleaning Mode------", MSG_INFO);
        // debugOutput::sendMessage("Activating position -> " + to_string(pos + 1) + " solenoid -> WATER", MSG_INFO);
        // debugOutput::sendMessage("Pin -> " + to_string(g_machine.getSelectedDispenser().getI2CPin(WATER)), MSG_INFO);
        // debugOutput::sendMessage("Activating position -> " + to_string(pos + 1) + " solenoid -> WATER", MSG_INFO);
        // debugOutput::sendMessage("Pin -> " + to_string(g_machine.getSelectedDispenser().getI2CPin(PRODUCT)), MSG_INFO);
    }
    else if (paymentMethod == "barcode" || paymentMethod == "barcode_EAN-13" || paymentMethod == "barcode_EAN-2" || paymentMethod == "plu")
    {
        debugOutput::sendMessage("Print receipt:", MSG_INFO);
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
    return 0;
}

size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

// This function sends the transaction details to the cloud using libcurl, if it fails, it stores the data to be sent in the write_curl_to_file function
// TODO: This will be replaced with an AWS IoT method!
// bool stateDispenseEnd::sendTransactionToCloud(double volume_remaining)
// {
//     DF_ERROR e_ret = OK;

//     // std::string volume_remaining = to_string(g_machine.getSelectedDispenser().getSelectedProductVolumeRemaining());
//     // char EndTime[50];
//     // time(&rawtime);
//     // timeinfo = localtime(&rawtime);
//     // strftime(EndTime, 50, "%F %T", timeinfo);

//     std::string start_time = g_machine.getSelectedDispenser().getSelectedProductDispenseStartTime();
//     std::string end_time = g_machine.getSelectedDispenser().getSelectedProductDispenseEndTime();
//     double price = getFinalPrice();
//     std::string price_string = to_string(price);
//     debugOutput::sendMessage("Final price" + price_string, MSG_INFO);
//     std::string target_volume = to_string(g_machine.getSelectedDispenser().getSelectedProduct()->getTargetVolume());
//     std::string product = g_machine.getSelectedDispenser().getSelectedProduct()->m_name;
//     // std::string machine_id = getMachineID();
//     std::string machine_id = g_machine.getMachineId();
//     // std::string pid = getProductID(m_slot);
//     std::string pid = g_machine.getSelectedDispenser().getSelectedProduct()->m_product_id_combined_with_location_for_backend;
//     // std::string units = g_machine.getSelectedDispenser().getSelectedProduct()->getDisplayUnits();
//     std::string units = g_machine.getSizeUnit();
//     std::string readBuffer;
//     std::string volume_remaining_units_converted_string;
//     std::string coupon = m_pMessaging->getCouponCode();
//     std::string button_press_duration = to_string(g_machine.getSelectedDispenser().getButtonPressedTotalMillis());
//     std::string dispense_button_count = to_string(g_machine.getSelectedDispenser().getDispenseButtonPressesDuringDispensing());
//     std::string pnumberString = g_machine.getSelectedDispenser().getSelectedProduct()->getPNumberAsPString();
//     double volume_remaining_converted;
//     std::string dispensed_volume_units_converted;
//     double dispensed_volume = ceil(g_machine.getSelectedDispenser().getSelectedProductVolumeDispensed());
//     if (dispensed_volume <= g_machine.getSelectedDispenser().getSelectedProduct()->getVolumePerTick(false))
//     {
//         dispensed_volume_units_converted = "0";
//     }
//     else
//     {
//         double dv = g_machine.convertVolumeMetricToDisplayUnits(dispensed_volume);
//         dispensed_volume_units_converted = to_string(dv);
//     }

//     volume_remaining_converted = g_machine.convertVolumeMetricToDisplayUnits(volume_remaining);
//     volume_remaining_units_converted_string = to_string(volume_remaining_converted);

//     std::string curl_param = "contents=" + product + "&quantity_requested=" + target_volume + "&quantity_dispensed=" + dispensed_volume_units_converted + "&size_unit=" + units + "&price=" + price_string + "&productId=" + pid + "&start_time=" + start_time + "&end_time=" + end_time + "&MachineSerialNumber=" + machine_id + "&paymentMethod=Printer&volume_remaining_ml=" + to_string(volume_remaining) + "&quantity_dispensed_ml=" + to_string(g_machine.getSelectedDispenser().getSelectedProductVolumeDispensed()) + "&volume_remaining=" + volume_remaining_units_converted_string + "&coupon=" + coupon + "&buttonDuration=" + button_press_duration + "&buttonTimes=" + dispense_button_count + "&pnumber=" + pnumberString;
//     char buffer[1080];
//     strcpy(buffer, curl_param.c_str());

//     curl = curl_easy_init();
//     if (!curl)
//     {
//         debugOutput::sendMessage("ERROR: cURL failed to init", MSG_INFO);
//         return e_ret;
//     }

//     debugOutput::sendMessage("cURL init success. Will send: " + portal_base_url+ "/api/machine_data/pushPrinterOrder?" + curl_param, MSG_INFO);

//     curl_easy_setopt(curl, CURLOPT_URL, portal_base_url + "api/machine_data/pushPrinterOrder");
//     curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);
//     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//     curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
//     curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);

//     res = curl_easy_perform(curl);
//     if (res == CURLE_OPERATION_TIMEDOUT)
//     {
//         debugOutput::sendMessage("sendTransactionToCloud: CURL timed out (" + to_string(CURLOPT_TIMEOUT_MS) + "ms). err: " + to_string(res) + " Will buffer!", MSG_INFO);
//         write_curl_to_file(curl_param);
//     }
//     else if (res != CURLE_OK)
//     {
//         debugOutput::sendMessage("sendTransactionToCloud: CURL fail. err: " + to_string(res) + " Will buffer!", MSG_INFO);
//         write_curl_to_file(curl_param);
//     }
//     else
//     {
//         if (readBuffer == "true")
//         {
//             debugOutput::sendMessage("sendTransactionToCloud: CURL succes.", MSG_INFO);

//             // set transaction as processed in database.
//             std::string sql;
//             std::string start_time = g_machine.getSelectedDispenser().getSelectedProductDispenseStartTime();
//             sql = ("UPDATE transactions SET processed_by_backend=1 WHERE start_time='" + start_time + "';");
//             databaseUpdateSql(sql, USAGE_DB_PATH);
//         }
//         else
//         {
//             debugOutput::sendMessage("sendTransactionToCloud: CURL readbuffer fail.", MSG_INFO);
//             write_curl_to_file(curl_param);
//         }
//         readBuffer = "";
//     }

//     curl_easy_cleanup(curl);

//     return e_ret;
// }

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

// std::string stateDispenseEnd::getProductID(int slot)
// {
//     rc = sqlite3_open(CONFIG_DB_PATH, &db);

//     sqlite3_stmt *stmt;

//     debugOutput::sendMessage("Product ID getter START", MSG_INFO);

//     std::string sql_string_pid = "SELECT productId FROM products WHERE slot=" + std::to_string(slot) + ";";

//     sqlite3_prepare(db, sql_string_pid.c_str(), -1, &stmt, NULL);
//     sqlite3_step(stmt);
//     std::string str = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
//     ;
//     sqlite3_finalize(stmt);
//     sqlite3_close(db);
//     return str;
// }

// std::string stateDispenseEnd::getMachineID()
// {

//     rc = sqlite3_open(CONFIG_DB_PATH, &db);

//     sqlite3_stmt *stmt;

//     debugOutput::sendMessage("Machine ID getter START", MSG_INFO);

//     if (rc)
//     {
//         //       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
//         // TODO: Error handling here...
//     }
//     else
//     {
//         //       fprintf(stderr, "Opened database successfully\n");
//     }

//     sqlite3_prepare(db, "SELECT machine_id FROM machine;", -1, &stmt, NULL);
//     sqlite3_step(stmt);
//     std::string str = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
//     ;
//     sqlite3_finalize(stmt);
//     sqlite3_close(db);
//     return str;
// }

void stateDispenseEnd::databaseUpdateSql(string sqlStatement, string dbPath)
{

    // FIXME: DB needs fully qualified link to find...obscure with XML loading.
    char *zErrMsg = 0;
    if (dbPath == USAGE_DB_PATH)
    {
        // USAGE DB
        rc = sqlite3_open(USAGE_DB_PATH, &db);
    }
    else
    {
        // CONFIG DB
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

// DF_ERROR stateDispenseEnd::dispenseEndUpdateDB(bool isValidTransaction)
// {
//     char *zErrMsg = 0;
//     std::string product_name = (g_machine.getSelectedDispenser().getSelectedProduct()->m_name).c_str();
//     std::string target_volume;
//     std::string button_press_duration = to_string(g_machine.getSelectedDispenser().getButtonPressedTotalMillis());
//     std::string dispense_button_count = to_string(g_machine.getSelectedDispenser().getDispenseButtonPressesDuringDispensing());
//     std::string start_time = (g_machine.getSelectedDispenser().getSelectedProductDispenseStartTime());
//     std::string end_time = (g_machine.getSelectedDispenser().getSelectedProductDispenseEndTime());
//     std::string pnumberPString = g_machine.getSelectedDispenser().getSelectedProduct()->getPNumberAsPString();
//     int pnumber = g_machine.getSelectedDispenser().getSelectedProduct()->getPNumber();
//     double price;
//     std::string price_string;
//     std::string product_status;
//     std::string slot_status_text;
//     price = getFinalPrice();
//     price_string = to_string(price);

//     target_volume = to_string(ceil(g_machine.getSelectedDispenser().getSelectedProduct()->getTargetVolume()));
//     char size = g_machine.getSelectedDispenser().getSelectedSizeAsChar();

//     // everything rounded to the ml.
//     double volume_dispensed_since_restock = ceil(g_machine.getSelectedDispenser().getSelectedProduct()->getProductVolumeDispensedSinceLastRestock());
//     double volume_dispensed_total_ever = ceil(g_machine.getSelectedDispenser().getSelectedProduct()->getProductVolumeDispensedTotalEver());
//     double volume_remaining = ceil(g_machine.getSelectedDispenser().getSelectedProductVolumeRemaining());

//     double dispensed_volume = ceil(g_machine.getSelectedDispenser().getSelectedProductVolumeDispensed());

//     double updated_volume_remaining;
//     double updated_volume_dispensed_since_restock;
//     double updated_volume_dispensed_total_ever;

//     if (dispensed_volume <= g_machine.getSelectedDispenser().getSelectedProduct()->getVolumePerTick(false))
//     {
//         dispensed_volume = 0;
//     }
//     updated_volume_dispensed_total_ever = volume_dispensed_total_ever + dispensed_volume;
//     updated_volume_remaining = volume_remaining - dispensed_volume;
//     updated_volume_dispensed_since_restock = volume_dispensed_since_restock + dispensed_volume;

//     // update product state
//     if (g_machine.getSelectedDispenser().getSelectedProduct()->getProductState() == PRODUCT_STATE_PROBLEM_EMPTY)
//     {
//         debugOutput::sendMessage("State of selected product is EMPTY. There are repercussions for other products. Is the base empty? Additives empty?... ", MSG_INFO);
//         updated_volume_remaining = 0;
//     }
//     // else if (g_machine.getSelectedDispenser().getSlotState() == SLOT_STATE_PROBLEM_NEEDS_ATTENTION)
//     // {
//     //     // do nothing
//     // }
//     // else if (g_machine.getSelectedDispenser().getSlotState() == SLOT_STATE_DISABLED)
//     // {
//     //     // do nothing
//     // }
//     else if (updated_volume_remaining < CONTAINER_EMPTY_THRESHOLD_ML)
//     {
//         // disabled states are only manually changeable.
//         // if (g_machine.getSelectedDispenser().getSlotState() != SLOT_STATE_DISABLED_COMING_SOON && g_machine.getSelectedDispenser().getSlotState() != SLOT_STATE_DISABLED)
//         // {
//         //     g_machine.getSelectedDispenser().setSlotState(SLOT_STATE_AVAILABLE_LOW_STOCK);
//         // }
//         // else
//         // {
//         //     debugOutput::sendMessage("State will not be set, as it's disabled", MSG_INFO);
//         // }
//         g_machine.getSelectedDispenser().getSelectedProduct()->setProductState(PRODUCT_STATE_AVAILABLE_LOW_STOCK);

//         debugOutput::sendMessage("Almost empty warning: " + to_string(updated_volume_remaining), MSG_INFO);
//     }

//     std::string dispensed_volume_str = to_string(dispensed_volume);
//     std::string updated_volume_remaining_str = to_string(updated_volume_remaining);
//     std::string updated_volume_dispensed_total_ever_str = to_string(updated_volume_dispensed_total_ever);
//     std::string updated_volume_dispensed_since_restock_str = to_string(updated_volume_dispensed_since_restock);
//     // std::string slot_state_str = g_machine.getSelectedDispenser().getSlotStateAsString();
//     int selectedPNumber = g_machine.getSelectedDispenser().getSelectedProduct()->getPNumber();
//     std::string selected_product_state_str = g_machine.getSelectedDispenser().getSelectedProduct()->getProductStateAsString();

//     // std::string product_id = getProductID(slot);
//     std::string product_id = g_machine.getSelectedDispenser().getSelectedProduct()->m_product_id_combined_with_location_for_backend;
//     // Get mix product object to be used for updating local station database
//     std::map<std::string, std::vector<double>> mixProductDispenseObject = g_machine.getSelectedDispenser().getMixDispenseReport();

//     // FIXME: DB needs fully qualified link to find...obscure with XML loading.
//     debugOutput::sendMessage("Update DB at dispense end: Vol dispensed: " + dispensed_volume_str, MSG_INFO);

//     // std::string s = std::format("{:.2f}", 3.14159265359); // s == "3.14"

//     // update transactions table
//     // if (isValidTransaction)
//     // {
//     //     std::string sql1;
//     //     sql1 = ("INSERT INTO transactions (product,quantity_requested,price,start_time,quantity_dispensed,end_time,volume_remaining,slot,button_duration,button_times,processed_by_backend,product_id, soapstand_product_serial) VALUES ('" + product_name + "'," + target_volume + "," + price_string + ",'" + start_time + "'," + dispensed_volume_str + ",'" + end_time + "'," + updated_volume_remaining_str + "," + to_string(g_machine.getSelectedDispenserNumber()) + "," + button_press_duration + "," + dispense_button_count + "," + to_string(false) + ",'" + product_id + "','" + pnumberPString + "');");
//     //     databaseUpdateSql(sql1, USAGE_DB_PATH);
//     // }

//     // update product table with p Number
//     // std::string sql2;
//     // // Iterate over the mix product dispense object to get individual values for each product
//     // for (const auto &entry : mixProductDispenseObject)
//     // {

//     //     volume_dispensed_total_ever = ceil(g_machine.getSelectedDispenser().getProductFromPNumber(std::stoi(entry.first))->getProductVolumeDispensedTotalEver());
//     //     volume_dispensed_since_restock = ceil(g_machine.getSelectedDispenser().getProductFromPNumber(std::stoi(entry.first))->getProductVolumeDispensedSinceLastRestock());
//     //     updated_volume_dispensed_total_ever_str = to_string(volume_dispensed_total_ever + entry.second[0]);
//     //     updated_volume_dispensed_since_restock_str = to_string(volume_dispensed_since_restock + entry.second[0]);
//     //     sql2 = ("UPDATE products SET volume_dispensed_total=" + updated_volume_dispensed_total_ever_str + ", volume_remaining=" + std::to_string(entry.second[1]) + ", volume_dispensed_since_restock=" + updated_volume_dispensed_since_restock_str + " WHERE soapstand_product_serial='" + entry.first + "';");
//     //     databaseUpdateSql(sql2, CONFIG_DB_PATH);
//     // }

//     // // update dipenser table
//     // // std::string sql3;
//     // // sql3 = ("UPDATE slots SET status_text='" + slot_state_str + "' WHERE slot_id=" + to_string(g_machine.getSelectedDispenserNumber()) + ";");
//     // // databaseUpdateSql(sql3, CONFIG_DB_PATH);

//     // std::string sql4;
//     // sql4 = ("UPDATE products SET status_text='" + selected_product_state_str + "' WHERE soapstand_product_serial=" + to_string(selectedPNumber) + ";");
//     // databaseUpdateSql(sql4, CONFIG_DB_PATH);

//     // reload (changed) db values
//     g_machine.getSelectedDispenser().getSelectedProduct()->loadParameters(false);
//     g_machine.getSelectedDispenser().resetMixDispenseReport();
//     DF_ERROR dfRet = OK;
//     return dfRet;
// }

double stateDispenseEnd::getFinalPrice()
{
    // for fixed prices: will return fixed price
    // for custom volumes: will return price calculated from dispensed volume
    char size = g_machine.getSelectedDispenser().getSelectedSizeAsChar();
    double price_per_ml;
    double volume_dispensed;
    double price;
    bool isCustomSizeEnabled = g_machine.getSelectedDispenser().getSelectedProduct()->getIsSizeEnabled(SIZE_CUSTOM_CHAR);

    std::cout << size;
    // If custom volume is enabled, adjust the price to actual quantity dispensed
    if (size == SIZE_CUSTOM_CHAR || isCustomSizeEnabled)
    {

        bool isDiscountEnabled;
        double discountVolume;
        double discountPrice;
        volume_dispensed = g_machine.getSelectedDispenser().getSelectedProductVolumeDispensed();
        g_machine.getSelectedDispenser().getSelectedProduct()->customDispenseDiscountData(&isDiscountEnabled, &discountVolume, &discountPrice);

        if (isDiscountEnabled && (volume_dispensed > discountVolume))
        {
            // with price reduction at larger quantities
            price_per_ml = discountPrice;
            debugOutput::sendMessage("Custom volume discount price will be applied. ", MSG_INFO);
        }
        else
        {
            debugOutput::sendMessage("No custom volume discount will be applied.", MSG_INFO);
            price_per_ml = g_machine.getSelectedDispenser().getSelectedProduct()->getPrice(SIZE_CUSTOM_CHAR);
        }

        price = price_per_ml * volume_dispensed;
        
        debugOutput::sendMessage("Final price: " + std::to_string(price), MSG_INFO);
        if (size != SIZE_CUSTOM_CHAR)
        {
            price = std::min(price, m_pMessaging->getRequestedPrice());
        }
    }
    else if (size == SIZE_TEST_CHAR)
    {
        price = 0;
    }
    else
    {
        price = m_pMessaging->getRequestedPrice();
    }
    return price;
}

// This function prints the receipts by calling a system function (could be done better)
void stateDispenseEnd::setup_and_print_receipt()
{

    char chars_cost[MAX_BUF];
    char chars_volume_formatted[MAX_BUF];
    char chars_price_per_ml_formatted[MAX_BUF];

    string cost = (chars_cost);

    std::string paymentMethod = g_machine.getPaymentMethod();
    std::string name_receipt = (g_machine.getSelectedDispenser().getSelectedProduct()->getProductName());
    debugOutput::sendMessage("P-Number:" + to_string(g_machine.getSelectedDispenser().getSelectedProduct()->getPNumber()), MSG_INFO);
    debugOutput::sendMessage("Name on receipt:" + name_receipt, MSG_INFO);
    std::string units = g_machine.getSizeUnit();
    double price = getFinalPrice();
    double price_per_ml;
    debugOutput::sendMessage("Price final for receipt:" + to_string(price), MSG_INFO);

    double volume_dispensed;
    char size = g_machine.getSelectedDispenser().getSelectedSizeAsChar();

    if (size == 's')
    {
        volume_dispensed = g_machine.getSelectedDispenser().getSelectedProduct()->m_nVolumeTarget_s;
    }
    else if (size == 'm')
    {
        volume_dispensed = g_machine.getSelectedDispenser().getSelectedProduct()->m_nVolumeTarget_m;
    }
    else if (size == 'l')
    {
        volume_dispensed = g_machine.getSelectedDispenser().getSelectedProduct()->m_nVolumeTarget_l;
    }
    else if (size == 'c')
    {
        bool isDiscountEnabled;
        double discountVolume;
        double discountPrice;
        g_machine.getSelectedDispenser().getSelectedProduct()->customDispenseDiscountData(&isDiscountEnabled, &discountVolume, &discountPrice);

        volume_dispensed = g_machine.getSelectedDispenser().getSelectedProductVolumeDispensed();
        if (isDiscountEnabled && (volume_dispensed > discountVolume))
        {
            // with price reduction at larger quantities
            price_per_ml = discountPrice;
        }
        else
        {
            price_per_ml = g_machine.getSelectedDispenser().getSelectedProduct()->getPrice(SIZE_CUSTOM_CHAR);
        }
    }
    else if (size == 't')
    {
        debugOutput::sendMessage("No recreipts printed for test dispenses", MSG_INFO);
        volume_dispensed = g_machine.getSelectedDispenser().getSelectedProductVolumeDispensed();
        // normal
        // price_per_ml = g_machine.getSelectedDispenser().getSelectedProduct()->getPrice(size);
        // with reduction for larger quantities
        price_per_ml = 0;
        return; // no receipts for tests
    }
    else
    {
        debugOutput::sendMessage("invalid size provided" + size, MSG_INFO);
    }

    std::string plu = g_machine.getSelectedDispenser().getSelectedProduct()->getFinalPLU(size, price, g_machine.getPaymentMethod());

    // convert units
    if (units == "oz")
    {
        // volume_dispensed = ceil(volume_dispensed * ML_TO_OZ);
        volume_dispensed = ceil(g_machine.convertVolumeMetricToDisplayUnits(volume_dispensed));
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
    if (size == 'c' || size == 't')
    {
        if (base_unit == "oz")
        {
            price_per_unit = price_per_ml * ML_TO_OZ;
        }
        else if (base_unit == "l")
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
    // else if (size == 't')
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

    // machine tmp;
    string promoCode = m_pMessaging->getCouponCode();
    debugOutput::sendMessage("Price changed to " + receipt_cost, MSG_INFO);
    g_machine.print_receipt(name_receipt, receipt_cost, receipt_volume_formatted, now, units, paymentMethod, plu, promoCode, true);
    // for (int i = 0; i < 100; i++)
    // {
    //     debugOutput::sendMessage("End receipt print from end dispense.", MSG_INFO);
    //     usleep(100000);
    // }
}

// DF_ERROR stateDispenseEnd::print_text(string text)
// {
//     string printerstring = text;
//     string printer_command_string = "echo '\n" + printerstring + "' > /dev/ttyS4";
//     system(printer_command_string.c_str());
// }


void stateDispenseEnd::sendEndTransactionMessageToUI(bool isValid)
{
    usleep(100000);
    std::string status;
    if (isValid){
        status = "valid";
    }else{
        status = "invalid";
    }
    std::string start_time = g_machine.getSelectedDispenser().getSelectedProductDispenseStartTime();
    std::string end_time = g_machine.getSelectedDispenser().getSelectedProductDispenseEndTime();
    std::string button_press_duration = to_string(g_machine.getSelectedDispenser().getButtonPressedTotalMillis());
    std::string button_press_count = to_string(g_machine.getSelectedDispenser().getDispenseButtonPressesDuringDispensing());
    std::string volume_dispensed = to_string(g_machine.getSelectedDispenser().getSelectedProductVolumeDispensed());
    std::string volume_remaining = to_string(g_machine.getSelectedDispenser().getSelectedProductVolumeRemaining());
    // Get mix product object as a string to pass to UI
    std::string pNumber_dispense_info_string = mapToString(g_machine.getSelectedDispenser().getMixDispenseReport());

    std::string message = "finalTransactionMessage|status|" + status + "|start_time|" + start_time + "|end_time|" + end_time + "|button_press_duration|" + button_press_duration + "|button_press_count|" + button_press_count + "|volume_dispensed|" + volume_dispensed + "|volume_remaining|"+ volume_remaining+"|pNumber_dispense_info|" + pNumber_dispense_info_string;
    usleep(100000);                                 // send message delay
    m_pMessaging->sendMessageOverIP(message, true); // send to UI
}

std::string stateDispenseEnd::mapToString(const std::map<std::string, std::vector<double>> &dictionary)
{
    std::stringstream ss;
    for (const auto &entry : dictionary)
    {

        std::cout << "Product: P-" << entry.first << ", Dispensed volume: " << entry.second[0] << ", Volume remaining: " << entry.second[1] << std::endl;
        ss << "\"P-" << entry.first << "\":[" << to_string(entry.second[0]) << "," << to_string(entry.second[1]) << "]"
           << ",";
    }
    std::string result = ss.str();
    if (!result.empty())
    {
        result.pop_back();
    }
    return result;
}