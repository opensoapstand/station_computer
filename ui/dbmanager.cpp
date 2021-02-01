#include "dbmanager.h"

DbManager::DbManager(const QString& path)
{
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (!m_db.open())
    {
        qDebug() << "Error: connection with database failed";
    }
    else
    {
        qDebug() << "Database: connection ok";
    }
}

bool DbManager::addPageClick(const QString& page){

    bool success = false;
    // you should check if args are ok first...
    QSqlQuery click_query;
    click_query.prepare("INSERT INTO clicks (time, page_info) VALUES (:time, :page);");
    click_query.bindValue(":time", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    click_query.bindValue(":page", page);
    if(click_query.exec())
    {
        success = true;
        qDebug() << "addPageClick success!";
    }
    else
    {
        qDebug() << "addPageClick error:"
                 << click_query.lastError();
    }

    return success;
}

QString DbManager::getProductName(int slot){

    QSqlQuery product_query;
    QString product_name;

    product_query.prepare("SELECT name FROM products WHERE slot=:slot");
    product_query.bindValue(":slot", slot);
    product_query.exec();

    while (product_query.next()) {
            product_name = product_query.value(0).toString();

            //qDebug() << "Product: " << product_name << endl;
        }

    return product_name;
}

double DbManager::getProductPrice(int slot, char ml){

    QSqlQuery price_query;
    double price;

    if (ml == 'l'){
        price_query.prepare("SELECT price_l FROM products WHERE slot=:slot");
    }
    else if (ml == 's'){
        price_query.prepare("SELECT price_s FROM products WHERE slot=:slot");
    }
    //price_query.prepare("SELECT price FROM products WHERE slot=:slot");
    price_query.bindValue(":slot", slot);
    price_query.exec();

    while (price_query.next()) {
            price = price_query.value(0).toDouble();

            //qDebug() << "Price: " << price << endl;
        }

    return price;
}

double DbManager::getProductVolume(int slot, char ml){

    QSqlQuery volume_query;
    double volume;

    if (ml == 'l'){
        volume_query.prepare("SELECT volume_target_l FROM products WHERE slot=:slot");
    }
    else if (ml == 's'){
        volume_query.prepare("SELECT volume_target_s FROM products WHERE slot=:slot");
    }
    //volume_query.prepare("SELECT volume_target FROM products WHERE slot=:slot");
    volume_query.bindValue(":slot", slot);
    volume_query.exec();

    while (volume_query.next()) {
            volume = volume_query.value(0).toDouble();

            //qDebug() << "Volume: " << volume << endl;
        }

    return volume;
}

bool DbManager::checkLevels(int slot){
    QSqlQuery level_query;
    double level;

    level_query.prepare("SELECT remaining_ml FROM products where slot=:slot");
    level_query.bindValue(":slot", slot);
    level_query.exec();

    while (level_query.next()) {
            level = level_query.value(0).toDouble();
            if (level > 0){
                return true;
            }
            else{
                return false;
            }
        }
}
