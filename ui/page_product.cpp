
//***************************************
//
// page_product.cpp
// GUI class for user to select size and
// payment for drink.
//
// Allows navigation to First product page
// payment page and page_idle page
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "page_product.h"
#include "ui_page_product.h"
#include <iostream>
#include <string>
#include <cmath>

#include "pagepayment.h"
#include "page_select_product.h"
#include "page_idle.h"
#include <curl/curl.h>
#include <json.hpp>


using json = nlohmann::json;
double promoPercent = 0.0;


// CTOR
pageProduct::pageProduct(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pageProduct)
{
    ui->setupUi(this);

    int product_slot___ = 99;

    QString bitmap_location;

    if(product_slot___ > 0 && product_slot___ <= 6) {
        bitmap_location.append("/home/df-admin/production/references/4_pay_select_page_l_");
        bitmap_location.append(QString::number(idlePage->userDrinkOrder->getOption()));
        bitmap_location.append(".png");
    } else {
        bitmap_location = "/home/df-admin/production/references/4_pay_select_page_l_1.png";
    }

    qDebug() << bitmap_location << endl;
    QPixmap background(bitmap_location);
    

    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /* Hacky transparent button */
    ui->previousPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->pagePayment_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->mainPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->promoKeyboard->setStyleSheet(" background-image: url(/home/df-admin/production/references/soapstand-keyboard.png); }");

    // TODO: ADD buttons to select size/price of drink
    ui->orderSmall_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->orderBig_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->promoInputButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    ui->promoCode->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->promoButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->discountLabel->setText("-$0.00");
    promoPercent = 0.0;
    ui->promoCode->clear();
    ui->promoCode->hide();
    ui->promoKeyboard->hide();
    ui->promoInputButton->show();
    

    /*Stations without coupon code */
    // ui->promoCode->hide();
    // ui->promoKeyboard->hide();
    // ui->promoInputButton->hide();
    // ui->discountLabel->hide();
    // ui->promoButton->hide();

    {
        selectIdleTimer = new QTimer(this);
        selectIdleTimer->setInterval(40);
        connect(ui->promoButton, SIGNAL(clicked()), this, SLOT(on_applyPromo_Button_clicked()));
        connect(ui->promoInputButton, SIGNAL(clicked()), this, SLOT(on_promoCodeInput_clicked()));
        connect(ui->buttonGroup, SIGNAL(buttonPressed(int)), this, SLOT(buttonWasClicked(int)));
        connect(selectIdleTimer, SIGNAL(timeout()), this, SLOT(onSelectTimeoutTick()));
    }
}

/*
 * Page Tracking reference to Select Drink, Payment Page and Idle page
 */
void pageProduct::setPage(page_select_product *pageSelect, page_dispenser* page_dispenser,page_error_wifi* pageWifiError,  page_idle* pageIdle, pagePayment* pagePayment, page_help* pageHelp)
{
    this->firstProductPage = pageSelect;
    this->paymentPage = pagePayment;
    this->idlePage = pageIdle;
    this->dispensingPage = page_dispenser;
    this->helpPage = pageHelp;
    this->wifiError = pageWifiError;
    ui->promoCode->clear();
    ui->promoCode->hide();
    ui->discountLabel->setText("-$0.00");

    /*
    Stations without Promo Code
    */
    // ui->promoCode->hide();
    // ui->promoKeyboard->hide();
    // ui->promoInputButton->hide();
    // ui->discountLabel->hide();
    // ui->promoButton->hide();
}

// DTOR
pageProduct::~pageProduct()
{
    delete ui;
}

void pageProduct::cancelTimers(){
    selectIdleTimer->stop();
}

/* GUI */
void pageProduct::on_previousPage_Button_clicked()
{

//    qDebug() << "pageProduct: Previous button" << endl;
    while(!stopSelectTimers()){

    };
    selectIdleTimer->stop();
    firstProductPage->showFullScreen();
    ui->discountLabel->setText("-$0.00");
    ui->promoInputButton->show();
    
    
    // ui->promoCode->hide();
    // ui->promoKeyboard->hide();
    // ui->promoInputButton->hide();
    // ui->discountLabel->hide();
    // ui->promoButton->hide();

    usleep(100);
    this->hide();

}

void pageProduct::on_pagePayment_Button_clicked()
{
   qDebug() << "pageProduct: Pay button" << endl;

    ui->mainPage_Button->setEnabled(false);
    ui->previousPage_Button->setEnabled(false);
    qDebug() << "ahoyy11" ;
    DbManager db(DB_PATH);

    this->stopSelectTimers();
    selectIdleTimer->stop();

    if (db.getPaymentMethod(idlePage->userDrinkOrder->getOption()) == "qr" || db.getPaymentMethod(idlePage->userDrinkOrder->getOption()) == "tap"){
        CURL *curl;
        CURLcode res;
        curl = curl_easy_init();
        
        curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/ping");
        
        res = curl_easy_perform(curl);
        if(res!= CURLE_OK){
            wifiError->showEvent(wifiErrorEvent);
            wifiError->showFullScreen();
            this->hide();
        }
        else{
           
            ui->totalPriceLabel->text();
            paymentPage->showFullScreen();
            this->hide();
        }
        
    } else if (db.getPaymentMethod(idlePage->userDrinkOrder->getOption()) == "barcode" || db.getPaymentMethod(idlePage->userDrinkOrder->getOption()) == "plu"){
        db.closeDB();
        dispensingPage->showEvent(dispenseEvent);
        dispensingPage->showFullScreen();
        this->hide();
    }
     db.closeDB();
}

void pageProduct::resizeEvent(QResizeEvent *event){
    int product_slot___ = idlePage->userDrinkOrder->getOption();
    qDebug() << "ahoyy1" ;
    DbManager db(DB_PATH);

    ui->mainPage_Button->setEnabled(true);
    ui->previousPage_Button->setEnabled(true);

    char drinkSize;
    if (idlePage->userDrinkOrder->getSizeOption() == SMALL_DRINK){
        drinkSize = 's';
    }
    if (idlePage->userDrinkOrder->getSizeOption() == LARGE_DRINK){
        drinkSize = 'l';
    }

    QString bitmap_location;

    if(product_slot___ > 0 && product_slot___ <= 9) {
        bitmap_location.append("/home/df-admin/production/references/4_pay_select_page_l_");
        bitmap_location.append(QString::number(product_slot___));
        bitmap_location.append(".png");
    } else {
        bitmap_location = "/home/df-admin/production/references/4_pay_select_page_l_1.png";
    }

    QPixmap background(bitmap_location);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);

    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);
    idlePage->userDrinkOrder->setDrinkSize(LARGE_DRINK);

    if(selectIdleTimer == nullptr) {
        selectIdleTimer = new QTimer(this);
        selectIdleTimer->setInterval(40);
        connect(selectIdleTimer, SIGNAL(timeout()), this, SLOT(onSelectTimeoutTick()));
    }

    ui->priceLabel->setText("$"+QString::number(db.getProductPrice(product_slot___, drinkSize), 'f', 2));
    ui->totalPriceLabel->setText("$"+QString::number(db.getProductPrice(idlePage->userDrinkOrder->getOption(), drinkSize), 'f', 2));

    // ui->totalPriceLabel->setText("$"+QString::number(idlePage->userDrinkOrder->getPrice()));
    ui->label_price_small->setText("$"+QString::number(db.getProductPrice(product_slot___, 's'), 'f', 2));
    ui->label_price_large->setText("$"+QString::number(db.getProductPrice(product_slot___, 'l'), 'f', 2));
    ui->discountLabel->setText("-$0.00");


    // double v;
    // v = db.getProductVolume(product_slot___, 's');
    // if (db.getUnits(product_slot___) == "l" || db.getUnits(product_slot___) == "ml"){
    //     if (db.getProductVolume(product_slot___, drinkSize) < 1000){
    //         ui->label_size_small->setText(QString::number(v) + "ml");
    //     }else{
    //         ui->label_size_small->setText(QString::number(v/1000) + "L");
    //     }
    // }else if  (db.getUnits(product_slot___) == "oz" ){
    //         ui->label_size_small->setText(QString::number(ceil(v*ML_TO_OZ)) + "oz");
    // }

    
    // v = db.getProductVolume(product_slot___, 'l');
    // if (db.getUnits(product_slot___) == "l" || db.getUnits(product_slot___) == "ml"){
    //     if (db.getProductVolume(product_slot___, drinkSize) < 1000){
    //         ui->label_size_large->setText( QString::number(v) + "ml");
    //     }else{
    //         ui->label_size_large->setText(QString::number(v/1000) + "L");
    //     }
    // }else if  (db.getUnits(product_slot___) == "oz" ){
    //         ui->label_size_large->setText(QString::number(ceil(v*ML_TO_OZ)) + "oz");
    // }






    // if (db.getProductVolume(product_slot___, 's') < 1000){
    //     ui->label_size_small->setText(QString::number(db.getProductVolume(product_slot___, 's')) + " " + db.getUnits(product_slot___));
    // }else{
    //     ui->label_size_small->setText(QString::number(db.getProductVolume(product_slot___, 's')/1000) + "L");
    // }

    // if (db.getProductVolume(product_slot___, 'l') < 1000){
    //     ui->label_size_large->setText(QString::number(db.getProductVolume(product_slot___, 'l')) + " " + db.getUnits(product_slot___));
    // }else{
    //     ui->label_size_large->setText(QString::number(db.getProductVolume(product_slot___, 'l')/1000) + "L");
    // }

    // if (db.getUnits(product_slot___) == "l" || db.getUnits(product_slot___) == "ml"){
    //     if (db.getProductVolume(product_slot___, drinkSize) < 1000){
    //         ui->productLabel->setText((db.getProductName(product_slot___)) + " " + QString::number(db.getProductVolume(product_slot___, drinkSize)) + "ml");
    //     }else{
    //         ui->productLabel->setText((db.getProductName(product_slot___)) + " " + QString::number(db.getProductVolume(product_slot___, drinkSize)/1000) + "L");
    //     }
    // }else if  (db.getUnits(product_slot___) == "oz" ){
    //         ui->productLabel->setText((db.getProductName(product_slot___)) + " " + QString::number(db.getProductVolume(product_slot___, drinkSize)*ML_TO_OZ) + "ozzz");

    // }


    ui->label_price_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;");
    ui->label_price_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #FFFFFF;");
    ui->promoCode->clear();
    ui->promoCode->hide();
    promoPercent = 0.0;

//    qDebug() << "Start pageProduct Timers" << endl;
    selectIdleTimer->start(1000);
    _selectIdleTimeoutSec = 400;
    
    db.closeDB();
    // do after db closed
    set_label_volume(ui->label_size_small, product_slot___, 's');
    set_label_volume(ui->label_size_large, product_slot___, 'l');
    set_label_product(product_slot___, drinkSize);
}

void pageProduct::showEvent(QShowEvent *event){
   int product_slot___ = idlePage->userDrinkOrder->getOption();
   qDebug() << "ahoyy12" ;
    DbManager db(DB_PATH);

    ui->mainPage_Button->setEnabled(true);
    ui->previousPage_Button->setEnabled(true);

    char drinkSize;
    if (idlePage->userDrinkOrder->getSizeOption() == SMALL_DRINK){
        drinkSize = 's';
    }
    if (idlePage->userDrinkOrder->getSizeOption() == LARGE_DRINK){
        drinkSize = 'l';
    }
    ui->priceLabel->setText("$"+QString::number(db.getProductPrice(product_slot___, drinkSize), 'f', 2));
    // ui->totalPriceLabel->setText("$"+QString::number(db.getProductPrice(product_slot___, drinkSize), 'f', 2));
    ui->totalPriceLabel->setText("$"+QString::number(idlePage->userDrinkOrder->getPrice()));
    ui->label_price_small->setText("$"+QString::number(db.getProductPrice(product_slot___, 's'), 'f', 2));
    ui->label_price_large->setText("$"+QString::number(db.getProductPrice(product_slot___, 'l'), 'f', 2));

    // if (db.getProductVolume(product_slot___, 's') < 1000){
    //     ui->label_size_small->setText(QString::number(db.getProductVolume(product_slot___, 's')) + " " + db.getUnits(product_slot___));
    // }else{
    //     ui->label_size_small->setText(QString::number(db.getProductVolume(product_slot___, 's')/1000) + "L");
    // }

    // if (db.getProductVolume(product_slot___, 'l') < 1000){
    //     ui->label_size_large->setText(QString::number(db.getProductVolume(product_slot___, 'l')) + " " + db.getUnits(product_slot___));
    // }else{
    //     ui->label_size_large->setText(QString::number(db.getProductVolume(product_slot___, 'l')/1000) + "L");
    // }

    // if (db.getProductVolume(product_slot___, drinkSize) < 1000){
    //     ui->productLabel->setText((db.getProductName(product_slot___)) + " " + QString::number(db.getProductVolume(product_slot___, drinkSize)) +  " " + db.getUnits(product_slot___));
    // }else{
    //     ui->productLabel->setText((db.getProductName(product_slot___)) + " " + QString::number(db.getProductVolume(product_slot___, drinkSize)/1000) + "L");
    // }
    ui->label_price_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: normal; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;");
    ui->label_price_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: normal; font-weight: bold; font-size: 36px; line-height: 44px; color: #FFFFFF;");
    ui->label_size_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #D2E4CD;");
    ui->label_size_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #5E8500;");
    ui->promoCode->clear();
    ui->promoCode->hide();
    promoPercent = 0.0;

    db.closeDB();
    set_label_product(product_slot___, drinkSize);
 set_label_volume(ui->label_size_small, product_slot___, 's');
    set_label_volume(ui->label_size_large, product_slot___, 'l');
}

void pageProduct::set_label_product(int product_slot___, char drinkSize){
    // qDebug() << "ahoyy1erer3" ;
    // DbManager db(DB_PATH);

    // if (db.getUnits(product_slot___) == "l" || db.getUnits(product_slot___) == "ml"){
    //     if (db.getProductVolume(product_slot___, drinkSize) < 1000){
    //         ui->productLabel->setText((db.getProductName(product_slot___)) + " " + QString::number(db.getProductVolume(product_slot___, drinkSize)) + "ml");
    //     }else{
    //         ui->productLabel->setText((db.getProductName(product_slot___)) + " " + QString::number(db.getProductVolume(product_slot___, drinkSize)/1000) + "L");
    //     }
    // }else if  (db.getUnits(product_slot___) == "oz" ){
    //         ui->productLabel->setText((db.getProductName(product_slot___)) + " " + QString::number(ceil(db.getProductVolume(product_slot___, drinkSize)*ML_TO_OZ)) + "oz");

    // }
    // db.closeDB();


    qDebug() << "ahoyy1erer3" ;
    DbManager db(DB_PATH);
    QString product_name = db.getProductName(product_slot___);
    db.closeDB();

    ui->productLabel->setText(product_name + " " + getSizeToVolume(product_slot___, drinkSize));
    
}

void pageProduct::set_label_volume(QLabel* label, int product_slot___, char drinkSize){
    // qDebug() << "db for label volume" ;
    // double v;
    // QString units;
    // DbManager db(DB_PATH);
    // v = db.getProductVolume(product_slot___, drinkSize);
    // units = db.getUnits(product_slot___);
    // db.closeDB();
    //         // ui->label_size_small->setText(QString::number(v) + "ml");

    label->setText(getSizeToVolume(product_slot___, drinkSize));

    // if (units == "l" || units == "ml"){
    //     if (v < 1000){
    //         label->setText(QString::number(v) + "ml");
    //     }else{
    //         label->setText(QString::number(v/1000) + "L");
    //     }
    // }else if  (units == "oz" ){
    //         label->setText(QString::number(ceil(v*ML_TO_OZ)) + "oz");
    // }else{
    //     qDebug() << "Unhandled unit system: " << units ; 
    // }
    
    // v = db.getProductVolume(product_slot___, 'l');
    // if (units == "l" || units == "ml"){
    //     if (db.getProductVolume(product_slot___, drinkSize) < 1000){
    //         ui->label_size_large->setText( QString::number(v) + "ml");
    //     }else{
    //         ui->label_size_large->setText(QString::number(v/1000) + "L");
    //     }
    // }else if  (units == "oz" ){
    //         ui->label_size_large->setText(QString::number(ceil(v*ML_TO_OZ)) + "oz");
    // }
}

QString pageProduct::getSizeToVolume( int product_slot___, char drinkSize){
    qDebug() << "db for label volume" ;
    double v;
    QString units;
    DbManager db(DB_PATH);
    v = db.getProductVolume(product_slot___, drinkSize);
    units = db.getUnits(product_slot___);
    db.closeDB();
            // ui->label_size_small->setText(QString::number(v) + "ml");

    QString volume_as_string;

    if (units == "l" || units == "ml"){
        if (v < 1000){
            volume_as_string = QString::number(v) + "ml";
        }else{
            volume_as_string = QString::number(v/1000) + "L";
        }
    }else if  (units == "oz" ){
            volume_as_string = QString::number(ceil(v*ML_TO_OZ)) + "oz";
    }else{
        qDebug() << "Unhandled unit system: " << units ; 
    }

    return volume_as_string;
}

void pageProduct::onSelectTimeoutTick(){
    if(-- _selectIdleTimeoutSec >= 0) {
//        qDebug() << "pageProduct: Tick Down - " << _selectIdleTimeoutSec << endl;
    } else {
//        qDebug() << "Timer Done!" << _selectIdleTimeoutSec << endl;
        selectIdleTimer->stop();

        mainPage();
    }
}

bool pageProduct::stopSelectTimers(){
//    qDebug() << "Stop pageProduct Timers" << endl;
    if(selectIdleTimer != nullptr) {
       // qDebug() << "Enter Stop" << endl;
        selectIdleTimer->stop();
        return true;
    } else {
        return false;
    }
}

void pageProduct::mainPage()
{
//    qDebug() << "pageProduct: mainPage button" << endl;
    this->stopSelectTimers();
    selectIdleTimer->stop();
    idlePage->showFullScreen();
//    usleep(100);
    this->hide();
}

void pageProduct::on_mainPage_Button_clicked()
{
//    qDebug() << "pageProduct: helpPage button" << endl;
    this->stopSelectTimers();
    selectIdleTimer->stop();
    helpPage->showFullScreen();
    this->hide();
}

// on_Small_Order button listener
void pageProduct::on_orderSmall_Button_clicked()
{
    QString bitmap_location;
    
    int product_slot___ = idlePage->userDrinkOrder->getOption();

    if(product_slot___ > 0 && product_slot___ <= 9) {
        bitmap_location.append("/home/df-admin/production/references/4_pay_select_page_s_");
        bitmap_location.append(QString::number(product_slot___));
        bitmap_location.append(".png");
    } else {
        bitmap_location = "/home/df-admin/production/references/4_pay_select_page_s_1.png";
    }


    QPixmap background(bitmap_location);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    idlePage->userDrinkOrder->setDrinkSize(SMALL_DRINK);
    _selectIdleTimeoutSec = 140;

    char drinkSize = 's';
    qDebug() << "ahoyy13" ;
    DbManager db(DB_PATH);
    
    
    ui->priceLabel->setText("$"+QString::number(db.getProductPrice(idlePage->userDrinkOrder->getOption(), drinkSize), 'f', 2));
    // ui->totalPriceLabel->setText("$"+QString::number(db.getProductPrice(idlePage->userDrinkOrder->getOption(), drinkSize), 'f', 2));
    updatePriceAfterPromo(promoPercent);
    ui->label_price_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: bold; font-size: 36px; line-height: 44px; color: #FFFFFF;");
    ui->label_price_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;");
    ui->label_size_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #D2E4CD;");
    ui->label_size_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #5E8500;");

    // if (db.getProductVolume(product_slot___, drinkSize) < 1000){
    //     ui->productLabel->setText((db.getProductName(product_slot___)) + " " + QString::number(db.getProductVolume(product_slot___, drinkSize)) + " " + db.getUnits(product_slot___));
    // }else{
    //     ui->productLabel->setText((db.getProductName(product_slot___)) + " " + QString::number(db.getProductVolume(product_slot___, drinkSize)/1000) + "L");
    // }

    db.closeDB();
    //after db closed
    set_label_product(product_slot___, drinkSize);

    
    

}



// on_Large_Order button listener
void pageProduct::on_orderBig_Button_clicked()
{
    QString bitmap_location;

    int product_slot___ = idlePage->userDrinkOrder->getOption();

    if(product_slot___ > 0 && product_slot___ <= 9) {
        bitmap_location.append("/home/df-admin/production/references/4_pay_select_page_l_");
        bitmap_location.append(QString::number(product_slot___));
        bitmap_location.append(".png");
    } else {
        bitmap_location = "/home/df-admin/production/references/4_pay_select_page_l_1.png";

    }
    
    QPixmap background(bitmap_location);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    idlePage->userDrinkOrder->setDrinkSize(LARGE_DRINK);
    _selectIdleTimeoutSec = 140;
    qDebug() << "ahoyy14" ;
    DbManager db(DB_PATH);
//    db.addPageClick("Large Drink Size Selected");

    char drinkSize = 'l';

    ui->priceLabel->setText("$"+QString::number(db.getProductPrice(idlePage->userDrinkOrder->getOption(), drinkSize), 'f', 2));
    // ui->totalPriceLabel->setText("$"+QString::number(db.getProductPrice(idlePage->userDrinkOrder->getOption(), drinkSize), 'f', 2));
    updatePriceAfterPromo(promoPercent);
    ui->label_price_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;");
    ui->label_price_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #FFFFFF;");
    ui->label_size_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #D2E4CD;");
    ui->label_size_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #5E8500;");

    // if (db.getProductVolume(product_slot___, drinkSize) < 1000){
    //     ui->productLabel->setText((db.getProductName(product_slot___)) + " " + QString::number(db.getProductVolume(product_slot___, drinkSize)) + " " + db.getUnits(product_slot___));
    // }else{
    //     ui->productLabel->setText((db.getProductName(product_slot___)) + " " + QString::number(db.getProductVolume(product_slot___, drinkSize)/1000) + "L");
    // }
    db.closeDB();
    // do after db closed
    set_label_product(product_slot___, drinkSize);

}

size_t WriteCallback_coupon(char* contents, size_t size, size_t nmemb, void *userp){
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void pageProduct::on_promoCodeInput_clicked(){
    QObject* button = QObject::sender();
    ui->promoCode->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #5E8580;border-color:#5E8580;");
    // ui->promoInputButton->hide();
    ui->promoKeyboard->show();
    ui->promoCode->show();
}

void pageProduct::updatePriceAfterPromo(double discountPercent){
    double discount;
    QString old_price = (ui->priceLabel->text()).split("$")[1];
    double price = old_price.toDouble();
    discount = discountPercent * price /100;
    price = (100-discountPercent) * price / 100;
    ui->discountLabel->setText("-$"+QString::number(discount,  'f', 2));
    idlePage->userDrinkOrder->setPrice(price);
    ui->totalPriceLabel->setText("$"+QString::number(price, 'f', 2));

}

void pageProduct::on_applyPromo_Button_clicked()
{

    QString promocode = ui->promoCode->text();
    ui->promoKeyboard->hide();
    CURL *curl;
    CURLcode res;
    long http_code = 0;
    if(promocode != ""){
        readBuffer.clear();
        curl = curl_easy_init();
            
        curl_easy_setopt(curl, CURLOPT_URL, ("https://soapstandportal.com/api/coupon/find/"+promocode).toUtf8().constData());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback_coupon);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if(res!=CURLE_OK){
        ui->promoCode->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #f44336;border-color:#f44336;");
            qDebug()<< "Invalid Coupon" << endl;
        }
        else {
            int new_percent;
            
            if(http_code==200){
                json coupon_obj = json::parse(readBuffer);
                if(coupon_obj["active"]){
                    new_percent = coupon_obj["discount_amount"];
                    updatePriceAfterPromo(new_percent);
                    promoPercent = new_percent;
                }
                else{
                    qDebug()<< "Invalid Coupon" << endl;
                    ui->promoCode->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #75756f;border-color:#f44336;");

                }
              
            }
            else{
                qDebug()<< "Invalid Coupon" << endl;
                ui->promoCode->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #f44336;border-color:#f44336;");

            }

        }
    }
    
}



void pageProduct::buttonWasClicked(int buttonID){

    QAbstractButton *buttonpressed = ui->buttonGroup->button(buttonID);
    //qDebug() << buttonpressed->text();
    QString buttonText = buttonpressed->objectName();

  if(buttonText=="backspace"){
        ui->promoCode->backspace();
    }
    else if(buttonText=="done"){
        if(ui->promoCode->text() == ""){
            ui->promoCode->hide();
        }
        ui->promoKeyboard->hide();

    }
    else if(buttonText.mid(0,3)=="num"){
        ui->promoCode->setText(ui->promoCode->text() + buttonText.mid(3,1));

    }
    else{
        ui->promoCode->setText(ui->promoCode->text() + buttonText);
    }
}
