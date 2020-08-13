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

#define databaseName "mini-4"

using namespace std;

class database_logger
{
public:
    database_logger();
    ~database_logger();

    void eventLog(QString id, QDateTime curTime, int eventCode,
                   QString text, int volume);

    void paymentLog(string date, string CT, string TT,
                   string am, string pan, string ref,
                   string mop, string AN, string spdh);

    void surveyLog(QDateTime curTime, QString drinkAnswer, QString emaillAddress, QString fillOrNot);
    void productLog(int option, QString price, QString name, QString fileName, QByteArray productImg, bool price1Edit, bool nameEdit, QString machineid);
    void productLog(int option, QString amount, QString machineid);
    void price2Update(int option, QString price2, QString machineid);



    bool checkDataBaseConnection();
    void reconnectDatabase();

    /*Set up*/
    void productLogSetup(int numRow, int numDrinks, QString machineid);
    void machineIdSetup(QString machineId, int numDrink);

    /* getter */
    int getInventory(int eventCode,QString machineid);
    int getNumProductLog(QString machineid);
    int getNumMachine();
    QString getMachineID(QString machineid);
    QString getAllMachineID(int number);
    QSqlDatabase getDB();
    QByteArray getImgArray(int eventCode, QString machineid);

    void resetProductLog(QString machineid);
    //void createTable(QString tablename, int numberOfCols, QString, QString);//

private:
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
};

#endif // DATABASE_LOGGER_H
