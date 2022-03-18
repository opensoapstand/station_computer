//***************************************
//
// product.cpp
// Product (Model) class Implementation:
//
// Owns current beverage use/transaction.
// Holds and coordinates values from database
// to dispense classes.
//
// created: 01-2022
// by:Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************
#include "product.h"
#include <stdio.h>
#include <string>
#include "../objects/debugOutput.h"

// Default CTOR
product::product(int slot)
{
    m_nSlot = slot;
    this->reloadParametersFromDb();
}

static int db_sql_callback(void *data, int argc, char **argv, char **azColName)
{
    int i;
    //    fprintf(stderr, "%s: ", (const char *)data);

    for (i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        // if()
    }

    // setProductName("test");
    // setIsStillProduct(false);

    printf("\n");
    return 0;
}

// CTOR with Option Slot
// TODO: Should call this from Message Mediator
//       And figure out storage/reference location...
// product::product(int slot)
// {
//     setSlot(slot);

//     sqlite3 *db;
//     char *zErrMsg = 0;
//     int rc;

//     string data("CALLBACK FUNCTION");

//     // FIXME: Figure out better URI file opening reference...
//     rc = sqlite3_open("/home/drinkfill/bitbucket/drinkfill/db/sqlite/drinkfill-sqlite.db", &db);

//     if (rc)
//     {
//         // fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
//     }
//     else
//     {
//         // fprintf(stderr, "Opened database successfully\n");
//     }

//     string sql = "SELECT inventory.inventory_id, product.product_id,product.name, product.calibration_const, pricing.small_price, pricing.large_price FROM inventory INNER JOIN product ON inventory.product_id = product.product_id INNER JOIN pricing ON inventory.product_id = pricing.product_id WHERE inventory.inventory_id = " + to_string(m_nSlot) + ";";

//     rc = sqlite3_exec(db, sql.c_str(), db_sql_callback, (void *)data.c_str(), NULL);

//     if (rc != SQLITE_OK)
//         cerr << "Error SELECT" << endl;
//     else
//     {
//         //  cout << "Operation OK!" << endl;
//     }

//     sqlite3_close(db);
// }

// Test CTOR
// product::product(int slot, string name, double calibration_const, double nVolumePerTick, int dispense_speed_pwm,
//                  double nVolumeTarget_s, double nVolumeTarget_m, double nVolumeTarget_l, double nVolumeTarget_c_min, double nVolumeTarget_c_max,
//                  double price_small, double price_medium, double price_large, double price_custom_per_liter,
//                  string nPLU_small, string nPLU_medium, string nPLU_large, string nPLU_c,
//                  string paymentMethod, string name_receipt, string display_unit)
// {
//     m_nSlot = slot;
//     m_name = name;
//     m_nVolumeDispensed = 0.0;
//     m_nVolumePerTick = nVolumePerTick; // m_nVolumePerTick = 6; //  6ml per tick is standard
//     m_nDispenseSpeedPWM = dispense_speed_pwm;
//     m_calibration_const = calibration_const;

//     m_nVolumeTarget_m = nVolumeTarget_m;
//     m_nVolumeTarget_l = nVolumeTarget_l;
//     m_nVolumeTarget_s = nVolumeTarget_s;
//     m_nVolumeTarget_c_min = nVolumeTarget_c_min;
//     m_nVolumeTarget_c_max = nVolumeTarget_c_max;

//     m_price_small = price_small;
//     m_price_medium = price_medium;
//     m_price_large = price_large;
//     m_price_custom_per_liter = price_custom_per_liter;

//     m_nPLU_small = nPLU_small;
//     m_nPLU_medium = nPLU_medium;
//     m_nPLU_large = nPLU_large;
//     m_nPLU_custom = nPLU_c;

//     m_paymentMethod = paymentMethod;
//     m_name_receipt = name_receipt;
//     m_display_unit = display_unit;
// }

// DTOR
product::~product()
{
}

// Set the Product option slot
// void product::setSlot(int slot)
// {
//     m_nSlot = slot;
// }
// Set the Product option slot
int product::getSlot()
{
    return m_nSlot;
}

// Set the Product Name
// TODO: Redefine function prototype, no argument.
void product::setProductName(string productName)
{
    // TODO: SQLite database Query could be better option.
    m_name = productName;
}

double product::getVolumePerTick()
{
    return m_nVolumePerTick;
}
bool product::registerFlowSensorTick()
{
    //    cout << "Registering Flow!!" << endl << "Vol disp: " << m_nVolumeDispensed << endl << "vol per tick: " << m_nVolumePerTick << endl;
    m_nVolumeDispensed += m_nVolumePerTick;
}

// TODO: Function name is inaccurate...deduct sale would be better
void product::recordSale(int volume)
{
    // TODO: SQLite database Update.
}

// TODO: This function could live somewhere else...linked to future maintenance.
void product::refill(int volume)
{
    // TODO: SQLite database Update.
}

double product::getVolumeSinceLastPoll()
{
    int temp = m_nVolumeDispensedSinceLastPoll;

    m_nVolumeDispensed += m_nVolumeDispensedSinceLastPoll;

    return temp;
}

double product::getVolumeDispensedPreviously()
{
    return m_nVolumeDispensedPreviously;
}

// Reset values onEntry()
DF_ERROR product::initDispense(int nVolumeToDispense, double nPrice)
{

    DF_ERROR dfRet = ERROR_BAD_PARAMS;
    m_nVolumeTarget = nVolumeToDispense;
    m_price = nPrice;
    m_nVolumeDispensed = 0;
    m_nVolumeDispensedPreviously = 0;
    m_nVolumeDispensedSinceLastPoll = 0;
    // m_nVolumePerTick = getVolPerTick();
    // m_PWM = getPWM();

    // Set Start Time
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(m_nStartTime, 50, "%F %T", timeinfo);

    return dfRet;
}

int product::getPWM()
{
    return m_nDispenseSpeedPWM;
}
int product::getPWMFromDB()
{
    // abandonned by Lode. What about updating the whole product properties at once when needed.
    rc = sqlite3_open(DB_PATH, &db);

    sqlite3_stmt *stmt;

    // debugOutput::sendMessage("Machine ID getter START", MSG_INFO);

    if (rc)
    {
        // fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        // TODO: Error handling here...
    }
    else
    {
        //  fprintf(stderr, "Opened database successfully\n");
    }
#ifdef USE_OLD_DATABASE

    string sql_string = "SELECT pwm FROM products WHERE slot=" + to_string(m_nSlot) + ";";
#else
    string sql_string = "SELECT dispense_speed FROM products WHERE slot=" + to_string(m_nSlot) + ";";
#endif
    /* Create SQL statement for transactions */
    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);
    std::string str = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
    ;
    int pwm = stod(str);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    //     cout << "INSIDE getPWM() and PWM is = " << str << endl;
    return pwm;
}

double product::getVolPerTick()
{
    return m_volumePerTick;
}

double product::getVolPerTickFromDB()
{
    // abandonned by Lode. What about updating the whole product properties at once when needed.

    rc = sqlite3_open(DB_PATH, &db);

    sqlite3_stmt *stmt;

    // debugOutput::sendMessage("Machine ID getter START", MSG_INFO);

    if (rc)
    {
        // fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        // TODO: Error handling here...
    }
    else
    {
        //       fprintf(stderr, "Opened database successfully\n");
    }

    string sql_string = "SELECT volume_per_tick FROM products WHERE slot=" + to_string(m_nSlot) + ";";
    /* Create SQL statement for transactions */
    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);
    std::string str = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
    double vol_per_tick = stod(str);
    sqlite3_finalize(stmt);

    sqlite3_close(db);
    //     cout << str << endl;
    return vol_per_tick;
}

DF_ERROR product::stopDispense()
{
    DF_ERROR dfRet = ERROR_BAD_PARAMS;

    //    m_nVolumeDispensed = 0;
    m_nVolumeDispensedSinceLastPoll = 0;
    m_nVolumeDispensedPreviously = 0;

    return dfRet;
}

// VolumeDispense check!
bool product::isDispenseTargetVolumeReached()
{
    bool bRet = false;

    if (m_nVolumeTarget <= m_nVolumeDispensed)
    {
        // cout << "Target HIT!" << endl;
        bRet = true;
    }
    return bRet;
}

void product::productInfo()
{
    //    cout << "Option: " << m_nSlot << endl;
    //    cout << "Name: " << m_name << endl;
    //    cout << "Dispense Volume: " << m_nVolumeDispensed << endl;
    //    cout << "Target Volume: " << m_nVolumeTarget << endl;
    //    cout << "Calibration: " << m_calibration_const << endl;
    //    cout << "Price: " << m_price << endl;
    //    cout << "Is Still?: " << m_isStillProduct << endl;
    //    cout << "Volume per Tick: " << m_nVolumePerTick << endl;
}

void product::productVolumeInfo()
{
    // cout << "Volume since last poll: " << m_nVolumeDispensedSinceLastPoll << endl;
    // cout << "How much to dispense: " << m_nVolumeTarget << endl;
    //	cout << "Dispensed so far: " << m_nVolumeDispensed << endl;
}

double product::getTargetVolume(char size)
{
#ifdef USE_OLD_DATABASE
    if (size == 's')
    {
        return m_nVolumeTarget_s;
    }
    else
    {
        return m_nVolumeTarget_l;
    }

#else
    if (size == 's')
    {
        return m_nVolumeTarget_s;
    }
    else if (size == 'm')
    {
        return m_nVolumeTarget_m;
    }
    else if (size == 'l')
    {
        return m_nVolumeTarget_l;
    }
    else if (size == 'c')
    {
        return m_nVolumeTarget_c_max;
    }
    else if (size == 't')
        return m_nVolumeTarget_c_max;

#endif
}

double product::getPrice(char size)
{
#ifdef USE_OLD_DATABASE
    if (size == 's')
    {
        return m_price_small;
    }
    else
    {
        return m_price_large;
    }

#else

    if (size == 's')
    {
        return m_price_small;
    }
    else if (size == 'm')
    {
        return m_price_medium;
    }
    else if (size == 'l')
    {
        return m_price_large;
    }
    else if (size == 'c')
    {
        return m_price_custom_per_liter;
    }
    else if (size == 't')
    {
        return m_price_custom_per_liter;
    }
#endif
}

bool product::getIsEnabled()
{
    return this->isEnabled;
}
void product::setIsEnabled(bool isEnabled)
{
    this->isEnabled = isEnabled;
}

string product::getDisplayUnits()
{
    return m_display_unit;
}

string product::getPLU(char size)
{
#ifdef USE_OLD_DATABASE
    if (size == 's')
    {
        return m_nPLU_small;
    }
    else
    {
        return m_nPLU_large;
    }

#else
    if (size == 's')
    {
        return m_nPLU_small;
    }
    else if (size == 'm')
    {
        return m_nPLU_medium;
    }
    else if (size == 'l')
    {
        return m_nPLU_large;
    }

    else if (size == 'c')
    {
        return m_nPLU_custom;
    }
    else if (size == 't')
    {
        return m_nPLU_custom;
    }
#endif
}

// #ifdef USE_OLD_DATABASE
// bool product::reloadParametersFromDb()
// {

//     // m_nSlot = slot;
//     //     m_name = name;
//     //     m_nVolumeDispensed = 0.0;
//     //     m_nVolumeTarget_l = nVolumeTarget_l;
//     //     m_nVolumeTarget_s = nVolumeTarget_s;
//     //     m_calibration_const = calibration_const;
//     //     m_price_large = price_large;
//     //     m_price_small = price_small;
//     //     DUMMY
//     //     m_nVolumePerTick = nVolumePerTick; // m_nVolumePerTick = 6; //  6ml per tick is standard
//     //     m_nPLU_large = nPLU_large;
//     //     m_nPLU_small = nPLU_small;
//     //     STATE_DUMMY
//     //     STATE_DUMMY
//     //     STATE_DUMMY
//     //     m_name_receipt = name_receipt;
//     //     m_paymentMethod = paymentMethod;
//     //     dumm,
//     //     m_nDispenseSpeedPWM = dispense_speed_pwm;
//     //     dum,    my
//     //     dummy
//     //     m_display_unit = display_unit;

//     //     value

//     rc = sqlite3_open(DB_PATH, &db);

//     sqlite3_stmt *stmt;

//     string sql_string = "SELECT * FROM products WHERE slot=" + to_string(m_nSlot) + ";";

//     debugOutput::sendMessage("Reload parameters from database: " + sql_string, MSG_INFO);

//     /* Create SQL statement for transactions */
//     sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);

//     int status;
//     status = sqlite3_step(stmt);
//     while (status == SQLITE_ROW)
//     {
//         int columns_count = sqlite3_data_count(stmt);

//         for (int column_index = 0; column_index < columns_count; column_index++)
//         {

//             switch (column_index)
//             {
//             case (DB_PRODUCTS_SLOT):
//             {
//                 if (m_nSlot != sqlite3_column_int(stmt, column_index))
//                 {

//                     debugOutput::sendMessage("DB_PRODUCTS_SLOT unexpected value. " + to_string(sqlite3_column_int(stmt, column_index)), MSG_INFO);
//                 }
//             }
//             break;
//             case (DB_PRODUCTS_NAME):
//             {
//                 m_name = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
//             }
//             break;
//             case (DB_PRODUCTS_VOLUME_DISPENSED):
//             {
//             }
//             break;
//             case (DB_PRODUCTS_VOLUME_TARGET_L):
//             {
//                 m_nVolumeTarget_l = sqlite3_column_double(stmt, column_index);
//             }
//             break;
//             case (DB_PRODUCTS_VOLUME_TARGET_S):
//             {
//                 m_nVolumeTarget_s = sqlite3_column_double(stmt, column_index);
//             }
//             break;
//             case (DB_PRODUCTS_CALIBRATION_CONST):
//             {
//                 m_calibration_const = sqlite3_column_double(stmt, column_index);
//             }
//             break;
//             case (DB_PRODUCTS_PRICE_L):
//             {
//                 m_price_large = sqlite3_column_double(stmt, column_index);
//             }
//             break;
//             case (DB_PRODUCTS_PRICE_S):
//             {
//                 m_price_small = sqlite3_column_double(stmt, column_index);
//             }
//             break;
//             case (DB_PRODUCTS_IS_STILL):
//             {
//             }
//             break;
//             case (DB_PRODUCTS_VOLUME_PER_TICK):
//             {
//                 m_nVolumePerTick = sqlite3_column_double(stmt, column_index);
//             }
//             break;
//             case (DB_PRODUCTS_PLU_L):
//             {
//                 m_nPLU_large = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
//             }
//             break;
//             case (DB_PRODUCTS_PLU_S):
//             {
//                 m_nPLU_small = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
//             }
//             break;
//             case (DB_PRODUCTS_FULL_ML):
//             {
//             }
//             break;
//             case (DB_PRODUCTS_REMAINING_ML):
//             {
//             }
//             break;
//             case (DB_PRODUCTS_TOTAL_DISPENSED):
//             {
//             }
//             break;
//             case (DB_PRODUCTS_NAME_RECEIPT):
//             {
//                 m_name_receipt = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
//             }
//             break;
//             case (DB_PRODUCTS_PAYMENT):
//             {
//                 m_paymentMethod = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
//             }
//             break;
//             case (DB_PRODUCTS_LAST_REFILL):
//             {
//             }
//             break;
//             case (DB_PRODUCTS_PWM):
//             {
//                 m_nDispenseSpeedPWM = sqlite3_column_int(stmt, column_index);
                
//             }
//             break;
//             case (DB_PRODUCTS_BUFFER):
//             {
//             }
//             break;
//             case (DB_PRODUCTS_PRODUCT_ID):
//             {
//             }
//             break;
//             case (DB_PRODUCTS_UNITS):
//             {
//                 m_display_unit = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
//             }
//             break;
//             default:
//             {
//                 debugOutput::sendMessage("Unexpected column index" + to_string(column_index), MSG_INFO);
//             }
//             break;
//             }
//         }
//         status = sqlite3_step(stmt);
//     }; // every sqlite3_step returns a row. if it returns 0, it's run over all the rows.

//     // std::string str = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));

//     // int pwm = stod(str);

//     // int pwm = sqlite3_column_int(stmt, 0);
//     // // debugOutput debugInfo;

//     // // debugOutput::sendMessage("***************************************************************************", MSG_INFO);
//     // sqlite3_column_int(stmt,0);

//     sqlite3_finalize(stmt);
//     sqlite3_close(db);
//     //     cout << "INSIDE getPWM() and PWM is = " << str << endl;
//     return true;
// }

// #else

bool product::reloadParametersFromDb()
{


// m_nSlot = slot;
//     m_name = name;
//     m_nVolumeDispensed = 0.0;
//     m_nVolumePerTick = nVolumePerTick; // m_nVolumePerTick = 6; //  6ml per tick is standard
//     m_nDispenseSpeedPWM = dispense_speed_pwm;
//     m_calibration_const = calibration_const;

//     m_nVolumeTarget_m = nVolumeTarget_m;
//     m_nVolumeTarget_l = nVolumeTarget_l;
//     m_nVolumeTarget_s = nVolumeTarget_s;
//     m_nVolumeTarget_c_min = nVolumeTarget_c_min;
//     m_nVolumeTarget_c_max = nVolumeTarget_c_max;

//     m_price_small = price_small;
//     m_price_medium = price_medium;
//     m_price_large = price_large;
//     m_price_custom_per_liter = price_custom_per_liter;

//     m_nPLU_small = nPLU_small;
//     m_nPLU_medium = nPLU_medium;
//     m_nPLU_large = nPLU_large;
//     m_nPLU_custom = nPLU_c;

//     m_paymentMethod = paymentMethod;
//     m_name_receipt = name_receipt;
//     m_display_unit = display_unit;

    debugOutput::sendMessage("000000 ", MSG_INFO);
    rc = sqlite3_open(DB_PATH, &db);


    debugOutput::sendMessage("11111111 ", MSG_INFO);

    sqlite3_stmt *stmt;
    debugOutput::sendMessage("22222 ", MSG_INFO);



    string sql_string = "SELECT * FROM products WHERE slot=" + to_string(m_nSlot) + ";";
    debugOutput::sendMessage("33333 " + sql_string, MSG_INFO);




    /* Create SQL statement for transactions */
    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);

    int status;
    status = sqlite3_step(stmt);
    
    while (status == SQLITE_ROW)
    {
        debugOutput::sendMessage("process record: " + sql_string, MSG_INFO);
        int columns_count = sqlite3_data_count(stmt);
        debugOutput::sendMessage("colll count:  " + to_string(columns_count), MSG_INFO);

        for (int column_index = 0; column_index < columns_count; column_index++)
        {

            debugOutput::sendMessage("column index: " + to_string(column_index), MSG_INFO);
            #ifdef USE_OLD_DATABASE
            switch (column_index)
            {
            case (DB_PRODUCTS_SLOT):
            {
                if (m_nSlot != sqlite3_column_int(stmt, column_index))
                {

                    debugOutput::sendMessage("DB_PRODUCTS_SLOT unexpected value. " + to_string(sqlite3_column_int(stmt, column_index)), MSG_INFO);
                }
            }
            break;
            case (DB_PRODUCTS_NAME):
            {
                m_name = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
            }
            break;
            case (DB_PRODUCTS_VOLUME_DISPENSED):
            {
            }
            break;
            case (DB_PRODUCTS_VOLUME_TARGET_L):
            {
                m_nVolumeTarget_l = sqlite3_column_double(stmt, column_index);
            }
            break;
            case (DB_PRODUCTS_VOLUME_TARGET_S):
            {
                m_nVolumeTarget_s = sqlite3_column_double(stmt, column_index);
            }
            break;
            case (DB_PRODUCTS_CALIBRATION_CONST):
            {
                m_calibration_const = sqlite3_column_double(stmt, column_index);
            }
            break;
            case (DB_PRODUCTS_PRICE_L):
            {
                m_price_large = sqlite3_column_double(stmt, column_index);
            }
            break;
            case (DB_PRODUCTS_PRICE_S):
            {
                m_price_small = sqlite3_column_double(stmt, column_index);
            }
            break;
            case (DB_PRODUCTS_IS_STILL):
            {
            }
            break;
            case (DB_PRODUCTS_VOLUME_PER_TICK):
            {
                m_nVolumePerTick = sqlite3_column_double(stmt, column_index);
            }
            break;
            case (DB_PRODUCTS_PLU_L):
            {
                m_nPLU_large = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
            }
            break;
            case (DB_PRODUCTS_PLU_S):
            {
                m_nPLU_small = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
            }
            break;
            case (DB_PRODUCTS_FULL_ML):
            {
            }
            break;
            case (DB_PRODUCTS_REMAINING_ML):
            {
            }
            break;
            case (DB_PRODUCTS_TOTAL_DISPENSED):
            {
            }
            break;
            case (DB_PRODUCTS_NAME_RECEIPT):
            {
                m_name_receipt = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
            }
            break;
            case (DB_PRODUCTS_PAYMENT):
            {
                m_paymentMethod = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
            }
            break;
            case (DB_PRODUCTS_LAST_REFILL):
            {
            }
            break;
            case (DB_PRODUCTS_PWM):
            {
                m_nDispenseSpeedPWM = sqlite3_column_int(stmt, column_index);
                
            }
            break;
            case (DB_PRODUCTS_BUFFER):
            {
            }
            break;
            case (DB_PRODUCTS_PRODUCT_ID):
            {
            }
            break;
            case (DB_PRODUCTS_UNITS):
            {
                m_display_unit = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
            }
            break;
            default:
            {
                debugOutput::sendMessage("Unexpected column index" + to_string(column_index), MSG_INFO);
            }
            break;
            }

#else
            //debugOutput::sendMessage("Col index: " + to_string(column_index), MSG_INFO);
            switch (column_index)
            {
            case DB_PRODUCTS_PRODUCTID:
            {
            }
            break;
            case DB_PRODUCTS_SOAPSTAND_PRODUCT_SERIAL:
            {
            }
            break;
            case DB_PRODUCTS_SLOT:
            {
                if (m_nSlot != sqlite3_column_int(stmt, column_index))
                {
                    debugOutput::sendMessage("DB_PRODUCTS_SLOT unexpected value. " + to_string(sqlite3_column_int(stmt, column_index)), MSG_INFO);
                }else{
                    debugOutput::sendMessage("DB_PRODUCTS_SLOT slot matching. ", MSG_INFO);
                }
            }
            break;
            case DB_PRODUCTS_NAME:
            {
                m_name = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
            }
            break;
            case DB_PRODUCTS_SIZE_UNIT:
            {
                m_display_unit = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
            }
            break;
            case DB_PRODUCTS_CURRENCY:
            {
            }
            break;
            case DB_PRODUCTS_PAYMENT:
            {
                m_paymentMethod = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
            }
            break;
            case DB_PRODUCTS_NAME_RECEIPT:
            {
                m_name_receipt = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
            }
            break;
            case DB_PRODUCTS_DISPENSE_SPEED:
            {
                m_nDispenseSpeedPWM = sqlite3_column_int(stmt, column_index);
                debugOutput::sendMessage("Speed PWM (0..255):" + to_string(m_nDispenseSpeedPWM), MSG_INFO);
            }
            break;
            case DB_PRODUCTS_CALIBRATION_CONST:
            {
                m_calibration_const = sqlite3_column_double(stmt, column_index);
                debugOutput::sendMessage("DB_PRODUCTS_CALIBRATION_CONST:" + to_string(m_calibration_const), MSG_INFO);
            }
            break;
            case DB_PRODUCTS_VOLUME_PER_TICK:
            {
                m_nVolumePerTick = sqlite3_column_double(stmt, column_index);
            }
            break;
            case DB_PRODUCTS_LAST_RESTOCK:
            {
            }
            break;
            case DB_PRODUCTS_VOLUME_FULL:
            {
            }
            break;
            case DB_PRODUCTS_VOLUME_REMAINING:
            {
            }
            break;
            case DB_PRODUCTS_VOLUME_DISPENSED_SINCE_RESTOCK:
            {
            }
            break;
            case DB_PRODUCTS_VOLUME_DISPENSED_TOTAL:
            {
            }
            break;
            case DB_PRODUCTS_IS_ENABLED_SMALL:
            {
            }
            break;
            case DB_PRODUCTS_IS_ENABLED_MEDIUM:
            {
            }
            break;
            case DB_PRODUCTS_IS_ENABLED_LARGE:
            {
            }
            break;
            case DB_PRODUCTS_IS_ENABLED_CUSTOM:
            {
            }
            break;
            case DB_PRODUCTS_SIZE_SMALL:
            {
                m_nVolumeTarget_s = sqlite3_column_double(stmt, column_index);
                debugOutput::sendMessage("m_nVolumeTarget_s:" + to_string(m_nVolumeTarget_s), MSG_INFO);
            }
            break;
            case DB_PRODUCTS_SIZE_MEDIUM:
            {
                m_nVolumeTarget_m = sqlite3_column_double(stmt, column_index);
                debugOutput::sendMessage("m_nVolumeTarget_m:" + to_string(m_nVolumeTarget_m), MSG_INFO);
            }
            break;
            case DB_PRODUCTS_SIZE_LARGE:
            {
                m_nVolumeTarget_l = sqlite3_column_double(stmt, column_index);
                debugOutput::sendMessage("m_nVolumeTarget_l:" + to_string(m_nVolumeTarget_l), MSG_INFO);
            }
            break;
            case DB_PRODUCTS_SIZE_CUSTOM_MIN:
            {
                m_nVolumeTarget_c_min = sqlite3_column_double(stmt, column_index);
            }
            break;
            case DB_PRODUCTS_SIZE_CUSTOM_MAX:
            {
                m_nVolumeTarget_c_max = sqlite3_column_double(stmt, column_index);
            }
            break;
            case DB_PRODUCTS_PRICE_SMALL:
            {
                m_price_small = sqlite3_column_double(stmt, column_index);
            }
            break;
            case DB_PRODUCTS_PRICE_MEDIUM:
            {
                m_price_medium = sqlite3_column_double(stmt, column_index);
            }
            break;
            case DB_PRODUCTS_PRICE_LARGE:
            {
                m_price_large = sqlite3_column_double(stmt, column_index);
            }
            break;
            case DB_PRODUCTS_PRICE_CUSTOM:
            {
                m_price_custom_per_liter = sqlite3_column_double(stmt, column_index);
            }
            break;
            case DB_PRODUCTS_PLU_SMALL:
            {
                m_nPLU_small = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
            }
            break;
            case DB_PRODUCTS_PLU_MEDIUM:
            {
                m_nPLU_medium = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
            }
            break;
            case DB_PRODUCTS_PLU_LARGE:
            {
                m_nPLU_large = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
            }
            break;
            case DB_PRODUCTS_PLU_CUSTOM:
            {
                m_nPLU_custom = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
            }
            break;
            case DB_PRODUCTS_PID_SMALL:
            {
            }
            break;
            case DB_PRODUCTS_PID_MEDIUM:
            {
            }
            break;
            case DB_PRODUCTS_PID_LARGE:
            {
            }
            break;
            case DB_PRODUCTS_PID_CUSTOM:
            {
            }
            break;
            case DB_PRODUCTS_FLAVOUR:
            {
            }
            break;
            case DB_PRODUCTS_IMAGE_URL:
            {
            }
            break;
            case DB_PRODUCTS_TYPE:
            {
            }
            break;
            case DB_PRODUCTS_INGREDIENTS:
            {
            }
            break;
            case DB_PRODUCTS_FEATURES:
            {
            }
            break;
            default:
            {
                debugOutput::sendMessage("Unexpected column index" + to_string(column_index), MSG_INFO);
            }
            break;
            }
        #endif
        }
        status = sqlite3_step(stmt); // next record
    }; // every sqlite3_step returns a row. if it returns 0, it's run over all the rows.

    debugOutput::sendMessage("DB status: " + to_string(status), MSG_INFO);


    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return true;
}



// #endif

bool product::testParametersFromDb()
{

    debugOutput::sendMessage("***************************************************************************", MSG_INFO);
    // abandonned by Lode. What about updating the whole product properties at once when needed.
    rc = sqlite3_open(DB_PATH, &db);

    sqlite3_stmt *stmt;

    // debugOutput::sendMessage("Machine ID getter START", MSG_INFO);

    if (rc)
    {
        // fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        // TODO: Error handling here...
    }
    else
    {
        //  fprintf(stderr, "Opened database successfully\n");
    }
#ifdef USE_OLD_DATABASE

    // string sql_string = "SELECT * FROM products;";
    string sql_string = "SELECT pwm FROM products WHERE slot=" + to_string(m_nSlot) + ";";
    // string sql_string = "SELECT column_name FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME = 'products'" ;

#else
    string sql_string = "SELECT dispense_speed FROM products WHERE slot=" + to_string(m_nSlot) + ";";
#endif

    /* Create SQL statement for transactions */
    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);

    // int status = sqlite3_step(stmt);  // every sqlite3_step returns a row. if it returns 0, it's run over all the rows.
    // if (status != SQLITE_DONE && status !=SQLITE_ROW){
    //     //https://www.sqlite.org/c3ref/step.html
    //     //error!
    //     debugOutput::sendMessage("DB PROBLEM: " + to_string(status), MSG_INFO);
    //     return false;
    // }
    int pwm;
    int status;
    status = sqlite3_step(stmt);
    while (status == SQLITE_ROW)
    {
        int columns_count = sqlite3_data_count(stmt);

        for (int i = 0; i < columns_count; i++)
        {
            pwm = sqlite3_column_int(stmt, i);
            debugOutput::sendMessage("values " + to_string(i) + ": " + to_string(pwm), MSG_INFO);
        }
        status = sqlite3_step(stmt);
    }; // every sqlite3_step returns a row. if it returns 0, it's run over all the rows.

    // std::string str = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));

    // int pwm = stod(str);

    // int pwm = sqlite3_column_int(stmt, 0);
    // // debugOutput debugInfo;

    // // debugOutput::sendMessage("***************************************************************************", MSG_INFO);
    // sqlite3_column_int(stmt,0);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    //     cout << "INSIDE getPWM() and PWM is = " << str << endl;
    return pwm;
}
