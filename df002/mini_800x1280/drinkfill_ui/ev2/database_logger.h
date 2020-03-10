//***************************************
//
// database_logger.h
// database connection and data collection
//
// created: 12-AUG-2019
// by: Jason Wang
//
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************
#ifndef DATABASE_LOGGER_H
#define DATABASE_LOGGER_H

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include <QDateTime>

#include <string>
#include <cstring>

#define small 1
#define large 2
#define databaseName "mini-3"

using namespace std;

class database_logger
{
public:
    database_logger();
    ~database_logger();

    void event_log(QString id, QDateTime curTime, int eventCode,
                   QString text, int volume);

    void paymentLog(string date, string CT, string TT,
                   string am, string pan, string ref,
                   string mop, string AN, string spdh);

    void surveyLog(QDateTime curTime, QString drinkAnswer, QString emaillAddress, QString fillOrNot, QString bottleAnswer);

    bool checkDataBaseConnection();
    void reconnectDatabase();
    int getInventory(int eventCode, int inventoryValue, QString machID);

    int getNumProductLog();
    QByteArray getImgArray(int eventCode, QString machID);
    QString getMachineID();
    QSqlDatabase getDB();
    QString getProductPrice(int eventCode, int size, QString machID); //size if define as small = 1 and large = 2
    QString getProductName(int eventCode, QString machID);
    //QString getProductVolume(int eventCode);

    bool checkEmailDuplication(QString emailInput);

private:
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
};

#endif // DATABASE_LOGGER_H
