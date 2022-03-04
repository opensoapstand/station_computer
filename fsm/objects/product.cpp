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
    ;
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
}

double product::getPrice(char size)
{
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
}

string product::getDisplayUnits(){
    return m_display_unit;
}

string product::getPLU(char size)
{
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
}


