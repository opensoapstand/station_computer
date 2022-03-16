
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
pageProduct::pageProduct(QWidget *parent) : QWidget(parent),
                                            ui(new Ui::pageProduct)
{
    ui->setupUi(this);

    //int product_slot___ = 99;


    orderSizeButtons[0] = ui->orderSmall_Button;
    orderSizeButtons[1] = ui->orderMedium_Button;
    orderSizeButtons[2] = ui->orderBig_Button;
    orderSizeButtons[3] = ui->orderCustom_Button;

// reset_and_show_page_elements();
   
  
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



void pageProduct::reset_and_show_page_elements(){
    int product_slot___ = idlePage->currentProductOrder->getSelectedSlot();

    if (! (product_slot___ > 0 && product_slot___ <= SLOT_COUNT)){
         qDebug() << "ERROR: NO DEFAULT order size" << endl;
    }


    QString bitmap_location;

#ifdef CUSTOM_VOLUME_DISPENSING
    bitmap_location = "/home/df-admin/production/references/page_product_generic.png";
    uint16_t orderSizeButtons_xywh[4][4] = {{580,960,131,126},{730,960,131,126},{880,960,131,126},{570,110,421,111}};


    
#else
    uint16_t orderSizeButtons_xywh[4][4] = {{564,1088,209,126},{1,1,1,1},{790,1087,198,126},{1,1,1,1}};
    if (product_slot___ > 0 && product_slot___ <= SLOT_COUNT)
    {
        bitmap_location.append("/home/df-admin/production/references/4_pay_select_page_l_");
        bitmap_location.append(QString::number(idlePage->currentProductOrder->getSelectedSlot()));
        bitmap_location.append(".png");
    }
    else
    {
        bitmap_location = "/home/df-admin/production/references/4_pay_select_page_l_1.png";
    }

#endif

    qDebug() << bitmap_location << endl;
    QPixmap background(bitmap_location);

    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    //ordersize buttons
      /* Hacky transparent button */
    ui->previousPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->pagePayment_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->mainPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->promoKeyboard->setStyleSheet(" background-image: url(/home/df-admin/production/references/soapstand-keyboard.png); }");

    // TODO: ADD buttons to select size/price of drink
    // ui->orderSmall_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    // ui->orderBig_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    for(int8_t i=0;i<4;i++){
        orderSizeButtons[i]->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
        orderSizeButtons[i]->move(orderSizeButtons_xywh[i][0],orderSizeButtons_xywh[i][1]);
        orderSizeButtons[i]->resize(orderSizeButtons_xywh[i][2],orderSizeButtons_xywh[i][3]);
    }




    idlePage->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);

    ui->mainPage_Button->setEnabled(true);
    ui->previousPage_Button->setEnabled(true);

    // char drinkSize;
    // if (idlePage->currentProductOrder->getSelectedSizeOption() == SIZE_SMALL_INDEX)
    // {
    //     drinkSize = 's';
    // }
    // if (idlePage->currentProductOrder->getSelectedSizeOption() == SIZE_LARGE_INDEX)
    // {
    //     drinkSize = 'l';
    // }

   
  

    qDebug() << "ahoyy1";
    DbManager db(DB_PATH);
    ui->priceLabel->setText("$" + QString::number(selectedProductOrder->getSelectedProductPrice(), 'f', 2));
    ui->totalPriceLabel->setText("$" + QString::number(selectedProductOrder->getSelectedProductPrice(), 'f', 2));

    ui->label_price_small->setText("$" + QString::number(db.getProductPrice(product_slot___, 's'), 'f', 2));
    ui->label_price_large->setText("$" + QString::number(db.getProductPrice(product_slot___, 'l'), 'f', 2));
     db.closeDB();
    ui->discountLabel->setText("-$0.00");

    ui->label_price_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;");
    ui->label_price_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #FFFFFF;");
    
    ui->promoCode->clear();
    ui->promoCode->hide();
    promoPercent = 0.0;

    set_label_volume(ui->label_size_small, product_slot___, 's');
    set_label_volume(ui->label_size_large, product_slot___, 'l');
    set_label_product(product_slot___);
}

void pageProduct::resizeEvent(QResizeEvent *event)
{
  
   reset_and_show_page_elements();



    if (selectIdleTimer == nullptr)
    {
        selectIdleTimer = new QTimer(this);
        selectIdleTimer->setInterval(40);
        connect(selectIdleTimer, SIGNAL(timeout()), this, SLOT(onSelectTimeoutTick()));
    }

    //    qDebug() << "Start pageProduct Timers" << endl;
    selectIdleTimer->start(1000);
    _selectIdleTimeoutSec = 400;

   
    // do after db closed
   
}


/*
 * Page Tracking reference to Select Drink, Payment Page and Idle page
 */
void pageProduct::setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, pagePayment *pagePayment, page_help *pageHelp)
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
    selectedProductOrder = idlePage->currentProductOrder;

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

void pageProduct::cancelTimers()
{
    selectIdleTimer->stop();
}

/* GUI */
void pageProduct::on_previousPage_Button_clicked()
{

    //    qDebug() << "pageProduct: Previous button" << endl;
    while (!stopSelectTimers())
    {
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
    qDebug() << "ahoyy11";
    DbManager db(DB_PATH);

    this->stopSelectTimers();
    selectIdleTimer->stop();

    if (db.getPaymentMethod(idlePage->currentProductOrder->getSelectedSlot()) == "qr" || db.getPaymentMethod(idlePage->currentProductOrder->getSelectedSlot()) == "tap")
    {
        CURL *curl;
        CURLcode res;
        curl = curl_easy_init();

        curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/ping");

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            wifiError->showEvent(wifiErrorEvent);
            wifiError->showFullScreen();
            this->hide();
        }
        else
        {

            ui->totalPriceLabel->text();
            paymentPage->showFullScreen();
            this->hide();
        }
    }
    else if (db.getPaymentMethod(idlePage->currentProductOrder->getSelectedSlot()) == "barcode" || db.getPaymentMethod(idlePage->currentProductOrder->getSelectedSlot()) == "plu")
    {
        db.closeDB();
        dispensingPage->showEvent(dispenseEvent);
        dispensingPage->showFullScreen();
        this->hide();
    }
    db.closeDB();
}

void pageProduct::showEvent(QShowEvent *event)
{
    int product_slot___ = idlePage->currentProductOrder->getSelectedSlot();
    qDebug() << "ahoyy12";
    DbManager db(DB_PATH);

    ui->mainPage_Button->setEnabled(true);
    ui->previousPage_Button->setEnabled(true);

    // char drinkSize;
    // if (idlePage->currentProductOrder->getSelectedSizeOption() == SIZE_SMALL_INDEX)
    // {
    //     drinkSize = 's';
    // }
    // if (idlePage->currentProductOrder->getSelectedSizeOption() == SIZE_LARGE_INDEX)
    // {
    //     drinkSize = 'l';
    // }
    ui->priceLabel->setText("$" + QString::number(selectedProductOrder->getSelectedProductPrice(), 'f', 2));
    ui->totalPriceLabel->setText("$" + QString::number(selectedProductOrder->getSelectedProductPrice()));
    
    ui->label_price_small->setText("$" + QString::number(db.getProductPrice(product_slot___, 's'), 'f', 2));
    ui->label_price_large->setText("$" + QString::number(db.getProductPrice(product_slot___, 'l'), 'f', 2));

    ui->label_price_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: normal; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;");
    ui->label_price_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: normal; font-weight: bold; font-size: 36px; line-height: 44px; color: #FFFFFF;");
    ui->label_size_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #D2E4CD;");
    ui->label_size_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #5E8500;");
    ui->promoCode->clear();
    ui->promoCode->hide();
    promoPercent = 0.0;

    db.closeDB();
    set_label_product(product_slot___);
    set_label_volume(ui->label_size_small, product_slot___, 's');
    set_label_volume(ui->label_size_large, product_slot___, 'l');
}

void pageProduct::set_label_product(int product_slot___)
{

    qDebug() << "ahoyy1erer3";
    DbManager db(DB_PATH);
    QString product_name = db.getProductName(product_slot___);
    db.closeDB();

    ui->productLabel->setText(product_name + " " + selectedProductOrder->getSelectedSizeToVolumeWithCorrectUnits());
}

void pageProduct::set_label_volume(QLabel *label, int product_slot___, char drinkSize)
{
    // qDebug() << "db for label volume" ;

    label->setText(selectedProductOrder->getSelectedSizeToVolumeWithCorrectUnits());
}


void pageProduct::onSelectTimeoutTick()
{
    if (--_selectIdleTimeoutSec >= 0)
    {
        //        qDebug() << "pageProduct: Tick Down - " << _selectIdleTimeoutSec << endl;
    }
    else
    {
        //        qDebug() << "Timer Done!" << _selectIdleTimeoutSec << endl;
        selectIdleTimer->stop();

        mainPage();
    }
}

bool pageProduct::stopSelectTimers()
{
    //    qDebug() << "Stop pageProduct Timers" << endl;
    if (selectIdleTimer != nullptr)
    {
        // qDebug() << "Enter Stop" << endl;
        selectIdleTimer->stop();
        return true;
    }
    else
    {
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

void pageProduct::loadOrderSize(int sizeIndex){
     // QString bitmap_location;

    int product_slot___ = idlePage->currentProductOrder->getSelectedSlot();

    //DF_QT_SIZES tmp [6] = {INVALID_DRINK, SIZE_SMALL_INDEX, MEDIUM_DRINK, SIZE_LARGE_INDEX, CUSTOM_DRINK, TEST_DRINK};

    
    selectedProductOrder->setSelectedSize(sizeIndex);
    _selectIdleTimeoutSec = 140;

    ui->priceLabel->setText("$" + QString::number(selectedProductOrder->getSelectedProductPrice(), 'f', 2));
    ui->totalPriceLabel->setText("$" + QString::number(selectedProductOrder->getSelectedProductPrice(), 'f', 2));
    // updatePriceAfterPromo(promoPercent);
    ui->label_price_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: bold; font-size: 36px; line-height: 44px; color: #FFFFFF;");
    ui->label_price_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;");
    ui->label_size_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #D2E4CD;");
    ui->label_size_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #5E8500;");

    // db.closeDB();
    // // after db closed
    set_label_product(product_slot___);
}
// on_Small_Order button listener
void pageProduct::on_orderSmall_Button_clicked()
{
    qDebug() << "button small";
    this->loadOrderSize(SIZE_SMALL_INDEX);
    // QString bitmap_location;

    // int product_slot___ = idlePage->currentProductOrder->getSelectedSlot();

    // if (product_slot___ > 0 && product_slot___ <= 9)
    // {
    //     bitmap_location.append("/home/df-admin/production/references/4_pay_select_page_s_");
    //     bitmap_location.append(QString::number(product_slot___));
    //     bitmap_location.append(".png");
    // }
    // else
    // {
    //     bitmap_location = "/home/df-admin/production/references/4_pay_select_page_s_1.png";
    // }

    // QPixmap background(bitmap_location);
    // background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    // QPalette palette;
    // palette.setBrush(QPalette::Background, background);
    // this->setPalette(palette);

    // idlePage->currentProductOrder->setSelectedSize(SIZE_SMALL_INDEX);
    // _selectIdleTimeoutSec = 140;

    // char drinkSize = 's';
    // qDebug() << "ahoyy13";
    // DbManager db(DB_PATH);

    // ui->priceLabel->setText("$" + QString::number(db.getProductPrice(idlePage->currentProductOrder->getSelectedSlot(), drinkSize), 'f', 2));
    // // ui->totalPriceLabel->setText("$"+QString::number(db.getProductPrice(idlePage->currentProductOrder->getSelectedSlot(), drinkSize), 'f', 2));
    // updatePriceAfterPromo(promoPercent);
    // ui->label_price_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: bold; font-size: 36px; line-height: 44px; color: #FFFFFF;");
    // ui->label_price_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;");
    // ui->label_size_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #D2E4CD;");
    // ui->label_size_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #5E8500;");

    // db.closeDB();
    // // after db closed
    // set_label_product(product_slot___, drinkSize);
}

// on_Large_Order button listener
void pageProduct::on_orderBig_Button_clicked()
{
    qDebug() << "button biiig";
    this->loadOrderSize(SIZE_LARGE_INDEX);
    // QString bitmap_location;

    // int product_slot___ = idlePage->currentProductOrder->getSelectedSlot();

    // if (product_slot___ > 0 && product_slot___ <= 9)
    // {
    //     bitmap_location.append("/home/df-admin/production/references/4_pay_select_page_l_");
    //     bitmap_location.append(QString::number(product_slot___));
    //     bitmap_location.append(".png");
    // }
    // else
    // {
    //     bitmap_location = "/home/df-admin/production/references/4_pay_select_page_l_1.png";
    // }

    // QPixmap background(bitmap_location);
    // background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    // QPalette palette;
    // palette.setBrush(QPalette::Background, background);
    // this->setPalette(palette);

    // idlePage->currentProductOrder->setSelectedSize(SIZE_LARGE_INDEX);
    // _selectIdleTimeoutSec = 140;
    // qDebug() << "ahoyy14";
   
    // DbManager db(DB_PATH);
    // //    db.addPageClick("Large Drink Size Selected");

    // char drinkSize = 'l';

    // ui->priceLabel->setText("$" + QString::number(db.getProductPrice(idlePage->currentProductOrder->getSelectedSlot(), drinkSize), 'f', 2));
    
    // updatePriceAfterPromo(promoPercent);
    // ui->label_price_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #5E8580;");
    // ui->label_price_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: light; font-weight: bold; font-size: 36px; line-height: 44px; color: #FFFFFF;");
    // ui->label_size_large->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #D2E4CD;");
    // ui->label_size_small->setStyleSheet("font-family: Montserrat; background-image: url(/home/df-admin/production/references/background.png); font-style: semibold; font-weight: semibold; font-size: 20px; line-height: 24px; color: #5E8500;");

    // db.closeDB();
    // // do after db closed
    // set_label_product(product_slot___, drinkSize);
}

size_t WriteCallback_coupon(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void pageProduct::on_promoCodeInput_clicked()
{
    QObject *button = QObject::sender();
    ui->promoCode->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #5E8580;border-color:#5E8580;");
    // ui->promoInputButton->hide();
    ui->promoKeyboard->show();
    ui->promoCode->show();
}

void pageProduct::updatePriceAfterPromo(double discountPercent)
{
    double discount;
    QString old_price = (ui->priceLabel->text()).split("$")[1];
    double price = old_price.toDouble();
    discount = discountPercent * price / 100;
    price = (100 - discountPercent) * price / 100;
    ui->discountLabel->setText("-$" + QString::number(discount, 'f', 2));
    
    idlePage->currentProductOrder->setSelectedOverrulePrice(price);
    ui->totalPriceLabel->setText("$" + QString::number(price, 'f', 2));
}

void pageProduct::on_applyPromo_Button_clicked()
{

    QString promocode = ui->promoCode->text();
    ui->promoKeyboard->hide();
    CURL *curl;
    CURLcode res;
    long http_code = 0;
    if (promocode != "")
    {
        readBuffer.clear();
        curl = curl_easy_init();

        curl_easy_setopt(curl, CURLOPT_URL, ("https://soapstandportal.com/api/coupon/find/" + promocode).toUtf8().constData());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback_coupon);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (res != CURLE_OK)
        {
            ui->promoCode->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #f44336;border-color:#f44336;");
            qDebug() << "Invalid Coupon" << endl;
        }
        else
        {
            int new_percent;

            if (http_code == 200)
            {
                json coupon_obj = json::parse(readBuffer);
                if (coupon_obj["active"])
                {
                    new_percent = coupon_obj["discount_amount"];
                    updatePriceAfterPromo(new_percent);
                    promoPercent = new_percent;
                }
                else
                {
                    qDebug() << "Invalid Coupon" << endl;
                    ui->promoCode->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #75756f;border-color:#f44336;");
                }
            }
            else
            {
                qDebug() << "Invalid Coupon" << endl;
                ui->promoCode->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #f44336;border-color:#f44336;");
            }
        }
    }
}

void pageProduct::buttonWasClicked(int buttonID)
{

    QAbstractButton *buttonpressed = ui->buttonGroup->button(buttonID);
    // qDebug() << buttonpressed->text();
    QString buttonText = buttonpressed->objectName();

    if (buttonText == "backspace")
    {
        ui->promoCode->backspace();
    }
    else if (buttonText == "done")
    {
        if (ui->promoCode->text() == "")
        {
            ui->promoCode->hide();
        }
        ui->promoKeyboard->hide();
    }
    else if (buttonText.mid(0, 3) == "num")
    {
        ui->promoCode->setText(ui->promoCode->text() + buttonText.mid(3, 1));
    }
    else
    {
        ui->promoCode->setText(ui->promoCode->text() + buttonText);
    }
}

void pageProduct::on_orderCustom_Button_clicked()
{
     qDebug() << "button custom";
}

void pageProduct::on_orderMedium_Button_clicked()
{
qDebug() << "button medium";
}
