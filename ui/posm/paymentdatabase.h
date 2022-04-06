//***************************************
//
// paymentDatabase.h
// database connection and data collection
//
// created: 15-OCT-2019
// by: Lode Ameije & Ash Singla
//
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************
#ifndef PAYMENTDATABASE_H
#define PAYMENTDATABASE_H

#include <QtDebug>
#include <QtSql/QSqlError>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlRecord>

#include <string>
#include <cstring>
//#include "database_logger.h"

using namespace std;

class paymentDatabase
{
public:
    paymentDatabase();
    ~paymentDatabase();

    void paymentLog(string date, string CT, string TT,
                   string am, string pan, string ref,
                   string mop, string AN, string spdh);
    bool checkDataBaseConnection();
    void reconnectDatabase();

    //void createTable(QString tablename, int numberOfCols, QString, QString);//

private:
   // QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");


};

#endif // PAYMENTDATABASE_H
