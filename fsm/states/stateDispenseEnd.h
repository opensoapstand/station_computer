//***************************************
//
// stateDispenseEnd.h
// dispense state class
//
// created: 26-06-2020
// by: Jason Wang & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef STATEDISPENSEEND__H_
#define STATEDISPENSEEND__H_

#include "../dftypes.h"
#include "../objects/messageMediator.h"
#include "../../library/printer/Adafruit_Thermal.h"
#include "../components/gpio.h"
#include "../components/mcpgpio.h"
#include "../../library/qr/qrcodegen.hpp"
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace qrcodegen;
#include <stdio.h>

//#include <sqlite3.h>

#include "../fsm.h"

#include "stateVirtual.h"

#include <ctime>

#define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"

class stateDispenseEnd : public stateVirtual
{
    public:
        stateDispenseEnd();
        stateDispenseEnd(messageMediator * message); 
        ~stateDispenseEnd();

        string toString();

        dispenser* cassettes;

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
        struct tm * timeinfo;

        char now[50];

        DF_ERROR updateDB();
        DF_ERROR sendDB();
        DF_ERROR printer();
        DF_ERROR QRgen();
        std::string toSvgString(const QrCode &qr, int border);
        void printQr(const QrCode &qr);

};
#endif
