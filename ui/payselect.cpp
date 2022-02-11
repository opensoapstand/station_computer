
//***************************************
//
// payselect.cpp
// GUI class for user to select size and
// payment for drink.
//
// Allows navigation to First product page
// payment page and idle page
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "payselect.h"
#include "ui_payselect.h"
#include <iostream>
#include <string>

#include "paypage.h"
#include "productpage_1.h"
#include "idle.h"
#include <curl/curl.h>
#include <json.hpp>


using json = nlohmann::json;
double promoPercent = 0.0;


// CTOR
paySelect::paySelect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::paySelect)
{
    ui->setupUi(this);

    int checkOption = 99;

    QString bitmap_location;

        if(checkOption > 0 && checkOption <= 6) {
            bitmap_location.append(":/light/4_pay_select_page_l_");
            bitmap_location.append(QString::number(idlePage->userDrinkOrder->getOption()));
            bitmap_location.append(".png");
        } else {
            bitmap_location = ":/light/4_pay_select_page_l_1.png";
        }

    QPixmap background(bitmap_location);

    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /* Hacky transparent button */
    ui->previousPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->payPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->mainPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->promoKeyboard->setStyleSheet(" background-image: url(:/light/soapstand-keyboard.png); }");

    // TODO: ADD buttons to select size/price of drink
    ui->orderSmall_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->orderBig_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->promoInputButton->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

    ui->promoCode->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->promoButton->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
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
void paySelect::setPage(productPage_1 *pageSelect, dispensePage* pageDispense,wifiErrorPage* pageWifiError,  idle* pageIdle, payPage* pagePayment, help* pageHelp)
{
    this->firstProductPage = pageSelect;
    this->paymentPage = pagePayment;
    this->idlePage = pageIdle;
    this->dispensingPage = pageDispense;
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
paySelect::~paySelect()
{
    delete ui;
}

void paySelect::cancelTimers(){
    selectIdleTimer->stop();
}

/* GUI */
void paySelect::on_previousPage_Button_clicked()
{

//    qDebug() << "paySelect: Previous button" << endl;
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

void paySelect::on_payPage_Button_clicked()
{
   qDebug() << "paySelect: Pay button" << endl;

    ui->mainPage_Button->setEnabled(false);
    ui->previousPage_Button->setEnabled(false);

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
            db.closeDB();
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
}

void paySelect::resizeEvent(QResizeEvent *event){
    int checkOption = idlePage->userDrinkOrder->getOption();
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

    if(checkOption > 0 && checkOption <= 9) {
        bitmap_location.append(":/light/4_pay_select_page_l_");
        bitmap_location.append(QString::number(checkOption));
        bitmap_location.append(".png");
    } else {
        bitmap_location = ":/light/4_pay_select_page_l_1.png";
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

    ui->priceLabel->setText("$"+QString::number(db.getProductPrice(checkOption, drinkSize), 'f', 2));
    ui->totalPriceLabel->setText("$"+QString::number(db.getProductPrice(idlePage->userDrinkOrder->getOption(), drinkSize), 'f', 2));

    // ui->totalPriceLabel->setText("$"+QString::number(idlePage->userDrinkOrder->getPrice()));
    ui->price_sLabel->setText("$"+QString::number(db.getProductPrice(checkOption, 's'), 'f', 2));
    ui->price_lLabel->setText("$"+QString::number(db.getProductPrice(checkOption, 'l'), 'f', 2));
    ui->discountLabel->setText("-$0.00");
    if (db.getProductVolume(checkOption, 's') < 1000){
        ui->volume_sLabel->setText(QString::number(db.getProductVolume(checkOption, 's')) + " " + db.getUnits(checkOption));
    }else{
        ui->volume_sLabel->setText(QString::number(db.getProductVolume(checkOption, 's')/1000) + "L");
    }

    if (db.getProductVolume(checkOption, 'l') < 1000){
        ui->volume_lLabel->setText(QString::number(db.getProductVolume(checkOption, 'l')) + " " + db.getUnits(checkOption));
    }else{
        ui->volume_lLabel->setText(QString::number(db.getProductVolume(checkOption, 'l')/1000) + "L");
    }

    if (db.getProductVolume(checkOption, drinkSize) < 1000){
        ui->productLabel->setText((db.getProductName(checkOption)) + " " + QString::number(db.getProductVolume(checkOption, drinkSize)) + " " + db.getUnits(checkOption));
    }else{
        ui->productLabel->setText((db.getProductName(checkOption)) + " " + QString::number(db.getProductVolume(checkOption, drinkSize)/1000) + "L");
    }

    ui->price_sLabel->setStyleSheet("font-family: Montserrat; background-image: url(:/light/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;");
    ui->price_lLabel->setStyleSheet("font-family: Montserrat; background-image: url(:/light/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #FFFFFF;");
    ui->promoCode->clear();
    ui->promoCode->hide();
    promoPercent = 0.0;

//    qDebug() << "Start paySelect Timers" << endl;
    selectIdleTimer->start(1000);
    _selectIdleTimeoutSec = 400;

    db.closeDB();
}

void paySelect::showEvent(QShowEvent *event){
   int checkOption = idlePage->userDrinkOrder->getOption();
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
    ui->priceLabel->setText("$"+QString::number(db.getProductPrice(checkOption, drinkSize), 'f', 2));
    // ui->totalPriceLabel->setText("$"+QString::number(db.getProductPrice(checkOption, drinkSize), 'f', 2));
    ui->totalPriceLabel->setText("$"+QString::number(idlePage->userDrinkOrder->getPrice()));
    ui->price_sLabel->setText("$"+QString::number(db.getProductPrice(checkOption, 's'), 'f', 2));
    ui->price_lLabel->setText("$"+QString::number(db.getProductPrice(checkOption, 'l'), 'f', 2));

    if (db.getProductVolume(checkOption, 's') < 1000){
        ui->volume_sLabel->setText(QString::number(db.getProductVolume(checkOption, 's')) + " " + db.getUnits(checkOption));
    }else{
        ui->volume_sLabel->setText(QString::number(db.getProductVolume(checkOption, 's')/1000) + "L");
    }

    if (db.getProductVolume(checkOption, 'l') < 1000){
        ui->volume_lLabel->setText(QString::number(db.getProductVolume(checkOption, 'l')) + " " + db.getUnits(checkOption));
    }else{
        ui->volume_lLabel->setText(QString::number(db.getProductVolume(checkOption, 'l')/1000) + "L");
    }

    if (db.getProductVolume(checkOption, drinkSize) < 1000){
        ui->productLabel->setText((db.getProductName(checkOption)) + " " + QString::number(db.getProductVolume(checkOption, drinkSize)) +  " " + db.getUnits(checkOption));
    }else{
        ui->productLabel->setText((db.getProductName(checkOption)) + " " + QString::number(db.getProductVolume(checkOption, drinkSize)/1000) + "L");
    }
    ui->price_sLabel->setStyleSheet("font-family: Montserrat; background-image: url(:/light/background.png); font-style: normal; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;");
    ui->price_lLabel->setStyleSheet("font-family: Montserrat; background-image: url(:/light/background.png); font-style: normal; font-weight: bold; font-size: 36px; line-height: 44px; color: #FFFFFF;");
    ui->volume_lLabel->setStyleSheet("font-family: Montserrat; background-image: url(:/light/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #D2E4CD;");
    ui->volume_sLabel->setStyleSheet("font-family: Montserrat; background-image: url(:/light/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #5E8500;");
    ui->promoCode->clear();
    ui->promoCode->hide();
    promoPercent = 0.0;

    db.closeDB();
}

void paySelect::onSelectTimeoutTick(){
    if(-- _selectIdleTimeoutSec >= 0) {
//        qDebug() << "paySelect: Tick Down - " << _selectIdleTimeoutSec << endl;
    } else {
//        qDebug() << "Timer Done!" << _selectIdleTimeoutSec << endl;
        selectIdleTimer->stop();

        mainPage();
    }
}

bool paySelect::stopSelectTimers(){
//    qDebug() << "Stop paySelect Timers" << endl;
    if(selectIdleTimer != nullptr) {
       // qDebug() << "Enter Stop" << endl;
        selectIdleTimer->stop();
        return true;
    } else {
        return false;
    }
}

void paySelect::mainPage()
{
//    qDebug() << "paySelect: mainPage button" << endl;
    this->stopSelectTimers();
    selectIdleTimer->stop();
    idlePage->showFullScreen();
//    usleep(100);
    this->hide();
}

void paySelect::on_mainPage_Button_clicked()
{
//    qDebug() << "paySelect: helpPage button" << endl;
    this->stopSelectTimers();
    selectIdleTimer->stop();
    helpPage->showFullScreen();
    this->hide();
}

// on_Small_Order button listener
void paySelect::on_orderSmall_Button_clicked()
{
    QString bitmap_location;
    
    int checkOption = idlePage->userDrinkOrder->getOption();

    if(checkOption > 0 && checkOption <= 9) {
        bitmap_location.append(":/light/4_pay_select_page_s_");
        bitmap_location.append(QString::number(checkOption));
        bitmap_location.append(".png");
    } else {
        bitmap_location = ":/light/4_pay_select_page_s_1.png";
    }


    QPixmap background(bitmap_location);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    idlePage->userDrinkOrder->setDrinkSize(SMALL_DRINK);
    _selectIdleTimeoutSec = 140;

    char drinkSize = 's';

    DbManager db(DB_PATH);
    
    
    ui->priceLabel->setText("$"+QString::number(db.getProductPrice(idlePage->userDrinkOrder->getOption(), drinkSize), 'f', 2));
    // ui->totalPriceLabel->setText("$"+QString::number(db.getProductPrice(idlePage->userDrinkOrder->getOption(), drinkSize), 'f', 2));
    updatePriceAfterPromo(promoPercent);
    ui->price_sLabel->setStyleSheet("font-family: Montserrat; background-image: url(:/light/background.png); font-style: semibold; font-weight: bold; font-size: 36px; line-height: 44px; color: #FFFFFF;");
    ui->price_lLabel->setStyleSheet("font-family: Montserrat; background-image: url(:/light/background.png); font-style: semibold; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;");
    ui->volume_sLabel->setStyleSheet("font-family: Montserrat; background-image: url(:/light/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #D2E4CD;");
    ui->volume_lLabel->setStyleSheet("font-family: Montserrat; background-image: url(:/light/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #5E8500;");

    if (db.getProductVolume(checkOption, drinkSize) < 1000){
        ui->productLabel->setText((db.getProductName(checkOption)) + " " + QString::number(db.getProductVolume(checkOption, drinkSize)) + " " + db.getUnits(checkOption));
    }else{
        ui->productLabel->setText((db.getProductName(checkOption)) + " " + QString::number(db.getProductVolume(checkOption, drinkSize)/1000) + "L");
    }

    db.closeDB();
    

}

// on_Large_Order button listener
void paySelect::on_orderBig_Button_clicked()
{
    QString bitmap_location;

    int checkOption = idlePage->userDrinkOrder->getOption();

    if(checkOption > 0 && checkOption <= 9) {
        bitmap_location.append(":/light/4_pay_select_page_l_");
        bitmap_location.append(QString::number(checkOption));
        bitmap_location.append(".png");
    } else {
        bitmap_location = ":/light/4_pay_select_page_l_1.png";
    }

    QPixmap background(bitmap_location);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    idlePage->userDrinkOrder->setDrinkSize(LARGE_DRINK);
    _selectIdleTimeoutSec = 140;

    DbManager db(DB_PATH);
//    db.addPageClick("Large Drink Size Selected");

    char drinkSize = 'l';

    ui->priceLabel->setText("$"+QString::number(db.getProductPrice(idlePage->userDrinkOrder->getOption(), drinkSize), 'f', 2));
    // ui->totalPriceLabel->setText("$"+QString::number(db.getProductPrice(idlePage->userDrinkOrder->getOption(), drinkSize), 'f', 2));
    updatePriceAfterPromo(promoPercent);
    ui->price_sLabel->setStyleSheet("font-family: Montserrat; background-image: url(:/light/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;");
    ui->price_lLabel->setStyleSheet("font-family: Montserrat; background-image: url(:/light/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #FFFFFF;");
    ui->volume_lLabel->setStyleSheet("font-family: Montserrat; background-image: url(:/light/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #D2E4CD;");
    ui->volume_sLabel->setStyleSheet("font-family: Montserrat; background-image: url(:/light/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #5E8500;");

    if (db.getProductVolume(checkOption, drinkSize) < 1000){
        ui->productLabel->setText((db.getProductName(checkOption)) + " " + QString::number(db.getProductVolume(checkOption, drinkSize)) + " " + db.getUnits(checkOption));
    }else{
        ui->productLabel->setText((db.getProductName(checkOption)) + " " + QString::number(db.getProductVolume(checkOption, drinkSize)/1000) + "L");
    }

    db.closeDB();
}

size_t WriteCallback_coupon(char* contents, size_t size, size_t nmemb, void *userp){
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void paySelect::on_promoCodeInput_clicked(){
    QObject* button = QObject::sender();
    ui->promoCode->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #5E8580;border-color:#5E8580;");
    // ui->promoInputButton->hide();
    ui->promoKeyboard->show();
    ui->promoCode->show();
}

void paySelect::updatePriceAfterPromo(double discountPercent){
    double discount;
    QString old_price = (ui->priceLabel->text()).split("$")[1];
    double price = old_price.toDouble();
    discount = discountPercent * price /100;
    price = (100-discountPercent) * price / 100;
    ui->discountLabel->setText("-$"+QString::number(discount,  'f', 2));
    idlePage->userDrinkOrder->setPrice(price);
    ui->totalPriceLabel->setText("$"+QString::number(price, 'f', 2));

}

void paySelect::on_applyPromo_Button_clicked()
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



void paySelect::buttonWasClicked(int buttonID){

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
