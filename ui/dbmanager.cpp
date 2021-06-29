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

QString DbManager::getProductReceiptName(int slot){

    QSqlQuery product_query;
    QString product_name;

    product_query.prepare("SELECT name_receipt FROM products WHERE slot=:slot");
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

//double DbManager::getProductTargetVolume(int slot){

//    QSqlQuery volume_query;
//    double volume;

//    volume_query.prepare("SELECT volume_target FROM products WHERE slot=:slot");
//    volume_query.bindValue(":slot", slot);
//    volume_query.exec();

//    while (volume_query.next()) {
//            volume = volume_query.value(0).toDouble();

//            //qDebug() << "Volume: " << volume << endl;
//        }

//    return volume;
//}

double DbManager::getProductVolumePerTick(int slot){

    QSqlQuery vol_per_tick_query;
    double vol_per_tick;

    vol_per_tick_query.prepare("SELECT volume_per_tick FROM products WHERE slot=:slot");
    vol_per_tick_query.bindValue(":slot", slot);
    vol_per_tick_query.exec();

    while (vol_per_tick_query.next()) {
            vol_per_tick = vol_per_tick_query.value(0).toDouble();

            //qDebug() << "Volume Per Tick: " << vol_per_tick << endl;
        }

    return vol_per_tick;
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

double DbManager::getFullProduct(int slot){

    QSqlQuery full_query;
    double full;

    full_query.prepare("SELECT full_ml FROM products WHERE slot=:slot");
    full_query.bindValue(":slot", slot);
    full_query.exec();

    while (full_query.next()) {
            full = full_query.value(0).toDouble();

        }

    return full;
}

QString DbManager::getPaymentMethod(int slot){

    QSqlQuery paymeny_query;
    QString payment_method;

    paymeny_query.prepare("SELECT payment FROM products WHERE slot=:slot");
    paymeny_query.bindValue(":slot", slot);
    paymeny_query.exec();

    while (paymeny_query.next()) {
            payment_method = paymeny_query.value(0).toString();

            //qDebug() << "Product: " << product_name << endl;
        }

    return payment_method;
}

bool DbManager::checkLevels(int slot){
    QSqlQuery level_query;
    double level;

    level_query.prepare("SELECT remaining_ml FROM products where slot=:slot");
    level_query.bindValue(":slot", slot);
    level_query.exec();

    while (level_query.next()) {
            level = level_query.value(0).toDouble();
            if (level > getProductVolume(slot, 'l')){
                return true;
            }
            else{
                //emailEmpty(slot);
                return false;
            }
        }
}

bool DbManager::refill(int slot){
    QSqlQuery refill_query;
    bool success=false;

    refill_query.prepare("UPDATE products SET remaining_ml=full_ml WHERE slot=:slot");
    refill_query.bindValue(":slot", slot);
//    refill_query.bindValue(":remaining", remaining);
    if(refill_query.exec())
    {
        qDebug() << "remaining ml updated successfully!";
        refill_query.prepare("UPDATE products SET total_dispensed=0 WHERE slot=:slot");
        refill_query.bindValue(":slot", slot);
        if(refill_query.exec()){
            qDebug() << "total ml dispensed update successful!";

            QSqlQuery refill_date;
            refill_date.prepare("UPDATE products SET last_refill=:time WHERE slot=:slot");
            refill_date.bindValue(":slot", slot);
            refill_date.bindValue(":time", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

            if (refill_date.exec()){
                success=true;
            }else{
                qDebug() << "refill date error:"
                         << refill_date.lastError();
                success=false;
            }

        }
        else{
            qDebug() << "total ml dispensed update error:"
                     << refill_query.lastError();
            success=false;
        }
    }
    else
    {
        qDebug() << "remaining ml update error:"
                 << refill_query.lastError();
        success=false;
    }

    return success;
}

bool DbManager::sellout(int slot){
    QSqlQuery sellout_query;
    bool success=false;
    double remaining = getFullProduct(slot);

    sellout_query.prepare("UPDATE products SET remaining_ml=0 WHERE slot=:slot");
    sellout_query.bindValue(":slot", slot);
//    sellout_query.bindValue(":remaining", remaining);
    if(sellout_query.exec())
    {
        qDebug() << "remaining ml updated successfully!";
        sellout_query.prepare("UPDATE products SET total_dispensed=full_ml WHERE slot=:slot");
        sellout_query.bindValue(":slot", slot);
        if(sellout_query.exec()){
            qDebug() << "total ml dispensed update successful!";
            success=true;
        }
        else{
            qDebug() << "total ml dispensed update error:"
                     << sellout_query.lastError();
            success=false;
        }
    }
    else
    {
        qDebug() << "remaining ml update error:"
                 << sellout_query.lastError();
        success=false;
    }

    return success;
}

void DbManager::emailEmpty(int slot){
    QString mt_product = getProductName(slot);
    QString email_subject = mt_product + " has sold out!";
    QString email_body = mt_product + " has sold out";
    QString email_recipients = "paddy@drinkfill.com";
    QString email = "echo '" + email_body + "' | mail -s '" + email_subject +"' -a 'From: Stongs Soapstand <hello@drinkfill.com>' " + email_recipients + " | screen -d -m";

    //qDebug() << email.toStdString().c_str();

    system(email.toStdString().c_str());
}

int DbManager::getTotalTransactions(){
    QSqlQuery transaction_query;
    double transactions;

    transaction_query.prepare("SELECT COUNT(*) FROM transactions;");
    transaction_query.exec();

    while (transaction_query.next()) {
            transactions = transaction_query.value(0).toInt();

        }

    return transactions;
}

int DbManager::getNumberOfProducts(){
    QSqlQuery products_query;
    int products;

    products_query.prepare("SELECT COUNT(*) FROM products;");
    products_query.exec();

    while (products_query.next()) {
            products = products_query.value(0).toInt();

        }

    return products;
}

double DbManager::getTotalDispensed(int slot){
    QSqlQuery dispensed_query;
    double dispensed;

    dispensed_query.prepare("SELECT total_dispensed FROM products WHERE slot=:slot");
    dispensed_query.bindValue(":slot", slot);
    dispensed_query.exec();

    while (dispensed_query.next()) {
            dispensed = dispensed_query.value(0).toDouble();

        }

    return dispensed;
}

double DbManager::getRemaining(int slot){
    QSqlQuery remaining_query;
    double remaining;

    remaining_query.prepare("SELECT remaining_ml FROM products WHERE slot=:slot");
    remaining_query.bindValue(":slot", slot);
    remaining_query.exec();

    while (remaining_query.next()) {
            remaining = remaining_query.value(0).toDouble();

        }

    return remaining;
}

QString DbManager::getLastRefill(int slot){
    QSqlQuery refill_date_query;
    QString refill_date_string;

    refill_date_query.prepare("SELECT last_refill FROM products WHERE slot=:slot");
    refill_date_query.bindValue(":slot", slot);
    refill_date_query.exec();

    while (refill_date_query.next()) {
            refill_date_string = refill_date_query.value(0).toString();

            //qDebug() << "Product: " << product_name << endl;
        }

    return refill_date_string;
}
