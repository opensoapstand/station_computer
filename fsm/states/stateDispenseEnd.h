//***************************************
//
// stateDispenseEnd.h
// dispense state class
//
// created: 01-2022
// by:Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
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

    // dispenser *productDispensers;

    DF_ERROR onEntry();
    DF_ERROR onAction();
    DF_ERROR onExit();

private:
    char dispensedVolumeToSmallestFixedSize();
    void adjustSizeToDispensedVolume();
    int m_slot_index;
    int m_slot;
    // char command;

    sqlite3 *db;
    int rc;

    time_t rawtime;
    struct tm *timeinfo;

    char now[50];

    DF_ERROR dispenseEndUpdateDB(bool isValidTransaction);
    bool sendTransactionToCloud(double volume_remaining);
    void setup_and_print_receipt();
    void sendEndTransactionMessageToUI();
    
    // std::string getMachineID();
    std::string getProductID(int slot);

    double getFinalPrice();

    DF_ERROR handleTransactionPayment();
    DF_ERROR dispenseEndUpdateTransactionDB();
    DF_ERROR databaseUpdateSql(string sqlStatement, string dbPath);

    // DF_ERROR print_text(std::string text);

    void write_curl_to_file(std::string curl_params);
    CURL *curl;
    CURLcode res;
    char *curl_data;
};
#endif