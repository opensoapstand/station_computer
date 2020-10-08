//***************************************
//
// drink.cpp
// Drink (Model) class Implementation:
//
// Owns current beverage use/transaction.
// Holds and coordinates values from database
// to dispense classes.
//
// created: 26-06-2020
// by: Jason Wang & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************
#include "drink.h"

// Default CTOR
drink::drink()
{
}

static int callback(void *data, int argc, char **argv, char **azColName)
{
    int i;
    fprintf(stderr, "%s: ", (const char *)data);

    for (i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        // if()
    }

    // setDrinkName("test");
    // setIsStillDrink(false);

    printf("\n");
    return 0;
}

// CTOR with Option Slot
// TODO: Should call this from Message Mediator
//       And figure out storage/reference location...
drink::drink(int slot)
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
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }

    string sql = "SELECT inventory.inventory_id, product.product_id,product.name, product.calibration_const, pricing.small_price, pricing.large_price FROM inventory INNER JOIN product ON inventory.product_id = product.product_id INNER JOIN pricing ON inventory.product_id = pricing.product_id WHERE inventory.inventory_id = " + to_string(m_nSlot) + ";";

    rc = sqlite3_exec(db, sql.c_str(), callback, (void*)data.c_str(), NULL); 
  
    if (rc != SQLITE_OK) 
        cerr << "Error SELECT" << endl; 
    else {
        cout << "Operation OK!" << endl; 
    } 


    sqlite3_close(db);
}

// Test CTOR
drink::drink(int slot, string name, double nVolumeDispensed, double nVolumeTarget, double calibration_const, double price, bool isStillDrink, double nVolumePerTick){
    m_nSlot = slot;
    m_name = name;
    m_nVolumeDispensed = nVolumeDispensed;
    m_nVolumeTarget = nVolumeTarget;
    m_calibration_const = calibration_const;
    m_price = price;
    m_isStillDrink = isStillDrink;
    m_nVolumePerTick = nVolumePerTick;

    // XXX: Find calculation for this...
    // m_nVolumePerTick = 20; // Seems like best guesstimate tick.
}

// DTOR
drink::~drink()
{
}

// Set the Drink option slot
void drink::setSlot(int slot)
{
    m_nSlot = slot;
}

// Set the Drink Name
// TODO: Redefine function prototype, no argument.
void drink::setDrinkName(string drinkName)
{
    // TODO: SQLite database Query could be better option.
    m_name = drinkName;
}

// TODO: Redefine function prototype, no argument.
void drink::setIsStillDrink(bool isStillDrink)
{
    // TODO: SQLite database Query could be better option.
    m_isStillDrink = isStillDrink;
}

// TODO: Redefine function prototype, no argument.
bool drink::getIsStillDrink()
{
    // TODO: SQLite database Query could be better option.
    return m_isStillDrink;
}

bool drink::registerFlowSensorTick(){
    m_nVolumeDispensed += m_nVolumePerTick;
}

// TODO: Function name is inaccurate...deduct sale would be better
void drink::recordSale(int volume)
{
    // TODO: SQLite database Update.
}

// TODO: This function could live somewhere else...linked to future maintenance.
void drink::refill(int volume)
{
    // TODO: SQLite database Update.
}

int drink::getVolumeSinceLastPoll()
{
    int temp = m_nVolumeDispensedSinceLastPoll;

    m_nVolumeDispensed += m_nVolumeDispensedSinceLastPoll;

    return temp;
}

// Reset values onEntry()
DF_ERROR drink::startDispense(int nVolumeToDispense)
{
    DF_ERROR dfRet = ERROR_BAD_PARAMS;

    m_nVolumeTarget = nVolumeToDispense;
    m_nVolumeDispensed = 0;
    m_nVolumeDispensedSinceLastPoll = 0;

    return dfRet;
}

// VolumeDispense check!
bool drink::isDispenseComplete()
{
    bool bRet = false;

    if (m_nVolumeTarget < m_nVolumeDispensed){
        cout << "Target HIT!" << endl;
        bRet = true;
    }
    return bRet;
}

void drink::drinkInfo() {
    cout << "Drink Option: " << m_nSlot << endl;
    cout << "Drink Name: " << m_name << endl;
    cout << "Drink Dispense Volume: " << m_nVolumeDispensed << endl;
    cout << "Drink Target Volume: " << m_nVolumeTarget << endl;
    cout << "Drink Calibration: " << m_calibration_const << endl;
    cout << "Drink Price: " << m_price << endl;
    cout << "Drink is Still?: " << m_isStillDrink << endl;
}

void drink::drinkVolumeInfo(){
    // cout << "Volume since last poll: " << m_nVolumeDispensedSinceLastPoll << endl;
    // cout << "How much to dispense: " << m_nVolumeTarget << endl;
	cout << "Dispensed so far: " << m_nVolumeDispensed << endl;
}