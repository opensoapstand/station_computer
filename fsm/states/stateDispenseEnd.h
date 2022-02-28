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
#include "../../library/printer/Adafruit_Thermal.h"
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
    int pos;
    char command;
    char size;

    sqlite3 *db;
    int rc;

    time_t rawtime;
    struct tm *timeinfo;

    char now[50];

    DF_ERROR updateDB();
    DF_ERROR sendDB();
    DF_ERROR printer();
    std::string getMachineID();
    std::string getProductID(int slot);
    void bufferCURL(std::string curl_params);
    std::string getUnits(int slot);

    CURL *curl;
    CURLcode res;
    char *curl_data;
};
#endif
