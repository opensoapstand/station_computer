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
product::product()
{
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
product::product(int slot, string name, double calibration_const, double nVolumePerTick, int dispense_speed_pwm,
                 double nVolumeTarget_s, double nVolumeTarget_m, double nVolumeTarget_l, double nVolumeTarget_c_min, double nVolumeTarget_c_max,
                 double price_small, double price_medium, double price_large, double price_custom_per_liter,
                 string nPLU_small, string nPLU_medium, string nPLU_large, string nPLU_c,
                 string paymentMethod, string name_receipt, string display_unit)
{
    m_nSlot = slot;
    m_name = name;
    m_nVolumeDispensed = 0.0;
    m_nVolumePerTick = nVolumePerTick; // m_nVolumePerTick = 6; //  6ml per tick is standard
    m_nDispenseSpeedPWM = dispense_speed_pwm;
    m_calibration_const = calibration_const;

    m_nVolumeTarget_m = nVolumeTarget_m;
    m_nVolumeTarget_l = nVolumeTarget_l;
    m_nVolumeTarget_s = nVolumeTarget_s;
    m_nVolumeTarget_c_min = nVolumeTarget_c_min;
    m_nVolumeTarget_c_max = nVolumeTarget_c_max;

    m_price_small = price_small;
    m_price_medium = price_medium;
    m_price_large = price_large;
    m_price_custom_per_liter = price_custom_per_liter;

    m_nPLU_small = nPLU_small;
    m_nPLU_medium = nPLU_medium;
    m_nPLU_large = nPLU_large;
    m_nPLU_custom = nPLU_c;

    m_paymentMethod = paymentMethod;
    m_name_receipt = name_receipt;
    m_display_unit = display_unit;
    

}

// DTOR
product::~product()
{
}

// Set the Product option slot
void product::setSlot(int slot)
{
    m_nSlot = slot;
}
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


double product::getVolumePerTick(){
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
    //m_nVolumePerTick = getVolPerTick();
    //m_PWM = getPWM();

    // Set Start Time
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(m_nStartTime, 50, "%F %T", timeinfo);

    return dfRet;
}

int product::getPWM(){
    return m_nDispenseSpeedPWM;
}
int product::getPWMFromDB()
{
    // abandonned by Lode. What about updating the whole product properties at once when needed.
    rc = sqlite3_open(DB_PATH, &db);

    sqlite3_stmt *stmt;

    //debugOutput::sendMessage("Machine ID getter START", MSG_INFO);

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

double product::getVolPerTick(){
    return m_volumePerTick;
}

double product::getVolPerTickFromDB()
{
    // abandonned by Lode. What about updating the whole product properties at once when needed.

    rc = sqlite3_open(DB_PATH, &db);

    sqlite3_stmt *stmt;

    //debugOutput::sendMessage("Machine ID getter START", MSG_INFO);

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
    else if (size == 't')
    {
        return m_price_custom_per_liter;
    }
#endif
}

bool product::getIsEnabled(){
    return this->isEnabled;
}
void product::setIsEnabled(bool isEnabled){
    this->isEnabled = isEnabled;
}

string product::getDisplayUnits(){
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
    
    else if (size == 't')
    {
        return m_nPLU_custom;
    }
#endif
}







bool product::reloadParametersFromDb(){
    m_nSlot = slot;
    m_name = name;
    m_nVolumeDispensed = 0.0;
    m_nVolumePerTick = nVolumePerTick; // m_nVolumePerTick = 6; //  6ml per tick is standard
    m_nDispenseSpeedPWM = dispense_speed_pwm;
    m_calibration_const = calibration_const;

    m_nVolumeTarget_m = nVolumeTarget_m;
    m_nVolumeTarget_l = nVolumeTarget_l;
    m_nVolumeTarget_s = nVolumeTarget_s;
    m_nVolumeTarget_c_min = nVolumeTarget_c_min;
    m_nVolumeTarget_c_max = nVolumeTarget_c_max;

    m_price_small = price_small;
    m_price_medium = price_medium;
    m_price_large = price_large;
    m_price_custom_per_liter = price_custom_per_liter;

    m_nPLU_small = nPLU_small;
    m_nPLU_medium = nPLU_medium;
    m_nPLU_large = nPLU_large;
    m_nPLU_custom = nPLU_c;

    m_paymentMethod = paymentMethod;
    m_name_receipt = name_receipt;
    m_display_unit = display_unit;
    
}

bool product::reloadParametersFromDb()
{

    debugOutput::sendMessage("***************************************************************************", MSG_INFO);
    // abandonned by Lode. What about updating the whole product properties at once when needed.
    rc = sqlite3_open(DB_PATH, &db);

    sqlite3_stmt *stmt;

    //debugOutput::sendMessage("Machine ID getter START", MSG_INFO);

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
    
    string sql_string = "SELECT * FROM products;";
    // string sql_string = "SELECT pwm FROM products WHERE slot=" + to_string(m_nSlot) + ";";
    //string sql_string = "SELECT column_name FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME = 'products'" ;

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
    while(status == SQLITE_ROW){
        int columns_count = sqlite3_data_count(stmt);

        for(int i=0;i<columns_count;i++){
            pwm = sqlite3_column_int(stmt, i);
            debugOutput::sendMessage("values " + to_string(i) + ": " + to_string(pwm), MSG_INFO);

        }
        status = sqlite3_step(stmt);
    };  // every sqlite3_step returns a row. if it returns 0, it's run over all the rows.
    


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

// static int db_sql_product_reload_callback(void *data, int argc, char **argv, char **azColName)
// {
//     // callback is called for every record.
//     int i;
//     int slot;
//     string name;
//     string name_receipt;
//     double volume_dispensed;
//     double volume_large;
//     double volume_medium;
//     double volume_small;
//     double volume_target_custom_min;
//     double volume_target_custom_max;
//     double calibration_const;
//     double price_large;
//     double price_small;
//     double price_medium;
//     double price_custom_per_liter;
//     int is_still;
//     double volume_per_tick;
//     int dispense_speed_pwm;
//     string paymentMethod;
//     string display_unit;

//     string plu_large;
//     string plu_medium;
//     string plu_small;
//     string plu_custom;

//     // debugOutput::sendMessage("Check product values from database:", MSG_INFO);
//     // for (i = 0; i < argc; i++)
//     // {
//     //     std::string colname = azColName[i];
//     //     if(argv[i]){
//     //     debugOutput::sendMessage("colname,value :" + colname + "," + argv[i], MSG_INFO); //+ std::string to_string(colname)
//     //     }else{
//     //         debugOutput::sendMessage("colname,NULL :" + colname , MSG_INFO); //+ std::string to_string(colname)
//     //     }
//     // }

//     debugOutput::sendMessage("-------------------Set product values from database:", MSG_INFO);
//     for (i = 0; i < argc; i++)
//     {
//         // printf("%s = %s\n", azColName[i], argv[i]);
//         std::string colname = azColName[i];

//         char *value = argv[i];
//         char dummy = '0';
//         char *dummy_pointer = &dummy;
//         if (!value)
//         {
//             debugOutput::sendMessage("colname, NULL --> set to 0 :" + colname, MSG_INFO);
//             value = dummy_pointer;
//         }

//         debugOutput::sendMessage("colname,value :" + colname + "," + value, MSG_INFO); //+ std::string to_string(colname)
// #ifdef USE_OLD_DATABASE
//         if (colname == "slot")
//         {
//             slot = atoi(argv[i]);
//         }
//         else if (colname == "name")
//         {
//             name = argv[i];
//         }
//         else if (colname == "name_receipt")
//         {
//             name_receipt = argv[i];
//         }
//         else if (colname == "volume_dispensed")
//         {
//             volume_dispensed = atof(argv[i]);
//         }
//         else if (colname == "volume_target_l")
//         {
//             volume_large = atof(argv[i]);
//         }
//         else if (colname == "volume_target_s")
//         {
//             volume_small = atof(argv[i]);
//         }
//         else if (colname == "calibration_const")
//         {
//             calibration_const = atof(argv[i]);
//         }
//         else if (colname == "price_l")
//         {
//             price_large = atof(argv[i]);
//         }
//         else if (colname == "price_s")
//         {
//             price_small = atof(argv[i]);
//         }
//         else if (colname == "is_still")
//         {
//             is_still = atoi(argv[i]);
//         }
//         else if (colname == "volume_per_tick")
//         {
//             volume_per_tick = atof(argv[i]);
//         }
//         else if (colname == "PLU_l")
//         {
//             plu_large = argv[i];
//         }
//         else if (colname == "PLU_s")
//         {
//             plu_small = argv[i];
//         }
//         else if (colname == "payment")
//         {
//             paymentMethod = argv[i];
//         }
//         else
//         {
//             debugOutput::sendMessage("unprocessed colname: " + colname, MSG_INFO); //+ std::string to_string(colname)
//         }

// #else

//         if (colname == "slot")
//         {
//             slot = atoi(value);
//         }
//         else if (colname == "name")
//         {
//             name = value;
//         }
//         else if (colname == "display_unit")
//         {
//             display_unit = value;
//         }
//         else if (colname == "name_receipt")
//         {
//             name_receipt = value;
//         }
//         else if (colname == "volume_remaining")
//         {
//             volume_dispensed = atof(value);
//         }
//         else if (colname == "size_custom_min")
//         {
//             volume_target_custom_min = atof(value);
//         }
//         else if (colname == "size_custom_max")
//         {
//             volume_target_custom_max = atof(value);
//         }
//         else if (colname == "size_small")
//         {
//             volume_small = atof(value);
//         }
//         else if (colname == "size_medium")
//         {
//             volume_medium = atof(value);
//         }
//         else if (colname == "size_large")
//         {
//             volume_large = atof(value);
//         }
//         else if (colname == "calibration_const")
//         {
//             calibration_const = atof(value);
//         }
//         else if (colname == "price_small")
//         {
//             price_small = atof(value);
//         }
//         else if (colname == "price_medium")
//         {
//             price_medium = atof(value);
//         }
//         else if (colname == "price_large")
//         {
//             price_large = atof(value);
//         }
//         else if (colname == "price_custom")
//         {
//             price_custom_per_liter = atof(value);
//         }
//         else if (colname == "volume_per_tick")
//         {
//             volume_per_tick = atof(value);
//         }
//         else if (colname == "dispense_speed")
//         {
//             dispense_speed_pwm = atof(value);
//         }
//         else if (colname == "plu_small")
//         {
//             plu_small = value;
//         }
//         else if (colname == "plu_medium")
//         {
//             plu_medium = value;
//         }
//         else if (colname == "plu_large")
//         {
//             plu_large = value;
//         }
//         else if (colname == "plu_custom")
//         {
//             plu_custom = value;
//         }
//         else if (colname == "payment")
//         {
//             paymentMethod = value;
//         }
//         else
//         {
//             debugOutput::sendMessage("unprocessed colname: " + colname, MSG_INFO); //+ std::string to_string(colname)
//         }

// #endif
//     }

//     g_productDispensers[slot - 1].setSlot(slot);
//     g_productDispensers[slot - 1].setProduct(
//         new product(slot, name, calibration_const, volume_per_tick, dispense_speed_pwm,
//                     volume_small, volume_medium, volume_large, volume_target_custom_min, volume_target_custom_max,
//                     price_small, price_medium, price_large, price_custom_per_liter,
//                     plu_small, plu_medium, plu_large, plu_custom,
//                     paymentMethod, name_receipt, display_unit));

//     return 0;
// }

// DF_ERROR product::reloadProductFromDB()
// {

//     // Product Setup
//     // load the SQLITE manager

//     char *zErrMsg = 0;
//     int rc;
//     const char *data = "Callback function called";

//     rc = sqlite3_open(DB_PATH, &db);

//     // debugOutput::sendMessage("DB GETTER START", MSG_INFO);

//     if (rc)
//     {
//         // fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
//         // TODO: Error handling here...

//         // debugOutput::sendMessage("Database opening error", MSG_INFO);
//     }
//     else
//     {
//         //       fprintf(stderr, "Opened database successfully\n\n");
//     }

//     /* Create SQL statement */
//     std::string sql11 = "SELECT * FROM products WHERE slot=" + to_string(getSlot()) + ";";
//     std::string sql11 = "SELECT * from products where slot=";
//     char *sql = new char[sql11.length() + 1];
//     strcpy(sql, sql11.c_str());

//     /* Execute SQL statement */
//     rc = sqlite3_exec(db, sql, db_sql_product_callback, (void *)data, &zErrMsg);

//     if (rc != SQLITE_OK)
//     {
//         // debugOutput::sendMessage("Product info SQL error (OR DB PATH opening ERROR!!)", MSG_INFO);
//         //       fprintf(stderr, "SQL error: %s\n", zErrMsg);
//         sqlite3_free(zErrMsg);
//     }
//     else
//     {
//         //       fprintf(stdout, "Operation done successfully\n");
//     }
//     sqlite3_close(db);

//     // debugOutput::sendMessage("Products intialized.", MSG_INFO);

//     return OK;
// }



