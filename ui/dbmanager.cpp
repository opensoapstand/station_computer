
//***************************************
//
// dbmanager.cpp
// Database Manager class to get and set
// database values.
//
// created: 2021
// by: Padraig Riley
//
// copyright 2021 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "dbmanager.h"
#include "df_util.h" // lode added for settings

#include <QVector>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

DbManager::DbManager(const QString &path)
{
    // m_dbPath2 = CONFIG_DB_PATH;
    // setPath(path);
}
DbManager::DbManager()
{
}

// DTOR
DbManager::~DbManager()
{
}

void DbManager::closeDb()
{
    // qDebug() << "Close db";
    // no problem, even if called hundred times after one other and the db was not open.
    QSqlDatabase::database("qt_sql_ui_connection").close();
    if (QSqlDatabase::contains("qt_sql_ui_connection"))
    {
        QSqlDatabase::removeDatabase("qt_sql_ui_connection");
    }

    // // this works, but outputs a warning line all the time (even if I close the queries)
    // // the warning:  connection 'qt_sql_ui_connection' is still in use, all queries will cease to work. (at the removeDatabase line)
    // if (db.isOpen())
    // {
    //     // QSqlQuery::clear();
    //     // db.close();
    //     // db = QSqlDatabase();
    //     // QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    //     // QSqlDatabase::removeDatabase("qt_sql_ui_connection");
    //     QSqlDatabase::database("qt_sql_ui_connection").close();
    //     // QSqlDatabase::removeDatabase("qt_sql_ui_connection");
    // }
    // else
    // {
    //     qDebug() << "Db was not open.";
    // }
    // if (QSqlDatabase::contains("qt_sql_ui_connection"))
    // {
    //     QSqlDatabase::removeDatabase("qt_sql_ui_connection");
    // }
    // // qDebug() << "Db close done.";

    // QString connectionName = "qt_sql_ui_connection";
    // if (QSqlDatabase::contains(connectionName))
    // {
    //     QSqlDatabase database = QSqlDatabase::database(connectionName);
    //     if (database.isOpen())
    //     {
    //         QSqlQuery query(database);
    //         while (query.next())
    //         {
    //             // Iterate through the result set to ensure all queries are processed
    //         }

    //         // Commit or rollback transactions
    //         if (database.transaction())
    //         {
    //             database.commit();
    //         }
    //         else
    //         {
    //             database.rollback();
    //         }

    //         database.close();
    //         // Optionally, remove the database connection
    //         QSqlDatabase::removeDatabase(connectionName);
    //         qDebug() << "Database connection closed.";
    //     }
    //     else
    //     {
    //         qDebug() << "Database connection is already closed.";
    //     }
    // }
    // else
    // {
    //     qDebug() << "Database connection does not exist.";
    // }
}

QSqlDatabase DbManager::openDb(QString dbname)
{
    // qDebug() << "Open db";
    QSqlDatabase m_db = QSqlDatabase::addDatabase("QSQLITE", "qt_sql_ui_connection");
    QString p = dbname;
    if (m_db.isOpen())
    {
        usleep(100000);
        qDebug() << "m_db is already open. Try to close.";
        m_db.close();
        m_db = QSqlDatabase();
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }
    try
    {
        // qDebug() << m_dbPath2; // CRASHES HERE
        // qDebug() << "db init2 path: " << p;
        m_db.setDatabaseName(p);
    }
    catch (const QSqlError &error)
    {
        qDebug() << "Database error: " << error.text();
    }

    int attempts = 10;
    // QSqlDatabase tmp  = QSqlDatabase::addDatabase("QSQLITE");
    // if (tmp.isOpen())
    // {
    //     usleep(100000);
    //     qDebug() << "m_db is already open. Try to close.";
    //     tmp.close();
    //     tmp = QSqlDatabase();
    //     QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    //     qDebug() << "m_db was already open. Closed it first.";
    // }

    if (m_db.connectionName().isEmpty())
    {
        qDebug() << "Database name was not empty.";
        // m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName(CONFIG_DB_PATH);
    }

    if (!m_db.open())
    {
        qDebug() << "Error: connection with database failed";
    }
    else
    {
        // qDebug() << "Database: connection ok";
    }

    bool isLocked = true;
    while (isLocked)
    {
        QSqlQuery q(m_db);
        if (q.exec("BEGIN EXCLUSIVE")) // tries to acquire the lock
        {
            q.exec("COMMIT"); // releases the lock immediately
            isLocked = false; // db is not locked

            if (isLocked)
            {

                qDebug() << "PROGRAM HALTED: Database is locked. Wait until unlocked. It's probably opened, close the db and the program will continue.";
            }
        }
        else
        {
            usleep(250000);
        }
    }
    return m_db;
}

bool DbManager::executeQuery(QString sql)
{
    bool success = false;

    {
        QSqlDatabase db = openDb(CONFIG_DB_PATH);
        QSqlQuery qry(db);
        qry.prepare(sql);

        success = qry.exec();
        if (!success)
        {
            qDebug() << "Did not execute sql. "
                     << qry.lastError() << " | " << qry.lastQuery();
            success = true;
        }
        else
        {
            qDebug() << qry.lastQuery();
        }
        qry.finish();
    }
    closeDb();
    return success;
}

bool DbManager::updateTableMachineWithInt(QString column, int value)
{
    updateTableMachineWithText(column, QString::number(value));
}

bool DbManager::updateTableMachineWithDouble(QString column, double value, int precision)
{

    updateTableMachineWithText(column, QString::number(value, 'f', precision));
}

bool DbManager::updateTableMachineWithText(QString column, QString value)
{
    QString sql_text = QString("UPDATE machine SET %1='%2'").arg(column, value);
    return executeQuery(sql_text);
}

bool DbManager::updateTableProductsWithInt(int pnumber, QString column, int value)
{

    return updateTableProductsWithText(pnumber, column, QString::number(value));
}

bool DbManager::updateTableProductsWithDouble(int pnumber, QString column, double value, int precision)
{

    return updateTableProductsWithText(pnumber, column, QString::number(value, 'f', precision));
}

bool DbManager::updateTableProductsWithText(int pnumber, QString column, QString value)
{
    QString sql_text = QString("UPDATE products SET %1='%2' WHERE pnumber=%3").arg(column, value, QString::number(pnumber));
    return executeQuery(sql_text);
}

bool DbManager::addPageClick(const QString &page)
{

    return true;
}

void DbManager::getAllSlotProperties(int slot,
                                     int *basePNumber,
                                     QVector<int> *additivesPNumbers,
                                     bool *is_enabled,
                                     QString *status_text)

{

    // WARNING: consider using having arguments sent to function "by reference" instead of pointers. At least, chatgpt suggests that it's safer.

    qDebug() << "Open db: load all slot properties for slot: " << slot << "From: " << CONFIG_DB_PATH;
    {
        QSqlDatabase db = openDb(CONFIG_DB_PATH);
        QSqlQuery qry(db);
        qry.prepare("SELECT slot_id, base_P-number, additives_P-numbers, is_enabled, status_text FROM slots WHERE slot_id=:slot");
        qry.bindValue(":slot", slot);
        bool success;
        success = qry.exec();

        if (!success)
        {
            qDebug() << "Open db: Attempted to load all slot properties for slot: " << slot;
            qDebug() << "Did not execute sql. "
                     << qry.lastError() << " | " << qry.lastQuery();
            // success = false;
        }

        QString additivesAsString;
        while (qry.next())
        {
            *basePNumber = qry.value(0).toInt();
            additivesAsString = qry.value(1).toString();
            *is_enabled = qry.value(2).toInt();
            *status_text = qry.value(3).toString();
        }
        qry.finish();

        QStringList stringList = additivesAsString.split(",");
        foreach (QString num, stringList)
        {
            additivesPNumbers->append(num.toInt());
        }
    }
    closeDb();
}

/*
productId
soapstand_product_serial
slot
name
size_unit
currency
payment
name_receipt
concentrate_multiplier
dispense_speed
threshold_flow
retraction_time
calibration_const
volume_per_tick
last_restock
volume_full
volume_remaining
volume_dispensed_since_restock
volume_dispensed_total
is_enabled_small
is_enabled_medium
is_enabled_large
is_enabled_custom
size_small
size_medium
size_large
size_custom_min
size_custom_max
price_small
price_medium
price_large
price_custom
plu_small
plu_medium
plu_large
plu_custom
pid_small
pid_medium
pid_large
pid_custom
flavour
image_url
type
ingredients
features
description
is_enabled_custom_discount
size_custom_discount
price_custom_discount

0	productId
1	soapstand_product_serial
2	slot
3	name
4	size_unit
5	currency
6	payment
7	name_receipt
8	concentrate_multiplier
9	dispense_speed
10	threshold_flow
11	retraction_time
12	calibration_const
13	volume_per_tick
14	last_restock
15	volume_full
16	volume_remaining
17	volume_dispensed_since_restock
18	volume_dispensed_total
19	is_enabled_small
20	is_enabled_medium
21	is_enabled_large
22	is_enabled_custom
23	size_small
24	size_medium
25	size_large
26	size_custom_min
27	size_custom_max
28	price_small
29	price_medium
30	price_large
31	price_custom
32	plu_small
33	plu_medium
34	plu_large
35	plu_custom
36	pid_small
37	pid_medium
38	pid_large
39	pid_custom
40	flavour
41	image_url
42	type
43	ingredients
44	features
45	description
46	is_enabled_custom_discount
47	size_custom_discount
48	price_custom_discount

 */
// void DbManager::getAllProductProperties(int slot, QString* soapstand_product_serial, QString* size_unit, QString* payment, int* concentrate_multiplier, int* dispense_speed, bool *isSizeEnabled, double* prices, double* volumes, QString* PLUs, QString* PIDs)
//   string table_products_columns[TABLE_PRODUCTS_COLUMN_COUNT] = {"productId", "soapstand_product_serial", "slot", "name", "size_unit", "currency", "payment", "name_receipt", "concentrate_multiplier", "dispense_speed", "threshold_flow", "retraction_time", "calibration_const", "volume_per_tick", "last_restock", "volume_full", "volume_remaining", "volume_dispensed_since_restock", "volume_dispensed_total", "is_enabled_small", "is_enabled_medium", "is_enabled_large", "is_enabled_custom", "size_small", "size_medium", "size_large", "size_custom_min", "size_custom_max", "price_small", "price_medium", "price_large", "price_custom", "plu_small", "plu_medium", "plu_large", "plu_custom", "pid_small", "pid_medium", "pid_large", "pid_custom", "flavour", "image_url", "type", "ingredients", "features", "description", "is_enabled_custom_discount", "size_custom_discount", "price_custom_discount"};
//  (productId, soapstand_product_serial, slot, name, size_unit, currency, payment, name_receipt, concentrate_multiplier, dispense_speed, threshold_flow, retraction_time, calibration_const, volume_per_tick, last_restock, volume_full, volume_remaining, volume_dispensed_since_restock, volume_dispensed_total, is_enabled_small, is_enabled_medium, is_enabled_large, is_enabled_custom, size_small, size_medium, size_large, size_custom_min, size_custom_max, price_small, price_medium, price_large, price_custom, plu_small, plu_medium, plu_large, plu_custom, pid_small, pid_medium, pid_large, pid_custom, flavour, image_url, type, ingredients, features, description, is_enabled_custom_discount, size_custom_discount, price_custom_discount)
void DbManager::getAllProductProperties(int pnumber,
                                        QString *productId,
                                        QString *soapstand_product_serial,
                                        QVector<int> *m_additivesPNumbers,
                                        QVector<double> *m_additivesRatios,
                                        QString *size_unit,
                                        QString *m_currency_deprecated, //_dummy_deprecated
                                        QString *m_payment_deprecated,  //_deprecated,
                                        QString *name_receipt,
                                        int *concentrate_multiplier,
                                        int *dispense_speed,
                                        double *threshold_flow,
                                        int *retraction_time,
                                        double *calibration_const,
                                        double *volume_per_tick,
                                        QString *last_restock,
                                        double *volume_full,
                                        double *volume_remaining,
                                        double *volume_dispensed_since_restock,
                                        double *volume_dispensed_total,
                                        int *is_enabled_custom_discount,
                                        double *size_custom_discount,
                                        double *price_custom_discount,
                                        bool *isSizeEnabled, double *prices, double *volumes, QString *PLUs, QString *PIDs)
    // void getAllProductProperties(int slot,
    //                              QString *productId,
    //                              QString *soapstand_product_serial,
    //                              QVector<int> *m_additivesPNumbers,
    //                              QVector<double> *m_additivesRatios,
    //                              QString *size_unit,
    //                              QString *name_receipt,
    //                              QString *m_currency_deprecated, //_dummy_deprecated
    //                              QString *m_payment_deprecated,  //_deprecated,
    //                              int *concentrate_multiplier,
    //                              int *dispense_speed,
    //                              double *threshold_flow,
    //                              int *retraction_time,
    //                              double *calibration_const,
    //                              double *volume_per_tick,
    //                              QString *last_restock,
    //                              double *volume_full,
    //                              double *volume_remaining,
    //                              double *volume_dispensed_since_restock,
    //                              double *volume_dispensed_total,
    //                              int *is_enabled_custom_discount,
    //                              double *size_custom_discount,
    //                              double *price_custom_discount,
    //                              bool *isSizeEnabled, double *prices, double *volumes, QString *PLUs, QString *PIDs);

{
    // qDebug() << "Open db";
    qDebug() << "Open db: load all product properties for pnumber: " << pnumber << "From: " << CONFIG_DB_PATH;
    {
        QSqlDatabase db = openDb(CONFIG_DB_PATH);
        QSqlQuery qry(db);
        // qry.prepare("SELECT soapstand_product_serial, size_unit, payment, is_enabled_small, is_enabled_medium, is_enabled_large, is_enabled_custom, size_small, size_medium, size_large, size_custom_max,price_small,price_medium, price_large,price_custom FROM products WHERE pnumber=:pnumber");
        qry.prepare("SELECT productId, soapstand_product_serial, additives_pnumbers, additives_ratios, slot, name, size_unit, name_receipt, concentrate_multiplier, dispense_speed, threshold_flow, retraction_time, calibration_const, volume_per_tick, last_restock, volume_full, volume_remaining, volume_dispensed_since_restock, volume_dispensed_total, is_enabled_small, is_enabled_medium, is_enabled_large, is_enabled_custom, size_small, size_medium, size_large, size_custom_min, size_custom_max, price_small, price_medium, price_large, price_custom, plu_small, plu_medium, plu_large, plu_custom, pid_small, pid_medium, pid_large, pid_custom, flavour, image_url, type, ingredients, features, description, is_enabled_custom_discount, size_custom_discount, price_custom_discount FROM products WHERE soapstand_product_serial=:pnumber");
        qry.bindValue(":pnumber", pnumber);
        bool success;
        success = qry.exec();

        if (!success)
        {
            qDebug() << "Open db: Attempted to load all product properties for pnumber: " << pnumber;
            qDebug() << "Did not execute sql. "
                     << qry.lastError() << " | " << qry.lastQuery();
            // success = false;
        }

        QString additives_pnumbers;
        QString additives_ratios;

        while (qry.next())
        {
            *productId = qry.value(0).toString();
            *soapstand_product_serial = qry.value(1).toString();
            additives_pnumbers = qry.value(2).toString();
            additives_ratios = qry.value(3).toString();
            *size_unit = qry.value(6).toString();
            *name_receipt = qry.value(7).toString();
            *concentrate_multiplier = qry.value(8).toInt();
            *dispense_speed = qry.value(9).toInt();
            *threshold_flow = qry.value(10).toDouble();
            *retraction_time = qry.value(11).toInt();
            *calibration_const = qry.value(12).toDouble();
            *volume_per_tick = qry.value(13).toDouble();
            *last_restock = qry.value(14).toString();
            *volume_full = qry.value(15).toDouble();
            *volume_remaining = qry.value(16).toDouble();
            *volume_dispensed_since_restock = qry.value(17).toDouble();
            *volume_dispensed_total = qry.value(18).toDouble();

            isSizeEnabled[1] = qry.value(19).toInt(); // small is index 1!!
            isSizeEnabled[2] = qry.value(20).toInt();
            isSizeEnabled[3] = qry.value(21).toInt();
            isSizeEnabled[4] = qry.value(22).toInt();

            volumes[1] = qry.value(23).toDouble();
            volumes[2] = qry.value(24).toDouble();
            volumes[3] = qry.value(25).toDouble();
            // size custom min
            volumes[4] = qry.value(27).toDouble(); // size custom max.

            prices[1] = qry.value(28).toDouble();
            prices[2] = qry.value(29).toDouble();
            prices[3] = qry.value(30).toDouble();
            prices[4] = qry.value(31).toDouble();

            PLUs[SIZE_SMALL_INDEX] = qry.value(32).toString();
            PLUs[SIZE_MEDIUM_INDEX] = qry.value(33).toString();
            PLUs[SIZE_LARGE_INDEX] = qry.value(34).toString();
            PLUs[SIZE_CUSTOM_INDEX] = qry.value(35).toString();

            PIDs[1] = qry.value(36).toString();
            PIDs[2] = qry.value(37).toString();
            PIDs[3] = qry.value(38).toString();
            PIDs[4] = qry.value(39).toString();

            *is_enabled_custom_discount = qry.value(46).toInt();
            *size_custom_discount = qry.value(47).toDouble();
            *price_custom_discount = qry.value(48).toDouble();
        }
        qry.finish();
    }
    closeDb();

    for (int i = 0; i < 4; i++)
    {

        qDebug() << prices[i];
    }
}

/*
0	machine_id
1	soapstand_customer_id
2	template
3	location
4	controller_type
5	controller_id
6	screen_type
7	screen_id
8	has_receipt_printer
9	receipt_printer_is_online
10	receipt_printer_has_paper
11	has_tap_payment
12	hardware_version
13	software_version
14	aws_port
15	pump_id_slot_1
16	pump_id_slot_2
17	pump_id_slot_3
18	pump_id_slot_4
19	is_enabled_slot_1
20	is_enabled_slot_2
21	is_enabled_slot_3
22	is_enabled_slot_4
23	coupons_enabled
24	status_text_slot_1
25	status_text_slot_2
26	status_text_slot_3
27	status_text_slot_4
28	has_empty_detection
29	enable_pump_ramping
30	enable_pump_reversal
31	dispense_buttons_count
32	maintenance_pwd
33	show_transactions
34	help_text_html
35	idle_page_type
36	admin_pwd

QString* machine_id,
int* soapstand_customer_id,
QString* template,
QString* location,
QString* controller_type,
QString* controller_id,
QString* screen_type,
QString* screen_id,
int* has_receipt_printer,
int* receipt_printer_is_online,
int* receipt_printer_has_paper,
int* has_tap_payment,
QString* hardware_version,
QString* software_version,
int* aws_port,

int* coupons_enabled,
int* has_empty_detection,
int* enable_pump_ramping,
int* enable_pump_reversal,
int* dispense_buttons_count,
QString* maintenance_pwd,
int* show_transactions,
QString* help_text_html,
QString* idle_page_type,

QString* pump_id_slots*,
int* is_enabled_slots*,
QString* status_text_slots

*/

void DbManager::getAllMachineProperties(
    QString *machine_id,
    QString *soapstand_customer_id,
    QString *ttttemplate,
    QString *location,
    QString *controller_type,
    QString *controller_id,
    QString *screen_type,
    QString *screen_id,
    int *has_receipt_printer,
    int *receipt_printer_is_online,
    int *receipt_printer_has_paper,
    int *has_tap_payment,
    QString *hardware_version,
    QString *software_version,
    int *aws_port,

    int *coupons_enabled,
    int *has_empty_detection,
    int *enable_pump_ramping,
    int *enable_pump_reversal,
    int *dispense_buttons_count,
    QString *maintenance_pwd,
    int *show_transactions,
    QString *help_text_html,
    QString *idle_page_type,
    QString *admin_pwd,

    QString *pump_id_slots,
    int *is_enabled_slots,
    QString *status_text_slots,
    double *alert_temperature)
{
    qDebug() << " db... all machine properties from: " << CONFIG_DB_PATH;
    {
        QSqlDatabase db = openDb(CONFIG_DB_PATH);
        QSqlQuery qry(db);

        qry.prepare("SELECT machine_id,soapstand_customer_id,template,location,controller_type,controller_id,screen_type,'screen_id',has_receipt_printer,receipt_printer_is_online,receipt_printer_has_paper,has_tap_payment,hardware_version,software_version,aws_port,pump_id_slot_1,pump_id_slot_2,pump_id_slot_3,pump_id_slot_4,is_enabled_slot_1,is_enabled_slot_2,is_enabled_slot_3,is_enabled_slot_4,coupons_enabled,status_text_slot_1,status_text_slot_2,status_text_slot_3,status_text_slot_4,has_empty_detection,enable_pump_ramping,enable_pump_reversal,dispense_buttons_count,maintenance_pwd,show_transactions,help_text_html,idle_page_type,admin_pwd,alert_temperature FROM machine");
        bool success;
        success = qry.exec();
        if (!success)
        {
            qDebug() << "Did not execute sql. "
                     << qry.lastError() << " | " << qry.lastQuery();
            // success = false;
        }

        while (qry.next())
        {
            *machine_id = qry.value(0).toString();
            *soapstand_customer_id = qry.value(1).toString();
            *ttttemplate = qry.value(2).toString();
            *location = qry.value(3).toString();
            *controller_type = qry.value(4).toString();
            *controller_id = qry.value(5).toString();
            *screen_type = qry.value(6).toString();
            *screen_id = qry.value(7).toString();
            *has_receipt_printer = qry.value(8).toInt();
            *receipt_printer_is_online = qry.value(9).toInt();
            *receipt_printer_has_paper = qry.value(10).toInt();
            *has_tap_payment = qry.value(11).toInt();
            *hardware_version = qry.value(12).toString();
            *software_version = qry.value(13).toString();
            *aws_port = qry.value(14).toInt();
            pump_id_slots[0] = qry.value(15).toString();
            pump_id_slots[1] = qry.value(16).toString();
            pump_id_slots[2] = qry.value(17).toString();
            pump_id_slots[3] = qry.value(18).toString();
            is_enabled_slots[0] = qry.value(19).toInt();
            is_enabled_slots[1] = qry.value(20).toInt();
            is_enabled_slots[2] = qry.value(21).toInt();
            is_enabled_slots[3] = qry.value(22).toInt();
            *coupons_enabled = qry.value(23).toInt();
            status_text_slots[0] = qry.value(24).toString();
            status_text_slots[1] = qry.value(25).toString();
            status_text_slots[2] = qry.value(26).toString();
            status_text_slots[3] = qry.value(27).toString();
            *has_empty_detection = qry.value(28).toInt();
            *enable_pump_ramping = qry.value(29).toInt();
            *enable_pump_reversal = qry.value(30).toInt();
            *dispense_buttons_count = qry.value(31).toInt();
            *maintenance_pwd = qry.value(32).toString();
            *show_transactions = qry.value(33).toInt();
            *help_text_html = qry.value(34).toString();
            *idle_page_type = qry.value(35).toString();
            *admin_pwd = qry.value(36).toString();
            *alert_temperature = qry.value(37).toDouble();
        }
        qry.finish();
    }
    closeDb();
}

// QString DbManager::getPaymentMethod(int slot)
// {
//     // used by Ash in tap. to do --> get tap init out of constructor.
//     qDebug() << "********* DEPRECATED *********** ";
//     qDebug() << "DB call: get payment method for slot";
//     QString payment_method;
//     {
//         QSqlDatabase db = openDb();
//         QSqlQuery qry(db);

//         qry.prepare("SELECT payment FROM products WHERE slot=:slot");
//         qry.bindValue(":slot", slot);
//         bool success;
//         success = qry.exec();

//         while (qry.next())
//         {
//             payment_method = qry.value(0).toString();
//         }
//         qry.finish();
//     }
//     closeDb();
//     return payment_method;
// }

uint32_t DbManager::getNumberOfRows(QString table)
{
    qDebug() << " db... getNumberOfRows";
    QString qry_text = QString("SELECT COUNT(*) FROM %1").arg(table);

    uint32_t row_count = 0;
    {
        QSqlDatabase db = openDb(CONFIG_DB_PATH);
        QSqlQuery qry(db);
        qry.prepare(qry_text);

        bool success;
        success = qry.exec();

        if (qry.first())
        {
            row_count = qry.value(0).toInt();
        }

        qry.finish();
    }
    closeDb();
    return row_count;
}

// void DbManager::emailEmpty(int slot)
// {
    // QString mt_product = getProductName(slot);
    // QString email_subject = mt_product + " has sold out!";
    // QString email_body = mt_product + " has sold out";
    // QString email_recipients = "paddy@drinkfill.com";
    // QString email = "echo '" + email_body + "' | mail -s '" + email_subject + "' -a 'From: Stongs Soapstand <hello@drinkfill.com>' " + email_recipients + " | screen -d -m";

    // system(email.toStdString().c_str());
// }

void DbManager::addUserInteraction(QString session_id, QString role, QString page, QString event)
{
    addUserInteraction(session_id, role, page, event, "");
}
void DbManager::addUserInteraction(QString session_id, QString role, QString page, QString event, QString data)
{

    {
        QSqlDatabase db = openDb(USAGE_DB_PATH);
        QSqlQuery qry(db);

        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        qry.prepare("INSERT INTO events (time,session_id,access_level,page,event, data) VALUES (:time,:session_id,:access_level,:page,:event,:data);");
        qry.bindValue(":time", time);
        qry.bindValue(":session_id", session_id);
        qry.bindValue(":access_level", role);
        qry.bindValue(":page", page);
        qry.bindValue(":event", event);
        qry.bindValue(":data", data);

        bool success;
        success = qry.exec();

        if (!success)
        {
            qDebug() << "Failed to write user interaction. error type: " << qry.lastError().type() << "Error message:" << qry.lastError().text();
            qDebug() << "Error message:" << qry.lastError().text();
            qDebug() << "Query:" << qry.lastQuery();
        }
        qry.finish();
    }
    closeDb();
}

QString DbManager::getHardwareVersion()
{
    QString hardwareVersion;
    // Your database code here
    // Populate hardwareVersion from the database
    return hardwareVersion;
}
void DbManager::addTemperature(QString machine_id, double temperature_1, double temperature_2, QString alert)
{

    {
        QSqlDatabase db = openDb(USAGE_DB_PATH);
        QSqlQuery qry(db);

        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        qry.prepare("INSERT INTO temperature (time,machine_id,temperature_1,temperature_2,alert) VALUES (:time,:machine_id,:temperature_1,:temperature_2,:alert);");
        qry.bindValue(":time", time);
        qry.bindValue(":machine_id", machine_id);
        double temperature_1_rounded = std::round(temperature_1 * 100) / 100; // round to 2 decimal places
        double temperature_2_rounded = std::round(temperature_2 * 100) / 100; // round to 2 decimal places

        qry.bindValue(":temperature_1", temperature_1_rounded);
        qry.bindValue(":temperature_2", temperature_2_rounded);
        qry.bindValue(":alert", alert);

        bool success;
        success = qry.exec();

        if (!success)
        {
            qDebug() << "Failed to write user interaction. error type: " << qry.lastError().type() << "Error message:" << qry.lastError().text();
            qDebug() << "Error message:" << qry.lastError().text();
            qDebug() << "Query:" << qry.lastQuery();
        }
        qry.finish();
    }
    closeDb();
}

bool DbManager::getRecentTransactions(QString values[][5], int count, int *count_retreived)
{
    // get most recent transactions
    // if less records available then asked for in count, return the retrieved count in count_retreived
    {
        QSqlDatabase db = openDb(USAGE_DB_PATH);
        QSqlQuery qry(db);
        qry.prepare("SELECT id,end_time,quantity_dispensed,price,product_id FROM transactions ORDER BY id DESC LIMIT :count");
        qry.bindValue(":count", count);

        qDebug() << " db retreive transactions: " << count;
        qry.exec();
        int i = 0;
        while (qry.next())
        {
            for (uint8_t j = 0; j < 5; j++)
            {

                if (j == 3)
                {
                    // price rounding
                    double price = qry.value(j).toDouble();
                    values[i][j] = QString::number(price, 'f', 2);
                }
                else
                {
                    values[i][j] = qry.value(j).toString();
                }
            }
            i++;
            *count_retreived = i;
        }
        qry.finish();
    }
    closeDb();
    return true;
}

void DbManager::getPrinterStatus(bool *isOnline, bool *hasPaper)
{

    {
        QSqlDatabase db = openDb(CONFIG_DB_PATH);
        QSqlQuery qry(db);
        // bool is_online = false;
        // bool has_paper = false;

        qry.prepare("SELECT receipt_printer_is_online,receipt_printer_has_paper FROM machine");
        qry.exec();

        while (qry.next())
        {
            *isOnline = (qry.value(0).toInt() == 1);
            *hasPaper = (qry.value(1).toInt() == 1);
        }
        qry.finish();
    }
    closeDb();
}
