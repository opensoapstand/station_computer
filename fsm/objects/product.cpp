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
// by:Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************
#include "product.h"
#include <stdio.h>
#include <string>
#include "../objects/debugOutput.h"
#include <fstream>

// Default CTOR
product::product()
{
}

void product::init(int pnumber)
{
    m_pnumber = pnumber;
    this->loadParameters();
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
//     m_pnumber = slot;
// }
// Set the Product option slot
int product::getSlot()
{
    return m_pnumber;
}

void product::loadProductPropertiesFromCsv()
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
        // debugOutput::sendMessage(getPNumberAsPString() , MSG_INFO);
        // debugOutput::sendMessage(m_product_properties[CSV_PRODUCT_COL_NAME_UI] , MSG_INFO);

        bool stringsAreDifferent;
        // debugOutput::sendMessage("Product found in products file and loaded:"
        stringsAreDifferent = m_product_properties[CSV_PRODUCT_COL_ID].compare(getPNumberAsPString());
        if (!(stringsAreDifferent))
        {
            debugOutput::sendMessage("Product found in products file and loaded: " + m_product_properties[CSV_PRODUCT_COL_ID] + " " + m_product_properties[CSV_PRODUCT_COL_NAME], MSG_INFO);
            return;
        }
    }
    debugOutput::sendMessage("Could not load product from products file (not found). Pnumber " + std::to_string(m_pnumber), MSG_ERROR);
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
    // minimum threshold to consider dispensing.
    return m_nThresholdFlow;
}
double product::getThresholdFlow_max_allowed()
{
    // when using kegs, an empty keg results in pressurised CO2 or beergas being pushed through the flow sensor. That speeds up the flowsensor.
    return m_nThresholdFlow_maximum_allowed;
}
int product::getRetractionTimeMillis()
{
    return m_nRetractionTimeMillis;
}

int product::getPWM()
{
    return m_nDispenseSpeedPWM;
}

double product::getVolPerTick()
{
    return m_volumePerTick;
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
        return m_price_custom_per_ml;
    }
    // else if (size == 'd')
    // {
    //     return m_price_custom_discount_per_liter;
    // }
    else if (size == 't')
    {
        return m_price_custom_per_ml;
    }
    else if (size == SIZE_EMPTY_CONTAINER_DETECTED_CHAR)
    {
        return m_price_custom_per_ml;
    }
    else
    {
        debugOutput::sendMessage("ERROR: Unknown volume parameter for price: " + size, MSG_INFO);
        return 666;
    }
}

bool product::getIsEnabled()
{
    return this->m_is_enabled;
}
void product::setIsEnabled(bool isEnabled)
{
    this->m_is_enabled = isEnabled;
}

string product::getStatusText()
{
    return m_status_text;
}
void product::setStatusText(string statusText)
{
    this->m_status_text = statusText;
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

string product::getFinalPLU(char size, double price)
{

    string base_plu = getBasePLU(size);
    char chars_plu_dynamic_formatted[MAX_BUF];

    std::string paymentMethod = getPaymentMethod();
    if (paymentMethod == "plu")
    {
        return base_plu;
    }

    if (size == SIZE_CUSTOM_CHAR)
    {

        if (paymentMethod == "barcode" || paymentMethod == "barcode_EAN-13")
        {
            if (base_plu.size() != 8)
            {
                // debugOutput::sendMessage("Custom plu: " + plu, MSG_INFO);
                debugOutput::sendMessage("ERROR custom plu length must be of length eight. (e.g. standard drinkfill preamble(627987) + 2digit product code) : " + base_plu, MSG_WARNING);
                string fake_plu = "66666666";
                base_plu = fake_plu;
            }

            snprintf(chars_plu_dynamic_formatted, sizeof(chars_plu_dynamic_formatted), "%5.2f", price); // will always be at least 5 chars long e.g. 3.456 --> " 3.46" , 1234.456 --> "1234.45"
        }
        else if (paymentMethod == "barcode_EAN-2")
        {
            if (base_plu.size() != 7)
            {
                // debugOutput::sendMessage("Custom plu: " + plu, MSG_INFO);
                debugOutput::sendMessage("ERROR custom plu length must be of length seven. provided: " + base_plu, MSG_WARNING);
                string fake_plu = "6666666";
                base_plu = fake_plu;
            }

            snprintf(chars_plu_dynamic_formatted, sizeof(chars_plu_dynamic_formatted), "%6.2f", price); // will always be at least 6 chars long e.g. 3.456 --> "  3.45" , 1234.456 --> "1234.46"
        }
        else
        {
            debugOutput::sendMessage("ERROR Payment method not expected: " + paymentMethod, MSG_ERROR);
            string fake_plu = "66666666";
            base_plu = fake_plu;
        }

        string plu_dynamic_price = (chars_plu_dynamic_formatted);

        string plu_dynamic_formatted = base_plu + plu_dynamic_price;

        // 3.14 --> " 3.14" --> " 314" --> "0314"
        // 140.00 -->
        std::string toReplace(".");
        size_t pos = plu_dynamic_formatted.find(toReplace);
        if (pos != -1)
        {
            plu_dynamic_formatted.replace(pos, toReplace.length(), "");
        }

        std::string toReplace2(" ");
        pos = plu_dynamic_formatted.find(toReplace2);
        while (pos != -1)
        {
            plu_dynamic_formatted.replace(pos, toReplace2.length(), "0");
            pos = plu_dynamic_formatted.find(toReplace2);
        }

        if (plu_dynamic_formatted.length() != 12)
        {
            debugOutput::sendMessage("ERROR Generated barcode has an error. Was the price more than 99.99?  Its length should be twelve: " + plu_dynamic_formatted, MSG_ERROR);
        }
        return plu_dynamic_formatted;
    }

    return base_plu;
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

int product::getPNumber()
{
    return m_pnumber;
}

string product::getPNumberAsPString()
{
    return "P-" + std::to_string(m_pnumber);
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
// example of adding columns to table
// void product::addColumnExample()
// {
//     executeSQLStatement("ALTER TABLE products ADD price_custom_discount REAL");
//     // executeSQLStatement("ALTER TABLE products ADD price_custom_discount REAL AFTER price_custom");
//     executeSQLStatement("UPDATE products SET price_custom_discount=0.01;");
// }
void product::executeSQLStatement(string sql_string)
{

    int rc = sqlite3_open(CONFIG_DB_PATH, &db);
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

    int rc = sqlite3_open(CONFIG_DB_PATH, &db);
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
    string table_products_columns[TABLE_PRODUCTS_COLUMN_COUNT] = {
        "soapstand_product_serial",
        "mix_pnumbers",
        "mix_ratios",
        "productId",
        "slot",
        "name",
        "size_unit",
        "currency",
        "payment",
        "name_receipt",
        "concentrate_multiplier",
        "dispense_speed",
        "threshold_flow",
        "retraction_time",
        "calibration_const",
        "volume_per_tick",
        "last_restock",
        "volume_full",
        "volume_remaining",
        "volume_dispensed_since_restock",
        "volume_dispensed_total",
        "is_enabled_small",
        "is_enabled_medium",
        "is_enabled_large",
        "is_enabled_custom",
        "size_small",
        "size_medium",
        "size_large",
        "size_custom_min",
        "size_custom_max",
        "price_small",
        "price_medium",
        "price_large",
        "price_custom",
        "plu_small",
        "plu_medium",
        "plu_large",
        "plu_custom",
        "pid_small",
        "pid_medium",
        "pid_large",
        "pid_custom",
        "flavour",
        "image_url",
        "type",
        "ingredients",
        "features",
        "description",
        "is_enabled_custom_discount",
        "size_custom_discount",
        "price_custom_discount",
        "is_enabled",
        "status_text"

    };
    bool is_valid = true;

    int rc = sqlite3_open(CONFIG_DB_PATH, &db);
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
        int columns_count = sqlite3_data_count(stmt);
        //  debugOutput::sendMessage("colll count:  " + to_string(columns_count), MSG_INFO);

        // for (int column_index = 0; column_index < columns_count; column_index++)
        // int columns_count = 3;
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

                if (row >= TABLE_PRODUCTS_COLUMN_COUNT)
                {

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
    // if (is_valid){
    // debugOutput::sendMessage("Database check: Valid. Please note that no NULL values are allowed in text fields.", MSG_INFO);
    // }
    return is_valid;
}

string product::getProductName()
{
    return m_product_properties[CSV_PRODUCT_COL_NAME];
}

std::string product::dbFieldAsValidString(sqlite3_stmt *stmt, int column_index)
{
    // protect against null values in db.
    const unsigned char *column_text = sqlite3_column_text(stmt, column_index);
    if (column_text != nullptr)
    {
        return std::string(reinterpret_cast<const char *>(column_text));
    }
    else
    {
        debugOutput::sendMessage("Null value in db at column index: " + to_string(column_index), MSG_WARNING);
        // Handle the case when the value is NULL
        return "";
    }
}

bool product::loadParameters()
{
    bool success = true;
    success &= loadParametersFromDb();
    success &= loadParametersFromCsv();
    return success;
}
bool product::loadParametersFromCsv()
{

    loadProductPropertiesFromCsv();
    return true;
}

bool product::loadParametersFromDb()
{

    for (uint8_t i = 0; i < 4; i++)
    {
        isEnabledSizes[i] = false;
    }

    bool valid = isDbValid();

    if (!valid)
    {
        debugOutput::sendMessage("ABORT: Unexpected database layout.", MSG_ERROR);
        return false;
    }

    int rc = sqlite3_open(CONFIG_DB_PATH, &db);
    sqlite3_stmt *stmt;
    string sql_string = "SELECT "
                        "soapstand_product_serial," // 0
                        "mix_pnumbers,"
                        "mix_ratios,"
                        "productId,"
                        "name,"
                        "size_unit,"
                        "payment,"
                        "concentrate_multiplier,"
                        "dispense_speed," // 8
                        "threshold_flow,"
                        "retraction_time," // 10
                        "calibration_const,"
                        "volume_per_tick,"
                        "volume_full,"
                        "volume_remaining,"
                        "volume_dispensed_since_restock," // 15
                        "volume_dispensed_total,"
                        "is_enabled_small,"
                        "is_enabled_medium,"
                        "is_enabled_large," // 19
                        "is_enabled_custom,"
                        "size_small,"
                        "size_medium,"
                        "size_large,"
                        "size_custom_min,"
                        "size_custom_max," // 25
                        "price_small,"
                        "price_medium,"
                        "price_large,"
                        "price_custom," // 29
                        "plu_small,"
                        "plu_medium,"
                        "plu_large,"
                        "plu_custom,"
                        "is_enabled_custom_discount," //
                        "size_custom_discount,"
                        "price_custom_discount," //
                        "is_enabled,"
                        "status_text"
                        " FROM products WHERE soapstand_product_serial='" +
                        std::to_string(m_pnumber) + "';";

    sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);

    int status;
    status = sqlite3_step(stmt);

    int numberOfRecordsFound = 0;

    while (status == SQLITE_ROW)
    {
        numberOfRecordsFound++;

        debugOutput::sendMessage("Record found for product : " + getPNumberAsPString(), MSG_INFO);

        // int columns_count = sqlite3_data_count(stmt);
        // debugOutput::sendMessage("colll count:  " + to_string(columns_count), MSG_INFO);

        m_pnumber = std::stoi(product::dbFieldAsValidString(stmt, 0));
        m_mix_pnumbers = product::dbFieldAsValidString(stmt, 1);
        m_mix_ratios = product::dbFieldAsValidString(stmt, 2);

        m_product_id_combined_with_location_for_backend = product::dbFieldAsValidString(stmt, 3);
        m_name = product::dbFieldAsValidString(stmt, 4);
        m_display_unit = product::dbFieldAsValidString(stmt, 5);
        m_paymentMethod = product::dbFieldAsValidString(stmt, 6);
        m_concentration_multiplier = sqlite3_column_double(stmt, 7);

        if (m_concentration_multiplier < 0.00000001)
        {
            debugOutput::sendMessage("Concentration multiplier was not set. Will default to 1. Was:" + to_string(m_concentration_multiplier), MSG_INFO);
            m_concentration_multiplier = 1.0;
        }

        m_nDispenseSpeedPWM = sqlite3_column_int(stmt, 8);
        m_nThresholdFlow = sqlite3_column_double(stmt, 9);
        m_nRetractionTimeMillis = sqlite3_column_int(stmt, 10);
        m_nThresholdFlow_maximum_allowed = sqlite3_column_double(stmt, 11); // debugOutput::sendMessage("DB_PRODUCTS_CALIBRATION_CONST (reused for maximum flow rate):" + to_string(m_nThresholdFlow_maximum_allowed), MSG_INFO);
        m_nVolumePerTick = sqlite3_column_double(stmt, 12);
        m_nVolumeFull = sqlite3_column_double(stmt, 13);
        m_nVolumeRemaining = sqlite3_column_double(stmt, 14);
        m_nVolumeDispensedSinceRestock = sqlite3_column_double(stmt, 15);
        m_nVolumeDispensedTotalEver = sqlite3_column_double(stmt, 16);
        isEnabledSizes[SIZE_INDEX_SMALL] = sqlite3_column_int(stmt, 17);
        isEnabledSizes[SIZE_INDEX_MEDIUM] = sqlite3_column_int(stmt, 18);
        isEnabledSizes[SIZE_INDEX_LARGE] = sqlite3_column_int(stmt, 19);
        isEnabledSizes[SIZE_INDEX_CUSTOM] = sqlite3_column_int(stmt, 20);
        m_nVolumeTarget_s = sqlite3_column_double(stmt, 21);
        m_nVolumeTarget_m = sqlite3_column_double(stmt, 22);
        m_nVolumeTarget_l = sqlite3_column_double(stmt, 23);
        m_nVolumeTarget_c_min = sqlite3_column_double(stmt, 24);
        m_nVolumeTarget_c_max = sqlite3_column_double(stmt, 25);
        m_price_small = sqlite3_column_double(stmt, 26);
        m_price_medium = sqlite3_column_double(stmt, 27);
        m_price_large = sqlite3_column_double(stmt, 28);
        m_price_custom_per_ml = sqlite3_column_double(stmt, 29);
        m_nPLU_small = product::dbFieldAsValidString(stmt, 30);
        m_nPLU_medium = product::dbFieldAsValidString(stmt, 31);
        m_nPLU_large = product::dbFieldAsValidString(stmt, 32);
        m_nPLU_custom = product::dbFieldAsValidString(stmt, 33);
        m_is_enabled_custom_discount = sqlite3_column_int(stmt, 34);
        m_nVolumeTarget_custom_discount = sqlite3_column_double(stmt, 35);
        m_price_custom_discount_per_liter = sqlite3_column_double(stmt, 36);
        m_is_enabled = sqlite3_column_int(stmt, 37);
        m_status_text = product::dbFieldAsValidString(stmt, 38);

        status = sqlite3_step(stmt); // next record
        // every sqlite3_step returns a row. if it returns 0, it's run over all the rows.
    }

    m_pnumber_loaded_from_db = false;
    if (numberOfRecordsFound == 1)
    {

        debugOutput::sendMessage("DB status: " + to_string(status), MSG_INFO);
        debugOutput::sendMessage("target vaolume serial number: : " + m_pnumber, MSG_INFO);
        debugOutput::sendMessage("mix pnumbers: : " + m_mix_pnumbers, MSG_INFO);
        debugOutput::sendMessage("mix ratios: : " + m_mix_ratios, MSG_INFO);
        debugOutput::sendMessage("target vaolume s: : " + to_string(m_nVolumeTarget_s), MSG_INFO);
        debugOutput::sendMessage("target vaolume medium: : " + to_string(m_nVolumeTarget_m), MSG_INFO);
        debugOutput::sendMessage("target vaolume l: : " + to_string(m_nVolumeTarget_l), MSG_INFO);
        debugOutput::sendMessage("target vaolume custom: : " + to_string(m_price_custom_per_ml), MSG_INFO);
        m_pnumber_loaded_from_db = true;
    }
    else if (numberOfRecordsFound > 1)
    {
        debugOutput::sendMessage("ERROR: Multiple db records found for product: " + std::to_string(m_pnumber), MSG_ERROR);
    }
    else
    {
        // debugOutput::sendMessage("No db record for product: " + std::to_string(m_pnumber), MSG_INFO);
        // debugOutput::sendMessage("no records for: " + sql_string, MSG_INFO);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return true;
}

bool product::testParametersFromDb()
{

    debugOutput::sendMessage("***************************************************************************", MSG_INFO);
    // abandonned by Lode. What about updating the whole product properties at once when needed.
    int rc = sqlite3_open(CONFIG_DB_PATH, &db);

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
    string sql_string = "SELECT dispense_speed FROM products WHERE slot=" + to_string(m_pnumber) + ";";

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

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return pwm;
}
