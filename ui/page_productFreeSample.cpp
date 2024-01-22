
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
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "page_productFreeSample.h"
#include "ui_page_productFreeSample.h"
#include <iostream>
#include <string>
#include <cmath>

#include "page_select_product.h"
#include "page_product_mixing.h"
#include "page_idle.h"
#include <curl/curl.h>
#include <json.hpp>
#include <QMovie>
using json = nlohmann::json;
extern bool isFreeEmailOrder;

// CTOR
page_product_freeSample::page_product_freeSample(QWidget *parent) : QWidget(parent),
                                                                ui(new Ui::page_product_freeSample)
{
    ui->setupUi(this);

    selectIdleTimer = new QTimer(this);
    selectIdleTimer->setInterval(40);
    connect(selectIdleTimer, SIGNAL(timeout()), this, SLOT(onSelectTimeoutTick()));

    connect(ui->pushButton_promo_input, SIGNAL(clicked()), this, SLOT(on_lineEdit_promo_codeInput_clicked()));
    connect(ui->buttonGroup, SIGNAL(buttonPressed(int)), this, SLOT(keyboardButtonPressed(int)));
    connect(ui->pushButton_continue, SIGNAL(clicked()), this, SLOT(on_pushButton_continue()));

    ui->label_gif->hide();
    statusbarLayout = new QVBoxLayout(this);
    keyboardLayout = new QVBoxLayout(this);
}

/*
 * Page Tracking reference to Select Drink, Payment Page and Idle page
 */
void page_product_freeSample::setPage(page_select_product *pageSelect, page_product_mixing *p_page_product_mixing, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_payment_tap_serial *page_payment_tap_serial, page_payment_tap_tcp *page_payment_tap_tcp, page_help *pageHelp, page_product *page_product, page_email *page_email, statusbar *statusbar, keyboard *keyboard)
{
    this->p_page_select_product = pageSelect;
    this->p_page_product_mixing = p_page_product_mixing;
    this->p_page_idle = pageIdle;
    this->p_page_dispense = page_dispenser;
    this->p_page_help = pageHelp;
    this->p_page_wifi_error = pageWifiError;
    this->p_page_product = page_product;
    this->p_statusbar = statusbar;
    this->p_keyboard = keyboard;
    this->p_page_email = page_email;

    ui->label_discount_tag->hide();
    ui->label_gif->hide();
    p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_ORDER_OVERVIEW_PATH);

}

// DTOR
page_product_freeSample::~page_product_freeSample()
{
    delete ui;
}

void page_product_freeSample::cancelTimers()
{
    selectIdleTimer->stop();
}

/* GUI */

void page_product_freeSample::showEvent(QShowEvent *event)
{
    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    statusbarLayout->addWidget(p_statusbar);            // Only one instance can be shown. So, has to be added/removed per page.
    // statusbarLayout->setContentsMargins(0, 1874, 0, 0); // int left, int top, int right, int bottom);
    statusbarLayout->addWidget(p_keyboard);    
    statusbarLayout->setContentsMargins(14, 1374, 15, 51); // int left, int top, int right, int bottom);
     
    // keyboardLayout->setContentsMargins(0, 0, 0, 0);

    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget
    // p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(ui->promoKeyboard); 
    if (p_page_idle->thisMachine->hasMixing()){
        ui->promoKeyboard->findChild<QLabel*>("label_keyboard_background")->setGeometry(QRect(0, 0, 841, 364));
        ui->promoKeyboard->findChild<QPushButton*>("a")->setGeometry(QRect(53, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("b")->setGeometry(QRect(403, 252, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("backspace")->setGeometry(QRect(728, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("c")->setGeometry(QRect(253, 252, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("d")->setGeometry(QRect(203, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("done")->setGeometry(QRect(628, 252, 125, 62));
        ui->promoKeyboard->findChild<QPushButton*>("e")->setGeometry(QRect(203, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("f")->setGeometry(QRect(278, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("g")->setGeometry(QRect(353, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("h")->setGeometry(QRect(428, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("i")->setGeometry(QRect(578, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("j")->setGeometry(QRect(503, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("k")->setGeometry(QRect(578, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("l")->setGeometry(QRect(653, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("m")->setGeometry(QRect(553, 252, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("n")->setGeometry(QRect(478, 252, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("num0")->setGeometry(QRect(728, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton*>("num1")->setGeometry(QRect(53, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton*>("num2")->setGeometry(QRect(128, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton*>("num3")->setGeometry(QRect(203, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton*>("num4")->setGeometry(QRect(278, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton*>("num5")->setGeometry(QRect(353, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton*>("num6")->setGeometry(QRect(428, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton*>("num7")->setGeometry(QRect(503, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton*>("num8")->setGeometry(QRect(578, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton*>("num9")->setGeometry(QRect(653, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton*>("o")->setGeometry(QRect(653, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("p")->setGeometry(QRect(728, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("q")->setGeometry(QRect(53, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("r")->setGeometry(QRect(278, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("s")->setGeometry(QRect(128, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("t")->setGeometry(QRect(353, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("u")->setGeometry(QRect(503, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("v")->setGeometry(QRect(328, 252, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("w")->setGeometry(QRect(128, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("x")->setGeometry(QRect(178, 252, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("y")->setGeometry(QRect(428, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton*>("z")->setGeometry(QRect(103, 252, 62, 62));

        QString coupon_icon_path = p_page_idle->thisMachine->getTemplatePathFromName(COUPON_ICON_UNAVAILABLE_PATH);
        p_page_idle->thisMachine->addPictureToLabel(ui->label_coupon_icon, coupon_icon_path);
    }

    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_PRODUCT_FREESAMPLE_CSS);
    ui->label_page_title->setStyleSheet(styleSheet);
    ui->pushButton_promo_input->setStyleSheet(styleSheet);
    ui->lineEdit_promo_code->setProperty("class", "promoCode");
    ui->lineEdit_promo_code->setStyleSheet(styleSheet);
    setup_qr_code();
    //  ui->label_product_title->setStyleSheet(styleSheet);
    ui->label_selected_volume->setStyleSheet(styleSheet);

    ui->label_invoice_name->setProperty("class", "labelOrderOverview");
    ui->label_discount_tag->setProperty("class", "labelDiscount");

    ui->label_invoice_coupon_title->setStyleSheet(styleSheet);
    ui->label_invoice_size_title->setStyleSheet(styleSheet);
    ui->label_invoice_name->setStyleSheet(styleSheet);
    ui->label_discount_tag->setStyleSheet(styleSheet);
    ui->label_invoice_box->setStyleSheet(styleSheet);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_steps);
    ui->label_steps->setStyleSheet(styleSheet);

    ui->label_steps->show();
    ui->label_invoice_discount_name->setProperty("class", "labelDiscountName");
    ui->label_invoice_discount_name->setStyleSheet(styleSheet);
    ui->label_gif->setStyleSheet(styleSheet);
    ui->line_invoice->setStyleSheet(styleSheet);
    ui->pushButton_select_product_page->setStyleSheet(styleSheet);
   

    QString picturePath = p_page_idle->thisMachine->getSelectedProduct()->getProductPicturePath();
    styleSheet.replace("%IMAGE_PATH%", picturePath);
    // ui->label_product_photo->setStyleSheet(styleSheet);
    /* Hacky transparent button */
    ui->pushButton_previous_page->setProperty("class", "buttonBGTransparent");
    ui->pushButton_previous_page->setStyleSheet(styleSheet);
    ui->pushButton_continue->setStyleSheet(styleSheet);
    // p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_continue, "offline");
    p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_continue, "proceed_free");

    ui->pushButton_to_help->setProperty("class", "buttonBGTransparent");
    ui->pushButton_to_help->setStyleSheet(styleSheet);

    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_invoice_coupon_title);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_invoice_size_title);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_page_title);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_select_product_page);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_discount_tag);
   
    QString keyboard = KEYBOARD_IMAGE_PATH;
    QString keyboard_picture_path = p_page_idle->thisMachine->getTemplatePathFromName(KEYBOARD_IMAGE_PATH);
    p_page_idle->thisMachine->addPictureToLabel(ui->label_keyboard_background, keyboard_picture_path);
    ui->label_keyboard_background->lower();
    p_page_idle->thisMachine->setCouponState(enabled_not_set);

    ui->pushButton_continue->hide();
    
    _selectIdleTimeoutSec = 400;
    selectIdleTimer->start(1000);
    reset_and_show_page_elements();
    
}

void page_product_freeSample::resizeEvent(QResizeEvent *event)
{
}

void page_product_freeSample::onSelectTimeoutTick()
{
    if (--_selectIdleTimeoutSec >= 0)
    {
    }
    else
    {
        selectIdleTimer->stop();
        hideCurrentPageAndShowProvided(p_page_idle);
    }
}

void page_product_freeSample::enterButtonPressed(){
    if (m_readyToSendCoupon && p_page_idle->thisMachine->getCouponState() != enabled_processing_input)
    {
        m_readyToSendCoupon = false;
        qDebug() << "Done clicked, initiated apply promo.";
        // hack, sometimes it appears like the 'done' button code is called twice.
        p_page_idle->thisMachine->setCouponState(enabled_processing_input);
        reset_and_show_page_elements();
        apply_promo_code(ui->lineEdit_promo_code->text());
    }
    else
    {
        qDebug() << "ASSERT ERROR: Illegal press. Still processing other call.";
    }
}

void page_product_freeSample::reset_and_show_page_elements()
{
    qDebug() << "Reset and show page elements";

    QString bitmap_location;
    QString full_path = p_page_idle->thisMachine->getTemplatePathFromName(IMAGE_BUTTON_HELP);
    p_page_idle->thisMachine->addPictureToLabel(ui->label_help, full_path);
    ui->label_invoice_name->setText(p_page_idle->thisMachine->getSelectedProduct()->getProductName());

    // by default hide all coupon and discount elements.
    ui->promoKeyboard->hide();
    ui->pushButton_promo_input->hide();
    ui->lineEdit_promo_code->hide();
    ui->label_invoice_discount_name->hide();
    ui->label_discount_tag->hide();
    QString selected_volume = p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(p_page_idle->thisMachine->getSelectedProduct()->getSelectedSize(), true, true);
    ui->label_selected_volume->setText(selected_volume);
    m_readyToSendCoupon = false;

    switch (p_page_idle->thisMachine->getCouponState())
    {
    case (enabled_invalid_input):
    {
        qDebug() << "Coupon state: Invalid coupon input";
        p_page_idle->thisMachine->resetCouponDiscount();
        p_page_idle->thisMachine->addCssClassToObject(ui->lineEdit_promo_code, "promoCode_invalid", PAGE_PRODUCT_FREESAMPLE_CSS);
        QString promo_code_input_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "lineEdit_promo_code->invalid");
        ui->lineEdit_promo_code->setText(promo_code_input_text);
        ui->lineEdit_promo_code->show();
        ui->pushButton_promo_input->show();
    }
    break;
    case (enabled_not_eligible):
    {
        qDebug() << "Coupon state: Not eligible for station";
        p_page_idle->thisMachine->resetCouponDiscount();
        p_page_idle->thisMachine->addCssClassToObject(ui->lineEdit_promo_code, "promoCode_invalid", PAGE_PRODUCT_FREESAMPLE_CSS);
        QString promo_code_input_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "lineEdit_promo_code->ineligible");
        ui->lineEdit_promo_code->setText(promo_code_input_text);
        ui->lineEdit_promo_code->show();
        ui->pushButton_promo_input->show();
    }
    break;
    case (network_error):
    {
        qDebug() << "Coupon state: Network error";
        p_page_idle->thisMachine->addCssClassToObject(ui->lineEdit_promo_code, "promoCode_network_error", PAGE_PRODUCT_FREESAMPLE_CSS);
        QString promo_code_input_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "lineEdit_promo_code->network_error");
        ui->lineEdit_promo_code->setText(promo_code_input_text);
        ui->lineEdit_promo_code->show();
        ui->pushButton_promo_input->show();
    }
    break;
    case (enabled_valid_active):
    {
        qDebug() << "Coupon state: Coupon valid";
        p_page_idle->thisMachine->addCssClassToObject(ui->lineEdit_promo_code, "promoCode_valid", PAGE_PRODUCT_FREESAMPLE_CSS);
        QString promo_code_input_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "lineEdit_promo_code->valid");
        QString entered_coupon_code = ui->lineEdit_promo_code->text().toUpper();
        // ui->lineEdit_promo_code->setText(promo_code_input_text);
        ui->lineEdit_promo_code->setText("Valid Coupon: " + entered_coupon_code);
        QString coupon_icon_path = p_page_idle->thisMachine->getTemplatePathFromName(COUPON_ICON_AVAILABLE_PATH);
        if (p_page_idle->thisMachine->hasMixing()){
            p_page_idle->thisMachine->addPictureToLabel(ui->label_coupon_icon, coupon_icon_path);
        }
        ui->label_invoice_discount_name->hide();
        ui->label_discount_tag->show();
        ui->lineEdit_promo_code->show();
        ui->pushButton_promo_input->show();
        ui->pushButton_continue->show();
        ui->pushButton_continue->raise();

    }
    break;
    case (disabled):
    {
        qDebug() << "Coupon state: Disabled";
    }
    break;
    case (enabled_processing_input):
    {
        qDebug() << "Coupon state: Process input";
        ui->lineEdit_promo_code->show();
        p_page_idle->thisMachine->addCssClassToObject(ui->lineEdit_promo_code, "promoCode_processing", PAGE_PRODUCT_FREESAMPLE_CSS);
    }
    break;
    case (enabled_show_keyboard):
    {
        qDebug() << "Coupon state: Show keyboard";
        // ui->promoKeyboard->show();
        // p_keyboard->setVisibility(true);
        p_keyboard->registerCallBack(std::bind(&page_product_freeSample::enterButtonPressed, this));
        p_keyboard->initializeKeyboard(true, ui->lineEdit_promo_code);
        ui->lineEdit_promo_code->clear();
        ui->lineEdit_promo_code->show();
        p_page_idle->thisMachine->addCssClassToObject(ui->lineEdit_promo_code, "promoCode", PAGE_PRODUCT_FREESAMPLE_CSS);
        m_readyToSendCoupon = true;
    }
    break;
    case (enabled_not_set):
    {
        qDebug() << "Coupon state: Enabled, not set";
        QString promo_code_input_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "lineEdit_promo_code->invite");
        ui->lineEdit_promo_code->setText(promo_code_input_text);
        ui->lineEdit_promo_code->show();
        ui->pushButton_promo_input->show();
        p_page_idle->thisMachine->resetCouponDiscount();
    }
    break;

    default:
    {
        qDebug() << "Coupon state: Invalid" << p_page_idle->thisMachine->getCouponState();
        p_page_idle->thisMachine->resetCouponDiscount();
    }
    break;
    }


    ui->pushButton_previous_page->setEnabled(true);
    ui->pushButton_to_help->setEnabled(true);

    _selectIdleTimeoutSec = 400;
}

void page_product_freeSample::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{

    if (p_page_idle->thisMachine->getCouponState() == enabled_show_keyboard ||
        p_page_idle->thisMachine->getCouponState() == enabled_invalid_input ||
        p_page_idle->thisMachine->getCouponState() == enabled_processing_input ||
        p_page_idle->thisMachine->getCouponState() == network_error)
    {
        p_page_idle->thisMachine->setCouponState(enabled_not_set);
    }

    selectIdleTimer->stop();
    statusbarLayout->removeWidget(p_statusbar); // Only one instance can be shown. So, has to be added/removed per page.
    statusbarLayout->removeWidget(p_keyboard); // Only one instance can be shown. So, has to be added/removed per page.

    p_page_idle->thisMachine->pageTransition(this, pageToShow);
}

void page_product_freeSample::on_pushButton_to_help_clicked()
{
    hideCurrentPageAndShowProvided(p_page_help);
}

size_t WriteCallback_coupon2(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}



void page_product_freeSample::apply_promo_code(QString promocode)
{
    QString image_path = p_page_idle->thisMachine->getTemplatePathFromName("soapstandspinner.gif");
    
    QMovie *movie = new QMovie(image_path);
    ui->label_gif->setMovie(movie);
    movie->start();
    CURL *curl;
    CURLcode res;
    long http_code = 0;
    QString machine_id = p_page_idle->thisMachine->getMachineId();
    QString product_serial = p_page_idle->thisMachine->getSelectedProduct()->getPNumberAsPString();
    QString portal_base_url = this->p_page_idle->thisMachine->getPortalBaseUrl();

    // csuccess
    p_page_idle->thisMachine->setCouponState(enabled_invalid_input);

    if (promocode != "")
    {
        ui->label_gif->show();
        readBuffer.clear();
        curl = curl_easy_init();
        {
            if (!curl)
            {
                qDebug() << "page_product_freeSample: apply promo cURL failed init";
                p_page_idle->thisMachine->setCouponState(network_error);
                return;
            }
            curl_easy_setopt(curl, CURLOPT_URL, (portal_base_url+"api/coupon/find/" + promocode + "/" + machine_id + "/" + product_serial).toUtf8().constData());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback_coupon2);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            res = curl_easy_perform(curl);
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (res != CURLE_OK)
            {
                qDebug() << "Backend coupon response: curl backend problem. error code: " << res;
                p_page_idle->thisMachine->setCouponState(network_error);
            }
            else
            {
                int new_percent;
                QString myQString = QString::fromStdString(readBuffer);
                p_page_idle->thisMachine->setCouponConditions(myQString);
                ui->label_gif->hide();
                qDebug() << myQString;

                if (http_code == 200)
                {
                    json coupon_obj = json::parse(readBuffer);
                    //For free samples, the coupons are created with 0% discount as the price is already 0. 
                    // With 0 price, customer need to have a valid coupon code to restrict the system abuse
                    if (coupon_obj["active"] && coupon_obj["discount_amount"]==0)
                    {
                        qDebug() << "Backend coupon response: Valid. Discount percentage: " << new_percent;
                        new_percent = coupon_obj["discount_amount"];

                        p_page_idle->thisMachine->setCouponCode(promocode);
                        p_page_idle->thisMachine->setDiscountPercentageFraction((new_percent * 1.0) / 100);
                        p_page_idle->thisMachine->setCouponState(enabled_valid_active);
                    }
                    else
                    {
                        qDebug() << "Backend coupon response: Invalid ";
                        p_page_idle->thisMachine->setCouponState(enabled_invalid_input);
                    }
                }
                else
                {
                    qDebug() << "Backend coupon response: http 200 response ";
                    if (myQString == "Not Eligible")
                    {
                        p_page_idle->thisMachine->setCouponState(enabled_not_eligible);
                    }
                    else
                    {
                        p_page_idle->thisMachine->setCouponState(enabled_invalid_input);
                    }
                }
            }
        }
    }
    else
    {
        qDebug() << "Coupon: No input";
        p_page_idle->thisMachine->setCouponState(enabled_not_set);
    }
    reset_and_show_page_elements();
}

void page_product_freeSample::on_pushButton_previous_page_clicked()
{
    this->return_to_selectProductPage();
}

void page_product_freeSample::on_lineEdit_promo_codeInput_clicked()
{
    p_page_idle->thisMachine->setCouponState(enabled_show_keyboard);
    statusbarLayout->removeWidget(p_statusbar);
    reset_and_show_page_elements();
}

void page_product_freeSample::on_pushButton_continue()
{
    isFreeEmailOrder = true;
    ui->pushButton_to_help->setEnabled(false);
    ui->pushButton_previous_page->setEnabled(false);  
    hideCurrentPageAndShowProvided(p_page_dispense);
  
}

void page_product_freeSample::return_to_selectProductPage()
{
    p_page_idle->thisMachine->resetCouponDiscount();
    p_page_idle->thisMachine->setCouponState(enabled_not_set);
    p_page_idle->thisMachine->hasMixing() ? hideCurrentPageAndShowProvided(p_page_product_mixing) : hideCurrentPageAndShowProvided(p_page_product);
}

void page_product_freeSample::on_pushButton_select_product_page_clicked()
{
    this->return_to_selectProductPage();
}


void page_product_freeSample:: setup_qr_code(){
    QPixmap map;
        map = QPixmap(360, 360);
        map.fill(QColor("#895E25"));
        
        QPainter painter(&map);
        //Retrieve station id from database and convert hyphen (-) with underscore(_)
        QString stationId = p_page_idle->thisMachine->getMachineId().replace("-","_").toLower();
        //Retrieve station location from database. Split the name using comma delimiter as to use first identifier only.
        //Lowercase the location and convert hyphen (-) with underscore(_)
        QString stationLocation = p_page_idle->thisMachine->getMachineLocation().split(',').first().replace("-","_").toLower();
        // build up qr content (link)
        // https://www.aelen.com/freesip?utm_source=station_ubc_nest_ap_2&utm_medium=qr&utm_campaign=free_sip
        QString qrdata = "https://www.aelen.com/freesip?utm_source=station_" +stationLocation +"_" +stationId +"&utm_medium=qr&utm_campaign=free_sip";
        // create qr code graphics
        p_page_idle->thisMachine->hasMixing() ? paintSampleQR(painter, QSize(360, 360), qrdata, QColor("white")) : paintSampleQR(painter, QSize(360, 360), qrdata, QColor("white"));
        // paintSampleQR(painter, QSize(360, 360), qrdata, QColor("white"));
        ui->label_qrCode->setPixmap(map);
}


void page_product_freeSample::paintSampleQR(QPainter &painter, const QSize sz, const QString &data, QColor fg)
{
    // NOTE: At this point you will use the API to get the encoding and format you want, instead of my hardcoded stuff:
    qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(data.toUtf8().constData(), qrcodegen::QrCode::Ecc::HIGHH);
    const int s = qr.getSize() > 0 ? qr.getSize() : 1;
    const double w = sz.width();
    const double h = sz.height();
    const double aspect = w / h;
    const double size = ((aspect > 1.0) ? h : w);
    const double scale = size / (s + 2);
    // NOTE: For performance reasons my implementation only draws the foreground parts in supplied color.
    // It expects background to be prepared already (in white or whatever is preferred).

    painter.setPen(Qt::NoPen);

    painter.setBrush(fg);
    for (int y = 0; y < s; y++)
    {
        for (int x = 0; x < s; x++)
        {
            const int color = qr.getModule(x, y); // 0 for white, 1 for black
            if (0 != color)
            {
                if (p_page_idle->thisMachine->hasMixing()){
                    QColor customColor("#FFF7ED");
                    painter.setBrush(customColor);
                }
                const double rx1 = (x + 1) * scale, ry1 = (y + 1) * scale;
                QRectF r(rx1, ry1, scale, scale);
                painter.drawRects(&r, 1);
            }
        }
    }
}
