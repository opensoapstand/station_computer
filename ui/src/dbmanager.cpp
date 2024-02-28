
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
    QString sql_text = QString("UPDATE products SET %1='%2' WHERE soapstand_product_serial=%3").arg(column, value, QString::number(pnumber));
    return executeQuery(sql_text);
}

bool DbManager::updateTableSlotsWithInt(int slot, QString column, int value)
{
    return updateTableSlotsWithText(slot, column, QString::number(value));
}

bool DbManager::updateTableSlotsWithDouble(int slot, QString column, double value, int precision)
{
    return updateTableSlotsWithText(slot, column, QString::number(value, 'f', precision));
}

bool DbManager::updateTableSlotsWithText(int slot, QString column, QString value)
{
    QString sql_text = QString("UPDATE slots SET %1='%2' WHERE slot_id=%3").arg(column, value, QString::number(slot));
    return executeQuery(sql_text);
}

bool DbManager::addPageClick(const QString &page)
{
    return true;
}

bool DbManager::getAllSlotProperties(int slot,
                                     QVector<int> &dispensePNumbers,
                                     int &basePNumber,
                                     QVector<int> &additivesPNumbers,
                                     bool &is_enabled,
                                     QString &status_text)

{
    // WARNING: consider using having arguments sent to function "by reference" instead of pointers. At least, chatgpt suggests that it's safer.

    qDebug() << "Open db: load all slot properties for slot: " << slot << "From: " << CONFIG_DB_PATH;
    {
        QSqlDatabase db = openDb(CONFIG_DB_PATH);
        QSqlQuery qry(db);
        qry.prepare("SELECT dispense_pnumbers, base_pnumber, additive_pnumbers, is_enabled, status_text FROM slots WHERE slot_id=:slot");
        qry.bindValue(":slot", slot);
        // qry.prepare(QString("SELECT 'slot_id,base_P-number', 'additives_P-numbers', is_enabled, status_text FROM slots WHERE slot_id=%1").arg(QString::number(slot)));
        bool success;
        success = qry.exec();

        if (!success)
        {
            qDebug() << "Open db: Attempted to load all slot properties for slot: " << slot;
            qDebug() << "Did not execute sql. "
                     << qry.lastError() << " | " << qry.lastQuery();
            return false;
        }

        QString additivesAsString;
        QString dispensePNumbersAsString;
        QString basePNumberAsString;
        int row_count = 0;
        while (qry.next())
        {
            row_count++;
            dispensePNumbersAsString = qry.value(0).toString();
            // *basePNumber = qry.value(1).toInt();
            basePNumberAsString = qry.value(1).toString();
            additivesAsString = qry.value(2).toString();
            is_enabled = qry.value(3).toInt();
            status_text = qry.value(4).toString();
        }
        qry.finish();

        if (row_count == 0)
        {
            qDebug() << "ERROR: No record in db table slots for slot " << QString::number(slot);
            return false;
        }
        if (row_count > 1)
        {
            qDebug() << "ERROR: Multiple records(" << QString::number(row_count) << ") in db table slots for slot " << QString::number(slot);
            return false;
        }

        df_util::csvQStringToQVectorInt(additivesAsString, additivesPNumbers);
        df_util::csvQStringToQVectorInt(dispensePNumbersAsString, dispensePNumbers);
        if (basePNumberAsString.isEmpty())
        {
            if (dispensePNumbersAsString.size() >= 1)
            {
                basePNumber = dispensePNumbers[0];
            }
            else
            {
                qDebug("Error: no base product (or dispense product in db for slot. )");
            }
        }
        else
        {
            basePNumber = basePNumberAsString.toInt();
        }
    }
    closeDb();
    return true;
}

bool DbManager::getAllProductProperties(int pnumber,
                                        QString *productId,
                                        QString *soapstand_product_serial,
                                        QVector<int> &mixPNumbers,
                                        QVector<double> &mixRatios,
                                        // QString *size_unit,
                                        // QString *m_currency_deprecated, //_dummy_deprecated
                                        // QString *m_payment_deprecated,  //_deprecated,
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
                                        bool *is_enabled,
                                        QString *status_text,
                                        bool *isSizeEnabled, double *prices, double *volumes, QString *PLUs, QString *PIDs)

{
    QString mix_pnumbers_str;
    QString mix_ratios_str;
    int row_count = 0;

    qDebug() << "Open db: load all product properties for pnumber: " << pnumber << "From: " << CONFIG_DB_PATH;
    {
        QSqlDatabase db = openDb(CONFIG_DB_PATH);
        QSqlQuery qry(db);
        qry.prepare(

            "SELECT "
            "soapstand_product_serial," // 0
            "mix_pnumbers,"
            "mix_ratios,"
            "productId,"
            "name,"
            "currency,"
            "name_receipt,"
            "concentrate_multiplier,"
            "dispense_speed," // 8
            "threshold_flow,"
            "retraction_time," // 10
            "calibration_const,"
            "volume_per_tick,"
            "last_restock,"
            "volume_full,"
            "volume_remaining,"
            "volume_dispensed_since_restock," // 16
            "volume_dispensed_total,"
            "is_enabled_small,"
            "is_enabled_medium,"
            "is_enabled_large," // 20
            "is_enabled_custom,"
            "size_small,"
            "size_medium,"
            "size_large,"
            "size_custom_min,"
            "size_custom_max," // 26
            "price_small,"
            "price_medium,"
            "price_large,"
            "price_custom," // 30
            "plu_small,"
            "plu_medium,"
            "plu_large,"
            "plu_custom,"
            "pid_small," // 35
            "pid_medium,"
            "pid_large,"
            "pid_custom,"
            "flavour,"
            "image_url,"
            "type," // 41
            "ingredients,"
            "features,"
            "description,"
            "is_enabled_custom_discount," // 45
            "size_custom_discount,"
            "price_custom_discount," // 47
            "is_enabled,"
            "status_text," // 49
            "is_enabled_sample," 
            "size_sample,"
            "price_sample "
            "FROM products WHERE soapstand_product_serial=:pnumber"

        );
        qry.bindValue(":pnumber", pnumber);
        bool success;
        success = qry.exec();

        if (!success)
        {
            qDebug() << "Open db: Attempted to load all product properties for pnumber: " << pnumber;
            qDebug() << "Did not execute sql. "
                     << qry.lastError() << " | " << qry.lastQuery();
            return false;
        }
        
        while (qry.next())
        {
            row_count++;

            *soapstand_product_serial = qry.value(0).toString();
            mix_pnumbers_str = qry.value(1).toString();
            mix_ratios_str = qry.value(2).toString();
            *productId = qry.value(3).toString();

            *name_receipt = qry.value(6).toString();
            *concentrate_multiplier = qry.value(7).toInt();
            *dispense_speed = qry.value(8).toInt();
            *threshold_flow = qry.value(9).toDouble();
            *retraction_time = qry.value(10).toInt();
            *calibration_const = qry.value(11).toDouble();
            *volume_per_tick = qry.value(12).toDouble();
            *last_restock = qry.value(13).toString();
            *volume_full = qry.value(14).toDouble();
            *volume_remaining = qry.value(15).toDouble();
            *volume_dispensed_since_restock = qry.value(16).toDouble();
            *volume_dispensed_total = qry.value(17).toDouble();

            isSizeEnabled[1] = qry.value(18).toInt(); // small is index 1!!
            isSizeEnabled[2] = qry.value(19).toInt();
            isSizeEnabled[3] = qry.value(20).toInt();
            isSizeEnabled[4] = qry.value(21).toInt();

            volumes[1] = qry.value(22).toDouble();
            volumes[2] = qry.value(23).toDouble();
            volumes[3] = qry.value(24).toDouble();
            // size custom min
            volumes[4] = qry.value(26).toDouble(); // size custom max.

            prices[1] = qry.value(27).toDouble();
            prices[2] = qry.value(28).toDouble();
            prices[3] = qry.value(29).toDouble();
            prices[4] = qry.value(30).toDouble();

            PLUs[SIZE_SMALL_INDEX] = qry.value(31).toString();
            PLUs[SIZE_MEDIUM_INDEX] = qry.value(32).toString();
            PLUs[SIZE_LARGE_INDEX] = qry.value(33).toString();
            PLUs[SIZE_CUSTOM_INDEX] = qry.value(34).toString();

            PIDs[1] = qry.value(35).toString();
            PIDs[2] = qry.value(36).toString();
            PIDs[3] = qry.value(37).toString();
            PIDs[4] = qry.value(38).toString();

            *is_enabled_custom_discount = qry.value(45).toInt();
            *size_custom_discount = qry.value(46).toDouble();
            *price_custom_discount = qry.value(47).toDouble();
            *is_enabled = qry.value(48).toInt();
            *status_text = qry.value(49).toString();
            // Sample size assignment
            isSizeEnabled[6] = qry.value(50).toInt();
            volumes[6] = qry.value(51).toDouble();
            prices[6] = qry.value(52).toDouble();
        }

        qry.finish();
    }
    closeDb();

    if (row_count == 0)
    {
        qDebug() << "ERROR: No record in db table products for product " << QString::number(pnumber);
        return false;
    }
    if (row_count > 1)
    {
        qDebug() << "ERROR: Multiple records (" << QString::number(row_count) << ")in db table products for product " << QString::number(pnumber);
        return false;
    }

    df_util::csvQStringToQVectorInt(mix_pnumbers_str, mixPNumbers);
    df_util::csvQStringToQVectorDouble(mix_ratios_str, mixRatios);
    return true;
}

bool DbManager::getAllMachineProperties(
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
    double *alert_temperature,
    QString *software_version_controller,
    int *is_enabled,
    QString *status_text,
    QString *payment,
    QString *size_unit,
    int *screen_sleep_time24h,
    int *screen_wakeup_time24h,
    int *buy_bottle_1,
    int *buy_bottle_2,
    QString *portal_base_url,
    int *enable_offline_payment)
{
    bool success;
    qDebug() << " db... all machine properties from: " << CONFIG_DB_PATH;
    {
        QSqlDatabase db = openDb(CONFIG_DB_PATH);
        QSqlQuery qry(db);

        qry.prepare(

            "SELECT "
            "machine_id," // 0
            "soapstand_customer_id,"
            "template,"
            "location,"
            "controller_type,"
            "controller_id,"
            "screen_type,"
            "'screen_id'," // 7
            "has_receipt_printer,"
            "receipt_printer_is_online,"
            "receipt_printer_has_paper,"
            "has_tap_payment,"
            "hardware_version,"
            "software_version,"
            "aws_port," // 14
            "coupons_enabled,"
            "has_empty_detection,"
            "enable_pump_ramping,"
            "enable_pump_reversal,"
            "dispense_buttons_count,"
            "maintenance_pwd," // 20
            "show_transactions,"
            "help_text_html,"
            "idle_page_type,"
            "admin_pwd,"
            "alert_temperature," // 25
            "software_version_controller,"
            "is_enabled,"
            "status_text,"
            "payment,"   // 29
            "size_unit," // 30
            "screen_sleep_time24h,"
            "screen_wakeup_time24h,"
            "buy_bottle_1,"
            "buy_bottle_2,"
            "portal_base_url,"
            "enable_offline_payment"
            " FROM machine"

        );

        success = qry.exec();
        if (!success)
        {
            qDebug() << "Open db: Attempted to load machine properties";
            qDebug() << "Did not execute sql. "
                     << qry.lastError() << " | " << qry.lastQuery();
        }
        int row_count = 0;
        while (qry.next())
        {
            row_count++;
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
            *coupons_enabled = qry.value(15).toInt();
            *has_empty_detection = qry.value(16).toInt();
            *enable_pump_ramping = qry.value(17).toInt();
            *enable_pump_reversal = qry.value(18).toInt();
            *dispense_buttons_count = qry.value(19).toInt();
            *maintenance_pwd = qry.value(20).toString();
            *show_transactions = qry.value(21).toInt();
            *help_text_html = qry.value(22).toString();
            *idle_page_type = qry.value(23).toString();
            *admin_pwd = qry.value(24).toString();
            *alert_temperature = qry.value(25).toDouble();
            *software_version_controller = qry.value(26).toString();
            *is_enabled = qry.value(27).toInt();
            *status_text = qry.value(28).toString();
            *payment = qry.value(29).toString();
            *size_unit = qry.value(30).toString();
            *screen_sleep_time24h = qry.value(31).toInt();
            *screen_wakeup_time24h = qry.value(32).toInt();
            *buy_bottle_1 = qry.value(33).toInt();
            *buy_bottle_2 = qry.value(34).toInt();
            *portal_base_url = qry.value(35).toString();
            *enable_offline_payment = qry.value(36).toInt();
        }
        qry.finish();

        if (row_count == 0)
        {
            qDebug() << "ERROR: No record in db table machine ";
            return false;
        }
        if (row_count > 1)
        {
            qDebug() << "ERROR: Multiple records(" << QString::number(row_count) << ") in db table machine";
            return false;
        }
    }
    closeDb();
    return success;
}

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

void DbManager::setPaymentTransaction(const std::map<std::string, std::string> &paymentObject)
{
    {
        QSqlDatabase db = openDb(USAGE_DB_PATH);
        QSqlQuery qry(db);

        qry.prepare("INSERT INTO payments(transaction_id,date,time,mac_label,amount,auth_code,ctrout_saf,card_number,card_type,status) VALUES (:transaction_id,:date,:time,:mac_label,:amount,:auth_code,:ctrout_saf,:card_number,:card_type,:status);");
        // qry.bindValue(":transaction_id", QVariant::fromValue(QString::fromStdString(paymentObject.at("session_id"))));
        qry.bindValue(":date", QVariant::fromValue(QString::fromStdString(paymentObject.at("date"))));
        qry.bindValue(":time", QVariant::fromValue(QString::fromStdString(paymentObject.at("time"))));
        qry.bindValue(":mac_label", QVariant::fromValue(QString::fromStdString(paymentObject.at("mac_label"))));
        qry.bindValue(":amount", QVariant::fromValue(QString::fromStdString(paymentObject.at("amount"))));
        qry.bindValue(":auth_code", QVariant::fromValue(QString::fromStdString(paymentObject.at("auth_code"))));
        qry.bindValue(":ctrout_saf", QVariant::fromValue(QString::fromStdString(paymentObject.at("ctrout_saf"))));
        qry.bindValue(":card_number", QVariant::fromValue(QString::fromStdString(paymentObject.at("card_number"))));
        qry.bindValue(":card_type", QVariant::fromValue(QString::fromStdString(paymentObject.at("card_type"))));
        qry.bindValue(":status", QVariant::fromValue(QString::fromStdString(paymentObject.at("status"))));
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
    qDebug() << "Payment database write";
    closeDb();
}