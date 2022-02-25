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
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef _PRODUCT_H
#define _PRODUCT_H

#include "../dftypes.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <time.h>

#include <sqlite3.h>

// #define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"

using namespace std;

class product
{
public:
        product();
        // product(int slot);
        product(int slot, string name, double calibration_const, double nVolumePerTick, int dispense_speed_pwm,
                double nVolumeTarget_s, double nVolumeTarget_m, double nVolumeTarget_l, double nVolumeTarget_c_min, double nVolumeTarget_c_max,
                double price_small, double price_m, double price_large, double price_c_per_liter, 
                string nPLU_small, string nPLU_m,  string nPLU_large, string nPLU_c,
                string paymentMethod, string name_receipt, string display_unit);
        ~product();

        //getter
        double getVolumePerTick();
        double getVolumeDispensed() { return m_nVolumeDispensed; }
        double getVolumeDispensedPreviously();
        double getTargetVolume(char size);
        double getPrice(char size);
        string getDisplayUnits();

        string getPLU(char size);
        string getPaymentMethod() { return m_paymentMethod; }

        //void setTargetVolume(double nVolumeTarget){m_nVolumeTarget = nVolumeTarget;};

        // Interrupt Helpers
        DF_ERROR initDispense(int nVolumeToDispense, double nPrice);
        DF_ERROR stopDispense();

        double getVolumeSinceLastPoll();
        bool isDispenseTargetVolumeReached();
        bool registerFlowSensorTick();

        // DB Updates
        void recordSale(int volume);
        void refill(int volume);

        void productInfo();
        void productVolumeInfo();

        int getPWM();
        int getPWMFromDB();
        double getVolPerTickFromDB();
        
        double m_volumePerTick;
        int m_nDispenseSpeedPWM;

        double m_nVolumeDispensedPreviously;
        double m_nVolumeTarget;
        double m_nVolumeTarget_l;            //how much to dispense (large)
        double m_nVolumeTarget_m;            //how much to dispense (medium)
        double m_nVolumeTarget_s;            //how much to dispense (small)
        double m_nVolumeTarget_c_min;            //custom volume dispensing: min (zero volume can be dispensed, but we will still charge for min)
        double m_nVolumeTarget_c_max;            //custom volume dispensing: max

        double m_nVolumeTarget_t = 10000000; //test dispense (infinite)
        double m_nVolumeDispensed;           //how much has been dispensed in this sale
        double m_price;
        double m_price_small;
        double m_price_medium;
        double m_price_large;
        double m_price_c_per_liter;

        string m_name;
        string m_display_unit;
        string m_name_receipt;
        double m_buttonPressDuration;
        string m_nPLU_small;
        string m_nPLU_medium;
        string m_nPLU_large;
        string m_nPLU_custom;
        string m_paymentMethod;
        // int m_PWM;
        double m_nVolumePerTick;

        time_t rawtime;
        struct tm *timeinfo;

        char m_nStartTime[50];

private:
        // TODO: Determine more data to modify per transaction...
        int m_nSlot;

        bool isDispenseFinished;
        double m_nVolumeDispensedSinceLastPoll;
        double m_calibration_const;

        

        double m_nTickCount;
        bool valueChange;

        void setSlot(int slot);
        void setProductName(string productName);
        double getVolPerTick();

        sqlite3 *db;
        int rc;
};

#endif
