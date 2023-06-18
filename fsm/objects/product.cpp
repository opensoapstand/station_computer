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
// by:Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************
#include "product.h"
#include <stdio.h>
#include <string>
#include "../objects/debugOutput.h"
#include <fstream>

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

void product::loadProductPropertiesFromCsv(string product_id)
{
    // Create an input filestream
    std::ifstream products_file(PRODUCT_DETAILS_TSV_PATH);

    if (!products_file.is_open())
    {
        debugOutput::sendMessage("Products file path could not be opened: " + std::string(PRODUCT_DETAILS_TSV_PATH), MSG_ERROR);
        return;
    }

    std::string line;

    while (std::getline(products_file, line))
    {
        std::string delimiter = "\t";
        size_t pos = 0;
        uint8_t token_index = 0;
        std::string token;
        while ((pos = line.find(delimiter)) != std::string::npos)
        {
            token = line.substr(0, pos);
            m_product_properties[token_index] = token;
            token_index++;
            line.erase(0, pos + delimiter.length());
        }

        // for (uint8_t i=0;i<token_index;i++){
        //     debugOutput::sendMessage(split_line[i] , MSG_INFO);

        // }
        // debugOutput::sendMessage(m_product_properties[CSV_PRODUCT_COL_ID] , MSG_INFO);
        // debugOutput::sendMessage(m_product_properties[CSV_PRODUCT_COL_NAME_UI] , MSG_INFO);

        bool stringsAreDifferent;
        stringsAreDifferent = m_product_properties[CSV_PRODUCT_COL_ID].compare(product_id);
        if (!(stringsAreDifferent))
        {
            debugOutput::sendMessage("Product found in products file and loaded: " + m_product_properties[CSV_PRODUCT_COL_ID] + " " + m_product_properties[CSV_PRODUCT_COL_NAME], MSG_INFO);
            return;
        }
    }
    debugOutput::sendMessage("Product not found in products file: " + product_id, MSG_ERROR);
}

// Set the Product Name
// TODO: Redefine function prototype, no argument.
// void product::setProductName(string productName)
// {
//     // TODO: SQLite database Query could be better option.
//     m_name = productName;
// }

double product::getVolumePerTick()
{
    return m_nVolumePerTick;
}

bool product::registerFlowSensorTick()
{
    // tick from flowsensor interrupt will increase dispensed volume.
    //    cout << "Registering Flow!!" << endl << "Vol disp: " << m_nVolumeDispensed << endl << "vol per tick: " << m_nVolumePerTick << endl;
    // cout << getVolumePerTick()<< endl;
    // cout << m_concentration_multiplier <<endl;
    m_nVolumeDispensed += getVolumePerTick() * m_concentration_multiplier;
    // m_nVolumeDispensed += 100.0;
}
void product::setVolumeDispensed(double volume)
{
    m_nVolumeDispensed = volume;
}
double product::getVolumeDispensed()
{
    return m_nVolumeDispensed;
}
void product::resetVolumeDispensed()
{
    m_nVolumeDispensed = 0;
}
double product::getThresholdFlow()
{
    return m_nThresholdFlow;
}
int product::getRetractionTimeMillis()
{
    return m_nRetractionTimeMillis;
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
    string sql_string = "SELECT dispense_speed FROM products WHERE slot=" + to_string(m_nSlot) + ";";

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

    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);
    std::string str = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
    double vol_per_tick = stod(str);
    sqlite3_finalize(stmt);

    sqlite3_close(db);
    //     cout << str << endl;
    return vol_per_tick;
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

double product::getVolumeFull()
{
    return m_nVolumeFull;
}
double product::getVolumeRemaining()
{
    return m_nVolumeRemaining;
}
double product::getVolumeDispensedTotalEver()
{
    // total volume ever dispensed by this slot.
    return m_nVolumeDispensedTotalEver;
}

double product::getVolumeDispensedSinceLastRestock()
{
    return m_nVolumeDispensedSinceRestock;
}

char product::getSizeCharFromTargetVolume(double volume)
{
    // this is a necessary evil as in transactions, the requested volume is not stored as char
    // #define VOLUME_MARGIN 0.1
    if (volume == m_nVolumeTarget_s)
    {
        return 's';
    }
    else if (volume == m_nVolumeTarget_m)
    {
        return 'm';
    }
    else if (volume == m_nVolumeTarget_l)
    {
        return 'l';
    }
    else if (volume == m_nVolumeTarget_c_max)
    {
        return 'c';
    }
    else
    {
        debugOutput::sendMessage("Get size from volume, not found, will default to custom dispense for volume " + to_string(volume), MSG_INFO);
        return 'c';
    }
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
    else if (size == 'c')
    {
        return m_nVolumeTarget_c_max;
    }
    else if (size == 't')
    {
        return TEST_DISPENSE_TARGET_VOLUME;
    }
    else if (size == SIZE_EMPTY_CONTAINER_DETECTED_CHAR)
    {
        return 666.0;
    }
    else
    {
        debugOutput::sendMessage("Unknown volume parameter: " + size, MSG_INFO);
    }
}

// double product::getCustomVolumePriceDependingOnDispensedVolume(double volume)
// {
//     // custom volume pricing is per ml. Often, pricing is more optimal when larger volumes are dispensed.
//     // if the volume provide is larger than "large volume pricing", the price per milliliter will be adjusted to match that.
//     if (volume >= getTargetVolume(SIZE_LARGE_CHAR))
//     {
//         // go for discount
//         // price per ml
//         double largePricePerMl = getPrice(SIZE_LARGE_CHAR) / getTargetVolume(SIZE_LARGE_CHAR);

//         return largePricePerMl;

//         // if (largePricePerMl < getPrice(SIZE_CUSTOM_CHAR)){
//         //     return getPrice(SIZE_CUSTOM_CHAR)
//         // }else{
//         //     return largePricePerMl
//         // }
//     }
//     else
//     {
//         return getPrice(SIZE_CUSTOM_CHAR);
//     }
// }

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
    else if (size == 'c')
    {
        return m_price_custom_per_liter;
    }
    // else if (size == 'd')
    // {
    //     return m_price_custom_discount_per_liter;
    // }
    else if (size == 't')
    {
        return m_price_custom_per_liter;
    }
    else if (size == SIZE_EMPTY_CONTAINER_DETECTED_CHAR)
    {
        return m_price_custom_per_liter;
    }
    else
    {
        debugOutput::sendMessage("ERROR: Unknown volume parameter for price: " + size, MSG_INFO);
        return 666;
    }
}

bool product::getIsEnabled()
{
    return this->isEnabled;
}
void product::setIsEnabled(bool isEnabled)
{
    this->isEnabled = isEnabled;
}
bool product::getIsSizeEnabled(char size)
{
    return isEnabledSizes[sizeCharToSizeIndex(size)];
}

char product::sizeIndexToSizeChar(int sizeIndex)
{
    return sizeIndexToChar[sizeIndex];
}

void product::customDispenseDiscountData(bool *isEnabled, double *discountVolume, double *discountPrice)
{

    *isEnabled = m_is_enabled_custom_discount;
    *discountVolume = m_nVolumeTarget_custom_discount;
    *discountPrice = m_price_custom_discount_per_liter;
}

int product::sizeCharToSizeIndex(char size)
{

    int size_index;
    switch (size)
    {
    case SIZE_SMALL_CHAR:
        size_index = SIZE_INDEX_SMALL;
        break;
    case SIZE_MEDIUM_CHAR:
        size_index = SIZE_INDEX_MEDIUM;
        break;
    case SIZE_LARGE_CHAR:
        size_index = SIZE_INDEX_LARGE;
        break;
    case SIZE_CUSTOM_CHAR:
        size_index = SIZE_INDEX_CUSTOM;
        break;
    default:
        debugOutput::sendMessage("ERROR Unknown size char: " + size, MSG_INFO);
        size_index = 666;
        break;
    }
    // debugOutput::sendMessage("aefasefasefasefasefasef size char: " + to_string(size_index), MSG_INFO);
    return size_index;
}

string product::getDisplayUnits()
{
    return m_display_unit;
}

double product::convertVolumeMetricToDisplayUnits(double volume)
{
    double converted_volume;

    if (getDisplayUnits() == "oz")
    {

        converted_volume = volume * ML_TO_OZ;
    }
    else if (getDisplayUnits() == "g")
    {

        converted_volume = volume * 1;
    }
    else
    {
        converted_volume = volume;
    }
    return converted_volume;
}

string product::getProductId()
{
    return m_soapstand_product_serial;
}

string product::getBasePLU(char size)
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

    else if (size == 'c')
    {
        return m_nPLU_custom;
    }
    else if (size == 't')
    {
        return m_nPLU_custom;
    }
    else
    {
        debugOutput::sendMessage("Unknown volume parameter for plu: " + size, MSG_INFO);
    }
}

void product::syncSoftwareVersionWithDb()
{
    string sql_string = "UPDATE machine SET software_version=\"" + std::string(CONTROLLER_VERSION) + "\";";
    executeSQLStatement(sql_string);
}

void product::addMaintenancePwdToMachineTable()
{
    executeSQLStatement("ALTER TABLE machine ADD maintenance_pwd TEXT");
    executeSQLStatement("UPDATE machine SET maintenance_pwd=\"soap\";");
}

void product::addShowTransactionsToMachineTable()
{
    executeSQLStatement("ALTER TABLE machine ADD show_transactions INTEGER");
    executeSQLStatement("UPDATE machine SET show_transactions=0;");
}
void product::addIsEnabledCustomDiscountToProductsTable()
{
    executeSQLStatement("ALTER TABLE products ADD is_enabled_custom_discount INT");
    // executeSQLStatement("ALTER TABLE products ADD is_enabled_custom_discount INT AFTER is_enabled_custom");
    executeSQLStatement("UPDATE products SET is_enabled_custom_discount=0;");
}
void product::addSizeCustomDiscountToProductsTable()
{
    executeSQLStatement("ALTER TABLE products ADD size_custom_discount REAL");
    // executeSQLStatement("ALTER TABLE products ADD size_custom_discount REAL AFTER size_custom_min");
    executeSQLStatement("UPDATE products SET size_custom_discount=2000;");
}
void product::addPriceCustomDiscountToProductsTable()
{
    executeSQLStatement("ALTER TABLE products ADD price_custom_discount REAL");
    // executeSQLStatement("ALTER TABLE products ADD price_custom_discount REAL AFTER price_custom");
    executeSQLStatement("UPDATE products SET price_custom_discount=0.01;");
}
void product::executeSQLStatement(string sql_string)
{

    rc = sqlite3_open(DB_PATH, &db);
    sqlite3_stmt *stmt;
    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);
    int status;
    status = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

bool product::isColumnInTable(string table, string column_name_to_find)
{
    bool contains_column_maintenance_pwd = false;
    // debugOutput::sendMessage("dcolumn nameee to seach e" + column_name_to_find, MSG_INFO);

    rc = sqlite3_open(DB_PATH, &db);
    sqlite3_stmt *stmt;
    string sql_string = "PRAGMA table_info(" + table + ");";

    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);
    int status;
    status = sqlite3_step(stmt);
    int row = 0;
    // debugOutput::sendMessage("process record: " + sql_string, MSG_INFO);
    while (status == SQLITE_ROW)
    {
        // debugOutput::sendMessage("roooow " + std::to_string(row), MSG_INFO);
        int columns_count = 3;
        for (int column_index = 0; column_index < columns_count; column_index++)
        {

            // debugOutput::sendMessage("dcolumn index " + std::to_string(column_index), MSG_INFO);
            // for every row, go over all the the items (0=cid, 1=name, 2=type,3=notnull,...)

            switch (column_index)
            {
            case (1):
            {
                string column_name = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
                // debugOutput::sendMessage("col name: " +column_name, MSG_INFO);
                if (!column_name.compare(column_name_to_find))
                {
                    // FOUND returns 0
                    contains_column_maintenance_pwd = true;
                }

                break;
            }
            default:
            {
            }
            break;
            }
        }

        status = sqlite3_step(stmt); // next record, every sqlite3_step returns a row. if it returns 0, it's run over all the rows.
        row++;
    };
    sqlite3_finalize(stmt);

    sqlite3_close(db);
    return contains_column_maintenance_pwd;
}

bool product::isDbValid()
{
    string table_products_columns[TABLE_PRODUCTS_COLUMN_COUNT] = {"productId", "soapstand_product_serial", "slot", "name", "size_unit", "currency", "payment", "name_receipt", "concentrate_multiplier", "dispense_speed", "threshold_flow", "retraction_time", "calibration_const", "volume_per_tick", "last_restock", "volume_full", "volume_remaining", "volume_dispensed_since_restock", "volume_dispensed_total", "is_enabled_small", "is_enabled_medium", "is_enabled_large", "is_enabled_custom", "size_small", "size_medium", "size_large", "size_custom_min", "size_custom_max", "price_small", "price_medium", "price_large", "price_custom", "plu_small", "plu_medium", "plu_large", "plu_custom", "pid_small", "pid_medium", "pid_large", "pid_custom", "flavour", "image_url", "type", "ingredients", "features", "description", "is_enabled_custom_discount", "size_custom_discount", "price_custom_discount"};
    bool is_valid = true;

    rc = sqlite3_open(DB_PATH, &db);
    sqlite3_stmt *stmt;
    string sql_string = "PRAGMA table_info(products);";

    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);
    // sqlite3_step(stmt);
    // std::string str = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));

    int status;
    status = sqlite3_step(stmt);
    int row = 0;
    // debugOutput::sendMessage("process record: " + sql_string, MSG_INFO);
    while (status == SQLITE_ROW)
    {
        // int columns_count = sqlite3_data_count(stmt);
        //  debugOutput::sendMessage("colll count:  " + to_string(columns_count), MSG_INFO);

        // for (int column_index = 0; column_index < columns_count; column_index++)
        int columns_count = 3;
        for (int column_index = 0; column_index < columns_count; column_index++)
        {
            // for every row, go over all the the items (0=cid, 1=name, 2=type,3=notnull,...)

            // debugOutput::sendMessage("column index: " + to_string(column_index), MSG_INFO);

            switch (column_index)
            {
            case (1):
            {
                string column_name = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
                // debugOutput::sendMessage("found name: " + column_name, MSG_INFO);
                // debugOutput::sendMessage("should be : " + table_products_columns[row], MSG_INFO);

                if (row >= TABLE_PRODUCTS_COLUMN_COUNT ){

                    debugOutput::sendMessage("ASSERT ERROR: More columns found than anticipated. CORRUPT DATABASE", MSG_ERROR);
                    debugOutput::sendMessage("************************** PLEASE FIX DATABASE ********************. ", MSG_ERROR);
                   is_valid = false;
                }
                if (!column_name.compare(table_products_columns[row]))
                {
                    // debugOutput::sendMessage("found name: " + column_name, MSG_INFO);
                }
                else
                {
                    debugOutput::sendMessage("ERROR: Corrupt database. Column name=" + column_name + " while it should be: " + table_products_columns[row], MSG_ERROR);
                    is_valid = false;
                }
                break;
            }
            default:
            {
            }
            break;
            }
        }

        status = sqlite3_step(stmt); // next record, every sqlite3_step returns a row. if it returns 0, it's run over all the rows.
        row++;
    };

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return is_valid;
}

string product::getProductName()
{
    return m_product_properties[CSV_PRODUCT_COL_NAME];
}

bool product::reloadParametersFromDb()
{

    for (uint8_t i = 0; i < 4; i++)
    {
        isEnabledSizes[i] = false;
    }

    syncSoftwareVersionWithDb();

    if (!isColumnInTable("products", "is_enabled_custom_discount"))
    {

        debugOutput::sendMessage("ERROR: Unexpected database layout, is_enabled_custom_discount not found  ******************************", MSG_ERROR);
        addIsEnabledCustomDiscountToProductsTable();
    }
    if (!isColumnInTable("products", "size_custom_discount"))
    {

        debugOutput::sendMessage("ERROR: Unexpected database layout, size_custom_discount not found  ******************************", MSG_ERROR);
        addSizeCustomDiscountToProductsTable();
    }
    if (!isColumnInTable("products", "price_custom_discount"))
    {

        debugOutput::sendMessage("ERROR: Unexpected database layout, price_custom_discount not found  ******************************", MSG_ERROR);
        addPriceCustomDiscountToProductsTable();
    }

    if (!isColumnInTable("machine", "maintenance_pwd"))
    // if (!isMaintenancePwdInMachineTable())
    {
        debugOutput::sendMessage("ERROR: Unexpected database layout, will add maintentance_pwd to end of table", MSG_ERROR);
        addMaintenancePwdToMachineTable();
    }

    if (!isColumnInTable("machine", "show_transactions"))
    // if (!isShowTransactionsInMachineTable())
    {
        debugOutput::sendMessage("ERROR: Unexpected database layout, will add show_transactions to end of table", MSG_ERROR);
        addShowTransactionsToMachineTable();
    }

    bool valid = isDbValid();

    if (!valid)
    {
        debugOutput::sendMessage("ABORT: Unexpected database layout.", MSG_ERROR);
        return false;
    }

    debugOutput::sendMessage("Database check: Valid. Please note that no NULL values are allowed in text fields.", MSG_INFO);
    rc = sqlite3_open(DB_PATH, &db);
    sqlite3_stmt *stmt;
    string sql_string = "SELECT * FROM products WHERE slot=" + to_string(m_nSlot) + ";";

    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);

    int status;
    status = sqlite3_step(stmt);

    while (status == SQLITE_ROW)
    {
        debugOutput::sendMessage("process record: " + sql_string, MSG_INFO);
        int columns_count = sqlite3_data_count(stmt);
        // debugOutput::sendMessage("colll count:  " + to_string(columns_count), MSG_INFO);

        for (int column_index = 0; column_index < columns_count; column_index++)
        {
            switch (column_index)
            {
            case DB_PRODUCTS_PRODUCTID:
            {
                m_dispenser_id = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
            }
            break;
            case DB_PRODUCTS_SOAPSTAND_PRODUCT_SERIAL:
            {
                m_soapstand_product_serial = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
                debugOutput::sendMessage("m_soapstand_product_serial:" + m_soapstand_product_serial, MSG_INFO);
            }
            break;
            case DB_PRODUCTS_SLOT:
            {
                if (m_nSlot != sqlite3_column_int(stmt, column_index))
                {
                    debugOutput::sendMessage("DB_PRODUCTS_SLOT unexpected value. " + to_string(sqlite3_column_int(stmt, column_index)), MSG_INFO);
                }
                else
                {
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
                // GET FROM products csv!!!
                // m_name_receipt = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
            }
            break;
            case DB_PRODUCTS_CONCENTRATION_MULTIPLIER:
            {
                m_concentration_multiplier = sqlite3_column_double(stmt, column_index);

                if (m_concentration_multiplier < 0.00000001)
                {
                    debugOutput::sendMessage("Concentration multiplier was not set. Will default to 1. Was:" + to_string(m_concentration_multiplier), MSG_INFO);
                    m_concentration_multiplier = 1.0;
                }
            }
            break;
            case DB_PRODUCTS_DISPENSE_SPEED:
            {
                m_nDispenseSpeedPWM = sqlite3_column_int(stmt, column_index);
                debugOutput::sendMessage("Speed PWM (0..255):" + to_string(m_nDispenseSpeedPWM), MSG_INFO);
            }
            break;
            case DB_PRODUCTS_THRESHOLD_FLOW:
            {
                m_nThresholdFlow = sqlite3_column_double(stmt, column_index);
                debugOutput::sendMessage("Flow threshold: " + to_string(m_nThresholdFlow), MSG_INFO);
            }
            break;
            case DB_PRODUCTS_RETRACTION_TIME:
            {
                m_nRetractionTimeMillis = sqlite3_column_int(stmt, column_index);
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

                //m_nVolumeDispensedSinceRestock = sqlite3_column_double(stmt, column_index);
            }
            break;
            case DB_PRODUCTS_VOLUME_FULL:
            {
                m_nVolumeFull = sqlite3_column_double(stmt, column_index);
            }
            break;
            case DB_PRODUCTS_VOLUME_REMAINING:
            {
                m_nVolumeRemaining = sqlite3_column_double(stmt, column_index);
            }
            break;
            case DB_PRODUCTS_VOLUME_DISPENSED_SINCE_RESTOCK:
            {
                m_nVolumeDispensedSinceRestock = sqlite3_column_double(stmt, column_index);
            }
            break;
            case DB_PRODUCTS_VOLUME_DISPENSED_TOTAL:
            {
                m_nVolumeDispensedTotalEver = sqlite3_column_double(stmt, column_index);
            }
            break;
            case DB_PRODUCTS_IS_ENABLED_SMALL:
            {
                isEnabledSizes[SIZE_INDEX_SMALL] = sqlite3_column_int(stmt, column_index);
            }
            break;
            case DB_PRODUCTS_IS_ENABLED_MEDIUM:
            {
                isEnabledSizes[SIZE_INDEX_MEDIUM] = sqlite3_column_int(stmt, column_index);
            }
            break;
            case DB_PRODUCTS_IS_ENABLED_LARGE:
            {
                isEnabledSizes[SIZE_INDEX_LARGE] = sqlite3_column_int(stmt, column_index);
            }
            break;
            case DB_PRODUCTS_IS_ENABLED_CUSTOM:
            {
                m_is_enabled_custom_discount = sqlite3_column_int(stmt, column_index);
            }
            break;
            case DB_PRODUCTS_IS_ENABLED_CUSTOM_DISCOUNT:
            {
                m_is_enabled_custom_discount = sqlite3_column_int(stmt, column_index);
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
                debugOutput::sendMessage("m_nVolumeTarget_c:" + to_string(m_nVolumeTarget_c_max), MSG_INFO);
            }
            break;
            case DB_PRODUCTS_SIZE_CUSTOM_DISCOUNT:
            {
                m_nVolumeTarget_custom_discount = sqlite3_column_double(stmt, column_index);
                debugOutput::sendMessage("m_nVolumeTarget_custom_volume_for_discount:" + to_string(m_nVolumeTarget_custom_discount), MSG_INFO);
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
            case DB_PRODUCTS_PRICE_CUSTOM_DISCOUNT:
            {
                m_price_custom_discount_per_liter = sqlite3_column_double(stmt, column_index);
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
                // GET FROM products csv!!!
            }
            break;
            case DB_PRODUCTS_FEATURES:
                // GET FROM products csv!!!
                {
                }
                break;
            case DB_PRODUCTS_DESCRIPTION:
                // GET FROM products csv!!!
                {
                }
                break;

            default:
            {
                debugOutput::sendMessage("Unexpected column index" + to_string(column_index), MSG_INFO);
            }
            break;
            }
        }
        status = sqlite3_step(stmt); // next record
    };                               // every sqlite3_step returns a row. if it returns 0, it's run over all the rows.

    debugOutput::sendMessage("DB status: " + to_string(status), MSG_INFO);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    loadProductPropertiesFromCsv(m_soapstand_product_serial);
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
    string sql_string = "SELECT dispense_speed FROM products WHERE slot=" + to_string(m_nSlot) + ";";

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

