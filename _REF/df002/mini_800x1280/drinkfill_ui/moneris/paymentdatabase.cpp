//***************************************
//
// paymentdatabase.cpp
// database connection and data collection
//
// created: 15-OCT-2019
// by: Jason Wang
//
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************
//#include "paymentdatabase.h"

//paymentDatabase::paymentDatabase()
//{
//    db.setHostName("localhost");
//    db.setUserName("drinkfill");
//    db.setPassword("Drinkfill");
//    db.setDatabaseName("/home/drinkfill/dev/evt/drinkfill_ui/database/payment.db");

//    if (db.open()){
//        qDebug() << "connected " << db.hostName();
//    }

//    else {
//        qDebug() << "connection failed";
//    }

//    QSqlQuery qry;
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
//        qDebug() << "Table Exist";
//    }
//}

//paymentDatabase::~paymentDatabase()
//{
//    db.close();
//    qDebug() << "database is closed";
//}

//void paymentDatabase::paymentLog(string date, string CT, string TT,
//                                string am, string pan, string ref,
//                                string mop, string AN, string spdh)
//{
//    QSqlQuery qry;

//    qry.prepare("INSERT INTO customer ("
//                "date,"
//                "cardType,"
//                "transactionType,"
//                "amount,"
//                "pan,"
//                "ref,"
//                "mop,"
//                "approvalNumber,"
//                "SPDH)"
//                "VALUES (:date, :cardType, :transactionType, :amount, :pan,"
//                ":ref, :mop, :approvalNumber, :SPDH)");

//    //  or can be done like this
//    //  "VALUES (?, ?, ?, ?)"); //addbindValue method

//    char buffer[50];
//    qry.bindValue(":date", strcpy(buffer,date.c_str())); //bindValue method
//    qry.bindValue(":cardType", strcpy(buffer,CT.c_str()));
//    qry.bindValue(":transactionType", strcpy(buffer,TT.c_str()));
//    qry.bindValue(":amount", strcpy(buffer,am.c_str()));
//    qry.bindValue(":pan", strcpy(buffer,pan.c_str()));
//    qry.bindValue(":ref", strcpy(buffer,ref.c_str()));
//    qry.bindValue(":mop", strcpy(buffer,mop.c_str()));
//    qry.bindValue(":approvalNumber", strcpy(buffer,AN.c_str()));
//    qry.bindValue(":SPDH", strcpy(buffer, spdh.c_str()));

//    qry.exec();
//}

//bool paymentDatabase::checkDataBaseConnection()
//{
//    if (!db.open())
//    {
//        qDebug() << "Database Disconnected";
//        return false;
//    }

//    return true;
//}

//void paymentDatabase::reconnectDatabase()
//{
//    db.setHostName("localhost");
//    db.setUserName("drinkfill");
//    db.setPassword("Drinkfill");
//    db.setDatabaseName("/home/drinkfill/dev/evt/drinkfill_ui/database/payment.db");

//    if (db.open()){
//        qDebug() << "connection restored " + db.hostName();
//    }

//    else {
//        qDebug() << "connection restore failed";
//    }

//}
