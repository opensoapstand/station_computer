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

#include <sqlite3.h>

// #define CONFIG_DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"

#define MAX_BUF 64

using namespace std;

class product
{
public:
        // product();
        product();
        void init(int pnumber);

        // product(int slot, string name, double calibration_const, double nVolumePerTick, int dispense_speed_pwm,
        //         double nVolumeTarget_s, double nVolumeTarget_m, double nVolumeTarget_l, double nVolumeTarget_c_min, double nVolumeTarget_c_max,
        //         double price_small, double price_m, double price_large, double price_custom_per_liter,
        //         string nPLU_small, string nPLU_medium,  string nPLU_large, string nPLU_c,
        //         string paymentMethod, string name_receipt, string display_unit);
        ~product();

        // getter

        double getVolumePerTick();

        double getVolumeFromSize(char size);
        char getSizeCharFromTargetVolume(double volume);
        void setTargetVolume(double volume);
        double setTargetVolumeFromSize(char size);
        double getTargetVolume();

        double getPrice(char size);
        // double getCustomVolumePriceDependingOnDispensedVolume(double volume);
        string getDisplayUnits();
        double convertVolumeMetricToDisplayUnits(double volume);
        string getProductName();
        string getBasePLU(char size);
        int getPNumber();
        string getPNumberAsPString();
        string getPaymentMethod() { return m_paymentMethod; }

        static std::string dbFieldAsValidString(sqlite3_stmt *stmt, int column_index);

        // void setTargetVolume(double nVolumeTarget){m_nVolumeTarget = nVolumeTarget;};

        bool getIsEnabled();
        void setIsEnabled(bool isEnabled);
        string getStatusText();
        void setStatusText(string statusText);

        bool getIsSizeEnabled(char size);
        
        // char getClosestLowerTargetVolume(double volume);
        // DB Updates
        // void recordSale(int volume);
        // void restockProduct(int volume);

        // void productInfo();
        // void productVolumeInfo();

        int getPWM();
        // int getPWMFromDB();
        // double getVolPerTickFromDB();
        bool loadParameters();
        bool loadParametersFromCsv();
        bool loadProductParametersFromDb();


        bool testParametersFromDb();

        double getVolumeDispensed();
        void customDispenseDiscountData(bool* isEnabled, double* discountVolume, double* discountPrice);
        void setVolumeDispensed(double volume);
        bool isDbValid();

        void resetVolumeDispensed();

        int sizeCharToSizeIndex(char size);
        char sizeIndexToSizeChar(int sizeIndex);

        void addMaintenancePwdToMachineTable();
        void addShowTransactionsToMachineTable();
        void addSizeCustomDiscountToProductsTable();
        void addPriceCustomDiscountToProductsTable();
        void addIsEnabledCustomDiscountToProductsTable();

        bool isColumnInTable(string table, string column_name);
        bool isMaintenancePwdInMachineTable();
        bool isShowTransactionsInMachineTable();
        void executeSQLStatement(string sql_string);

        void registerFlowSensorTickFromPcb();
        void registerFlowSensorTickFromInterrupt();
        int getSlot();

        double getVolumeRemaining();
        double getProductVolumeDispensedTotalEver();
        double getProductVolumeDispensedSinceLastRestock();
        double getVolumeFull();
        double getThresholdFlow();
        double getThresholdFlow_max_allowed();
        int getRetractionTimeMillis();

        string getFinalPLU(char size, double price);

        double m_volumePerTick;
        int m_nDispenseSpeedPWM;
        double m_nThresholdFlow;
        int m_nRetractionTimeMillis;

        double m_nVolumeTarget_l;     // how much to dispense (large)
        double m_nVolumeTarget_m;     // how much to dispense (medium)
        double m_nVolumeTarget_s;     // how much to dispense (small)
        double m_nVolumeTarget_c_min; // custom volume dispensing: min (zero volume can be dispensed, but we will still charge for min)
        double m_nVolumeTarget_custom_discount; // custom volume from which a discount is applied.
        double m_nVolumeTarget_c_max; // custom volume dispensing: max

        double m_nVolumeTarget_t = 10000000; // test dispense (infinite)


        double m_nVolumeTarget;

        // double m_price;
        double m_price_small;
        double m_price_medium;
        double m_price_large;
        double m_price_custom_per_ml;
        double m_price_custom_discount_per_liter;
        bool m_is_enabled_custom_discount;
        bool isEnabledSizes[4];

        char sizeIndexToChar[4] = SIZE_INDEX_TO_CHAR_ARRAY;



        string m_product_properties[100];
        string m_name;
        string m_product_id_combined_with_location_for_backend;
        string m_display_unit;
        string m_mix_pnumbers;
        string m_mix_ratios;


        // string m_name_receipt;
        // double m_buttonPressDuration;
        string m_nPLU_small;
        string m_nPLU_medium;
        string m_nPLU_large;
        string m_nPLU_custom;
        string m_paymentMethod;

        // int m_PWM;
        // DF_ERROR reloadProductFromDB();


        // void setSlot(int slot); // please don't set after instantiated!

        double m_nVolumeDispensed;

        double m_nVolumePerTick;
        // TODO: Determine more data to modify per transaction...

        double m_nVolumeRemaining;
        double m_nVolumeFull;
        double m_nVolumeDispensedSinceRestock;
        double m_nVolumeDispensedTotalEver;

        double m_concentration_multiplier; // some products are concentrated. In that case, we don't dispense the advertised volume. i.e. 1L of 10x concentrate --> dispense 100ml, 900ml of water needs to be added afterwards.
        int m_pnumber;
        bool m_pnumber_loaded_from_db;
        bool m_is_enabled;
        string m_status_text;
        void loadProductPropertiesFromCsv();
private:

        double m_nThresholdFlow_maximum_allowed;
        // double m_calibration_const;

        // void setProductName(string productName);
        double getVolPerTick();

        sqlite3 *db;
        // int rc;
};

#endif
