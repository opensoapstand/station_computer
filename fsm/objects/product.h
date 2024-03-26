//***************************************
//
// product.cpp
// Product (Model) class definition:
//
// Owns current beverage use/transaction.
// Holds and coordinates values from database
// to dispense classes.
//
// created: 01-2022
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef _PRODUCT_H
#define _PRODUCT_H

#include "../dftypes.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <time.h>

#include <sstream>
// #include <vector>

#include <sqlite3.h>

#define MAX_BUF 64
using namespace std;

class product
{
public:
        product();
        ~product();
        // void init(int pnumber, string size_unit, string paymentMethod);
        void init(int pnumber);
        int getPNumber();
        string getPNumberAsPString();

        bool isMixingProduct();
        void getMixRatiosDefault(double *&mixRatios, int &count);
        void getMixPNumbers(int *&pnumbers, int &count);

        int getMixProductsCount();

        int getBasePNumber();
        int getAdditivesCount();
        int getAdditivePNumber(int position);
        int getMixPNumber(int position);
        // double getAdditiveMixRatio(int position);
        // double getBaseMixRatio();
        double getMixRatio(int position);
        // double getMixRatio(int position, int lowDefaultHigh);

        bool getIsEnabled();
        void setIsEnabled(bool isEnabled);
        string getProductStatusText();
        void updateProductState(Dispense_behaviour dispenseState, bool isEmptyContainerDetectionEnabled);
        void setProductStateToEmpty(bool isEmptyContainerDetectionEnabled);
        // void setProductStatusText(string statusText);
        void setProductStateFromString(string productStateText);
        void setProductState(Product_state state);
        const char *getProductStateAsString();
        Product_state getProductState();

        int getPWM(); // deprecated...

        int getRetractionTimeMillis();
        double getPrice(char size);
        // string getDisplayUnits();
        // double convertVolumeMetricToDisplayUnits(double volume);
        string getProductName();
        string getBasePLU(char size);
        // string getPaymentMethod();
        string getFinalPLU(char size, double price, string paymentMethod);

        void customDispenseDiscountData(bool *isEnabled, double *discountVolume, double *discountPrice);

        void registerFlowSensorTickFromPcb();
        void registerFlowSensorTickFromInterrupt();
        double getVolumePerTick(bool accountForConcentrationMultiplier);

        double getVolumeDispensed();
        void setVolumeDispensed(double volume);
        void resetVolumeDispensed();
        double getVolumeRemaining();
        double getProductVolumeDispensedTotalEver();
        double getProductVolumeDispensedSinceLastRestock();
        double getVolumeFull();
        double getThresholdFlow();
        double getThresholdFlow_max_allowed();

        double getVolumeFromSize(char size);
        char getSizeCharFromTargetVolume(double volume);
        void setTargetVolumeFromSize(char size);
        char getTargetVolumeAsChar();
        void setTargetVolume(double volume);
        double getTargetVolume();
        int sizeCharToSizeIndex(char size);
        char sizeIndexToSizeChar(int sizeIndex);
        bool getIsSizeEnabled(char size);

        bool loadParameters(bool onlyLoadFromDb);
        bool loadProductParametersFromDb();
        // bool isColumnInTable(string table, string column_name);
        // void executeSQLStatement(string sql_string);
        bool isDbValid();
        bool testParametersFromDb();
        static std::string dbFieldAsValidString(sqlite3_stmt *stmt, int column_index);
        void loadProductPropertiesFromCsv();

        void parseMixPNumbersAndRatiosCsv(const std::string &mixPNumbersCsvString,
                                          const std::string &mixRatiosLowCsvString,
                                          const std::string &mixRatiosDefaultCsvString,
                                          const std::string &mixRatiosHighCsvString);
        void parseAndSetCustomMixRatios(const std::string &mixPNumbersCsvString, const std::string &mixRatiosCsvString);

        static void parseIntCsvString(const std::string &csvString, int *intArray, int &size);
        static void parseDoubleCsvString(const std::string &csvString, double *doubleArray, int &size);

        string m_name;
        string m_product_id_combined_with_location_for_backend;
        double m_nVolumeTarget_l;               // how much to dispense (large)
        double m_nVolumeTarget_m;               // how much to dispense (medium)
        double m_nVolumeTarget_s;               // how much to dispense (small)
        double m_nVolumeTarget_c_min;           // custom volume dispensing: min (zero volume can be dispensed, but we will still charge for min)
        double m_nVolumeTarget_custom_discount; // custom volume from which a discount is applied.
        double m_nVolumeTarget_c_max;           // custom volume dispensing: max
        double m_nVolumeTarget_f;               // how much to dispense (free sample)
        double m_nVolumeTarget_t = 10000000;    // test dispense (infinite)

private:
        double m_volumePerTick;
        int m_nDispenseSpeedPWM;
        double m_nThresholdFlow;
        int m_nRetractionTimeMillis;

        double m_nVolumeTarget;
        char m_nVolumeTargetAsChar;

        double m_price_small;
        double m_price_medium;
        double m_price_large;
        double m_price_custom_per_ml;
        double m_price_custom_discount_per_liter;
        bool m_is_enabled_custom_discount;
        bool isEnabledSizes[4];

        char sizeIndexToChar[4] = SIZE_INDEX_TO_CHAR_ARRAY;

        string m_product_properties[PRODUCT_DETAILS_FIELD_COUNT];

        // string m_display_unit;
        string m_mix_pnumbers_str;

        int m_mix_pnumbers[DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX];
        int m_mix_pnumbers_count;
        string m_mix_ratios_low_str;
        string m_mix_ratios_default_str;
        string m_mix_ratios_high_str;
        double m_mix_ratios_low[DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX];
        double m_mix_ratios_default[DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX];
        double m_mix_ratios_high[DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX];

        int m_mix_pnumbers_custom[DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX];
        int m_mix_pnumbers_custom_count;
        double m_mix_ratios_custom[DISPENSABLE_PRODUCTS_PER_SLOT_COUNT_MAX];

        // int *m_mix_pnumbers_used; // only adjust ratios!! this is not the place to create new products.
        double *m_mix_ratios_used;
        // int* m_mix_pnumbers_used_count; // only adjust ratios!! this is not the place to create new products.

        string m_nPLU_small;
        string m_nPLU_medium;
        string m_nPLU_large;
        string m_nPLU_custom;
        // string m_paymentMethod;

        double m_nVolumeDispensed;

        double m_nVolumePerTick;

        double m_nVolumeRemaining;
        double m_nVolumeFull;
        double m_nVolumeDispensedSinceRestock;
        double m_nVolumeDispensedTotalEver;

        double m_concentration_multiplier; // some products are concentrated. In that case, we don't dispense the advertised volume. i.e. 1L of 10x concentrate --> dispense 100ml, 900ml of water needs to be added afterwards.

        int m_pnumber;
        bool m_pnumber_loaded_from_db;
        bool m_is_enabled;
        bool m_is_empty_or_has_problem;
        string m_status_text;

        Product_state m_product_state;
        double m_nThresholdFlow_maximum_allowed;

        sqlite3 *db;
};

#endif
