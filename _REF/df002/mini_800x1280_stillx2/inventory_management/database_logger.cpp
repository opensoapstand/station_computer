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
    //    db.setHostName("drinkfilldb.cqraowv1leln.us-west-2.rds.amazonaws.com");
         db.setPort(3306);
    //     db.setDatabaseName("/home/drinkfill/Desktop/mini_800x1280/datacollection.db");
    //    db.setUserName("admin");
    //    db.setPassword("Drinkfill");
    //    db.setDatabaseName(databaseName);
          db.setDatabaseName("/home/soapstand/Downloads/drinkfill/df002/mini_800x1280/datacollection.db");

    if (db.open()){
        qDebug() << "connected " << db.hostName();
    }

    else {
        qDebug() << "connection failed";
    }
    QSqlQuery qry(db);

    qry.prepare("CREATE TABLE IF NOT EXISTS `products` ("
                "`option` INT NOT NULL,"
                "`drinkname` VARCHAR(20),"
                "`price1` VARCHAR(10),"
                "`price2` VARCHAR(10),"
                "`filename` VARCHAR (50),"
                "`imagedata` BLOB,"
                "`machine_id` VARCHAR(20),"
                "`volume` VARCHAR(20))");

    if(!qry.exec())
        qDebug() << qry.lastError();
}

database_logger::~database_logger()
{
    //db.close();
    //qDebug() << "database is closed";
}

//===============================Status Check================================================//
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
    //    db.setHostName("drinkfilldb.cqraowv1leln.us-west-2.rds.amazonaws.com");
         db.setPort(3306);
    //     db.setDatabaseName("/home/drinkfill/Desktop/mini_800x1280/datacollection.db");
    //    db.setUserName("admin");
    //    db.setPassword("Drinkfill");
    //    db.setDatabaseName(databaseName);
          db.setDatabaseName("/home/soapstand/Downloads/drinkfill/df002/mini_800x1280/datacollection.db");

    if (db.open()){
        qDebug() << "connection restored " + db.hostName();
    }

    else {
        qDebug() << "connection restore failed";
    }

}

//===========================================================================================//

//==============================Data Logging=================================================//
void database_logger::eventLog(QString id, QDateTime curTime, int eventCode, QString text, int volume)
{    
    QSqlQuery qry(db);
//    qry.prepare("CREATE TABLE IF NOT EXISTS event ("
//                "machine_id VARCHAR(20) NOT NULL,"
//                "time DATETIME NOT NULL,"
//                "event_code INT NOT NULL,"
//                "text VARCHAR(255),"
//                "inventory INT)");

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

    qry.exec();

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
}

void database_logger::surveyLog(QDateTime curTime, QString drinkAnswer, QString emaillAddress, QString fillOrNot)
{
    QSqlQuery qry(db);
//    qry.prepare("CREATE TABLE IF NOT EXISTS survey ("
//                "time DATETIME NOT NULL,"
//                "drinkAnswer VARCHAR(50),"
//                "emailAddress VARCHAR(50),"
//                "action VARCHAR(20))");

    qry.prepare("INSERT INTO survey ("
                "time,"
                "drinkAnswer,"
                "emailAddress,"
                "action)"
                "VALUES (:time, :drinkAnswer, :emailAddress, :action)");

    //  or can be done like this
    //  "VALUES (?, ?, ?, ?)"); //addbindValue method

    qry.bindValue(":time", curTime); //bindValue method
    qry.bindValue(":drinkAnswer", drinkAnswer);
    qry.bindValue(":emailAddress", emaillAddress);
    qry.bindValue(":action", fillOrNot);

    qry.exec();
    //qDebug() << "customer --> data has been recorded";
}
//========================================================================================================//

void database_logger::productLog(int option, QString price, QString name, QString fileName, QByteArray productImg, bool price1Edit, bool nameEdit, QString machineid)
{
    QSqlQuery qry(db);
    if(price1Edit){
        if(nameEdit){
            qry.prepare("UPDATE `products` SET "
                        "`price1` = :price1,"
                        "`drinkname` = :drinkname,"
                        "`filename` = :filename,"
                        "`imagedata`  = :imagedata WHERE `option` =" + QString::number(option) + " AND `machine_id` = " + "'" + machineid + "'");
            qry.bindValue(":price1", price);
            qry.bindValue(":drinkname", name);
            qry.bindValue(":filename", fileName);
            qry.bindValue(":imagedata", productImg);
            //qry.bindValue(":option", 2); //bindValue method
            if (!qry.exec())
            {
                qDebug() << qry.lastError();
            }
        }
        else {
            qry.prepare("UPDATE `products` SET "
                        "`price1` = :price1,"
                        "`filename` = :filename,"
                        "`imagedata`  = :imagedata WHERE `option` =" + QString::number(option)+ " AND `machine_id` = " + "'" + machineid + "'");
            qry.bindValue(":price1", price);
            qry.bindValue(":filename", fileName);
            qry.bindValue(":imagedata", productImg);
            if (!qry.exec())
            {
                qDebug() << qry.lastError();
            }
        }
    }

    else {
        if(nameEdit){
            qry.prepare("UPDATE `products` SET "
                        "`filename` = :filename,"
                        "`drinkname` = :drinkname,"
                        "`imagedata`  = :imagedata WHERE `option` =" + QString::number(option)+ " AND `machine_id` = " + "'" + machineid + "'");
            qry.bindValue(":drinkname", name);
            qry.bindValue(":filename", fileName);
            qry.bindValue(":imagedata", productImg);
            if (!qry.exec())
            {
                qDebug() << qry.lastError();
            }
        }
        else{
            qry.prepare("UPDATE `products` SET "
                        "`filename` = :filename,"
                        "`imagedata`  = :imagedata WHERE `option` =" + QString::number(option)+ " AND `machine_id` = " + "'" + machineid + "'");
            qry.bindValue(":filename", fileName);
            qry.bindValue(":imagedata", productImg);
            if (!qry.exec())
            {
                qDebug() << qry.lastError();
            }
        }
    }
}

void database_logger::productLog(int option, QString amount, QString machineid)
{
    QSqlQuery qry(db);
    qry.prepare("UPDATE `products` SET "
                "`volume` = :volume,"
                "`imagedata`  = :imagedata WHERE `option` =" + QString::number(option)+ " AND `machine_id` = " + "'" + machineid + "'");
    qry.bindValue(":volume", amount);
    if (!qry.exec())
    {
        qDebug() << qry.lastError();
    }
}

void database_logger::price2Update(int option, QString price2, QString machineid)
{
    QSqlQuery qry(db);
    qry.prepare("UPDATE `products` SET "
                "`price2` = :price2 "
                "WHERE `option` =" + QString::number(option)+ " AND `machine_id` = " + "'" + machineid + "'");

    qry.bindValue(":price2", price2);

    if (!qry.exec())
    {
        qDebug() << qry.lastError();
    }
}

//==================================Set Up=======================================================//
void database_logger::productLogSetup(int numRow, int numDrinks, QString machineid)
{
    int idx = 1;
    while(idx <= numDrinks && numRow <= numDrinks)
    {
        QSqlQueryModel* model = new QSqlQueryModel();
        model->setQuery("SELECT * FROM `products` WHERE `machine_id` = '" + QString(machineid) + "'" +
                        " AND `option` = " + QString::number(idx));

        if(model->rowCount() == 0)
        {
            QSqlQuery qry(db);
            qry.prepare("INSERT INTO `products` ("
                        "`option`,"
                        "`drinkname`,"
                        "`price1`,"
                        "`price2`,"
                        "`filename`,"
                        "`imagedata`,"
                        "`machine_id`,"
                        "`volume`)"
                        "VALUES (:option, :drinkname, :price1, :price2, :filename, :imagedata, :machine_id, :volume)");
            qry.bindValue(":option", idx); //bindValue method
            qry.bindValue(":drinkname", "Not Set");
            qry.bindValue(":price1", "Not Set");
            qry.bindValue(":price2", "Not Set");
            qry.bindValue(":filename", "Not Set");
            QByteArray temp;
            qry.bindValue(":imagedata", temp);
            qry.bindValue(":machine_id", machineid);
            qry.bindValue(":volume", "Not Set");
            if (qry.exec()){
                qDebug() << "Inserting line "<< idx;
                numRow++;
            }
            else {
                qDebug() << qry.lastError();
            }
        }
        idx++;
    }
}

void database_logger::machineIdSetup(QString machineId, int numDrinks)
{
    int idx = 1;
    while(idx <= numDrinks)
    {        
        QSqlQuery qry(db);
        qry.prepare("UPDATE `products` SET "
                    "`machine_id`  = :machine_id "
                    "WHERE `option` =" + QString::number(idx));

        qry.bindValue(":machine_id", machineId);
        if (qry.exec()){
            qDebug() << "Update line "<< idx;
        }
        else {
            qDebug() << qry.lastError();
        }

        idx++;
    }
}
//================================================================================================//


//---------------------------------------GET---------------------------------------------------------//
int database_logger::getInventory(int eventCode, QString machineid)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM `event` WHERE `event_code` = '" + QString::number(eventCode) + "'"
                    + "AND `machine_id` = '" + machineid + "'");

    while(model->canFetchMore())
    {
        model->fetchMore();
    }

    int currentInventory;

    if(model->rowCount() == 0)
    {
        currentInventory = int(NULL);
    }
    else
    {
        currentInventory = (model->record(model->rowCount()-1).value("inventory")).toInt();
        //currentInventory = inventoryValue;
    }

    qDebug() << "currentInventory is: " << currentInventory;
    return currentInventory;
}

int database_logger::getNumProductLog(QString machineid)
{
    QSqlQuery* qry= new QSqlQuery();
    qry->prepare("SELECT COUNT(*) FROM products where `machine_id` = '" + machineid + "'");
    qry->exec();
    qry->next();
    //qDebug() << "Number of rows in products: " << qry->value(0).toInt();

    qDebug() << "Number of products: " << qry->value(0).toInt();
    return qry->value(0).toInt();
}

int database_logger::getNumMachine()
{
    QSqlQuery* qry= new QSqlQuery();
    qry->prepare("SELECT COUNT(DISTINCT `machine_id`) FROM `products`");
    qry->exec();
    qry->next();

    //qDebug() << "Number of rows in products: " << qry->value(0).toInt();
    qDebug() << "Number of Machine: " << qry->value(0).toInt();
    return qry->value(0).toInt();
}

QByteArray database_logger::getImgArray(int eventCode, QString machineid)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM products WHERE `machine_id` = " + machineid + "AND `option` = " + QString::number(eventCode+1));

    //qDebug() << model->rowCount();
    QByteArray imgData;

    if(model->rowCount() == 0)
    {
        imgData = NULL;
    }
    else
    {
        imgData = (model->record(model->rowCount()-1).value(4)).toByteArray();
        //qDebug() << imgData;
        //currentInventory = inventoryValue;
    }
    return imgData;
}

QString database_logger::getMachineID(QString machineid)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM `products` WHERE `machine_id` =" + machineid);

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

QString database_logger::getAllMachineID(int number)
{
    QString machineID;

    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT DISTINCT `machine_id` FROM `products`");

    machineID = model->record(number).value("machine_id").toString();
    //qDebug() << "Number of rows in products: " << qry->value(0).toInt();
    return machineID;
}

QSqlDatabase database_logger::getDB()
{
    return db;
}

//-----------------------------------------------------------------------------------------------------//

void database_logger::resetProductLog(QString machineid)
{
//    QSqlQuery* qry= new QSqlQuery(db);

//    int products = getNumProductLog(machineid);
//    for(int i = 1; i >= products; i++)
//    {
//       //qry->prepare("DELETE FROM products WHERE option = " + QString::number(i));
//       qry->prepare("DELETE FROM `products`");
//       if(!qry->exec())
//           qDebug() << qry->lastError();
//    }
}




