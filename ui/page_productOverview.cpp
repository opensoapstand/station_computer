
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

#include "page_productOverview.h"
#include "ui_page_productOverview.h"
#include <iostream>
#include <string>
#include <cmath>

#include "page_payment.h"
#include "page_select_product.h"
#include "page_idle.h"
#include <curl/curl.h>
#include <json.hpp>
#include <QMovie>
using json = nlohmann::json;

// CTOR
pageProductOverview::pageProductOverview(QWidget *parent) : QWidget(parent),
                                                            ui(new Ui::pageProductOverview)
{
    ui->setupUi(this);

    ui->promoInputButton->setStyleSheet("QPushButton { border: 1px solid #5E8580}");
    ui->promoInputButton->setText("Discount code");
    ui->promoCode->setStyleSheet("QPushButton { background-color: transparent; border: 1px solid #5E8580 }");
    ui->page_payment_Button->show();
    ui->promoKeyboard->hide();
    // ui->label_gif->hide();
    QString css_title = "QLabel{"
                        "position: absolute;"
                        "width: 870px;"
                        "height: 102px;"
                        "left: 99px;"
                        "top: 261px;"
                        "font-family: 'Montserrat';"
                        "font-style: normal;"
                        "font-weight: 500;"
                        "font-size: 42px;"
                        "line-height: 48px;"
                        "text-transform: capitalize;"
                        "color: #438080;"
                        "}";
    //  ui->label_product_title->setStyleSheet(css_title);
    QString css_description = "QLabel{"
                              "position: absolute;"
                              //   "width: 894px;"
                              //   "height: 252px;"
                              //   "left: 95px;"
                              //   "top: 474px;"
                              "font-family: 'Montserrat';"
                              "font-style: normal;"
                              "font-weight: 400;"
                              "font-size: 24px;"
                              "line-height: 36px;"
                              "color: #438080;"
                              "}";

    ui->selectProductPage_Button->setStyleSheet("QPushButton { color:#555555; background-color: transparent; border: 0px }");
    ui->selectProductPage_Button->setStyleSheet(
        "QPushButton {"

        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 32px;"
        "line-height: 99px;"
        "letter-spacing: 1.5px;"
        "text-transform: lowercase;"
        "color: #003840;"
        "text-align: left;"
        "qproperty-alignment: AlignCenter;"
        "border: none;"
        "}");
    ui->selectProductPage_Button->setText("<- Back");
    // ui->label_product_description->setStyleSheet(css_description);

    // ui->label_product_description->setWordWrap(true);
    ui->label_selected_volume->setStyleSheet(
        "QLabel {"
        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 32px;"
        "line-height: 99px;"
        "letter-spacing: 1.5px;"
        "color: #003840;"
        "text-align: center;"
        "qproperty-alignment: AlignCenter;"
        "border: 2px solid #5E8580;"
        "}");

    QString orderOverviewStyle = "QLabel {"
                                 "font-family: 'Montserrat';"
                                 "font-style: normal;"
                                 "font-weight: 600;"
                                 "font-size: 28px;"
                                 "line-height: 40px;"
                                 "letter-spacing: 0px;"
                                 "color: #58595B;"
                                 "}";
    ui->label_invoice_name->setStyleSheet(orderOverviewStyle);
    ui->label_invoice_price->setStyleSheet(orderOverviewStyle);
    ui->label_discount_tag->setStyleSheet(orderOverviewStyle);
    ui->label_discount_tag->setText("Discount");
    ui->label_invoice_discount_amount->setStyleSheet(orderOverviewStyle);
    ui->label_invoice_box->setStyleSheet(
        "QLabel {"

        "border: 2px solid #5E8580;"
        "}");

    ui->promoButton->setStyleSheet(
        "QPushButton {"
        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 32px;"
        "line-height: 99px;"
        "letter-spacing: 1.5px;"
        "text-transform: lowercase;"
        "background-color: #5E8580;"
        "border: 1px solid #3D6675;"
        "color: #FFFFFF;"
        "text-align: center;"
        "qproperty-alignment: AlignCenter;"
        "}");
    ui->promoButton->setText("Apply");
    ui->promoButton->hide();

    // ui->label_notify_us->setStyleSheet(
    //     "QLabel {"

    //     "font-family: 'Brevia';"
    //     "font-style: normal;"
    //     "font-weight: 75;"
    //     "font-size: 20px;"
    //     "line-height: 99px;"
    //     "letter-spacing: px;"
    //     "color: #FFFFFF;"
    //     "text-align: center;"
    //     "qproperty-alignment: AlignCenter;"
    //     "border: none;"
    //     "}");
    // ui->label_notify_us->setText("Help");

    ui->label_discount_code->setStyleSheet(
        "QLabel {"
        "font-family: 'Montserrat';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 24px;"
        "line-height: 99px;"
        "letter-spacing: 1.5px;"
        "text-transform: lowercase;"
        "color: #9F9F9F;"
        "text-align: center;"
        "qproperty-alignment: AlignCenter;"
        "}");
    ui->label_discount_code->setText("Discount code");

    ui->label_pay->setStyleSheet(
        "QLabel {"

        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 200;"
        "background-color: #5E8580;"
        "font-size: 48px;"
        "text-align: centre;"
        "line-height: auto;"
        "letter-spacing: 0px;"
        "qproperty-alignment: AlignCenter;"
        "border-radius: 20px;"
        "color: white;"
        "border: none;"
        "}");
    ui->label_pay->setText("Continue");

    QString css_discount_name = "QLabel{"
                                "font-family: 'Montserrat';"
                                "font-style: normal;"
                                "font-weight: 400;"
                                "font-size: 24px;"
                                "line-height: 29px;"
                                "color: #58595B;"
                                "}";
    ui->label_invoice_discount_name->setStyleSheet(css_discount_name);
    ui->label_invoice_discount_name->show();
    ui->label_invoice_discount_amount->show();
    ui->label_discount_tag->show();
    ui->promoCode->clear();
    ui->promoCode->show();
    // ui->promoButton->show();
    ui->promoKeyboard->hide();

    ui->label_total->setStyleSheet(
        "QLabel {"
        "font-family: 'Montserrat';"
        "font-style: normal;"
        "font-weight: 600;"
        "font-size: 36px;"
        "line-height: 40px;"
        "letter-spacing: 0px;"
        "color: #58595B;"
        "}");
    ui->label_total->setText("Total");
    couponHandler();

    {
        selectIdleTimer = new QTimer(this);
        selectIdleTimer->setInterval(40);
        // connect(ui->promoButton, SIGNAL(clicked()), this, SLOT(on_applyPromo_Button_clicked()));
        connect(ui->promoInputButton, SIGNAL(clicked()), this, SLOT(on_promoCodeInput_clicked()));
        connect(ui->buttonGroup, SIGNAL(buttonPressed(int)), this, SLOT(keyboardButtonPressed(int)));
        connect(selectIdleTimer, SIGNAL(timeout()), this, SLOT(onSelectTimeoutTick()));
    }

    ui->label_gif->setStyleSheet(
        "QLabel {"
        "font-family: 'Montserrat';"
        "font-style: normal;"
        "font-weight: 600;"
        "font-size: 36px;"
        "line-height: 40px;"
        "letter-spacing: 0px;"
        "color: #58595B;"
        "padding: 3px;"
        "border:8px solid #248C8C;"
        "border-radius: 20px;"
        "}");
    ui->label_gif->hide();
    // QString paymentMethod = selectedProductOrder->getSelectedPaymentMethod();
    // if(paymentMethod== "qr" || paymentMethod=="tap"){
    //     ui->label_payment_page->setText("Pay Now");
    // }
    // else{
    //     ui->label_payment_page->setText("Next");
    // }
}

/*
 * Page Tracking reference to Select Drink, Payment Page and Idle page
 */
void pageProductOverview::setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_payment *page_payment, page_help *pageHelp, pageProduct *page_product)
{
    this->p_page_select_product = pageSelect;
    this->paymentPage = page_payment;
    this->p_page_idle = pageIdle;
    this->p_page_dispense = page_dispenser;
    this->p_page_help = pageHelp;
    this->p_page_wifi_error = pageWifiError;
    this->p_page_product = page_product;
    ui->promoCode->clear();
    ui->promoCode->hide();
    ui->label_invoice_discount_amount->hide();
    ui->label_invoice_discount_name->hide();
    ui->label_discount_tag->hide();
    ui->label_gif->hide();
    selectedProductOrder = p_page_idle->currentProductOrder;

    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_ORDER_OVERVIEW_PATH);

    couponHandler();
}

// DTOR
pageProductOverview::~pageProductOverview()
{
    delete ui;
}

void pageProductOverview::setDefaultSize()
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

void pageProductOverview::cancelTimers()
{
    selectIdleTimer->stop();
}

/* GUI */

void pageProductOverview::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Product Overview>>>>>>>>>";
    QWidget::showEvent(event);

    selectedProductOrder->loadSelectedProductProperties();
    reset_and_show_page_elements();
}

void pageProductOverview::resizeEvent(QResizeEvent *event)
{
    qDebug() << "\n---Page_product Overview: resizeEvent";
    // QWidget::resizeEvent(event);
    // selectedProductOrder->loadSelectedProductProperties();
    reset_and_show_page_elements();
}

void pageProductOverview::onSelectTimeoutTick()
{
    if (--_selectIdleTimeoutSec >= 0)
    {
        //        qDebug() << "pageProductOverview: Tick Down - " << _selectIdleTimeoutSec << endl;
    }
    else
    {
        // qDebug() << "Timer Done!" << _selectIdleTimeoutSec << endl;
        selectIdleTimer->stop();

        mainPage();
    }
}

void pageProductOverview::reset_and_show_page_elements()
{
    QString bitmap_location;
    ui->label_product_photo->setStyleSheet("QLabel{border: 2px solid #5E8580;}");
    // ui->label_product_title->setText(selectedProductOrder->getSelectedProductName());
    // ui->label_product_description->setText(selectedProductOrder->getLoadedProductDescription());
    p_page_idle->addPictureToLabel(ui->label_product_photo, p_page_idle->currentProductOrder->getSelectedProductPicturePath());
    ui->label_selected_price->setText("$" + QString::number(selectedProductOrder->getSelectedPrice(), 'f', 2));
    qDebug() << "Selected size" << selectedProductOrder->getSelectedVolume();

   
    updatePrice();
    ui->label_invoice_name->setText(selectedProductOrder->getSelectedProductName());
    // ui->label_price_large->setText(selected_volume + " " + selectedProductOrder->getUnitsForSelectedSlot());

    // Reset the discount percentage to 0 and clear promo code field

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

bool pageProductOverview::stopSelectTimers()
{
    //    qDebug() << "Stop pageProductOverview Timers" << endl;
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

void pageProductOverview::mainPage()
{
    this->stopSelectTimers();
    p_page_idle->pageTransition(this, p_page_idle);
}

void pageProductOverview::on_mainPage_Button_clicked()
{
    this->stopSelectTimers();
    p_page_idle->pageTransition(this, p_page_help);
}

size_t WriteCallback_coupon1(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

// void pageProductOverview::updatePriceAfterPromo(double discountPercent)
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

void pageProductOverview::updatePrice()
{
     QString selected_volume;
    if (selectedProductOrder->getUnitsForSelectedSlot() == "oz")
    {
        selected_volume = QString::number(ceil((double)selectedProductOrder->getSelectedVolume() * (double)ML_TO_OZ * 1.0));
    }
    else
    {
        selected_volume = QString::number(selectedProductOrder->getSelectedVolume());
    }


    if (selectedProductOrder->getSelectedSize() == SIZE_CUSTOM_INDEX)
    {
        ui->label_selected_volume->setText("Custom Volume \n (maximum: " + selected_volume + " " + selectedProductOrder->getUnitsForSelectedSlot() + ")");

        double selectedPrice = selectedProductOrder->getSelectedPrice();
        double discount = selectedProductOrder->getDiscount();
        double selectedPriceCorrected = selectedProductOrder->getSelectedPriceCorrected();
        double discountFraction = selectedProductOrder->getDiscountPercentageFraction();
        QString units = selectedProductOrder->getUnitsForSelectedSlot();
        if (units == "ml")
        {
            units = "L";
            selectedPrice = selectedPrice * 1000;
        }

        else if (units == "g")
        {
            if (selectedProductOrder->getVolume(SIZE_CUSTOM_INDEX) == VOLUME_TO_TREAT_CUSTOM_DISPENSE_AS_PER_100G)
            {
                units = "100g";
                selectedPrice = selectedPrice * 100;
            }
            else
            {
                units = "kg";
                selectedPrice = selectedPrice * 1000;
            }
        }
        else if (units == "oz")
        {
            units = "oz";
            selectedPrice = selectedPrice * OZ_TO_ML;
        }
        // orderSizeLabelsselectedPrice[i]->setText("$" + QString::number(selectedPrice, 'f', 2) + "/" + units);

        selectedPriceCorrected = selectedPrice * (1 - discountFraction);
        double discountAmount = selectedPrice - selectedPriceCorrected;

        ui->label_invoice_price->setText("$" + QString::number(selectedPrice, 'f', 2) + "/" + units);
        ui->label_invoice_discount_amount->setText("-$" + QString::number(discountAmount, 'f', 2)+ "/" + units);

        ui->label_invoice_price_total->setText("$" + QString::number(selectedPriceCorrected, 'f', 2) + "/" + units);

        // // double selectedPrice = selectedProductOrder->getSelectedPrice();
        // // double discount = selectedProductOrder->getDiscount();
        // // double selectedPriceCorrected = selectedProductOrder->getSelectedPriceCorrected();

        // QString unitsInvoice;
        // selectedProductOrder->getCustomPriceDetails(&unitsInvoice, &selectedPrice, &discount, &selectedPriceCorrected);
    }
    else
    {
        double discountAmount = selectedProductOrder->getSelectedPrice() - selectedProductOrder->getSelectedPriceCorrected();
        ui->label_invoice_discount_amount->setText("-$" + QString::number(discountAmount, 'f', 2));
        ui->label_selected_volume->setText(selected_volume + " " + selectedProductOrder->getUnitsForSelectedSlot());
        ui->label_invoice_price->setText("$" + QString::number(selectedProductOrder->getSelectedPrice(), 'f', 2));
        ui->label_invoice_price_total->setText("$" + QString::number(selectedProductOrder->getSelectedPriceCorrected(), 'f', 2));
    }
}

void pageProductOverview::on_applyPromo_Button_clicked()
{

    QString promocode = ui->promoCode->text();
    QMovie *movie = new QMovie("/home/df-admin/drinkfill/ui/soapstandspinner.gif");
    ui->label_gif->setMovie(movie);
    movie->start();
    // ui->promoKeyboard->hide();

    CURL *curl;
    CURLcode res;
    long http_code = 0;
    if (promocode != "")
    {
        ui->label_gif->show();
        readBuffer.clear();
        curl = curl_easy_init();
        {

            if (!curl)
            {
                qDebug() << "pageProductOverview: apply promo cURL failed init";
                return;
            }
            curl_easy_setopt(curl, CURLOPT_URL, ("https://soapstandportal.com/api/coupon/find/" + promocode).toUtf8().constData());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback_coupon1);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            res = curl_easy_perform(curl);
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (res != CURLE_OK)
            {
                ui->promoCode->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #f44336;border-color:#f44336;");
                qDebug() << "Invalid Coupon curl problem. error code: " << res;
            }
            else
            {
                int new_percent;
                ui->label_gif->hide();

                if (http_code == 200)
                {
                    json coupon_obj = json::parse(readBuffer);
                    if (coupon_obj["active"])
                    {
                        new_percent = coupon_obj["discount_amount"];
                        selectedProductOrder->setPromoCode(promocode);
                        selectedProductOrder->setDiscountPercentageFraction((new_percent * 1.0) / 100);
                        qDebug() << "Apply coupon percentage: " << new_percent;
                        // loadProdSpecs();
                        qDebug() << "Promo";

                        ui->label_invoice_discount_amount->show();
                        ui->label_discount_tag->show();
                        // ui->label_invoice_discount_amount->setText("-$" + QString::number(selectedProductOrder->getDiscount(), 'f', 5));
                        // ui->label_invoice_price_total->setText("$" + QString::number(selectedProductOrder->getSelectedPriceCorrected(), 'f', 2));
                        // ui->label_invoice_price_total->setText("$" + QString::number(selectedProductOrder->getSelectedPriceCorrected(), 'f', 2)); // how to handle promo ?! todo!

                        updatePrice();

                    }
                    else
                    {
                        qDebug() << "Invalid Coupon";
                        ui->promoCode->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #75756f;border-color:#f44336;");
                    }
                }
                else
                {
                    qDebug() << "Invalid Coupon http 200 response";
                    ui->promoCode->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #f44336;border-color:#f44336;");
                }
            }
        }
    }
}

void pageProductOverview::keyboardButtonPressed(int buttonID)
{

    QAbstractButton *buttonpressed = ui->buttonGroup->button(buttonID);
    QString buttonText = buttonpressed->objectName();

    if (buttonText == "backspace")
    {
        ui->promoCode->backspace();
    }
    else if (buttonText == "done")
    {
        ui->promoKeyboard->hide();
        if (ui->promoCode->text() == "")
        {
            ui->promoCode->hide();
        }
        on_applyPromo_Button_clicked();
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
void pageProductOverview::on_previousPage_Button_clicked()
{

    this->return_to_selectProductPage();
}

void pageProductOverview::coupon_input_show()
{
}

void pageProductOverview::coupon_input_hide()
{
    ui->promoKeyboard->hide();
}

void pageProductOverview::coupon_input_reset()
{
}

void pageProductOverview::on_promoCodeInput_clicked()
{
    QObject *button = QObject::sender();
    ui->promoCode->setStyleSheet("font-family: Montserrat; font-style: normal; font-weight: bold; font-size: 28px; line-height: 44px; color: #5E8580;border-color:#5E8580;");
    ui->label_discount_code->hide();
    ui->promoInputButton->show();
    ui->promoKeyboard->show();
    qDebug() << "show promo keyboard.";
    ui->promoCode->show();
}
void pageProductOverview::couponHandler()
{
    qDebug() << "db for coupons";

    DbManager db(DB_PATH);
    bool coupons_enabled = db.getCouponsEnabled();
    db.closeDB();

    if (coupons_enabled)
    {
        qDebug() << "Coupons are enabled for this machine.";
        ui->promoInputButton->show();
        ui->promoInputButton->setEnabled(true);

        ui->promoCode->show();
        // ui->promoKeyboard->show();
        ui->promoInputButton->show();
        ui->label_invoice_discount_amount->show();
        ui->label_invoice_discount_name->show();
        ui->label_discount_tag->show();
        ui->promoButton->show();
        ui->label_discount_code->show();
    }
    else
    {
        qDebug() << "Coupons are disabled for this machine.";
        ui->promoInputButton->setEnabled(false);
        coupon_disable();
    }
}
void pageProductOverview::coupon_disable()
{
    ui->promoCode->hide();
    ui->promoKeyboard->hide();
    ui->promoInputButton->hide();
    ui->label_invoice_discount_amount->hide();
    ui->label_invoice_discount_name->hide();
    ui->label_discount_tag->hide();
    ui->promoButton->hide();
    ui->label_discount_code->hide();
}

void pageProductOverview::on_page_payment_Button_clicked()
{
    qDebug() << "pageProductOverview: Pay button";

    ui->mainPage_Button->setEnabled(false);
    ui->previousPage_Button->setEnabled(false);

    this->stopSelectTimers();
    QString paymentMethod = selectedProductOrder->getSelectedPaymentMethod();

    if (paymentMethod == "qr" || paymentMethod == "tap")
    {
        CURL *curl;
        CURLcode res;
        curl = curl_easy_init();

        if (!curl)
        {
            qDebug() << "pageProductOverview: cURL failed init";
            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/ping");
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            qDebug() << "ERROR: Failed to reach soapstandportal. error code: " + QString::number(res);

            // p_page_wifi_error->showEvent(wifiErrorEvent);
            // p_page_wifi_error->showFullScreen();
            // this->hide();
            p_page_idle->pageTransition(this, p_page_wifi_error);
        }
        else
        {
            QString feedback = QString::fromUtf8(readBuffer.c_str());
            qDebug() << "Server feedback readbuffer: " << feedback;

            ui->label_invoice_price->text();

            // paymentPage->showFullScreen();
            // this->hide();
            p_page_idle->pageTransition(this, paymentPage);
        }
        curl_easy_cleanup(curl);
        readBuffer = "";
    }
    else if (paymentMethod == "barcode" || paymentMethod == "plu")
    {
        // p_page_dispense->showEvent(dispenseEvent); // todo Lode: this enabled together with showfullscreen calls the showEvent twice. only showevent, does not display the dispense page though.
        // p_page_dispense->showFullScreen();
        // this->hide();
        p_page_idle->pageTransition(this, p_page_dispense);
    }
    else
    {
        qDebug() << "WARNING: No payment method detected.";
        // p_page_dispense->showFullScreen();
        // this->hide();

        p_page_idle->pageTransition(this, p_page_dispense);
    }
}

void pageProductOverview::return_to_selectProductPage()
{
    //    qDebug() << "pageProduct: Previous button" << endl;
    stopSelectTimers();
    //ui->promoKeyboard->hide();
    p_page_idle->pageTransition(this, p_page_product);
}
void pageProductOverview::on_selectProductPage_Button_clicked()
{
    this->return_to_selectProductPage();
}
