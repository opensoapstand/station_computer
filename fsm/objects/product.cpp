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
product::product(int slot)
{
    setSlot(slot);

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    string data("CALLBACK FUNCTION");

    // FIXME: Figure out better URI file opening reference...
    rc = sqlite3_open("/home/drinkfill/bitbucket/drinkfill/db/sqlite/drinkfill-sqlite.db", &db);

    if (rc)
    {
        // fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        // fprintf(stderr, "Opened database successfully\n");
    }

    string sql = "SELECT inventory.inventory_id, product.product_id,product.name, product.calibration_const, pricing.small_price, pricing.large_price FROM inventory INNER JOIN product ON inventory.product_id = product.product_id INNER JOIN pricing ON inventory.product_id = pricing.product_id WHERE inventory.inventory_id = " + to_string(m_nSlot) + ";";

    rc = sqlite3_exec(db, sql.c_str(), db_sql_callback, (void *)data.c_str(), NULL);

    if (rc != SQLITE_OK)
        cerr << "Error SELECT" << endl;
    else
    {
        //  cout << "Operation OK!" << endl;
    }

    sqlite3_close(db);
}

// Test CTOR
product::product(int slot, string name, double nVolumeDispensed, double nVolumeTarget_l, double nVolumeTarget_s, double calibration_const, double price_l, double price_s, bool isStillProduct, double nVolumePerTick, string nPLU_l, string nPLU_s, string paymentMethod, string name_receipt)
{
    m_nSlot = slot;
    m_name = name;
    m_nVolumeDispensed = nVolumeDispensed;
    m_nVolumeTarget_l = nVolumeTarget_l;
    m_nVolumeTarget_s = nVolumeTarget_s;
    m_calibration_const = calibration_const;
    m_price_l = price_l;
    m_price_s = price_s;
    // m_isStillProduct = isStillProduct;
    m_nVolumePerTick = nVolumePerTick;
    m_nPLU_l = nPLU_l;
    m_nPLU_s = nPLU_s;
    m_paymentMethod = paymentMethod;
    m_name_receipt = name_receipt;

    // XXX: Find calculation for this...
    // m_nVolumePerTick = 20; // Seems like best guesstimate tick.
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

// TODO: Redefine function prototype, no argument.
// void product::setIsStillProduct(bool isStillProduct)
// {
//     // TODO: SQLite database Query could be better option.
//     m_isStillProduct = isStillProduct;
// }

// TODO: Redefine function prototype, no argument.
// bool product::getIsStillProduct()
// {
//     // TODO: SQLite database Query could be better option.
//     return m_isStillProduct;
// }

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
    //    cout << "GETTING VOLUME DISPENSED AND IT IS: " << m_nVolumeDispensedPreviously << endl;
    return m_nVolumeDispensedPreviously;
}

// Reset values onEntry()
DF_ERROR product::startDispense(int nVolumeToDispense, double nPrice)
{
    DF_ERROR dfRet = ERROR_BAD_PARAMS;

    m_nVolumeTarget = nVolumeToDispense;
    m_price = nPrice;
    m_nVolumeDispensed = 0;
    m_nVolumeDispensedPreviously = 0;
    m_nVolumeDispensedSinceLastPoll = 0;
    m_nVolumePerTick = getVolPerTick();
    m_PWM = getPWM();

    return dfRet;
}

int product::getPWM()
{
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

    string sql_string = "SELECT pwm FROM products WHERE slot=" + to_string(m_nSlot) + ";";
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

DF_ERROR product::initDispense()
{
    m_nVolumeDispensed = 0;
    m_nVolumeDispensedPreviously = 0;

    // Set Start Time
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(m_nStartTime, 50, "%F %T", timeinfo);
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
bool product::isDispenseComplete()
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
    if (size == 'l')
    {
        return m_nVolumeTarget_l;
    }
    else if (size == 's')
    {
        return m_nVolumeTarget_s;
    }
    else if (size == 't')
        return m_nVolumeTarget_t;
}

double product::getPrice(char size)
{
    if (size == 'l')
    {
        return m_price_l;
    }
    else if (size == 's')
    {
        return m_price_s;
    }
}

string product::getPLU(char size)
{
    if (size == 'l')
    {
        return m_nPLU_l;
    }
    else if (size == 's')
    {
        return m_nPLU_s;
    }
}
