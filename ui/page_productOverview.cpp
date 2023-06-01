
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

#include "page_qr_payment.h"
#include "page_tap_payment.h"
#include "page_select_product.h"
#include "page_idle.h"
#include <curl/curl.h>
#include <json.hpp>
#include <QMovie>
using json = nlohmann::json;
std::string CTROUTD = "";
std::string MAC_KEY = "";
std::string MAC_LABEL = "";
std::string AUTH_CODE = "";
std::string SAF_NUM = "";
std::string socketAddr;

// CTOR
page_product_overview::page_product_overview(QWidget *parent) : QWidget(parent),
                                                                ui(new Ui::page_product_overview)
{
    ui->setupUi(this);

    selectIdleTimer = new QTimer(this);
    selectIdleTimer->setInterval(40);
    connect(selectIdleTimer, SIGNAL(timeout()), this, SLOT(onSelectTimeoutTick()));

    connect(ui->pushButton_promo_input, SIGNAL(clicked()), this, SLOT(on_lineEdit_promo_codeInput_clicked()));
    connect(ui->buttonGroup, SIGNAL(buttonPressed(int)), this, SLOT(keyboardButtonPressed(int)));

    ui->label_gif->hide();
}

/*
 * Page Tracking reference to Select Drink, Payment Page and Idle page
 */
void page_product_overview::setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_tap_payment *page_tap_payment, page_help *pageHelp, page_product *page_product)
{
    this->p_page_select_product = pageSelect;
    this->paymentQrPage = page_qr_payment;
    this->paymentTapPage = page_tap_payment;
    this->p_page_idle = pageIdle;
    this->p_page_dispense = page_dispenser;
    this->p_page_help = pageHelp;
    this->p_page_wifi_error = pageWifiError;
    this->p_page_product = page_product;

    ui->label_discount_tag->hide();
    ui->label_gif->hide();
    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_ORDER_OVERVIEW_PATH);
}

// DTOR
page_product_overview::~page_product_overview()
{
    delete ui;
}

void page_product_overview::cancelTimers()
{
    selectIdleTimer->stop();
}

/* GUI */

void page_product_overview::showEvent(QShowEvent *event)
{
    QString styleSheet = p_page_idle->getCSS(PAGE_PRODUCT_OVERVIEW_CSS);
    ui->pushButton_promo_input->setStyleSheet(styleSheet);
    ui->lineEdit_promo_code->setProperty("class", "promoCode");
    ui->lineEdit_promo_code->setStyleSheet(styleSheet);
    //  ui->label_product_title->setStyleSheet(styleSheet);
    ui->label_selected_volume->setStyleSheet(styleSheet);

    ui->label_invoice_name->setProperty("class", "labelOrderOverview");
    ui->label_invoice_price->setProperty("class", "labelOrderOverview");
    ui->label_discount_tag->setProperty("class", "labelDiscount");
    ui->label_invoice_discount_amount->setProperty("class", "labelDiscount");

    ui->label_invoice_name->setStyleSheet(styleSheet);
    ui->label_invoice_price->setStyleSheet(styleSheet);
    ui->label_invoice_price_total->setStyleSheet(styleSheet);
    ui->label_discount_tag->setStyleSheet(styleSheet);
    ui->label_invoice_discount_amount->setStyleSheet(styleSheet);
    ui->label_invoice_box->setStyleSheet(styleSheet);
    ui->label_pay->setStyleSheet(styleSheet);
    ui->label_pay->raise();
    ui->label_invoice_discount_name->setProperty("class", "labelDiscountName");
    ui->label_invoice_discount_name->setStyleSheet(styleSheet);
    ui->label_total->setStyleSheet(styleSheet);
    ui->label_gif->setStyleSheet(styleSheet);
    ui->invoice_line->setStyleSheet(styleSheet);
    ui->pushButton_select_product_page->setStyleSheet(styleSheet);

    ui->label_product_photo->setStyleSheet(styleSheet);
    /* Hacky transparent button */
    ui->pushButton_previous_page->setProperty("class", "buttonBGTransparent");
    ui->pushButton_previous_page->setStyleSheet(styleSheet);
    // ui->pushButton_continue->setProperty("class", "buttonBGTransparent");
    ui->pushButton_continue->setStyleSheet(styleSheet);
    ui->pushButton_continue->raise();
    // ui->pushButton_continue->setStyleSheet("QPushButton { background-color: red; border: 0px };QPushButton:pressed { background-color: green; border: 10px }");

    ui->pushButton_to_help->setProperty("class", "buttonBGTransparent");
    ui->pushButton_to_help->setStyleSheet(styleSheet);


    qDebug() << "<<<<<<< Page Enter: Product Overview>>>>>>>>>";
    QWidget::showEvent(event);

    p_page_idle->selectedProduct->loadProductProperties();
    reset_and_show_page_elements();
}

void page_product_overview::resizeEvent(QResizeEvent *event)
{
}

void page_product_overview::onSelectTimeoutTick()
{
    if (--_selectIdleTimeoutSec >= 0)
    {
    }
    else
    {
        // qDebug() << "Timer Done!" << _selectIdleTimeoutSec << endl;
        selectIdleTimer->stop();
        hideCurrentPageAndShowProvided(p_page_idle);
    }
}

void page_product_overview::reset_and_show_page_elements()
{

    p_page_idle->setTemplateTextToObject(ui->pushButton_select_product_page);
    p_page_idle->setTemplateTextToObject(ui->label_discount_tag);
    p_page_idle->setTemplateTextToObject(ui->label_pay);
    p_page_idle->setTemplateTextToObject(ui->label_total);
    p_page_idle->setTemplateTextToObject(ui->pushButton_continue);

    QString bitmap_location;
    p_page_idle->addPictureToLabel(ui->label_product_photo, p_page_idle->selectedProduct->getProductPicturePath());
    ui->label_selected_price->setText("$" + QString::number(p_page_idle->selectedProduct->getPrice(), 'f', 2));
    qDebug() << "Selected size" << p_page_idle->selectedProduct->getVolume();
    QString full_path = p_page_idle->getTemplatePathFromName(IMAGE_BUTTON_HELP);
    qDebug() << full_path;
    p_page_idle->addPictureToLabel(ui->label_help, full_path);

    updatePriceLabel();
    ui->label_invoice_name->setText(p_page_idle->selectedProduct->getProductName());

    ui->promoKeyboard->hide();
    if (areCouponsEnabled())
    {
        p_page_idle->setTemplateTextWithIdentifierToObject(ui->lineEdit_promo_code, "coupons_enable");

        ui->lineEdit_promo_code->show();
        if (p_page_idle->isPromoApplied())
        {
            p_page_idle->setTemplateTextWithIdentifierToObject(ui->lineEdit_promo_code, "valid");
        }
        ui->label_invoice_discount_amount->show();
        ui->label_invoice_discount_name->show();
        ui->pushButton_promo_input->show();
    }
    else
    {
        qDebug() << "Coupons not enabled";
        coupon_input_hide();
        ui->label_invoice_discount_amount->hide();
        ui->label_invoice_discount_name->hide();
        ui->label_discount_tag->hide();
    }

    if (!p_page_idle->isPromoApplied())
    {
        p_page_idle->setTemplateTextWithIdentifierToObject(ui->lineEdit_promo_code, "invite");
        ui->label_invoice_discount_amount->hide();
        ui->label_invoice_discount_name->hide();
        ui->label_discount_tag->hide();
    }
    ui->pushButton_previous_page->setEnabled(true);
    ui->pushButton_to_help->setEnabled(true);

    QString keyboard = KEYBOARD_IMAGE_PATH;
    QString keyboard_picture_path = p_page_idle->getTemplatePathFromName(KEYBOARD_IMAGE_PATH);
    // QString keyboard_style_sheet = " background-image: url(" + keyboard + "); }";
    p_page_idle->addPictureToLabel(ui->label_keyboard_background, keyboard_picture_path);
    ui->label_keyboard_background->lower();
    // loadProdSpecs();
    selectIdleTimer->start(1000);
    _selectIdleTimeoutSec = 400;
    
}

void page_product_overview::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    selectIdleTimer->stop();
    p_page_idle->pageTransition(this, pageToShow);
}

void page_product_overview::on_pushButton_to_help_clicked()
{
    hideCurrentPageAndShowProvided(p_page_help);
}

size_t WriteCallback_coupon1(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void page_product_overview::updatePriceLabel()
{
    QString selected_volume = p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(p_page_idle->selectedProduct->getSize(), true, true);

    if (p_page_idle->selectedProduct->getSize() == SIZE_CUSTOM_INDEX)
    {
        QString base = p_page_idle->getTemplateTextByElementNameAndPageAndIdentifier( ui->label_selected_volume, "custom_volume");
        ui->label_selected_volume->setText(base.arg(selected_volume));

        double selectedPrice = p_page_idle->selectedProduct->getPrice();
        double discount = p_page_idle->selectedProduct->getDiscount();
        double selectedPriceCorrected = p_page_idle->getPriceCorrectedAfterDiscount(selectedPrice);
        double discountFraction = p_page_idle->getDiscountPercentage();
        QString units = p_page_idle->selectedProduct->getUnitsForSlot();
        if (units == "ml")
        {
            units = "L";
            selectedPrice = selectedPrice * 1000;
        }
        else if (units == "g")
        {
            if (p_page_idle->selectedProduct->getVolume(SIZE_CUSTOM_INDEX) == VOLUME_TO_TREAT_CUSTOM_DISPENSE_AS_PER_100G)
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

        selectedPriceCorrected = selectedPrice * (1 - discountFraction);
        double discountAmount = selectedPrice - selectedPriceCorrected;

        ui->label_invoice_price->setText("$" + QString::number(selectedPrice, 'f', 2) + "/" + units);
        ui->label_invoice_discount_amount->setText("-$" + QString::number(discountAmount, 'f', 2) + "/" + units);

        ui->label_invoice_price_total->setText("$" + QString::number(selectedPriceCorrected, 'f', 2) + "/" + units);
    }
    else
    { 
        // The label_invoice_price total displays the discounted total even when the user goes back to the select_product page. It's intended behaviour so user doesnt have to retype the promo-code
        double selectedPrice = p_page_idle->selectedProduct->getPrice();
        double selectedPriceCorrected = p_page_idle->getPriceCorrectedAfterDiscount(selectedPrice);
        double discountFraction = p_page_idle->getDiscountPercentage();
        double discountAmount = selectedPrice - selectedPriceCorrected;
        ui->label_invoice_discount_amount->setText("-$" + QString::number(discountAmount, 'f', 2));
        ui->label_selected_volume->setText(selected_volume);
        ui->label_invoice_price->setText("$" + QString::number(selectedPrice, 'f', 2));
        ui->label_invoice_price_total->setText("$" + QString::number(selectedPriceCorrected, 'f', 2));
    }
}

void page_product_overview::apply_promo_code()
{
    QString promocode = ui->lineEdit_promo_code->text();
    QMovie *movie = new QMovie("soapstandspinner.gif");
    ui->label_gif->setMovie(movie);
    movie->start();
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
                qDebug() << "page_product_overview: apply promo cURL failed init";
                return;
            }
            curl_easy_setopt(curl, CURLOPT_URL, ("https://soapstandportal.com/api/coupon/find/" + promocode).toUtf8().constData());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback_coupon1);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            res = curl_easy_perform(curl);
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (res != CURLE_OK)
            {
                p_page_idle->addCssClassToObject(ui->lineEdit_promo_code, "promoCode_invalid", PAGE_PRODUCT_OVERVIEW_CSS);
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
                        qDebug() << "Apply coupon percentage: " << new_percent;
                        new_percent = coupon_obj["discount_amount"];
                        p_page_idle->setPromoCode(promocode);
                        p_page_idle->setDiscountPercentage((new_percent * 1.0) / 100);

                        if (p_page_idle->isPromoApplied())
                        {
                            ui->label_invoice_discount_amount->show();
                            ui->label_discount_tag->show();
                            p_page_idle->setTemplateTextWithIdentifierToObject(ui->lineEdit_promo_code, "valid");
                        }

                        p_page_idle->addCssClassToObject(ui->lineEdit_promo_code, "promoCode_valid", PAGE_PRODUCT_OVERVIEW_CSS);
                        updatePriceLabel();
                    }
                    else
                    {
                        qDebug() << "Invalid Coupon";
                        p_page_idle->addCssClassToObject(ui->lineEdit_promo_code, "promoCode_invalid", PAGE_PRODUCT_OVERVIEW_CSS);
                    }
                }
                else
                {
                    qDebug() << "Invalid Coupon http 200 response";
                    p_page_idle->addCssClassToObject(ui->lineEdit_promo_code, "promoCode_invalid", PAGE_PRODUCT_OVERVIEW_CSS);
                }
            }
        }
    }
}

void page_product_overview::keyboardButtonPressed(int buttonID)
{
    QAbstractButton *buttonpressed = ui->buttonGroup->button(buttonID);
    QString buttonText = buttonpressed->objectName();
    qDebug() << buttonText;
    QString promocode = ui->lineEdit_promo_code->text();
    qDebug() << promocode;
    if (buttonText == "backspace")
    {
        ui->lineEdit_promo_code->backspace();
    }
    else if (buttonText == "done")
    {
        ui->promoKeyboard->hide();
        ui->pushButton_promo_input->show();
        apply_promo_code();
    }
    else if (buttonText.mid(0, 3) == "num")
    {
        ui->lineEdit_promo_code->setText(ui->lineEdit_promo_code->text() + buttonText.mid(3, 1));
    }
    else
    {
        ui->lineEdit_promo_code->setText(ui->lineEdit_promo_code->text() + buttonText);
    }
}
void page_product_overview::on_pushButton_previous_page_clicked()
{

    this->return_to_selectProductPage();
}

void page_product_overview::coupon_input_show()
{
    qDebug() << "show promo keyboard.";
    ui->promoKeyboard->show();
    ui->pushButton_promo_input->hide();
    ui->lineEdit_promo_code->clear();
    ui->lineEdit_promo_code->show();
    p_page_idle->addCssClassToObject(ui->lineEdit_promo_code, "promoCode", PAGE_PRODUCT_OVERVIEW_CSS);
}

void page_product_overview::coupon_input_hide()
{
    ui->promoKeyboard->hide();
    ui->pushButton_promo_input->show();
}

void page_product_overview::coupon_input_reset()
{
}

void page_product_overview::on_lineEdit_promo_codeInput_clicked()
{
    coupon_input_show();
}

bool page_product_overview::areCouponsEnabled()
{
    qDebug() << "db for coupons";

    DbManager db(DB_PATH);
    bool coupons_enabled = db.getCouponsEnabled();
    db.closeDB();
    return coupons_enabled;
}

void page_product_overview::on_pushButton_continue_clicked()
{
    qDebug() << "page_product_overview: Pay button";

    ui->pushButton_to_help->setEnabled(false);
    ui->pushButton_previous_page->setEnabled(false);

    QString paymentMethod = p_page_idle->selectedProduct->getPaymentMethod();

    if (paymentMethod == "qr")
    {
        CURL *curl;
        CURLcode res;
        curl = curl_easy_init();

        if (!curl)
        {
            qDebug() << "page_product_overview: cURL failed init";
            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/ping");
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            qDebug() << "ERROR: Failed to reach soapstandportal. error code: " + QString::number(res);
            hideCurrentPageAndShowProvided(p_page_wifi_error);
        }
        else
        {
            QString feedback = QString::fromUtf8(readBuffer.c_str());
            qDebug() << "Server feedback readbuffer: " << feedback;

            ui->label_invoice_price->text();
            hideCurrentPageAndShowProvided(paymentQrPage);
        }
        curl_easy_cleanup(curl);
        readBuffer = "";
    }
    else if (paymentMethod == "tapTcp")
    {
        hideCurrentPageAndShowProvided(paymentTapPage);
    }
    else if (paymentMethod == "plu" || paymentMethod == "barcode" || paymentMethod == "barcode_EAN-2 " || paymentMethod == "barcode_EAN-13")
    {
        hideCurrentPageAndShowProvided(p_page_dispense);
    }
    else
    {
        qDebug() << "WARNING: No payment method detected.";
        hideCurrentPageAndShowProvided(p_page_dispense);
    }
}

void page_product_overview::return_to_selectProductPage()
{
    hideCurrentPageAndShowProvided(p_page_product);
}

void page_product_overview::on_pushButton_select_product_page_clicked()
{
    this->return_to_selectProductPage();
}