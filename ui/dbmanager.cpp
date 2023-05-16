
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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

DbManager::DbManager(const QString &path)
{
    //    qDebug() << "CREATING DB OBJECT" << endl;
    int attempts = 10;

    // while (!m_db.open() && attempts > 0)
    // {
    //     this->initialize(path);
    //     if (!m_db.open()){
    //         usleep(5);
    //         qDebug() << "init db failed. attempt: " << attempts;
    //         attempts --;
    //     }
    // }

    // TAKE EXTREME CARE. DO NOT NEST DBopen en db close!!!! hard to find error:("is still in use all queries will cease to work" )

    qDebug() << "db init";
    if (m_db.isOpen())
    {
        qDebug() << "m_db is already open. Try to close.";
        m_db.close();
        m_db = QSqlDatabase();
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
        qDebug() << "m_db was already open. Closed it first.";
    }

    if (m_db.connectionName().isEmpty())
    {
        // qDebug() << "connectionname is empty-->";
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName(path);
        // qDebug() << "m_db set connectionName";
    }
    else
    {
        qDebug() << "m_db connectionName is NOT EMPTY";
    }

    if (!m_db.open())
    {
        qDebug() << "Error: connection with database failed";
    }
    else
    {
        // qDebug() << "Database: connection ok";
    }

    while (isDatabaseLocked(m_db))
    {
        qDebug() << "Database is locked. Wait until unlocked";
        usleep(100000);
    }
}

// DTOR
DbManager::~DbManager()
{

    if (m_db.isOpen())
    {
        m_db.close();
        m_db = QSqlDatabase();
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }
}

// bool DbManager::initialize(const QString& path){

//     qDebug() << "db init";
//     if (m_db.isOpen()){
//         qDebug() << "m_db is already open. Try to close.";
//         m_db.close();
//         m_db = QSqlDatabase();
//         QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
//         qDebug() << "m_db was already open. Closed it first.";
//     }

//     if (m_db.connectionName().isEmpty()){
//         qDebug() << "connectionname is empty-->";
//         m_db = QSqlDatabase::addDatabase("QSQLITE");
//         m_db.setDatabaseName(path);
//         qDebug() << "m_db set connectionName";
//     }else{
//         qDebug() << "m_db connectionName is NOT EMPTY";
//     }
// }

bool DbManager::addPageClick(const QString &page)
{

    return true;
}

bool DbManager::isDatabaseLocked(const QSqlDatabase &db)
{
    // https://stackoverflow.com/questions/57744538/determine-whether-sqlite-database-is-locked
    QSqlQuery q(db);
    if (q.exec("BEGIN EXCLUSIVE")) // tries to acquire the lock
    {
        q.exec("COMMIT"); // releases the lock immediately
        return false;     // db is not locked
    }
    return true; // failed to acquire the lock: returns true (db is locked)
}

void DbManager::closeDB()
{
    qDebug() << "close db";
    if (m_db.isOpen())
    {
        {
            m_db.close();
            m_db = QSqlDatabase();
        }
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }
}

QString DbManager::getProductDrinkfillSerial(int slot)
{
    QSqlQuery qry;
    QString val;

    {
        qry.prepare("SELECT soapstand_product_serial FROM products WHERE slot=:slot");
        qry.bindValue(":slot", slot);
        qry.exec();

        while (qry.next())
        {
            val = qry.value(0).toString();
        }
    }

    return val;
}

void DbManager::updateTapToQR()
{

    QSqlQuery qry;
    {
        qry.prepare("UPDATE products SET payment=\"qr\";");
        qry.exec();
    }
}

void DbManager::getCustomDiscountProperties(int slot, bool *isEnabled, double *volumeDiscount, double *pricePerLiterDiscount)
{
    QSqlQuery qry;
    {
        qry.prepare("SELECT is_enabled_custom_discount,size_custom_discount,price_custom_discount FROM products WHERE slot=:slot");
        // qry.prepare("SELECT name,description,features,ingredients,is_enabled_small,is_enabled_medium,is_enabled_large,is_enabled_custom FROM products WHERE slot=:slot");
        qry.bindValue(":slot", slot);
        qry.exec();

        while (qry.next())
        {
            *isEnabled = qry.value(0).toInt();
            *volumeDiscount = qry.value(1).toDouble();
            *pricePerLiterDiscount = qry.value(2).toDouble();
        }
    }
}

void DbManager::getProductProperties(int slot, QString *product_id, bool *isSizeEnabled)
// void DbManager::getProductProperties(int slot, QString*name, QString *description, QString *features,  QString *ingredients, bool* isEnabledSmall,bool* isEnabledMedium,bool* isEnabledLarge,bool* isEnabledCustom)
{
    qDebug() << " db... product properties";
    QSqlQuery qry;
    {
        qry.prepare("SELECT soapstand_product_serial, is_enabled_small, is_enabled_medium, is_enabled_large, is_enabled_custom FROM products WHERE slot=:slot");
        // qry.prepare("SELECT name,description,features,ingredients,is_enabled_small,is_enabled_medium,is_enabled_large,is_enabled_custom FROM products WHERE slot=:slot");
        qry.bindValue(":slot", slot);
        qry.exec();

        while (qry.next())
        {
            *product_id = qry.value(0).toString();
            // *description = qry.value(1).toString();
            // *features = qry.value(2).toString();
            // *ingredients = qry.value(3).toString();
            isSizeEnabled[1] = qry.value(1).toInt(); // small is index 1!!
            isSizeEnabled[2] = qry.value(2).toInt();
            isSizeEnabled[3] = qry.value(3).toInt();
            isSizeEnabled[4] = qry.value(4).toInt();
        }
    }
}

QString DbManager::getProductName(int slot)
{
    QSqlQuery product_query;
    QString product_name;

    {

        product_query.prepare("SELECT name FROM products WHERE slot=:slot");
        product_query.bindValue(":slot", slot);
        product_query.exec();

        while (product_query.next())
        {
            product_name = product_query.value(0).toString();
        }
    }

    return product_name;
}

QString DbManager::getProductReceiptName(int slot)
{
    qDebug() << " db... getProductReceiptName";
    QSqlQuery product_query;
    QString product_name;

    {

        product_query.prepare("SELECT name_receipt FROM products WHERE slot=:slot");
        product_query.bindValue(":slot", slot);
        product_query.exec();

        while (product_query.next())
        {
            product_name = product_query.value(0).toString();
        }
    }

    return product_name;
}

double DbManager::getProductPrice(int slot, char size)
{
    qDebug() << " db... getProductPrice";
    QSqlQuery price_query;
    double price;

    {

        if (size == 'l')
        {
            price_query.prepare("SELECT price_large FROM products WHERE slot=:slot");
        }
        else if (size == 'm')
        {
            price_query.prepare("SELECT price_medium FROM products WHERE slot=:slot");
        }
        else if (size == 's')
        {
            price_query.prepare("SELECT price_small FROM products WHERE slot=:slot");
        }
        else if (size == 'c')
        {
            price_query.prepare("SELECT price_custom FROM products WHERE slot=:slot");
        }
        else
        {
            qDebug() << " invalid size to volume character found.  " << size;
        }

        price_query.bindValue(":slot", slot);
        price_query.exec();

        while (price_query.next())
        {
            price = price_query.value(0).toDouble();
        }
    }

    return price;
}

double DbManager::getProductVolumePerTick(int slot)
{
    qDebug() << " db... getProductVolumePerTick";
    QSqlQuery vol_per_tick_query;
    double vol_per_tick;

    {

        vol_per_tick_query.prepare("SELECT volume_per_tick FROM products WHERE slot=:slot");
        vol_per_tick_query.bindValue(":slot", slot);
        vol_per_tick_query.exec();

        while (vol_per_tick_query.next())
        {
            vol_per_tick = vol_per_tick_query.value(0).toDouble();
        }
    }

    return vol_per_tick;
}

double DbManager::getProductVolume(int slot, char size)
{
    qDebug() << " db... getProductVolume";
    QSqlQuery volume_query;
    double volume;

    {

        if (size == 'l')
        {
            volume_query.prepare("SELECT size_large FROM products WHERE slot=:slot");
        }
        else if (size == 'm')
        {
            volume_query.prepare("SELECT size_medium FROM products WHERE slot=:slot");
        }
        else if (size == 's')
        {
            volume_query.prepare("SELECT size_small FROM products WHERE slot=:slot");
        }
        else if (size == 'c')
        {
            volume_query.prepare("SELECT size_custom_max FROM products WHERE slot=:slot");
        }
        else
        {
            qDebug() << " invalid size to volume character found.  " << size;
        }

        volume_query.bindValue(":slot", slot);
        volume_query.exec();

        while (volume_query.next())
        {
            volume = volume_query.value(0).toDouble();
        }
    }
    qDebug() << " size to volume  " << size << "=" << volume;
    return volume;
}

double DbManager::getFullProduct(int slot)
{
    qDebug() << " db... getFullProduct";
    QSqlQuery full_query;
    double full;
    {

        full_query.prepare("SELECT volume_full FROM products WHERE slot=:slot");

        full_query.bindValue(":slot", slot);
        full_query.exec();

        while (full_query.next())
        {
            full = full_query.value(0).toDouble();
        }
    }
    return full;
}

QString DbManager::getPaymentMethod(int slot)
{
    qDebug() << " db... getPaymentMethod";
    QSqlQuery paymeny_query;
    QString payment_method;

    {

        paymeny_query.prepare("SELECT payment FROM products WHERE slot=:slot");
        paymeny_query.bindValue(":slot", slot);
        paymeny_query.exec();

        while (paymeny_query.next())
        {
            payment_method = paymeny_query.value(0).toString();
        }
    }
    return payment_method;
}

uint32_t DbManager::getNumberOfRows(QString table)
{
    qDebug() << " db... getNumberOfRows";
    // qDebug() << "rows: " << getNumberOfRows("products") << endl;
    QString qry = "SELECT COUNT(*) FROM ";

    qry.append(table);

    QSqlQuery query;
    uint32_t row_count = 0;

    query.exec(qry);

    if (query.first())
    {
        row_count = query.value(0).toInt();
    }

    return row_count;
}

bool DbManager::isProductVolumeInContainer(int slot)
{
    if (getEmptyContainerDetectionEnabled())
    {
        return getVolumeRemaining(slot) > 0;
    }
    else
    {
        return getVolumeRemaining(slot) > getProductVolume(slot, 'l'); // ----> TODO VERY BUGGY (only instance found of using char volume as a magic number)
    }
}

bool DbManager::refill(int slot)
{
    qDebug() << " db... refill";
    QSqlQuery refill_query;
    bool success = false;

    QString sql_set_vol = "UPDATE products SET volume_remaining=volume_full WHERE slot=:slot";
    QString sql_res_disp = "UPDATE products SET volume_dispensed_since_restock=0 WHERE slot=:slot";
    QString sql_set_time = "UPDATE products SET last_restock=:time WHERE slot=:slot";

    {
        refill_query.prepare(sql_set_vol);
        refill_query.bindValue(":slot", slot);
        if (refill_query.exec())
        {
            refill_query.prepare(sql_res_disp);
            refill_query.bindValue(":slot", slot);
            if (refill_query.exec())
            {
                QSqlQuery refill_date;
                refill_date.prepare(sql_set_time);
                refill_date.bindValue(":slot", slot);
                refill_date.bindValue(":time", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

                if (refill_date.exec())
                {
                    qDebug() << "Restock success. slot:" << QString::number(slot)
                             << " volume:" << QString::number(getVolumeRemaining(slot));

                    success = true;
                }
                else
                {
                    qDebug() << "refill date error:"
                             << refill_date.lastError();
                    success = false;
                }
            }
            else
            {
                qDebug() << "total ml dispensed update error:"
                         << refill_query.lastError();
                success = false;
            }
        }
        else
        {
            qDebug() << "remaining ml update error:"
                     << refill_query.lastError();
            success = false;
        }
    }

    return success;
}

void DbManager::emailEmpty(int slot)
{
    QString mt_product = getProductName(slot);
    QString email_subject = mt_product + " has sold out!";
    QString email_body = mt_product + " has sold out";
    QString email_recipients = "paddy@drinkfill.com";
    QString email = "echo '" + email_body + "' | mail -s '" + email_subject + "' -a 'From: Stongs Soapstand <hello@drinkfill.com>' " + email_recipients + " | screen -d -m";

    system(email.toStdString().c_str());
}

int DbManager::getTotalTransactions()
{
    qDebug() << " db... getTotalTransactions";
    QSqlQuery transaction_query;
    double transactions;

    {
        transaction_query.prepare("SELECT COUNT(*) FROM transactions;");
        transaction_query.exec();

        while (transaction_query.next())
        {
            transactions = transaction_query.value(0).toInt();
        }
    }

    return transactions;
}

int DbManager::getNumberOfProducts()
{
    qDebug() << " db... getNumberOfProducts";
    QSqlQuery products_query;
    int products;

    {
        products_query.prepare("SELECT COUNT(*) FROM products;");
        products_query.exec();

        while (products_query.next())
        {
            products = products_query.value(0).toInt();
        }
    }
    return products;
}

double DbManager::getVolumeDispensedSinceRestock(int slot)
{
    qDebug() << " db...vol dispensed since restock";
    QSqlQuery dispensed_query;
    double dispensed;
    {

        dispensed_query.prepare("SELECT volume_dispensed_since_restock FROM products WHERE slot=:slot");
        dispensed_query.bindValue(":slot", slot);
        dispensed_query.exec();

        while (dispensed_query.next())
        {
            dispensed = dispensed_query.value(0).toDouble();
        }
    }
    return dispensed;
}

double DbManager::getTotalDispensed(int slot)
{
    qDebug() << " db... getTotalDispensed";
    QSqlQuery dispensed_query;
    double dispensed;
    {
        dispensed_query.prepare("SELECT volume_dispensed_total FROM products WHERE slot=:slot");
        dispensed_query.bindValue(":slot", slot);
        dispensed_query.exec();

        while (dispensed_query.next())
        {
            dispensed = dispensed_query.value(0).toDouble();
        }
    }
    return dispensed;
}

#ifndef USE_OLD_DATABASE

int DbManager::getDispenseButtonCount()
{
    // QSqlQuery qry;
    int count;

    // QString qry_qstr = QString("SELECT dispense_buttons_count FROM machine");

    QSqlQuery mid_query;
    QString mid_string;

    {
        mid_query.prepare("SELECT dispense_buttons_count FROM machine");
        mid_query.exec();

        while (mid_query.next())
        {
            count = mid_query.value(0).toInt();
        }
    }
    return count;

    // // https://stackoverflow.com/questions/4214369/how-to-convert-qstring-to-stdstring/4644922#4644922
    // string qry_string = qry_qstr.toUtf8().constData();
    // qry.prepare(qry_string.c_str());

    // qry.exec();
    // if (qry.exec())
    // {
    // }
    // else
    // {
    //     qDebug() << "Failed to check slot available." << qry_qstr;
    // }

    // while (qry.next())
    //     qDebug() << qry.value(0);
    // {
    //     count = qry.value(0).toInt();
    // }

    // return count;
}

int DbManager::getSlotEnabled(int slot)
{
    QSqlQuery qry;
    bool enabled;

    QString qry_qstr = QString("SELECT is_enabled_slot_%1 FROM machine").arg(slot);

    // https://stackoverflow.com/questions/4214369/how-to-convert-qstring-to-stdstring/4644922#4644922
    string qry_string = qry_qstr.toUtf8().constData();
    qry.prepare(qry_string.c_str());
    qry.bindValue(":slot", slot);

    qry.exec();
    if (qry.exec())
    {
    }
    else
    {
        qDebug() << "Failed to check slot available." << qry_qstr;
    }

    while (qry.next())
    {
        qDebug() << qry.value(0);
        enabled = qry.value(0).toInt();
    }

    return enabled;
}

QString DbManager::getStatusText(int slot)
{
    QSqlQuery qry;
    QString val;

    QString qry_qstr = QString("SELECT status_text_slot_%1 FROM machine").arg(slot);

    // https://stackoverflow.com/questions/4214369/how-to-convert-qstring-to-stdstring/4644922#4644922
    string qry_string = qry_qstr.toUtf8().constData();
    qry.prepare(qry_string.c_str());
    qry.bindValue(":slot", slot);

    qry.exec();
    if (qry.exec())
    {
    }
    else
    {
        qDebug() << "Failed to check slot available." << qry_qstr;
    }

    while (qry.next())
    {
        qDebug() << qry.value(0);
        val = qry.value(0).toString();
    }

    return val;
}

bool DbManager::getRecentTransactions(QString values[][5], int count, int *count_retreived)
{
    // get number of most recent transactions
    QSqlQuery qry;
    // bool is_enabled;
    {
        // std::string sql_statement =  "SELECT id,end_time,quantity_dispensed,price,product FROM transactions ORDER BY id DESC LIMIT " + to_string(count);
        // qry.prepare(sql_statement.c_str());
        // qry.prepare(sql_statement.c_str());

        qry.prepare("SELECT id,end_time,quantity_dispensed,price,product FROM transactions ORDER BY id DESC LIMIT :count");
        qry.bindValue(":count", count);

        qDebug() << " db retreive transactions: " << count;
        qry.exec();
        int i = 0;
        while (qry.next())
        {
            for (uint8_t j = 0; j < 5; j++)
            {
                values[i][j] = qry.value(j).toString();
                qDebug() << "db bdafes: " << i << " : " << values[i][j];
            }
            i++;
            *count_retreived = i;
        }
    }
    return true;
}

bool DbManager::getPumpRampingEnabled()
{
    QSqlQuery qry;
    bool is_enabled;
    {
        qry.prepare("SELECT enable_pump_ramping FROM machine");
        qry.exec();

        while (qry.next())
        {
            is_enabled = (qry.value(0).toInt() == 1);
        }
    }
    return is_enabled;
}

bool DbManager::setPumpRampingEnabled(int isEnabled)
{
    QSqlQuery qry;
    bool enabled;

    QString qry_qstr = QString("UPDATE machine SET enable_pump_ramping=%1").arg(QString::number(isEnabled));
    string qry_string = qry_qstr.toUtf8().constData(); // https://stackoverflow.com/questions/4214369/how-to-convert-qstring-to-stdstring/4644922#4644922
    qry.prepare(qry_string.c_str());
    qry.exec();

    if (qry.exec())
    {
        return true;
    }
    else
    {
        qDebug() << "Failed to set pump ramping value." << qry_qstr;
        return false;
    }
}

bool DbManager::getEmptyContainerDetectionEnabled()
{
    QSqlQuery qry;
    bool is_enabled;
    {
        qry.prepare("SELECT has_empty_detection FROM machine");
        qry.exec();

        while (qry.next())
        {
            is_enabled = (qry.value(0).toInt() == 1);
        }
    }
    return is_enabled;
}

bool DbManager::setEmptyContainerDetectionEnabled(int isEnabled)
{
    QSqlQuery qry;
    bool enabled;

    QString qry_qstr = QString("UPDATE machine SET has_empty_detection=%1").arg(QString::number(isEnabled));
    string qry_string = qry_qstr.toUtf8().constData(); // https://stackoverflow.com/questions/4214369/how-to-convert-qstring-to-stdstring/4644922#4644922
    qry.prepare(qry_string.c_str());
    qry.exec();

    if (qry.exec())
    {
        return true;
    }
    else
    {
        qDebug() << "Failed to set empty container value." << qry_qstr;
        return false;
    }
}

bool DbManager::getCouponsEnabled()
{
    QSqlQuery qry;
    bool is_enabled;
    {
        qry.prepare("SELECT coupons_enabled FROM machine");
        qry.exec();

        while (qry.next())
        {
            is_enabled = (qry.value(0).toInt() == 1);
        }
    }
    return is_enabled;
}

QString DbManager::getTemplateName()
{
    QSqlQuery qry;
    QString val;
    {
        qry.prepare("SELECT template FROM machine");
        qry.exec();

        while (qry.next())
        {
            val = qry.value(0).toString();
        }
    }
    return val;
}

QString DbManager::getCustomerId()
{
    QSqlQuery qry;
    QString val;
    {
        qry.prepare("SELECT soapstand_customer_id FROM machine");
        qry.exec();

        while (qry.next())
        {
            val = qry.value(0).toString();
        }
    }
    return val;
}

bool DbManager::updateSlotAvailability(int slot, int isEnabled, QString status_text)
{
    QSqlQuery qry;
    bool enabled;

    // QString qry_qstr = QString("UPDATE machine SET is_enabled_slot_%1=0").arg(QString::number(slot), QString::number(isEnabled));
    QString qry_qstr = QString("UPDATE machine SET status_text_slot_%1='").arg(QString::number(slot)) + status_text + QString("',is_enabled_slot_%1=").arg(QString::number(slot)) + QString::number(isEnabled);
    qDebug() << qry_qstr << endl;
    string qry_string = qry_qstr.toUtf8().constData(); // https://stackoverflow.com/questions/4214369/how-to-convert-qstring-to-stdstring/4644922#4644922
    qry.prepare(qry_string.c_str());
    qry.bindValue(":slot", slot);
    qry.bindValue(":status_text", status_text);
    qry.bindValue(":isEnabled", isEnabled);
    qry.exec();

    if (qry.exec())
    {
        return true;
    }
    else
    {
        qDebug() << "Failed to set slot availability." << qry_qstr;
        return false;
    }
}

#endif

double DbManager::getVolumeRemaining(int slot)
{

    qDebug() << " db... getVolumeRemaining";
    QSqlQuery remaining_query;
    double remaining;
    {
        remaining_query.prepare("SELECT volume_remaining FROM products WHERE slot=:slot");
        remaining_query.bindValue(":slot", slot);
        remaining_query.exec();

        while (remaining_query.next())
        {
            remaining = remaining_query.value(0).toDouble();
        }
    }
    return remaining;
}

QString DbManager::getLastRestockDate(int slot)
{
    // last as in "most recent"
    qDebug() << " db... getLastRestockDate";
    QSqlQuery refill_date_query;
    QString refill_date_string;

    {
        refill_date_query.prepare("SELECT last_restock FROM products WHERE slot=:slot");
        refill_date_query.bindValue(":slot", slot);
        refill_date_query.exec();

        while (refill_date_query.next())
        {
            refill_date_string = refill_date_query.value(0).toString();

            // qDebug() << "Product: " << product_name << endl;
        }
    }
    return refill_date_string;
}

double DbManager::getTemperature()
{
    qDebug() << " db... getTemperature";
    QSqlQuery temperature_query;
    double temperature;

    {
        temperature_query.prepare("SELECT temp FROM temperature LIMIT 1 offset cast((SELECT count(*) FROM temperature) AS INT) - 1");
        temperature_query.exec();
        while (temperature_query.next())
        {
            temperature = temperature_query.value(0).toDouble();
        }
    }
    return temperature;
}

int DbManager::getPWM(int slot)
{
    QSqlQuery pwm_query;
    double pwm;

    pwm_query.prepare("SELECT dispense_speed FROM products WHERE slot=:slot");
    pwm_query.bindValue(":slot", slot);
    pwm_query.exec();
    {
        while (pwm_query.next())
        {
            pwm = pwm_query.value(0).toInt();
            qDebug() << "pwm" << slot << pwm << endl;
        }
    }

    return pwm;
}

double DbManager::getBuffer(int slot)
{
    qDebug() << " db... getBuffer";
    QSqlQuery buffer_query;
    double buffer;

    {
        buffer_query.prepare("SELECT buffer FROM products WHERE slot=:slot");
        buffer_query.bindValue(":slot", slot);
        buffer_query.exec();
        while (buffer_query.next())
        {
            buffer = buffer_query.value(0).toDouble();
        }
    }
    return buffer;
}

bool DbManager::updatePaymentsDb(QString date, QString time, QString txnType, QString amount, QString cardNo, QString refNo, QString authNo, QString cardType, QString status, QString isoCode, QString hostCode, QString tvr)
{
    qDebug() << " db... updatePaymentsDb";
    QSqlQuery payments_query;
    bool success;

    {
        payments_query.prepare("INSERT INTO payments (date, time, txnType, amount, cardNo, refNo, authNo, cardType, status, isoCode, hostCode, tvr) VALUES (:date, :time, :txnType, :amount, :cardNo, :refNo, :authNo, :cardType, :status, :isoCode, :hostCode, :tvr);");
        payments_query.bindValue(":date", date);
        payments_query.bindValue(":time", time);
        payments_query.bindValue(":txnType", txnType);
        payments_query.bindValue(":amount", amount);
        payments_query.bindValue(":cardNo", cardNo);
        payments_query.bindValue(":refNo", refNo);
        payments_query.bindValue(":authNo", authNo);
        payments_query.bindValue(":cardType", cardType);
        payments_query.bindValue(":status", status);
        payments_query.bindValue(":isoCode", isoCode);
        payments_query.bindValue(":hostCode", hostCode);
        payments_query.bindValue(":tvr", tvr);

        if (payments_query.exec())
        {
            success = true;
            //        qDebug() << "Payment Database updated successfully!";
        }
        else
        {
            //        qDebug() << "Payment Database update error:"
            //                 << payments_query.lastError();
        }
    }
    return success;
}

bool DbManager::updatePrice(int slot, int size, double new_price)
{
    qDebug() << " db... updatePriceSmall";
    QSqlQuery update_price_query;

    {

        switch (size)
        {

        case SIZE_SMALL_INDEX:
        {

            update_price_query.prepare("UPDATE products SET price_small = :new_price WHERE slot = :slot");
        }
        break;
        case SIZE_MEDIUM_INDEX:
        {

            update_price_query.prepare("UPDATE products SET price_medium = :new_price WHERE slot = :slot");
        }
        break;

        case SIZE_LARGE_INDEX:
        {
            update_price_query.prepare("UPDATE products SET price_large = :new_price WHERE slot = :slot");
        }
        break;
        case SIZE_CUSTOM_INDEX:
        {
            update_price_query.prepare("UPDATE products SET price_custom = :new_price WHERE slot = :slot");
        }
        break;
        default:
        {
        }
        break;
        }

        update_price_query.bindValue(":new_price", new_price);
        update_price_query.bindValue(":slot", slot);

        if (update_price_query.exec())
        {
            //        qDebug() << "Price updated successfully!";
            return true;
        }
        else
        {
            qDebug() << "Price update error: !"
                     << update_price_query.lastQuery()
                     << update_price_query.lastError();
            return false;
        }
    }
}

// bool DbManager::updatePriceSmall(int slot, double new_price)
// {
//     qDebug() << " db... updatePriceSmall";
//     QSqlQuery update_price_query;

//     {
// #ifdef USE_OLD_DATABASE
//         update_price_query.prepare("UPDATE products SET price_s = :new_price WHERE slot = :slot");
// #else
//         update_price_query.prepare("UPDATE products SET price_small = :new_price WHERE slot = :slot");
// #endif
//         update_price_query.bindValue(":new_price", new_price);
//         update_price_query.bindValue(":slot", slot);

//         if (update_price_query.exec())
//         {
//             //        qDebug() << "Price updated successfully!";
//             return true;
//         }
//         else
//         {
//             //        qDebug() << "Price update error: !"
//             //                 << update_price_query.lastQuery()
//             //                 << update_price_query.lastError();
//             return false;
//         }
//     }
// }

// bool DbManager::updatePriceLarge(int slot, double new_price)
// {
//     qDebug() << " db... updatePriceLarge";
//     QSqlQuery update_price_query;

//     {
// #ifdef USE_OLD_DATABASE
//         update_price_query.prepare("UPDATE products SET price_l = :new_price WHERE slot = :slot");
// #else
//         update_price_query.prepare("UPDATE products SET price_large = :new_price WHERE slot = :slot");
// #endif
//         update_price_query.bindValue(":new_price", new_price);
//         update_price_query.bindValue(":slot", slot);

//         if (update_price_query.exec())
//         {
//             //        qDebug() << "Price updated successfully!";
//             return true;
//         }
//         else
//         {
//             //        qDebug() << "Price update error: !"
//             //                 << update_price_query.lastError();
//             return false;
//         }
//     }
// }

// bool DbManager::updateTargetVolume_s(int slot, double new_volume)
// {
//     qDebug() << " db... updateTargetVolume_s";
//     QSqlQuery update_target_volume_query;
//     {
// #ifdef USE_OLD_DATABASE
//         update_target_volume_query.prepare("UPDATE products SET volume_target_s=:new_volume WHERE slot=:slot");
// #else
//         update_target_volume_query.prepare("UPDATE products SET size_small=:new_volume WHERE slot=:slot");
// #endif

//         update_target_volume_query.bindValue(":new_volume", new_volume);
//         update_target_volume_query.bindValue(":slot", slot);

//         if (update_target_volume_query.exec())
//         {
//             //        qDebug() << "Target Volume updated successfully!";
//             return true;
//         }
//         else
//         {
//             //        qDebug() << "Target volume update error: !"
//             //                 << update_target_volume_query.lastError();
//             return false;
//         }
//     }
// }

// bool DbManager::updateTargetVolume_l(int slot, double new_volume)
// {
//     qDebug() << " db... updateTargetVolume_l";
//     QSqlQuery update_target_volume_query;

//     {
// #ifdef USE_OLD_DATABASE
//         update_target_volume_query.prepare("UPDATE products SET volume_target_l=:new_volume WHERE slot=:slot");
// #else
//         update_target_volume_query.prepare("UPDATE products SET size_large=:new_volume WHERE slot=:slot");
// #endif

//         update_target_volume_query.bindValue(":new_volume", new_volume);

//         update_target_volume_query.bindValue(":slot", slot);

//         if (update_target_volume_query.exec())
//         {
//             //        qDebug() << "Target Volume updated successfully!";
//             return true;
//         }
//         else
//         {
//             //        qDebug() << "Target volume update error: !"
//             //                 << update_target_volume_query.lastError();
//             return false;
//         }
//     }
// }
bool DbManager::updateTargetVolume(int slot, int size, double new_volume)
{
    qDebug() << " db... updateTargetVolume_l";
    QSqlQuery update_target_volume_query;

    {

        switch (size)
        {
        case SIZE_SMALL_INDEX:
        {
            update_target_volume_query.prepare("UPDATE products SET size_small=:new_volume WHERE slot=:slot");
        }
        break;
        case SIZE_MEDIUM_INDEX:
        {
            update_target_volume_query.prepare("UPDATE products SET size_medium=:new_volume WHERE slot=:slot");
        }
        break;
        case SIZE_LARGE_INDEX:
        {
            update_target_volume_query.prepare("UPDATE products SET size_large=:new_volume WHERE slot=:slot");
        }
        break;
        default:
        {
            qDebug() << "size not handled... " << size;
        }
        }

        update_target_volume_query.bindValue(":new_volume", new_volume);

        update_target_volume_query.bindValue(":slot", slot);

        if (update_target_volume_query.exec())
        {
            //        qDebug() << "Target Volume updated successfully!";
            return true;
        }
        else
        {
            qDebug() << "Target volume update error: !"
                     << update_target_volume_query.lastError();
            return false;
        }
    }
}

bool DbManager::updateVolumePerTick(int slot, double new_volume_per_tick)
{
    qDebug() << " db... updateVolumePerTick";
    QSqlQuery update_volume_per_tick_query;

    {
        update_volume_per_tick_query.prepare("UPDATE products SET volume_per_tick=:new_volume_per_tick WHERE slot=:slot");
        update_volume_per_tick_query.bindValue(":new_volume_per_tick", new_volume_per_tick);
        update_volume_per_tick_query.bindValue(":slot", slot);

        if (update_volume_per_tick_query.exec())
        {
            //        qDebug() << "Volume per tick updated successfully!";
            return true;
        }
        else
        {
            //        qDebug() << "Volume per tick update error: !"
            //                 << update_volume_per_tick_query.lastError();
            return false;
        }
    }
}

bool DbManager::updateFullVolume(int slot, double new_full_volume)
{
    qDebug() << " db... updateFullVolume";
    QSqlQuery update_full_volume_query;
    {

        update_full_volume_query.prepare("UPDATE products SET volume_full=:new_full_volume WHERE slot=:slot");
        update_full_volume_query.bindValue(":new_full_volume", new_full_volume);
        update_full_volume_query.bindValue(":slot", slot);
        if (update_full_volume_query.exec())
        {
            qDebug() << "Full volume updated successfully!";
            return true;
        }
        else
        {
            qDebug() << "Full volume update error: !"
                     << update_full_volume_query.lastError().text();
            qDebug() << update_full_volume_query.lastQuery();
            return false;
        }
    }
}

bool DbManager::updatePWM(int slot, int new_pwm)
{
    qDebug() << " db... updatePWM";
    QSqlQuery pwm_query;
    {

        pwm_query.prepare("UPDATE products SET dispense_speed=:new_pwm WHERE slot=:slot");
        pwm_query.bindValue(":new_pwm", new_pwm);
        pwm_query.bindValue(":slot", slot);

        if (pwm_query.exec())
        {
            //        qDebug() << "PWM updated successfully!";
            return true;
        }
        else
        {
            //        qDebug() << "PWM update error: !"
            //                 << pwm_query.lastError();
            return false;
        }
    }
}

bool DbManager::updateBuffer(int slot, double new_buffer)
{
    qDebug() << " db... updateBuffer";
    QSqlQuery buffer_query;

    {
        buffer_query.prepare("UPDATE products SET buffer=:new_buffer WHERE slot=:slot");
        buffer_query.bindValue(":new_buffer", new_buffer);
        buffer_query.bindValue(":slot", slot);

        if (buffer_query.exec())
        {
            //        qDebug() << "Buffer updated successfully!";
            return true;
        }
        else
        {
            //        qDebug() << "Buffer update error: !"
            //                 << buffer_query.lastError();
            return false;
        }
    }
}

QString DbManager::getPLU(int slot, char size)
{
    qDebug() << " db... getPLU";
    QSqlQuery plu_query;
    QString plu_smalltring;
    {

        if (size == 's')
        {
            plu_query.prepare("SELECT PLU_small FROM products WHERE slot=:slot");
        }
        else if (size == 'm')
        {
            plu_query.prepare("SELECT PLU_medium FROM products WHERE slot=:slot");
        }
        else if (size == 'l')
        {
            plu_query.prepare("SELECT PLU_large FROM products WHERE slot=:slot");
        }
        else if (size == 'c')
        {
            plu_query.prepare("SELECT PLU_custom FROM products WHERE slot=:slot");
        }

        plu_query.bindValue(":slot", slot);
        plu_query.exec();

        while (plu_query.next())
        {
            plu_smalltring = plu_query.value(0).toString();
        }
    }
    return plu_smalltring;
}

bool DbManager::updatePluSmall(int slot, QString new_plu)
{
    qDebug() << " db... updatePluSmall";
    QSqlQuery plu_query;
    {
        plu_query.prepare("UPDATE products SET PLU_small=:new_plu WHERE slot=:slot");
        plu_query.bindValue(":new_plu", new_plu);
        plu_query.bindValue(":slot", slot);

        if (plu_query.exec())
        {
            //        qDebug() << "PLU updated successfully!";
            return true;
        }
        else
        {
            //        qDebug() << "PLU update error: !"
            //                 << plu_query.lastError();
            return false;
        }
    }
}

bool DbManager::updatePluLarge(int slot, QString new_plu)
{
    qDebug() << " db... updatePluLarge";
    QSqlQuery plu_query;

    {

        plu_query.prepare("UPDATE products SET PLU_large=:new_plu WHERE slot=:slot");
        plu_query.bindValue(":new_plu", new_plu);
        plu_query.bindValue(":slot", slot);

        if (plu_query.exec())
        {
            //        qDebug() << "PLU updated successfully!";
            return true;
        }
        else
        {
            //        qDebug() << "PLU update error: !"
            //                 << plu_query.lastError();
            return false;
        }
    }
}

void DbManager::printerStatus(bool *isOnline, bool *hasPaper)
{
    QSqlQuery qry;
    // bool is_online = false;
    // bool has_paper = false;

    {
        qry.prepare("SELECT receipt_printer_is_online,receipt_printer_has_paper FROM machine");
        qry.exec();

        while (qry.next())
        {
            *isOnline = (qry.value(0).toInt() == 1);
            *hasPaper = (qry.value(1).toInt() == 1);
        }
    }
}

bool DbManager::hasReceiptPrinter()
{
    QSqlQuery qry;
    bool is_enabled;
    {
        qry.prepare("SELECT has_receipt_printer FROM machine");
        qry.exec();

        while (qry.next())
        {
            is_enabled = (qry.value(0).toInt() == 1);
        }
    }
    return is_enabled;
}

QString DbManager::getMachineID()
{
    QSqlQuery mid_query;
    QString mid_string;

    {
        mid_query.prepare("SELECT machine_id FROM machine");
        mid_query.exec();

        while (mid_query.next())
        {
            mid_string = mid_query.value(0).toString();
        }
    }
    return mid_string;
}
QString DbManager::getMaintenanceAdminPassword()
{
    QSqlQuery mid_query;
    QString mid_string;

    {
        mid_query.prepare("SELECT maintenance_pwd FROM machine");
        mid_query.exec();

        while (mid_query.next())
        {
            mid_string = mid_query.value(0).toString();
        }
    }
    return mid_string;
}

QString DbManager::getHelpPageHtmlText()
{
    QSqlQuery mid_query;
    QString mid_string;

    {
        mid_query.prepare("SELECT help_text_html FROM machine");
        mid_query.exec();

        while (mid_query.next())
        {
            mid_string = mid_query.value(0).toString();
        }
    }
    return mid_string;
}

QString DbManager::getSizeSmall(int slot)
{
    qDebug() << " db... getSizeSmall";
    QSqlQuery small_vol_query;
    QString small_vol_string;
    {

        small_vol_query.prepare("SELECT size_small FROM products WHERE slot=:slot");

        small_vol_query.bindValue(":slot", slot);
        small_vol_query.exec();

        while (small_vol_query.next())
        {
            small_vol_string = small_vol_query.value(0).toString();

            // qDebug() << "Product: " << product_name << endl;
        }
    }
    return small_vol_string;
}

QString DbManager::getSizeMedium(int slot)
{
    qDebug() << " db... getSizeMedium";
    QSqlQuery medium_vol_query;
    QString medium_vol_string;
    {

        medium_vol_query.prepare("SELECT size_medium FROM products WHERE slot=:slot");

        medium_vol_query.bindValue(":slot", slot);
        medium_vol_query.exec();

        while (medium_vol_query.next())
        {
            medium_vol_string = medium_vol_query.value(0).toString();

            // qDebug() << "Product: " << product_name << endl;
        }
    }
    return medium_vol_string;
}

QString DbManager::getSizeLarge(int slot)
{
    qDebug() << " db... getSizeLarge";
    QSqlQuery large_vol_query;
    QString large_vol_string;
    {

        large_vol_query.prepare("SELECT size_large FROM products WHERE slot=:slot");

        large_vol_query.bindValue(":slot", slot);
        large_vol_query.exec();

        while (large_vol_query.next())
        {
            large_vol_string = large_vol_query.value(0).toString();

            // qDebug() << "Product: " << product_name << endl;
        }
    }
    return large_vol_string;
}

QString DbManager::getPriceSmall(int slot)
{
    qDebug() << " db... getPriceSmall";
    QSqlQuery small_price_query;
    QString small_price_string;
    {

        small_price_query.prepare("SELECT price_small FROM products WHERE slot=:slot");

        small_price_query.bindValue(":slot", slot);
        small_price_query.exec();

        while (small_price_query.next())
        {
            small_price_string = small_price_query.value(0).toString();

            // qDebug() << "Product: " << product_name << endl;
        }
    }
    return small_price_string;
}

QString DbManager::getPriceMedium(int slot)
{
    qDebug() << " db... getPriceMedium";
    QSqlQuery medium_price_query;
    QString medium_price_string;
    {

        medium_price_query.prepare("SELECT price_medium FROM products WHERE slot=:slot");

        medium_price_query.bindValue(":slot", slot);
        medium_price_query.exec();

        while (medium_price_query.next())
        {
            medium_price_string = medium_price_query.value(0).toString();
        }
    }
    return medium_price_string;
}

QString DbManager::getPriceLarge(int slot)
{
    qDebug() << " db... getPriceLarge";
    QSqlQuery large_price_query;
    QString large_price_string;
    {

        large_price_query.prepare("SELECT price_large FROM products WHERE slot=:slot");

        large_price_query.bindValue(":slot", slot);
        large_price_query.exec();

        while (large_price_query.next())
        {
            large_price_string = large_price_query.value(0).toString();
        }
    }
    return large_price_string;
}

QString DbManager::getIdlePageType()
{
    QSqlQuery query;
    QString idlePageType;

    if (query.exec("SELECT idle_page_type FROM machine"))
    {
        if (query.first())
        {
            idlePageType = query.value(0).toString();
        }
    }
    else
    {
        qWarning() << "Failed to get idle_page_type from database:" << query.lastError().text();
    }

    return idlePageType;
}

QString DbManager::getProductType(int slot)
{
    QSqlQuery product_type_query;
    QString product_type_string;

    {

        product_type_query.prepare("SELECT type FROM products WHERE slot=:slot");
        product_type_query.bindValue(":slot", slot);
        product_type_query.exec();

        while (product_type_query.next())
        {
            product_type_string = product_type_query.value(0).toString();
        }
    }
    return product_type_string;
}

bool DbManager::showTransactions()
{
    QSqlQuery qry;
    bool is_enabled;

    {
        qry.prepare("SELECT show_transactions FROM machine");
        qry.exec();

        while (qry.next())
        {
            is_enabled = (qry.value(0).toInt() == 1);
        }
    }
    return is_enabled;
}

QString DbManager::getProductID(int slot)
{
    QSqlQuery product_id_query;
    QString product_id_string;

    {

        product_id_query.prepare("SELECT productId FROM products WHERE slot=:slot");
        product_id_query.bindValue(":slot", slot);
        product_id_query.exec();

        while (product_id_query.next())
        {
            product_id_string = product_id_query.value(0).toString();

            // qDebug() << "Product: " << product_name << endl;
        }
    }
    return product_id_string;
}

QString DbManager::getUnits(int slot)
{
    qDebug() << " db... getUnits";
    QSqlQuery units_query;
    QString units_string;
    {
        units_query.prepare("SELECT size_unit FROM products WHERE slot=:slot");
        units_query.bindValue(":slot", slot);
        units_query.exec();

        while (units_query.next())
        {
            units_string = units_query.value(0).toString();

            // qDebug() << "Product: " << product_name << endl;
        }
    }
    return units_string;
}

int DbManager::getLastTransactionIdFromDb()
{
    qDebug() << " db... getLastTransactionIdFromDb" << endl;
    QSqlQuery lastTransactionIdQuery;
    int lastTransactionId = 0;

    {

        lastTransactionIdQuery.prepare("SELECT * FROM transactions ORDER BY id DESC LIMIT 1");
        lastTransactionIdQuery.exec();

        while (lastTransactionIdQuery.next())
        {
            lastTransactionId = lastTransactionIdQuery.value(0).toInt();
            ;
        }
    }
    qDebug() << " Last transaction :" << lastTransactionId << endl;

    return lastTransactionId;
}
