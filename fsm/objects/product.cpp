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

product::product()
{
}

product::~product()
{
    debugOutput::sendMessage("product: ~product", MSG_INFO);
}

void product::init(int pnumber)
{
    m_pnumber = pnumber;
    // m_display_unit = size_unit;
    // m_paymentMethod = paymentMethod;
    this->loadParameters();
}

int product::getPNumber()
{
    return m_pnumber;
}

string product::getPNumberAsPString()
{
    return "P-" + std::to_string(m_pnumber);
}

string product::getProductName()
{
    return m_product_properties[CSV_PRODUCT_COL_NAME];
}

int product::getBasePNumber()
{
    if (!isMixingProduct())
    {
        if (m_mix_pnumbers_count == 1) // if exactly one mix_pnumber, we consider it the base
        {
            return m_mix_pnumbers[0];
        }
        else
        {
            return m_pnumber;
        }
    }
    else
    {
        return m_mix_pnumbers[0];
    }
}

int product::getMixProductsCount()
{
    // sum of additives and base
    return m_mix_pnumbers_count;
}

int product::getAdditivesCount()
{
    if (isMixingProduct())
    {
        return m_mix_pnumbers_count - 1; // subtract base product
    }
    else
    {
        return 0;
    }
}

double product::getAdditiveMixRatio(int position)
{
    if (position == 0)
    {
        debugOutput::sendMessage("ASSERT ERROR: mixing position cannot be zero. ", MSG_ERROR);
        return 0;
    }
    return getMixRatio(position);
}

double product::getBaseMixRatio()
{
    // ratio of base product
    if (!isMixingProduct())
    {
        return 1.0;
    }
    else
    {
        return getMixRatio(0);
    }
}

double product::getMixRatio(int position)
{
    // ratio of mixing product (pos 0=base,  pos>=1 = additives.  pos1=additive1,...)
    if (!isMixingProduct())
    {
        return 1.0;
    }
    return m_mix_ratios[position];
}

int product::getAdditivePNumber(int position)
{
    if (position == 0)
    {
        // position starts from 1
        debugOutput::sendMessage("ASSERT ERROR: Additives numbering starts from 1. 0 was provided.", MSG_ERROR);
    }
    if (position > getAdditivesCount())
    {
        debugOutput::sendMessage("ASSERT ERROR: Additives position provided is too high. Additive does not exist.", MSG_ERROR);
        return PRODUCT_DUMMY;
    }
    return getMixPNumber(position); // first position is base pnumber. but mix pnumbers start from 1.
}

int product::getMixPNumber(int position)
{
    //  for (uint8_t i=0;i<m_mix_pnumbers_count;i++){
    //     debugOutput::sendMessage("array tesrrrrrrrtstj:  " + to_string(m_mix_pnumbers[i]) + " at pos: " + to_string(i), MSG_INFO);
    // }

    return m_mix_pnumbers[position]; // first position is base pnumber. but pnumbers additives start from 1.
}

bool product::isMixingProduct()
{
    // the implications are big: mixing products have a lot less properties than actual products (i.e. no calibration parameter)
    bool isMix = false;
    if (m_mix_pnumbers_count <= 0)
    {
        isMix = false;
    }
    else if (m_mix_pnumbers_count == 1)
    {
        isMix = false;
    }
    else if (m_mix_pnumbers_count > 1)
    {
        isMix = true;
    }
    return isMix;
}

void product::getMixRatios(double *&mixRatios, int &count)
{
    mixRatios = m_mix_ratios;
    count = m_mix_ratios_count;
}
void product::getMixPNumbers(int *&pnumbers, int &count)
{
    pnumbers = m_mix_pnumbers;
    count = m_mix_pnumbers_count;
}

double product::getVolumeDispensed()
{
    return m_nVolumeDispensed;
}

void product::setVolumeDispensed(double volume)
{
    m_nVolumeDispensed = volume;
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

    if (m_nThresholdFlow_maximum_allowed < getThresholdFlow())
    {
        // 2023-11: in the db, column "calibration_const" was reused to hold this variable, for AP. if not set, set it to an arbitray high value
        return 1000.0; // 1L per second. magic number
    }
    else
    {
        // when using kegs, an empty keg results in pressurised CO2 or beergas being pushed through the flow sensor. That speeds up the flowsensor.
        return m_nThresholdFlow_maximum_allowed;
    }
}

double product::getVolumePerTick()
{
    return m_nVolumePerTick;
}
void product::registerFlowSensorTickFromPcb()
{
    // tick from flowsensor interrupt will increase dispensed volume.
    // debugOutput::sendMessage("product: Flow poll TICK from pcb." + getPNumberAsPString(), MSG_INFO);
    m_nVolumeDispensed += getVolumePerTick() * m_concentration_multiplier;
}

void product::registerFlowSensorTickFromInterrupt()
{
    // tick from flowsensor interrupt will increase dispensed volume.
    // cout << "Registering Flow!!" << endl << "Vol disp: " << m_nVolumeDispensed << endl << "vol per tick: " << m_nVolumePerTick << endl;
    debugOutput::sendMessage("Product: Interrupt TICK from pcb." + getPNumberAsPString(), MSG_INFO);
    m_nVolumeDispensed += getVolumePerTick() * m_concentration_multiplier;
}

double product::getVolumeFull()
{
    return m_nVolumeFull;
}
double product::getVolumeRemaining()
{
    return m_nVolumeRemaining;
}
double product::getProductVolumeDispensedTotalEver()
{
    // total volume ever dispensed by this slot.
    return m_nVolumeDispensedTotalEver;
}

double product::getProductVolumeDispensedSinceLastRestock()
{
    return m_nVolumeDispensedSinceRestock;
}

char product::getTargetVolumeAsChar()
{
    return getSizeCharFromTargetVolume(m_nVolumeTarget);
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
    else if (volume == m_nVolumeTarget_f)
    {
        return 'f';
    }
    else if (volume == TEST_DISPENSE_TARGET_VOLUME)
    {
        return 't';
    }
    else
    {
        debugOutput::sendMessage("Get size from volume, not found, will default to custom dispense for volume " + to_string(volume), MSG_INFO);
        return 'c';
    }
}
double product::getTargetVolume()
{
    return m_nVolumeTarget;
}

void product::setTargetVolume(double volume)
{
    m_nVolumeTarget = volume;
}

void product::setTargetVolumeFromSize(char size)
{
    m_nVolumeTarget = getVolumeFromSize(size);
}

double product::getVolumeFromSize(char size)
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
    else if (size == 'f')
    {
        return m_nVolumeTarget_f;
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
//     if (volume >= getVolumeFromSize(SIZE_LARGE_CHAR))
//     {
//         // go for discount
//         // price per ml
//         double largePricePerMl = getPrice(SIZE_LARGE_CHAR) / getVolumeFromSize(SIZE_LARGE_CHAR);

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

// string product::getPaymentMethod()
// {
//     return m_paymentMethod;
// }
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

// string product::getDisplayUnits()
// {
//     return m_display_unit;
// }

string product::getFinalPLU(char size, double price, string paymentMethod)
{

    string base_plu = getBasePLU(size);
    debugOutput::sendMessage("Base PLU: " + base_plu, MSG_INFO);
    char chars_plu_dynamic_formatted[MAX_BUF];

    // std::string paymentMethod = getPaymentMethod();
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

// double product::convertVolumeMetricToDisplayUnits(double volume)
// {
//     double converted_volume;

//     if (getDisplayUnits() == "oz")
//     {

//         converted_volume = volume * ML_TO_OZ;
//     }
//     else if (getDisplayUnits() == "g")
//     {

//         converted_volume = volume * 1;
//     }
//     else
//     {
//         converted_volume = volume;
//     }
//     return converted_volume;
// }

int product::getRetractionTimeMillis()
{
    return m_nRetractionTimeMillis;
}

int product::getPWM()
{
    return m_nDispenseSpeedPWM;
}

string product::getBasePLU(char size)
{
    debugOutput::sendMessage("get plu for size:  " + to_string(size), MSG_ERROR);
    if (size == 's')
    {
        debugOutput::sendMessage("plu smaalalll " + m_nPLU_small, MSG_ERROR);

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
    // In fsm, we're only reading the db. Avoid writing. All results are sent to UI for updating db. This was a necessary transition...

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
    while (status == SQLITE_ROW)
    {
        int columns_count = 3;
        for (int column_index = 0; column_index < columns_count; column_index++)
        {
            // for every row, go over all the the items (0=cid, 1=name, 2=type,3=notnull,...)
            switch (column_index)
            {
            case (1):
            {
                string column_name = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_index)));
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
        "name",
        "currency",
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
        "status_text",
        "is_enabled_sample",
        "size_sample",
        "price_sample"};
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
    if (getPNumber() != CUSTOM_MIX_PNUMBER)
    {
        debugOutput::sendMessage("Product: Data loading for product: " + to_string(getPNumber()), MSG_INFO);
        success &= loadProductParametersFromDb();
        loadProductPropertiesFromCsv();
    }
    else
    {
        debugOutput::sendMessage("No data loading for custom mix product.", MSG_INFO);
    }
    return success;
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

        bool stringsAreDifferent;
        stringsAreDifferent = m_product_properties[CSV_PRODUCT_COL_ID].compare(getPNumberAsPString());
        if (!(stringsAreDifferent))
        {
            debugOutput::sendMessage("Product found in products file and loaded: " + m_product_properties[CSV_PRODUCT_COL_ID] + " " + m_product_properties[CSV_PRODUCT_COL_NAME], MSG_INFO);
            return;
        }
    }
    debugOutput::sendMessage("Could not load product from products file (not found). Pnumber " + std::to_string(m_pnumber), MSG_ERROR);
}

bool product::loadProductParametersFromDb()
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
                        "is_enabled,"
                        "concentrate_multiplier,"
                        "dispense_speed," // 7
                        "threshold_flow,"
                        "retraction_time," // 9
                        "calibration_const,"
                        "volume_per_tick,"
                        "volume_full,"
                        "volume_remaining,"
                        "volume_dispensed_since_restock," // 14
                        "volume_dispensed_total,"
                        "is_enabled_small,"
                        "is_enabled_medium,"
                        "is_enabled_large," // 18
                        "is_enabled_custom,"
                        "size_small,"
                        "size_medium,"
                        "size_large,"
                        "size_custom_min,"
                        "size_custom_max," // 24
                        "price_small,"
                        "price_medium,"
                        "price_large,"
                        "price_custom," // 28
                        "plu_small,"
                        "plu_medium,"
                        "plu_large,"
                        "plu_custom,"
                        "is_enabled_custom_discount," // 33
                        "size_custom_discount,"
                        "price_custom_discount," // 35
                        "status_text,"           // 36
                        "is_enabled_sample,"
                        "size_sample,"
                        "price_sample"
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
        m_mix_pnumbers_str = product::dbFieldAsValidString(stmt, 1);
        m_mix_ratios_str = product::dbFieldAsValidString(stmt, 2);

        m_product_id_combined_with_location_for_backend = product::dbFieldAsValidString(stmt, 3);
        m_name = product::dbFieldAsValidString(stmt, 4);
        // m_display_unit = product::dbFieldAsValidString(stmt, 5);
        m_is_enabled = sqlite3_column_int(stmt, 5);
        // m_paymentMethod = product::dbFieldAsValidString(stmt, 6);
        m_concentration_multiplier = sqlite3_column_double(stmt, 6);

        if (m_concentration_multiplier < 0.00000001)
        {
            debugOutput::sendMessage("Concentration multiplier was not set. Will default to 1. Was:" + std::to_string(m_concentration_multiplier), MSG_INFO);
            m_concentration_multiplier = 1.0;
        }

        m_nDispenseSpeedPWM = sqlite3_column_int(stmt, 7);
        m_nThresholdFlow = sqlite3_column_double(stmt, 8);
        m_nRetractionTimeMillis = sqlite3_column_int(stmt, 9);
        m_nThresholdFlow_maximum_allowed = sqlite3_column_double(stmt, 10);
        m_nVolumePerTick = sqlite3_column_double(stmt, 11);
        m_nVolumeFull = sqlite3_column_double(stmt, 12);
        m_nVolumeRemaining = sqlite3_column_double(stmt, 13);
        m_nVolumeDispensedSinceRestock = sqlite3_column_double(stmt, 14);
        m_nVolumeDispensedTotalEver = sqlite3_column_double(stmt, 15);
        isEnabledSizes[SIZE_INDEX_SMALL] = sqlite3_column_int(stmt, 16);
        isEnabledSizes[SIZE_INDEX_MEDIUM] = sqlite3_column_int(stmt, 17);
        isEnabledSizes[SIZE_INDEX_LARGE] = sqlite3_column_int(stmt, 18);
        isEnabledSizes[SIZE_INDEX_CUSTOM] = sqlite3_column_int(stmt, 19);
        m_nVolumeTarget_s = sqlite3_column_double(stmt, 20);
        m_nVolumeTarget_m = sqlite3_column_double(stmt, 21);
        m_nVolumeTarget_l = sqlite3_column_double(stmt, 22);
        m_nVolumeTarget_c_min = sqlite3_column_double(stmt, 23);
        m_nVolumeTarget_c_max = sqlite3_column_double(stmt, 24);
        m_price_small = sqlite3_column_double(stmt, 25);
        m_price_medium = sqlite3_column_double(stmt, 26);
        m_price_large = sqlite3_column_double(stmt, 27);
        m_price_custom_per_ml = sqlite3_column_double(stmt, 28);
        m_nPLU_small = product::dbFieldAsValidString(stmt, 29);
        m_nPLU_medium = product::dbFieldAsValidString(stmt, 30);
        m_nPLU_large = product::dbFieldAsValidString(stmt, 31);
        m_nPLU_custom = product::dbFieldAsValidString(stmt, 32);
        m_is_enabled_custom_discount = sqlite3_column_int(stmt, 33);
        m_nVolumeTarget_custom_discount = sqlite3_column_double(stmt, 34);
        m_price_custom_discount_per_liter = sqlite3_column_double(stmt, 35);

        m_status_text = product::dbFieldAsValidString(stmt, 36);
        m_nVolumeTarget_f = sqlite3_column_double(stmt, 38);
        status = sqlite3_step(stmt); // next record

        // every sqlite3_step returns a row. if status is 101=SQLITE_DONE, it's run over all the rows.
    }

    parseMixPNumbersAndRatiosCsv(m_mix_pnumbers_str, m_mix_ratios_str);
    // product::parseDoubleCsvString(m_mix_ratios_str, m_mix_ratios, m_mix_ratios_count);
    // product::parseIntCsvString(m_mix_pnumbers_str, m_mix_pnumbers, m_mix_pnumbers_count);

    // if (m_mix_pnumbers_count != m_mix_ratios_count)
    // {
    //     debugOutput::sendMessage("DB mixing ASSERT ERROR: Amount of mixing pnumber and their ratios is not equal!!! pnumbers:" + m_mix_pnumbers_str + " mixing ratios: " + m_mix_ratios_str, MSG_ERROR);
    // }

    m_pnumber_loaded_from_db = false;
    if (numberOfRecordsFound == 1)
    {
        debugOutput::sendMessage("DB loading ok. Found one match. status: " + to_string(status), MSG_INFO);
        debugOutput::sendMessage("DB target volume serial number: : " + m_pnumber, MSG_INFO);
        debugOutput::sendMessage("DB mix pnumbers: : " + m_mix_pnumbers_str, MSG_INFO);
        debugOutput::sendMessage("DB default mix ratios: : " + m_mix_ratios_str, MSG_INFO);
        debugOutput::sendMessage("DB target volume small:  " + to_string(m_nVolumeTarget_s), MSG_INFO);
        debugOutput::sendMessage("DB target volume medium: " + to_string(m_nVolumeTarget_m), MSG_INFO);
        debugOutput::sendMessage("DB target volume large : " + to_string(m_nVolumeTarget_l), MSG_INFO);
        debugOutput::sendMessage("DB target volume free : " + to_string(m_nVolumeTarget_f), MSG_INFO);
        debugOutput::sendMessage("DB PL small:  " + m_nPLU_small, MSG_INFO);
        debugOutput::sendMessage("DB PLU medium: " + m_nPLU_medium, MSG_INFO);
        debugOutput::sendMessage("DB PLU large : " + m_nPLU_large, MSG_INFO);
        debugOutput::sendMessage("DB target volume free : " + to_string(m_nVolumeTarget_f), MSG_INFO);
        debugOutput::sendMessage("DB target volume custom: " + to_string(m_price_custom_per_ml), MSG_INFO);

        m_pnumber_loaded_from_db = true;
    }
    else if (numberOfRecordsFound > 1)
    {
        debugOutput::sendMessage("ERROR: Multiple db records found for product: " + std::to_string(m_pnumber), MSG_ERROR);
    }
    else
    {
        debugOutput::sendMessage("No db record for product: " + std::to_string(m_pnumber), MSG_ERROR);
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

void product::parseMixPNumbersAndRatiosCsv(const std::string &mixPNumbersCsvString, const std::string &mixRatiosCsvString)
{

    product::parseDoubleCsvString(mixRatiosCsvString, m_mix_ratios, m_mix_ratios_count);
    product::parseIntCsvString(mixPNumbersCsvString, m_mix_pnumbers, m_mix_pnumbers_count);

    if (m_mix_pnumbers_count != m_mix_ratios_count)
    {
        debugOutput::sendMessage("DB mixing ASSERT ERROR: Amount of mixing pnumber and their ratios is not equal!!! pnumbers:" + mixPNumbersCsvString + " mixing ratios: " + mixRatiosCsvString, MSG_ERROR);
    }
}

void product::parseIntCsvString(const std::string &csvString, int *intArray, int &size)
{
    std::stringstream ss(csvString);
    std::string token;
    size = 0;

    while (std::getline(ss, token, ','))
    {

        if (size < DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX)
        {
            intArray[size] = std::stoi(token);
            // debugOutput::sendMessage("array teststj:  " + to_string(intArray[size]) + " at pos: " + to_string(size), MSG_INFO);

            size++;
        }
        else
        {
            debugOutput::sendMessage("ERROR: Array size exceeded maximum limit (will stop adding):  " + std::to_string(size), MSG_ERROR);
        }
    }
}

void product::parseDoubleCsvString(const std::string &csvString, double *doubleArray, int &size)
{
    std::stringstream ss(csvString);
    std::string token;
    size = 0;

    while (std::getline(ss, token, ','))
    {
        if (size < DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX)
        {
            doubleArray[size++] = std::stod(token);
        }
        else
        {
            debugOutput::sendMessage("ERROR: Array size exceeded maximum limit (will stop adding):  " + std::to_string(size), MSG_ERROR);
        }
    }
}