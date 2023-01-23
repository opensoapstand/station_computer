
//***************************************
//
// page_product.cpp
// GUI class for user to select size and
// payment for drink.
//
// Allows navigation to First product page
// payment page and page_idle page
//
// created: 05-04-2022
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "page_sendFeedback.h"
#include "ui_page_sendFeedback.h"
#include <iostream>
#include <string>
#include <cmath>

#include "page_payment.h"
#include "page_select_product.h"
#include "page_idle.h"
#include <curl/curl.h>
#include <json.hpp>

using json = nlohmann::json;

// CTOR
page_sendFeedback::page_sendFeedback(QWidget *parent) : QWidget(parent),
                                            ui(new Ui::page_sendFeedback)
{
    qDebug()<< "IN send feedback";
    ui->setupUi(this);
    connect(ui->previousPage_Button, SIGNAL(clicked()), this, SLOT(on_previousPage_Button_clicked()));

    // ui->promoInputButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    // ui->promoCode->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    // ui->promoButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    // QString css_title = "QLabel{"
    //                     "position: absolute;"
    //                     "width: 870px;"
    //                     "height: 102px;"
    //                     "left: 99px;"
    //                     "top: 261px;"
    //                     "font-family: 'Montserrat';"
    //                     "font-style: normal;"
    //                     "font-weight: 500;"
    //                     "font-size: 42px;"
    //                     "line-height: 48px;"
    //                     "text-transform: capitalize;"
    //                     "color: #438080;"
    //                     "}";
    // ui->label_product_title->setStyleSheet(css_title);
    // QString css_description = "QLabel{"
    //                           "position: absolute;"
    //                           //   "width: 894px;"
    //                           //   "height: 252px;"
    //                           //   "left: 95px;"
    //                           //   "top: 474px;"
    //                           "font-family: 'Montserrat';"
    //                           "font-style: normal;"
    //                           "font-weight: 400;"
    //                           "font-size: 24px;"
    //                           "line-height: 36px;"
    //                           "color: #438080;"
    //                           "}";
    // ui->label_product_description->setStyleSheet(css_description);
     ui->previousPage_Button->setStyleSheet("QPushButton { color:#555555; background-color: transparent; border: 0px }");
        ui->previousPage_Button->setStyleSheet(
"QPushButton {"

"font-family: 'Brevia';"
"font-style: normal;"
"font-weight: 75;"
"font-size: 32px;"
"line-height: 99px;"
"letter-spacing: 1.5px;"
"text-transform: lowercase;"
"color: #003840;"
"text-align: center;"
"qproperty-alignment: AlignCenter;"
"border: none;"
"}");
        ui->previousPage_Button->setText("<-Back");

    // ui->label_product_description->setWordWrap(true);


    // QString css_discount_name = "QLabel{"
    //                             "font-family: 'Montserrat';"
    //                             "font-style: normal;"
    //                             "font-weight: 400;"
    //                             "font-size: 24px;"
    //                             "line-height: 29px;"
    //                             "color: #58595B;"
    //                             "}";
    // ui->label_invoice_discount_name->setStyleSheet(css_discount_name);
    // ui->label_invoice_discount_name->hide();
    // ui->label_invoice_discount_amount->hide();
    // ui->label_discount_tag->hide();
    // ui->promoCode->clear();
    // ui->promoCode->hide();
    // ui->promoKeyboard->hide();

    // couponHandler();

    // {
    //     selectIdleTimer = new QTimer(this);
    //     selectIdleTimer->setInterval(40);
    //     connect(ui->promoButton, SIGNAL(clicked()), this, SLOT(on_applyPromo_Button_clicked()));
    //     connect(ui->promoInputButton, SIGNAL(clicked()), this, SLOT(on_promoCodeInput_clicked()));
    //     connect(ui->buttonGroup, SIGNAL(buttonPressed(int)), this, SLOT(keyboardButtonPressed(int)));
    //     connect(selectIdleTimer, SIGNAL(timeout()), this, SLOT(onSelectTimeoutTick()));
    // }
    // // QString paymentMethod = selectedProductOrder->getSelectedPaymentMethod();
    // // if(paymentMethod== "qr" || paymentMethod=="tap"){
    // //     ui->label_payment_page->setText("Pay Now");
    // // }
    // // else{
    // //     ui->label_payment_page->setText("Next");
    // }
}

/*
 * Page Tracking reference to Select Drink, Payment Page and Idle page
 */
void page_sendFeedback::setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_payment *page_payment, page_help *pageHelp, pageProduct *page_product, pagethankyou *page_thankyou )
{
    this->p_page_select_product = pageSelect;
    this->paymentPage = page_payment;
    this->p_page_idle = pageIdle;
    this->p_page_dispense = page_dispenser;
    this->p_page_help = pageHelp;
    this->p_page_wifi_error = pageWifiError;
    this->p_page_product = page_product;
    // ui->promoCode->clear();
    // ui->promoCode->hide();
    // ui->label_invoice_discount_amount->hide();
    // ui->label_invoice_discount_name->hide();
    // ui->label_discount_tag->hide();

    // selectedProductOrder = p_page_idle->currentProductOrder;
    // p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_ORDER_OVERVIEW_PATH);

    // couponHandler();
}

// DTOR
page_sendFeedback::~page_sendFeedback()
{
    delete ui;
}

void page_sendFeedback::setDefaultSize()
{

    bool default_size_set = false;

    for (uint8_t i = 0; i < 4; i++)
    {
        int size = product_sizes[i];
        if (selectedProductOrder->getLoadedProductSizeEnabled(size))
        {
            selectedProductOrder->setSelectedSize(product_sizes[i]);
        }
    }
}



/* GUI */

void page_sendFeedback::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Product Overview>>>>>>>>>";
    QWidget::showEvent(event);
        
    
    // selectedProductOrder->loadSelectedProductProperties();
    // reset_and_show_page_elements();
}


void page_sendFeedback::resizeEvent(QResizeEvent *event)
{
    qDebug() << "\n---Page_product Overview: resizeEvent";
    // QWidget::resizeEvent(event);
    // selectedProductOrder->loadSelectedProductProperties();
    // reset_and_show_page_elements();
}

void page_sendFeedback::onSelectTimeoutTick()
{
    if (--_selectIdleTimeoutSec >= 0)
    {
        //        qDebug() << "page_sendFeedback: Tick Down - " << _selectIdleTimeoutSec << endl;
    }
    else
    {
        // qDebug() << "Timer Done!" << _selectIdleTimeoutSec << endl;
        selectIdleTimer->stop();

        mainPage();
    }
}

void page_sendFeedback::reset_and_show_page_elements()
{
    QString bitmap_location;
    ui->label_product_photo->setStyleSheet("QLabel{border: 0px solid black;}");
    ui->label_product_title->setText(selectedProductOrder->getSelectedProductName());
    ui->label_product_description->setText(selectedProductOrder->getLoadedProductDescription());
    p_page_idle->addPictureToLabel(ui->label_product_photo, p_page_idle->currentProductOrder->getSelectedProductPicturePath());
    ui->label_selected_price->setText("$" + QString::number(selectedProductOrder->getSelectedPrice(), 'f', 2));
    qDebug () << "SElected size" << selectedProductOrder->getSelectedVolume();
    ui->label_invoice_price->setText("$" + QString::number(selectedProductOrder->getSelectedPrice(), 'f', 2));
    QString selected_volume;
    if(selectedProductOrder->getUnitsForSelectedSlot()== "oz"){
        selected_volume= QString::number(ceil((double)selectedProductOrder->getSelectedVolume() * (double)ML_TO_OZ * 1.0));   
    }
    else{
        selected_volume = QString::number(selectedProductOrder->getSelectedVolume());   

    }
        ui->label_selected_volume->setText(selected_volume + " " + selectedProductOrder->getUnitsForSelectedSlot());
    //Reset the discount percentage to 0 and clear promo code field

    ui->promoCode->clear();
    selectedProductOrder->setDiscountPercentageFraction((0 * 1.0) / 100);
    ui->label_invoice_discount_amount->hide();
    ui->label_discount_tag->hide();
    ui->previousPage_Button->setEnabled(true);
    ui->mainPage_Button->setEnabled(true);
    /* Hacky transparent button */
    ui->previousPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->page_payment_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    // ui->page_payment_Button->setStyleSheet("QPushButton { background-color: red; border: 0px };QPushButton:pressed { background-color: green; border: 10px }");
    
    ui->mainPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    QString keyboard = KEYBOARD_IMAGE_PATH;
    QString keyboard_style_sheet = " background-image: url(" + keyboard + "); }";
    ui->promoKeyboard->setStyleSheet(keyboard_style_sheet);

    // loadProdSpecs();
    selectIdleTimer->start(1000);
    _selectIdleTimeoutSec = 400;
}



bool page_sendFeedback::stopSelectTimers()
{
    //    qDebug() << "Stop page_sendFeedback Timers" << endl;
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

void page_sendFeedback::mainPage()
{
    //    qDebug() << "page_sendFeedback: mainPage button" << endl;
    this->stopSelectTimers();
    selectIdleTimer->stop();
    // qDebug() << "product to idle";
    // p_page_idle->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_idle);
}

void page_sendFeedback::on_mainPage_Button_clicked()
{
    //    qDebug() << "page_sendFeedback: helpPage button" << endl;
    this->stopSelectTimers();
    selectIdleTimer->stop();
    // p_page_help->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_help);
}




// void page_sendFeedback::updatePriceAfterPromo(double discountPercent)
// {
//     // QString normal_price = (ui->label_invoice_price->text()).split("$")[1];
//     // double price = normal_price.toDouble();
//     selectedProductOrder->setDiscountPercentageFraction(discountPercent / 100);

//     ui->label_invoice_discount_amount->show();
//     ui->label_invoice_discount_name->show();

//     double discount;
//     discount = selectedProductOrder->getDiscountPercentageFraction() * selectedProductOrder->getPrice(selectedProductOrder->getSelectedSize());
//     ui->label_invoice_discount_amount->setText("-$" + QString::number(discount, 'f', 2));

//     ui->label_invoice_discount_amount->setText("-$" + QString::number(selectedProductOrder->getDiscount(), 'f', 2));
//     ui->label_invoice_price_total->setText("$" + QString::number(selectedProductOrder->getSelectedPriceCorrected(), 'f', 2));
// }

void page_sendFeedback::keyboardButtonPressed(int buttonID)
{

    QAbstractButton *buttonpressed = ui->buttonGroup->button(buttonID);
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
void page_sendFeedback::on_previousPage_Button_clicked()
{

       qDebug() << "page_sendFeedback: Previous button" << endl;
    while (!stopSelectTimers())
    {
    };
    selectIdleTimer->stop();

    p_page_idle->pageTransition(this, p_page_product);
}


void page_sendFeedback::on_promoCodeInput_clicked()
{
    // QObject *button = QObject::sender();
    // ui->promoCode->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #5E8580;border-color:#5E8580;");
    // // ui->promoInputButton->hide();
    // ui->promoKeyboard->show();
    // qDebug() << "show promo keyboard.";
    // ui->promoCode->show();
}

void page_sendFeedback::on_page_payment_Button_clicked()
{
    // qDebug() << "page_sendFeedback: Pay button";

    // ui->mainPage_Button->setEnabled(false);
    // ui->previousPage_Button->setEnabled(false);

    // this->stopSelectTimers();
    // selectIdleTimer->stop();
    // QString paymentMethod = selectedProductOrder->getSelectedPaymentMethod();

    // if (paymentMethod == "qr" || paymentMethod == "tap")
    // {
    //     CURL *curl;
    //     CURLcode res;
    //     curl = curl_easy_init();

    //     if (!curl)
    //     {
    //         qDebug() << "page_sendFeedback: cURL failed init";
    //         return;
    //     }

    //     curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/ping");
    //     curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);

    //     res = curl_easy_perform(curl);
    //     if (res != CURLE_OK)
    //     {
    //         qDebug() << "ERROR: Failed to reach soapstandportal. error code: " + QString::number(res);

    //         // p_page_wifi_error->showEvent(wifiErrorEvent);
    //         // p_page_wifi_error->showFullScreen();
    //         // this->hide();
    //         p_page_idle->pageTransition(this, p_page_wifi_error);
    //     }
    //     else
    //     {
    //         QString feedback = QString::fromUtf8(readBuffer.c_str());
    //         qDebug() << "Server feedback readbuffer: " << feedback;

    //         ui->label_invoice_price->text();

    //         // paymentPage->showFullScreen();
    //         // this->hide();
    //         p_page_idle->pageTransition(this, paymentPage);
    //     }
    //     curl_easy_cleanup(curl);
    //     readBuffer = "";
    // }
    // else if (paymentMethod == "barcode" || paymentMethod == "plu")
    // {
    //     // p_page_dispense->showEvent(dispenseEvent); // todo Lode: this enabled together with showfullscreen calls the showEvent twice. only showevent, does not display the dispense page though.
    //     // p_page_dispense->showFullScreen();
    //     // this->hide();
    //     p_page_idle->pageTransition(this, p_page_dispense);
    // }
    // else
    // {
    //     qDebug() << "WARNING: No payment method detected.";
    //     // p_page_dispense->showFullScreen();
    //     // this->hide();
    //     p_page_idle->pageTransition(this, p_page_dispense);
    // }
}
