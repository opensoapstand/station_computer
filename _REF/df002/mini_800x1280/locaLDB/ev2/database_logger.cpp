//***************************************
//
// database_logger.cpp
// database connection and data collection
//
// created: 12-AUG-2019
// by: Jason Wang
//
// Version number: ev2.2
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************
#include "database_logger.h"

#include <QtDebug>
#include <QSqlError>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>


database_logger::database_logger()
{
    //db.setHostName("drinkfilldb.cqraowv1leln.us-west-2.rds.amazonaws.com");
    db.setPort(3306);
    //db.setUserName("admin");
    //db.setPassword("Drinkfill");
    //db.setDatabaseName(databaseName);
    //db.setDatabaseName("/home/drinkfill/dev/evt/drinkfill_ui/database/datacollection.db");
    db.setDatabaseName("/home/soapstand/Desktop/mini_800x1280/datacollection.db");

    if (db.open()){
        qDebug() << "connected " << db.hostName();
    }

    else {
        qDebug() << "connection failed: " << db.lastError();
    }


    QSqlQuery qry(db);
    qry.prepare("CREATE TABLE IF NOT EXISTS `survey` ("
                "`time` DATETIME NOT NULL,"
                "`drinkAnswer` VARCHAR(50),"
                "`emailAddress` VARCHAR(70),"
                "`bottleAnswer` VARCHAR(50),"
                "`action` VARCHAR(20))");
    if(qry.exec())
    {
        qry.lastError();
    }

    qry.prepare("CREATE TABLE IF NOT EXISTS `event` ("
                "`machine_id` VARCHAR(20) NOT NULL,"
                "`time` DATETIME NOT NULL,"
                "`event_code` INT NOT NULL,"
                "`text` VARCHAR(255),"
                "`inventory` INT);");
    if(qry.exec())
    {
        qry.lastError();
    }

    qry.prepare("CREATE TABLE IF NOT EXISTS `customer` ("
                "`date` VARCHAR(50) NOT NULL,"
                "`cardType` VARCHAR(10) NOT NULL,"
                "`transactionType` VARCHAR(10) NOT NULL,"
                "`amount` VARCHAR(10),"
                "`pan` VARCHAR(20),"
                "`ref` VARCHAR(50),"
                "`mop` VARCHAR(10),"
                "`approvalNumber` VARCHAR(10),"
                "`SPDH` VARCHAR(10));");
    if(qry.exec())
    {
        qry.lastError();
    }

}

database_logger::~database_logger()
{
    //db.close();
    //qDebug() << "database is closed";
}

void database_logger::event_log(QString id, QDateTime curTime, int eventCode, QString text, int volume)
{
    QSqlQuery qry(db);
//    qry.prepare("CREATE TABLE IF NOT EXISTS `event` ("
//                "`machine_id` VARCHAR(20) NOT NULL,"
//                "`time` DATETIME NOT NULL,"
//                "`event_code` INT NOT NULL,"
//                "`text` VARCHAR(255),"
//                "`inventory` INT)");

//    if (!qry.exec()){
//        qDebug() << qry.lastError().text();
//    }
//    else {
//        qDebug() << "event --> Table Exist";
//    }

    qry.prepare("INSERT INTO `event` ("
                "`machine_id`,"
                "`time`,"
                "`event_code`,"
                "`text`,"
                "`inventory`)"
                "VALUES (:machine_id, :time, :event_code, :text, :inventory)");

    //  or can be done like this
    //  "VALUES (?, ?, ?, ?)"); //addbindValue method

    qry.bindValue(":machine_id", id); //bindValue method
    qry.bindValue(":time", curTime);
    qry.bindValue(":event_code", eventCode);
    qry.bindValue(":text", text);
    qry.bindValue(":inventory", volume);

    qry.lastError();
    if(qry.exec())
    {
        qry.lastError();
    }

    //qDebug() << curTime;
    //qDebug() << "event --> data has been recorded";

}

void database_logger::paymentLog(string date, string CT, string TT, string am, string pan, string ref, string mop, string AN, string spdh)
{
    QSqlQuery qry(db);

//    qry.prepare("CREATE TABLE IF NOT EXISTS customer ("
//                "date VARCHAR(10) NOT NULL,"
//                "cardType VARCHAR(10) NOT NULL,"
//                "transactionType VARCHAR(10) NOT NULL,"
//                "amount VARCHAR(10),"
//                "pan VARCHAR(20),"
//                "ref VARCHAR(50),"
//                "mop VARCHAR(10),"
//                "approvalNumber VARCHAR(10),"
//                "SPDH VARCHAR(10))");
//    if (!qry.exec()){
//        qDebug() << qry.lastError().text();
//    }
//    else {
//        qDebug() << "customer --> Table Exist";
//    }

    qry.prepare("INSERT INTO `customer` ("
                "`date`,"
                "`cardType`,"
                "`transactionType`,"
                "`amount`,"
                "`pan`,"
                "`ref`,"
                "`mop`,"
                "`approvalNumber`,"
                "`SPDH`)"
                "VALUES (:date, :cardType, :transactionType, :amount, :pan,"
                ":ref, :mop, :approvalNumber, :SPDH)");

    //  or can be done like this
    //  "VALUES (?, ?, ?, ?)"); //addbindValue method

    char buffer[50];
    qry.bindValue(":date", strcpy(buffer,date.c_str())); //bindValue method
    qry.bindValue(":cardType", strcpy(buffer,CT.c_str()));
    qry.bindValue(":transactionType", strcpy(buffer,TT.c_str()));
    qry.bindValue(":amount", strcpy(buffer,am.c_str()));
    qry.bindValue(":pan", strcpy(buffer,pan.c_str()));
    qry.bindValue(":ref", strcpy(buffer,ref.c_str()));
    qry.bindValue(":mop", strcpy(buffer,mop.c_str()));
    qry.bindValue(":approvalNumber", strcpy(buffer,AN.c_str()));
    qry.bindValue(":SPDH", strcpy(buffer, spdh.c_str()));

    qry.exec();
    //qDebug() << "customer --> data has been recorded";
}

void database_logger::surveyLog(QDateTime curTime, QString drinkAnswer, QString emaillAddress, QString fillOrNot, QString bottleAnswer)
{
    QSqlQuery qry(db);
//    qry.prepare("CREATE TABLE IF NOT EXISTS survey ("
//                "time DATETIME NOT NULL,"
//                "drinkAnswer VARCHAR(50),"
//                "emailAddress VARCHAR(50),"
//                "action VARCHAR(20))");

//    if (!qry.exec()){
//        qDebug() << qry.lastError().text();
//    }
//    else {
//        qDebug() << "customer --> Table Exist";
//    }

    qry.prepare("INSERT INTO `survey` ("
                "`time`,"
                "`drinkAnswer`,"
                "`emailAddress`,"
                "`bottleAnswer`,"
                "`action`)"
                "VALUES (:time, :drinkAnswer, :emailAddress, :action, :bottleAnswer)");

    //  or can be done like this
    //  "VALUES (?, ?, ?, ?)"); //addbindValue method

    qry.bindValue(":time", curTime); //bindValue method
    qry.bindValue(":drinkAnswer", drinkAnswer);
    qry.bindValue(":emailAddress", emaillAddress);
    qry.bindValue(":action", fillOrNot);
    qry.bindValue(":bottleAnswer", bottleAnswer);

    qry.exec();
    //qDebug() << "customer --> data has been recorded";
}

bool database_logger::checkDataBaseConnection()
{
    if (!db.open())
    {
        qDebug() << "Database Disconnected";
        return false;
    }

    return true;
}

void database_logger::reconnectDatabase()
{
    //db.setHostName("drinkfilldb.cqraowv1leln.us-west-2.rds.amazonaws.com");
    db.setPort(3306);
    //db.setUserName("admin");
    //db.setPassword("Drinkfill");
    //db.setDatabaseName(databaseName);
    //db.setDatabaseName("/home/drinkfill/dev/evt/drinkfill_ui/database/datacollection.db");
    db.setDatabaseName("/home/soapstand/Desktop/mini_800x1280/datacollection.db");

    if (db.open()){
        qDebug() << "connection to db restored"; //+ db.hostName();
    }

    else {
        qDebug() << "connection failed: " << db.lastError();
    }

}

int database_logger::getInventory(int eventCode, int inventoryValue, QString machID)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM `event` WHERE `event_code` = " + QString::number(eventCode) + " AND `machine_id` = " + "'" + machID + "'");

    int currentInventory;

    if(model->rowCount() == 0)
    {
        currentInventory = inventoryValue;
    }
    else
    {
        currentInventory = (model->record(model->rowCount()-1).value("inventory")).toInt();
        //currentInventory = inventoryValue;
    }

    //qDebug() << "currentInventory is: " << currentInventory;
    return currentInventory;
}

bool database_logger::checkEmailDuplication(QString emailInput)
{
    QSqlQueryModel* model = new QSqlQueryModel();

    model->setQuery("SELECT * FROM `survey` WHERE `emailAddress` = '" + emailInput + "'");

//    qDebug() << model->lastError();
//    qDebug() << model->rowCount();

    if(model->rowCount() == 0)
    {
        qDebug() << "Email address not exist in database";
        return true; //no email address currently in the database
    }
    else {
        return false;
    }
}

int database_logger::getNumProductLog()
{
    QSqlQuery* qry= new QSqlQuery();
    qry->prepare("SELECT COUNT(*) FROM `products`");
    qry->exec();
    qry->next();
    //qDebug() << "Number of rows in products: " << qry->value(0).toInt();
    return qry->value(0).toInt();
}

QByteArray database_logger::getImgArray(int eventCode, QString machID)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM `products` WHERE `option` = " + QString::number(eventCode)+ " AND `machine_id` = " + "'" + machID + "'");

    //qDebug() << model->rowCount();
    QByteArray imgData;

    if(model->rowCount() == 0)
    {
        imgData = NULL;
    }
    else
    {
        imgData = (model->record(model->rowCount()-1).value(5)).toByteArray();
        //qDebug() << imgData;
        //currentInventory = inventoryValue;
    }
    return imgData;
}

QString database_logger::getMachineID()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM `products`");

    QString machineID;

    if(model->rowCount() == 0)
    {
        machineID = "Not Set";
    }
    else
    {
        machineID = (model->record(1).value("machine_id")).toString();
    }

    return machineID;
}

QSqlDatabase database_logger::getDB()
{
    return db;
}

QString database_logger::getProductPrice(int eventCode, int size, QString machID)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM `products` WHERE `option` = " + QString::number(eventCode) +
                    " AND `machine_id` = " + "'" + machID + "'");
    //qDebug() << model->rowCount();
    QString priceData;

    if(model->rowCount() == 0)
    {
        priceData = "Not Set";
    }
    else if (size == 1)
    {
        priceData = (model->record(model->rowCount()-1).value(2)).toByteArray(); //price 1
    }
    else if (size == 2)
    {
        priceData = (model->record(model->rowCount()-1).value(3)).toByteArray(); //price 2
    }
    else
    {
        priceData = "0";
    }
    return priceData;
}

QString database_logger::getProductName(int eventCode, QString machID)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM `products` WHERE `option` = " + QString::number(eventCode) + " AND `machine_id` = " + "'" + machID + "'");

    QString productNameData;

    if(model->rowCount() == 0)
    {
        productNameData = "Product Not Set";
    }
    else
    {
        productNameData = (model->record(model->rowCount()-1).value(1)).toByteArray();
    }
    return productNameData;
}

//QString database_logger::getProductVolume(int eventCode)
//{
//    QSqlQueryModel* model = new QSqlQueryModel();
//    model->setQuery("SELECT * FROM `products` WHERE `option` = " + QString::number(eventCode));

//    QString productNameData;

//    if(model->rowCount() == 0)
//    {
//        productNameData = "Product Not Set";
//    }
//    else
//    {
//        productNameData = (model->record(model->rowCount()-1).value(6)).toByteArray();
//    }
//    return productNameData;
//}
