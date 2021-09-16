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
//        sellout_query.prepare("UPDATE products SET total_dispensed=full_ml WHERE slot=:slot");
//        sellout_query.bindValue(":slot", slot);
//        if(sellout_query.exec()){
//            qDebug() << "total ml dispensed update successful!";
        success=true;
        }


    else
    {
        qDebug() << "remaining ml update error:"
                 << sellout_query.lastError();
        success=false;
    }

    return success;
}

bool DbManager::unsellout(int slot){
    QSqlQuery sellout_query;
    bool success=false;

    sellout_query.prepare("UPDATE products SET remaining_ml=full_ml-total_dispensed WHERE slot=:slot");
    sellout_query.bindValue(":slot", slot);
    if(sellout_query.exec())
    {
        qDebug() << "remaining ml updated successfully!";
        success=true;
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

double DbManager::getTemperature(){
    QSqlQuery temperature_query;
    double temperature;

    temperature_query.prepare("SELECT temp FROM temperature LIMIT 1 offset cast((SELECT count(*) FROM temperature) AS INT) - 1");
    temperature_query.exec();
    while (temperature_query.next()){
        temperature = temperature_query.value(0).toDouble();
    }
    return temperature;
}

int DbManager::getPWM(int slot){
    QSqlQuery pwm_query;
    double pwm;

    pwm_query.prepare("SELECT pwm FROM products WHERE slot=:slot");
    pwm_query.bindValue(":slot", slot);
    pwm_query.exec();

    while (pwm_query.next()) {
            pwm = pwm_query.value(0).toInt();

        }

    return pwm;
}

double DbManager::getBuffer(int slot){
    QSqlQuery buffer_query;
    double buffer;

    buffer_query.prepare("SELECT buffer FROM products WHERE slot=:slot");
    buffer_query.bindValue(":slot", slot);
    buffer_query.exec();
    while (buffer_query.next()){
        buffer = buffer_query.value(0).toDouble();
    }
    return buffer;
}

bool DbManager::updatePaymentsDb(QString date,QString time, QString txnType, QString amount, QString cardNo, QString refNo, QString authNo, QString cardType, QString status, QString isoCode, QString hostCode, QString tvr){
    QSqlQuery payments_query;
    bool success;

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

    if(payments_query.exec())
    {
        success = true;
        qDebug() << "Payment Database updated successfully!";
    }
    else
    {
        qDebug() << "Payment Database update error:"
                 << payments_query.lastError();
    }

    return success;
}

bool DbManager::updatePrice_s(int slot, double new_price){
    QSqlQuery update_price_query;

    update_price_query.prepare("UPDATE products SET price_s = :new_price WHERE slot = :slot");
    update_price_query.bindValue(":new_price", new_price);
    update_price_query.bindValue(":slot", slot);

    if(update_price_query.exec()){
        qDebug() << "Price updated successfully!";
        return true;
    }else{
        qDebug() << "Price update error: !"
                 << update_price_query.lastQuery()
                 << update_price_query.lastError();
        return false;
    }
}

bool DbManager::updatePrice_l(int slot, double new_price){
    QSqlQuery update_price_query;

    update_price_query.prepare("UPDATE products SET price_l = :new_price WHERE slot = :slot");
    update_price_query.bindValue(":new_price", new_price);
    update_price_query.bindValue(":slot", slot);

    if(update_price_query.exec()){
        qDebug() << "Price updated successfully!";
        return true;
    }else{
        qDebug() << "Price update error: !"
                 << update_price_query.lastError();
        return false;
    }
}

bool DbManager::updateTargetVolume_s(int slot, double new_volume){
    QSqlQuery update_target_volume_query;

    update_target_volume_query.prepare("UPDATE products SET volume_target_s=:new_volume WHERE slot=:slot");
    update_target_volume_query.bindValue(":new_volume", new_volume);
    update_target_volume_query.bindValue(":slot", slot);

    if(update_target_volume_query.exec()){
        qDebug() << "Target Volume updated successfully!";
        return true;
    }else{
        qDebug() << "Target volume update error: !"
                 << update_target_volume_query.lastError();
        return false;
    }

}

bool DbManager::updateTargetVolume_l(int slot, double new_volume){
    QSqlQuery update_target_volume_query;

    update_target_volume_query.prepare("UPDATE products SET volume_target_l=:new_volume WHERE slot=:slot");
    update_target_volume_query.bindValue(":new_volume", new_volume);
    update_target_volume_query.bindValue(":slot", slot);

    if(update_target_volume_query.exec()){
        qDebug() << "Target Volume updated successfully!";
        return true;
    }else{
        qDebug() << "Target volume update error: !"
                 << update_target_volume_query.lastError();
        return false;
    }

}

bool DbManager::updateVolumePerTick(int slot, double new_volume_per_tick){
    QSqlQuery update_volume_per_tick_query;

    update_volume_per_tick_query.prepare("UPDATE products SET volume_per_tick=:new_volume_per_tick WHERE slot=:slot");
    update_volume_per_tick_query.bindValue(":new_volume_per_tick", new_volume_per_tick);
    update_volume_per_tick_query.bindValue(":slot", slot);

    if(update_volume_per_tick_query.exec()){
        qDebug() << "Volume per tick updated successfully!";
        return true;
    }else{
        qDebug() << "Volume per tick update error: !"
                 << update_volume_per_tick_query.lastError();
        return false;
    }
}

bool DbManager::updateFullVolume(int slot, double new_full_volume){
    QSqlQuery update_full_volume_query;

    update_full_volume_query.prepare("UPDATE products SET full_ml=:new_full_volume WHERE slot=:slot");
    update_full_volume_query.bindValue(":new_full_volume", new_full_volume);
    update_full_volume_query.bindValue(":slot", slot);

    if(update_full_volume_query.exec()){
        qDebug() << "Full volume updated successfully!";
        return true;
    }else{
        qDebug() << "Full volume update error: !"
                 << update_full_volume_query.lastError();
        return false;
    }
}

bool DbManager::updatePWM(int slot, int new_pwm){
    QSqlQuery pwm_query;

    pwm_query.prepare("UPDATE products SET pwm=:new_pwm WHERE slot=:slot");
    pwm_query.bindValue(":new_pwm", new_pwm);
    pwm_query.bindValue(":slot", slot);

    if(pwm_query.exec()){
        qDebug() << "PWM updated successfully!";
        return true;
    }else{
        qDebug() << "PWM update error: !"
                 << pwm_query.lastError();
        return false;
    }
}

bool DbManager::updateBuffer(int slot, double new_buffer){
    QSqlQuery buffer_query;

    buffer_query.prepare("UPDATE products SET buffer=:new_buffer WHERE slot=:slot");
    buffer_query.bindValue(":new_buffer", new_buffer);
    buffer_query.bindValue(":slot", slot);

    if(buffer_query.exec()){
        qDebug() << "Buffer updated successfully!";
        return true;
    }else{
        qDebug() << "Buffer update error: !"
                 << buffer_query.lastError();
        return false;
    }
}

QString DbManager::getPLU(int slot, char size){
    QSqlQuery plu_query;
    QString plu_string;

    if (size == 's'){
        plu_query.prepare("SELECT PLU_s FROM products WHERE slot=:slot");
    }else if (size == 'l'){
        plu_query.prepare("SELECT PLU_l FROM products WHERE slot=:slot");
    }
    plu_query.bindValue(":slot", slot);
    plu_query.exec();

    while (plu_query.next()) {
            plu_string = plu_query.value(0).toString();
        }

    return plu_string;
}

bool DbManager::updatePLU_s(int slot, QString new_plu){
    QSqlQuery plu_query;

    plu_query.prepare("UPDATE products SET PLU_s=:new_plu WHERE slot=:slot");
    plu_query.bindValue(":new_plu", new_plu);
    plu_query.bindValue(":slot", slot);

    if(plu_query.exec()){
        qDebug() << "PLU updated successfully!";
        return true;
    }else{
        qDebug() << "PLU update error: !"
                 << plu_query.lastError();
        return false;
    }
}

bool DbManager::updatePLU_l(int slot, QString new_plu){
    QSqlQuery plu_query;

    plu_query.prepare("UPDATE products SET PLU_l=:new_plu WHERE slot=:slot");
    plu_query.bindValue(":new_plu", new_plu);
    plu_query.bindValue(":slot", slot);

    if(plu_query.exec()){
        qDebug() << "PLU updated successfully!";
        return true;
    }else{
        qDebug() << "PLU update error: !"
                 << plu_query.lastError();
        return false;
    }
}

QString DbManager::getMachineID(){
    QSqlQuery mid_query;
    QString mid_string;

    mid_query.prepare("SELECT machine_id FROM machine");
    mid_query.exec();

    while (mid_query.next()) {
            mid_string = mid_query.value(0).toString();
        }

    return mid_string;

}
