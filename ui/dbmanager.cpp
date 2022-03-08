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
#include "df_util.h"  // lode added for settings

DbManager::DbManager(const QString& path)
{
//    qDebug() << "CREATING DB OBJECT" << endl;
    if (m_db.isOpen()){
        m_db.close();
        m_db = QSqlDatabase();
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
        qDebug() << "m_db was already open. Closed it first." << endl;
    }

    if (m_db.connectionName().isEmpty()){
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName(path);
        qDebug() << "m_db set connectionName" << endl;
    }else{
        qDebug() << "m_db connectionName is NOT EMPTY" << endl;
    }

    if (!m_db.open())
    {
        qDebug() << "Error: connection with database failed";
    }
    else
    {
        qDebug() << "Database: connection ok";
    }
}

// DTOR
DbManager::~DbManager(){

    if (m_db.isOpen()){
        m_db.close();
        m_db = QSqlDatabase();
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }
}

bool DbManager::addPageClick(const QString& page){

    return true;
}

void DbManager::closeDB(){
    if (m_db.isOpen()){
        m_db.close();
        m_db = QSqlDatabase();
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }
}

QString DbManager::getProductName(int slot){

    QSqlQuery product_query;
    QString product_name;

    {

    product_query.prepare("SELECT name FROM products WHERE slot=:slot");
    product_query.bindValue(":slot", slot);
    product_query.exec();

    while (product_query.next()) {
            product_name = product_query.value(0).toString();
        }

    }

    return product_name;
}

QString DbManager::getProductReceiptName(int slot){

    QSqlQuery product_query;
    QString product_name;

    {

    product_query.prepare("SELECT name_receipt FROM products WHERE slot=:slot");
    product_query.bindValue(":slot", slot);
    product_query.exec();

    while (product_query.next()) {
            product_name = product_query.value(0).toString();
        }

    }

    return product_name;
}

double DbManager::getProductPrice(int slot, char ml){

    QSqlQuery price_query;
    double price;

    {

#ifdef USE_OLD_DATABASE
    if (ml == 'l'){
        price_query.prepare("SELECT price_l FROM products WHERE slot=:slot");
    }
    else if (ml == 's'){
        price_query.prepare("SELECT price_s FROM products WHERE slot=:slot");
    }
#else
    if (ml == 'l'){
        price_query.prepare("SELECT price_large FROM products WHERE slot=:slot");
    }
    else if (ml == 's'){
        price_query.prepare("SELECT price_small FROM products WHERE slot=:slot");
    }
#endif

    price_query.bindValue(":slot", slot);
    price_query.exec();

    while (price_query.next()) {
            price = price_query.value(0).toDouble();

        }
    }

    return price;
}


double DbManager::getProductVolumePerTick(int slot){

    QSqlQuery vol_per_tick_query;
    double vol_per_tick;

    {

    vol_per_tick_query.prepare("SELECT volume_per_tick FROM products WHERE slot=:slot");
    vol_per_tick_query.bindValue(":slot", slot);
    vol_per_tick_query.exec();

    while (vol_per_tick_query.next()) {
            vol_per_tick = vol_per_tick_query.value(0).toDouble();

        }
    }

    return vol_per_tick;
}

double DbManager::getProductVolume(int slot, char size){

    QSqlQuery volume_query;
    double volume;

    
#ifdef USE_OLD_DATABASE
    if (size == 'l'){
        volume_query.prepare("SELECT volume_target_l FROM products WHERE slot=:slot");
    }
    // else if (size == 'm'){
    //     volume_query.prepare("SELECT volume_medium FROM products WHERE slot=:slot");
    // }
    else if (size == 's'){
        volume_query.prepare("SELECT volume_target_s FROM products WHERE slot=:slot");
    }
    else if (size == 'c'){
        volume_query.prepare("SELECT volume_target_l FROM products WHERE slot=:slot");
    }
#else
    if (size == 'l'){
        volume_query.prepare("SELECT volume_large FROM products WHERE slot=:slot");
    }
    else if (size == 'm'){
        volume_query.prepare("SELECT volume_medium FROM products WHERE slot=:slot");
    }
    else if (size == 's'){
        volume_query.prepare("SELECT volume_small FROM products WHERE slot=:slot");
    }
    else if (size == 'c'){
        volume_query.prepare("SELECT volume_custom FROM products WHERE slot=:slot");
    }

#endif 

    volume_query.bindValue(":slot", slot);
    volume_query.exec();

    while (volume_query.next()) {
        volume = volume_query.value(0).toDouble();
    }
    

    return volume;
}

double DbManager::getFullProduct(int slot){

    QSqlQuery full_query;
    double full;

#ifdef USE_OLD_DATABASE
    full_query.prepare("SELECT full_ml FROM products WHERE slot=:slot");
#else
    full_query.prepare("SELECT volume_full FROM products WHERE slot=:slot");
#endif

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

    {

    paymeny_query.prepare("SELECT payment FROM products WHERE slot=:slot");
    paymeny_query.bindValue(":slot", slot);
    paymeny_query.exec();

    while (paymeny_query.next()) {
            payment_method = paymeny_query.value(0).toString();
        }
    }
    return payment_method;
}


uint32_t DbManager::getNumberOfRows(QString table){
    //qDebug() << "rows: " << getNumberOfRows("products") << endl;
    QString qry = "SELECT COUNT(*) FROM ";
    qry.append(table);

    QSqlQuery query;
    uint32_t row_count = 0;

    query.exec(qry);

    if(query.first()){
        row_count = query.value(0).toInt();
    }
    return row_count;
}


bool DbManager::remainingVolumeIsBiggerThanLargestFixedSize(int slot){
    QSqlQuery level_query;
    double level;
    
#ifdef USE_OLD_DATABASE
    level_query.prepare("SELECT remaining_ml FROM products where slot=:slot");

#else
    level_query.prepare("SELECT volume_remaining FROM products where slot=:slot");
#endif 

    level_query.bindValue(":slot", slot);
    level_query.exec();
    
    if( !level_query.exec() )
    {
        qDebug() << "ERROR: SQL query not successful: " << level_query.lastError() << endl;
        return false;
    }

    // if(level_query.first()){
    //     // takes first row
    //     qDebug() << "aeijfaijf" << level_query.value(0) << endl;
    // }

// CHECK IF things still work if first is skipped....
    while (level_query.next()) {

        level = level_query.value(0).toDouble();


        if (level > getProductVolume(slot, 'l'))
        {
            return true;
        }
        else
        {
            qDebug() << level << "Remaining volume lower than biggest dispense size."<< endl;
            return false;
        }
    }

}


bool DbManager::refill(int slot){
    QSqlQuery refill_query;
    bool success=false;

#ifdef USE_OLD_DATABASE
    QString sql_set_vol = "UPDATE products SET remaining_ml=full_ml WHERE slot=:slot";
    QString sql_res_disp = "UPDATE products SET total_dispensed=0 WHERE slot=:slot";
    QString sql_set_time = "UPDATE products SET last_refill=:time WHERE slot=:slot";
    
#else
    QString sql_set_vol = "UPDATE products SET volume_remaining=volume_full WHERE slot=:slot";
    QString sql_res_disp = "UPDATE products SET volume_dispensed_total=0 WHERE slot=:slot";
    QString sql_set_time = "UPDATE products SET last_restock=:time WHERE slot=:slot";
    
#endif
    
    refill_query.prepare(sql_set_vol);
    refill_query.bindValue(":slot", slot);
    if(refill_query.exec())
    {
        refill_query.prepare(sql_res_disp);
        refill_query.bindValue(":slot", slot);
        if(refill_query.exec()){
            QSqlQuery refill_date;
            refill_date.prepare(sql_set_time);
            refill_date.bindValue(":slot", slot);
            refill_date.bindValue(":time", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

            if (refill_date.exec()){
                qDebug() << "Restock success. slot:"<< QString::number(slot) 
                    << " volume:" << QString::number(getVolumeRemaining(slot))
                    ;
        
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
//    double remaining = getFullProduct(slot);
    {

#ifdef USE_OLD_DATABASE
    sellout_query.prepare("UPDATE products SET remaining_ml=0 WHERE slot=:slot");
#else
    sellout_query.prepare("UPDATE products SET volume_remaining=0 WHERE slot=:slot");
#endif
    sellout_query.bindValue(":slot", slot);
    if(sellout_query.exec())
    {
        success=true;
        }

    else
    {
       qDebug() << "remaining ml update error:"
                << sellout_query.lastError();
        success=false;
    }
    }

    return success;
}

bool DbManager::unsellout(int slot){
    QSqlQuery sellout_query;
    bool success=false;
    {

#ifdef USE_OLD_DATABASE
    sellout_query.prepare("UPDATE products SET remaining_ml=full_ml-total_dispensed WHERE slot=:slot");
    #else
    sellout_query.prepare("UPDATE products SET volume_remaining=volume_full-volume_dispensed_total WHERE slot=:slot");
#endif
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

    }
    return success;
}

void DbManager::emailEmpty(int slot){
    QString mt_product = getProductName(slot);
    QString email_subject = mt_product + " has sold out!";
    QString email_body = mt_product + " has sold out";
    QString email_recipients = "paddy@drinkfill.com";
    QString email = "echo '" + email_body + "' | mail -s '" + email_subject +"' -a 'From: Stongs Soapstand <hello@drinkfill.com>' " + email_recipients + " | screen -d -m";

    system(email.toStdString().c_str());
}

int DbManager::getTotalTransactions(){
    QSqlQuery transaction_query;
    double transactions;

    {
    transaction_query.prepare("SELECT COUNT(*) FROM transactions;");
    transaction_query.exec();

    while (transaction_query.next()) {
            transactions = transaction_query.value(0).toInt();

        }
    }

    return transactions;
}

int DbManager::getNumberOfProducts(){
    QSqlQuery products_query;
    int products;

    {
    products_query.prepare("SELECT COUNT(*) FROM products;");
    products_query.exec();

    while (products_query.next()) {
            products = products_query.value(0).toInt();

        }
    }
    return products;
}

double DbManager::getTotalDispensed(int slot){
    QSqlQuery dispensed_query;
    double dispensed;

#ifdef USE_OLD_DATABASE
    dispensed_query.prepare("SELECT total_dispensed FROM products WHERE slot=:slot");
#else
    dispensed_query.prepare("SELECT volume_dispensed_total FROM products WHERE slot=:slot");
#endif

    dispensed_query.bindValue(":slot", slot);
    dispensed_query.exec();

    while (dispensed_query.next()) {
        dispensed = dispensed_query.value(0).toDouble();
    }
    return dispensed;
}

double DbManager::getVolumeRemaining(int slot){
    QSqlQuery remaining_query;
    double remaining;

#ifdef USE_OLD_DATABASE
    remaining_query.prepare("SELECT remaining_ml FROM products WHERE slot=:slot");
#else
    remaining_query.prepare("SELECT volume_remaining FROM products WHERE slot=:slot");
#endif

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

    {
#ifdef USE_OLD_DATABASE
    refill_date_query.prepare("SELECT last_refill FROM products WHERE slot=:slot");
#else
    refill_date_query.prepare("SELECT last_restock FROM products WHERE slot=:slot");
#endif
    refill_date_query.bindValue(":slot", slot);
    refill_date_query.exec();

    while (refill_date_query.next()) {
            refill_date_string = refill_date_query.value(0).toString();

            //qDebug() << "Product: " << product_name << endl;
        }

    }
    return refill_date_string;
}

double DbManager::getTemperature(){
    QSqlQuery temperature_query;
    double temperature;

    {
    temperature_query.prepare("SELECT temp FROM temperature LIMIT 1 offset cast((SELECT count(*) FROM temperature) AS INT) - 1");
    temperature_query.exec();
    while (temperature_query.next()){
        temperature = temperature_query.value(0).toDouble();
    }
    }
    return temperature;
}

int DbManager::getPWM(int slot){
    QSqlQuery pwm_query;
    double pwm;

#ifdef USE_OLD_DATABASE
    pwm_query.prepare("SELECT pwm FROM products WHERE slot=:slot");
#else
    pwm_query.prepare("SELECT dispense_speed FROM products WHERE slot=:slot");
#endif

    pwm_query.bindValue(":slot", slot);
    pwm_query.exec();

    while (pwm_query.next()) {
        pwm = pwm_query.value(0).toInt();
        qDebug() << "pwm" << slot << pwm << endl;
    }

    
    return pwm;
}

double DbManager::getBuffer(int slot){
    QSqlQuery buffer_query;
    double buffer;

    {
    buffer_query.prepare("SELECT buffer FROM products WHERE slot=:slot");
    buffer_query.bindValue(":slot", slot);
    buffer_query.exec();
    while (buffer_query.next()){
        buffer = buffer_query.value(0).toDouble();
    }
    }
    return buffer;
}

bool DbManager::updatePaymentsDb(QString date,QString time, QString txnType, QString amount, QString cardNo, QString refNo, QString authNo, QString cardType, QString status, QString isoCode, QString hostCode, QString tvr){
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

    if(payments_query.exec())
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

bool DbManager::updatePriceSmall(int slot, double new_price){
    QSqlQuery update_price_query;

    {
#ifdef USE_OLD_DATABASE
    update_price_query.prepare("UPDATE products SET price_s = :new_price WHERE slot = :slot");
#else
    update_price_query.prepare("UPDATE products SET price_small = :new_price WHERE slot = :slot");
#endif
    update_price_query.bindValue(":new_price", new_price);
    update_price_query.bindValue(":slot", slot);

    if(update_price_query.exec()){
//        qDebug() << "Price updated successfully!";
        return true;
    }else{
//        qDebug() << "Price update error: !"
//                 << update_price_query.lastQuery()
//                 << update_price_query.lastError();
        return false;
    }
    }
}

bool DbManager::updatePriceLarge(int slot, double new_price){
    QSqlQuery update_price_query;

    
#ifdef USE_OLD_DATABASE
    update_price_query.prepare("UPDATE products SET price_l = :new_price WHERE slot = :slot");
#else
    update_price_query.prepare("UPDATE products SET price_large = :new_price WHERE slot = :slot");
#endif
    update_price_query.bindValue(":new_price", new_price);
    update_price_query.bindValue(":slot", slot);

    if(update_price_query.exec()){
//        qDebug() << "Price updated successfully!";
        return true;
    }else{
//        qDebug() << "Price update error: !"
//                 << update_price_query.lastError();
        return false;
    }
    
}

bool DbManager::updateTargetVolume_s(int slot, double new_volume){
    QSqlQuery update_target_volume_query;
{
#ifdef USE_OLD_DATABASE
    update_target_volume_query.prepare("UPDATE products SET volume_target_s=:new_volume WHERE slot=:slot");
#else
    update_target_volume_query.prepare("UPDATE products SET volume_small=:new_volume WHERE slot=:slot");
#endif


    update_target_volume_query.bindValue(":new_volume", new_volume);
    update_target_volume_query.bindValue(":slot", slot);

    if(update_target_volume_query.exec()){
//        qDebug() << "Target Volume updated successfully!";
        return true;
    }else{
//        qDebug() << "Target volume update error: !"
//                 << update_target_volume_query.lastError();
        return false;
    }
    }

}

bool DbManager::updateTargetVolume_l(int slot, double new_volume){
    QSqlQuery update_target_volume_query;

    {
#ifdef USE_OLD_DATABASE
    update_target_volume_query.prepare("UPDATE products SET volume_target_l=:new_volume WHERE slot=:slot");
#else
    update_target_volume_query.prepare("UPDATE products SET volume_large=:new_volume WHERE slot=:slot");
#endif



    update_target_volume_query.bindValue(":new_volume", new_volume);

    update_target_volume_query.bindValue(":slot", slot);

    if(update_target_volume_query.exec()){
//        qDebug() << "Target Volume updated successfully!";
        return true;
    }else{
//        qDebug() << "Target volume update error: !"
//                 << update_target_volume_query.lastError();
        return false;
    }
    }
}



bool DbManager::updateVolumePerTick(int slot, double new_volume_per_tick){
    QSqlQuery update_volume_per_tick_query;

    {
    update_volume_per_tick_query.prepare("UPDATE products SET volume_per_tick=:new_volume_per_tick WHERE slot=:slot");
    update_volume_per_tick_query.bindValue(":new_volume_per_tick", new_volume_per_tick);
    update_volume_per_tick_query.bindValue(":slot", slot);

    if(update_volume_per_tick_query.exec()){
//        qDebug() << "Volume per tick updated successfully!";
        return true;
    }else{
//        qDebug() << "Volume per tick update error: !"
//                 << update_volume_per_tick_query.lastError();
        return false;
    }
    }
}

bool DbManager::updateFullVolume(int slot, double new_full_volume){
    QSqlQuery update_full_volume_query;

#ifdef USE_OLD_DATABASE
    update_full_volume_query.prepare("UPDATE products SET full_ml=:new_full_volume WHERE slot=:slot");
#else
    update_full_volume_query.prepare("UPDATE products SET volume_full=:new_full_volume WHERE slot=:slot");
#endif
    update_full_volume_query.bindValue(":new_full_volume", new_full_volume);
    update_full_volume_query.bindValue(":slot", slot);

    if(update_full_volume_query.exec()){
//        qDebug() << "Full volume updated successfully!";
        return true;
    }else{
//        qDebug() << "Full volume update error: !"
//                 << update_full_volume_query.lastError();
        return false;
    }
    
}

bool DbManager::updatePWM(int slot, int new_pwm){
    QSqlQuery pwm_query;

    #ifdef USE_OLD_DATABASE
    pwm_query.prepare("UPDATE products SET pwm=:new_pwm WHERE slot=:slot");
#else
    pwm_query.prepare("UPDATE products SET dispense_speed=:new_pwm WHERE slot=:slot");
#endif
    pwm_query.bindValue(":new_pwm", new_pwm);
    pwm_query.bindValue(":slot", slot);

    if(pwm_query.exec()){
//        qDebug() << "PWM updated successfully!";
        return true;
    }else{
//        qDebug() << "PWM update error: !"
//                 << pwm_query.lastError();
        return false;
    
    }
}

bool DbManager::updateBuffer(int slot, double new_buffer){
    QSqlQuery buffer_query;

    {
    buffer_query.prepare("UPDATE products SET buffer=:new_buffer WHERE slot=:slot");
    buffer_query.bindValue(":new_buffer", new_buffer);
    buffer_query.bindValue(":slot", slot);

    if(buffer_query.exec()){
//        qDebug() << "Buffer updated successfully!";
        return true;
    }else{
//        qDebug() << "Buffer update error: !"
//                 << buffer_query.lastError();
        return false;
    }
    }
}

QString DbManager::getPLU(int slot, char size){
    QSqlQuery plu_query;
    QString plu_smalltring;

#ifdef USE_OLD_DATABASE
    if (size == 's'){
        plu_query.prepare("SELECT PLU_s FROM products WHERE slot=:slot");
    }else if (size == 'l'){
        plu_query.prepare("SELECT PLU_l FROM products WHERE slot=:slot");
    }
#else
    if (size == 's'){
        plu_query.prepare("SELECT PLU_small FROM products WHERE slot=:slot");
    }else if (size == 'l'){
        plu_query.prepare("SELECT PLU_large FROM products WHERE slot=:slot");
    }
#endif
    plu_query.bindValue(":slot", slot);
    plu_query.exec();

    while (plu_query.next()) {
            plu_smalltring = plu_query.value(0).toString();

    }
    return plu_smalltring;
}

bool DbManager::updatePluSmall(int slot, QString new_plu){
    QSqlQuery plu_query;

#ifdef USE_OLD_DATABASE
    plu_query.prepare("UPDATE products SET PLU_s=:new_plu WHERE slot=:slot");
#else
    plu_query.prepare("UPDATE products SET PLU_small=:new_plu WHERE slot=:slot");
#endif
    plu_query.bindValue(":new_plu", new_plu);
    plu_query.bindValue(":slot", slot);

    if(plu_query.exec()){
//        qDebug() << "PLU updated successfully!";
        return true;
    }else{
//        qDebug() << "PLU update error: !"
//                 << plu_query.lastError();
        return false;
    
    }
}

bool DbManager::updatePluLarge(int slot, QString new_plu){
    QSqlQuery plu_query;

    {
#ifdef USE_OLD_DATABASE
    plu_query.prepare("UPDATE products SET PLU_l=:new_plu WHERE slot=:slot");
#else
    plu_query.prepare("UPDATE products SET PLU_large=:new_plu WHERE slot=:slot");
#endif

    plu_query.bindValue(":new_plu", new_plu);
    plu_query.bindValue(":slot", slot);

    if(plu_query.exec()){
//        qDebug() << "PLU updated successfully!";
        return true;
    }else{
//        qDebug() << "PLU update error: !"
//                 << plu_query.lastError();
        return false;
    }
    }
}

QString DbManager::getMachineID(){
    QSqlQuery mid_query;
    QString mid_string;

    {
    mid_query.prepare("SELECT machine_id FROM machine");
    mid_query.exec();

    while (mid_query.next()) {
            mid_string = mid_query.value(0).toString();
        }


    }
    return mid_string;
}

QString DbManager::getProductID(int slot){
    QSqlQuery product_id_query;
    QString product_id_string;

    {
    product_id_query.prepare("SELECT product_id FROM products WHERE slot=:slot");
    product_id_query.bindValue(":slot", slot);
    product_id_query.exec();

    while (product_id_query.next()) {
            product_id_string = product_id_query.value(0).toString();

            //qDebug() << "Product: " << product_name << endl;
        }
    }
    return product_id_string;
}

QString DbManager::getUnits(int slot){
    QSqlQuery units_query;
    QString units_string;

#ifdef USE_OLD_DATABASE
    units_query.prepare("SELECT units FROM products WHERE slot=:slot");
#else
    units_query.prepare("SELECT display_unit FROM products WHERE slot=:slot");
#endif

    units_query.bindValue(":slot", slot);
    units_query.exec();

    while (units_query.next()) {
            units_string = units_query.value(0).toString();

            //qDebug() << "Product: " << product_name << endl;
    
    }
    return units_string;
}
