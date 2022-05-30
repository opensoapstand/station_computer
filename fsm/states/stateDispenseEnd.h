//***************************************
//
// stateDispenseEnd.h
// dispense state class
//
// created: 01-2022
// by:Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef STATEDISPENSEEND__H_
#define STATEDISPENSEEND__H_

#include "../dftypes.h"
#include "../objects/messageMediator.h"
#include "../objects/machine.h"
// #include "../../library/printer/Adafruit_Thermal.h"
#include "../components/gpio.h"
#include "../../library/qr/qrcodegen.hpp"
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <curl/curl.h>
#include <ctime>

#include "../fsm.h"
#include "stateVirtual.h"


class stateDispenseEnd : public stateVirtual
{
public:
    stateDispenseEnd();
    stateDispenseEnd(messageMediator *message);
    ~stateDispenseEnd();

    string toString();

    dispenser *productDispensers;



    DF_ERROR onEntry();
    DF_ERROR onAction();
    DF_ERROR onExit();

private:
    char dispensedVolumeToSmallestFixedSize();
    void adjustSizeToDispensedVolume();
    int pos;
    char command;
    // char size;

    sqlite3 *db;
    int rc;

    time_t rawtime;
    struct tm *timeinfo;

    char now[50];

    DF_ERROR dispenseEndUpdateDB(bool is_container_empty);
    DF_ERROR sendTransactionToCloud();
    DF_ERROR setup_and_print_receipt();
    // DF_ERROR print_receipt();

    // DF_ERROR print_receipt(string name_receipt, string receipt_cost, string receipt_volume_formatted,string time_stamp, string units, string paymentMethod, string plu);
    // DF_ERROR setup_receipt_from_data_and_slot(int slot, double volume_dispensed, double volume_requested, double price, string time_stamp);
    
    std::string getMachineID();
    std::string getProductID(int slot);
    std::string getUnits(int slot);
    std::string getUnitsFromDb(int slot);

    void loadProductData(int slot);

    double getFinalPrice();

    DF_ERROR handleTransactionPayment();
    DF_ERROR dispenseEndUpdateTransactionDB();
    DF_ERROR databaseUpdateSql(string sqlStatement);

    DF_ERROR print_text(std::string text);

    void bufferCURL(std::string curl_params);
    CURL *curl;
    CURLcode res;
    char *curl_data;
};
#endif
