
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

#include "page_productOverview.h"
#include "ui_page_productOverview.h"
#include <iostream>
#include <string>
#include <cmath>
#include <functional>
#include "page_qr_payment.h"
#include "page_payment_tap_tcp.h"
#include "page_select_product.h"
#include "page_product_mixing.h"
#include "page_idle.h"
#include <curl/curl.h>
#include <json.hpp>
#include <QMovie>
using json = nlohmann::json;
std::string CTROUTD = "";
std::string SAF_NUM = "";
std::string MAC_KEY = "";
std::string MAC_LABEL = "";
std::string AUTH_CODE = "";
std::map<std::string, std::string> tapPaymentObject;
std::string socketAddr;

bool isFreeEmailOrder = false;

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
    connect(ui->buttonGroup_continue, SIGNAL(buttonPressed(int)), this, SLOT(on_pushButton_continue(int)));

    ui->label_gif->hide();
    statusbarLayout = new QVBoxLayout(this);
    // keyboardLayout = new QVBoxLayout(this);
}

/*
 * Page Tracking reference to Select Drink, Payment Page and Idle page
 */
void page_product_overview::setPage(page_select_product *pageSelect, page_product_mixing *p_page_product_mixing, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_payment_tap_serial *page_payment_tap_serial, page_payment_tap_tcp *page_payment_tap_tcp, page_help *pageHelp, page_product *page_product, page_email *page_email, statusbar *statusbar, keyboard *keyboard)
{
    this->p_page_select_product = pageSelect;
    this->p_page_product_mixing = p_page_product_mixing;
    this->p_page_payment_qr = page_qr_payment;
    this->p_page_payment_tap_tcp = page_payment_tap_tcp;
    this->p_page_payment_tap_serial = page_payment_tap_serial;
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
    // ui->pushButton_continue_additional->hide();
    p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_ORDER_OVERVIEW_PATH);
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
    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    qDebug() << "is Custom mix? : " << p_page_idle->thisMachine->getSelectedProduct()->isCustomMix();
    QVector<double> customRatios = p_page_idle->thisMachine->getSelectedProduct()->getCustomMixRatios();
    qDebug() << "Mixing products (includes base) ratios:";

    for (int i = 0; i < customRatios.size(); i++)
    {
        qDebug() << QString::number(i) + " : " + QString::number(customRatios[i]);
    }

    statusbarLayout->addWidget(p_statusbar);            // Only one instance can be shown. So, has to be added/removed per page.
    statusbarLayout->setContentsMargins(0, 1874, 0, 0); // int left, int top, int right, int bottom);
    // statusbarLayout->addWidget(p_keyboard);
    // p_keyboard->move(15, 1371);

    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget
    // p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(ui->promoKeyboard);
    if (p_page_idle->thisMachine->hasMixing())
    {
        ui->promoKeyboard->findChild<QLabel *>("label_keyboard_background")->setGeometry(QRect(0, 0, 841, 364));
        ui->promoKeyboard->findChild<QPushButton *>("a")->setGeometry(QRect(53, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("b")->setGeometry(QRect(403, 252, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("backspace")->setGeometry(QRect(728, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("c")->setGeometry(QRect(253, 252, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("d")->setGeometry(QRect(203, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("done")->setGeometry(QRect(628, 252, 125, 62));
        ui->promoKeyboard->findChild<QPushButton *>("e")->setGeometry(QRect(203, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("f")->setGeometry(QRect(278, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("g")->setGeometry(QRect(353, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("h")->setGeometry(QRect(428, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("i")->setGeometry(QRect(578, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("j")->setGeometry(QRect(503, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("k")->setGeometry(QRect(578, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("l")->setGeometry(QRect(653, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("m")->setGeometry(QRect(553, 252, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("n")->setGeometry(QRect(478, 252, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("num0")->setGeometry(QRect(728, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton *>("num1")->setGeometry(QRect(53, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton *>("num2")->setGeometry(QRect(128, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton *>("num3")->setGeometry(QRect(203, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton *>("num4")->setGeometry(QRect(278, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton *>("num5")->setGeometry(QRect(353, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton *>("num6")->setGeometry(QRect(428, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton *>("num7")->setGeometry(QRect(503, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton *>("num8")->setGeometry(QRect(578, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton *>("num9")->setGeometry(QRect(653, 50, 62, 47));
        ui->promoKeyboard->findChild<QPushButton *>("o")->setGeometry(QRect(653, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("p")->setGeometry(QRect(728, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("q")->setGeometry(QRect(53, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("r")->setGeometry(QRect(278, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("s")->setGeometry(QRect(128, 179, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("t")->setGeometry(QRect(353, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("u")->setGeometry(QRect(503, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("v")->setGeometry(QRect(328, 252, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("w")->setGeometry(QRect(128, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("x")->setGeometry(QRect(178, 252, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("y")->setGeometry(QRect(428, 107, 62, 62));
        ui->promoKeyboard->findChild<QPushButton *>("z")->setGeometry(QRect(103, 252, 62, 62));

        QString coupon_icon_path = p_page_idle->thisMachine->getTemplatePathFromName(COUPON_ICON_UNAVAILABLE_PATH);
        p_page_idle->thisMachine->addPictureToLabel(ui->label_coupon_icon, coupon_icon_path);
    }

    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_PRODUCT_OVERVIEW_CSS);
    ui->label_page_title->setStyleSheet(styleSheet);
    ui->label_checkout_title->setStyleSheet(styleSheet);
    ui->pushButton_promo_input->setStyleSheet(styleSheet);
    ui->lineEdit_promo_code->setProperty("class", "promoCode");
    ui->lineEdit_promo_code->setStyleSheet(styleSheet);
    //  ui->label_product_title->setStyleSheet(styleSheet);
    ui->label_selected_volume->setStyleSheet(styleSheet);

    ui->label_invoice_name->setProperty("class", "labelOrderOverview");
    ui->label_invoice_price->setProperty("class", "labelOrderOverview");
    ui->label_discount_tag->setProperty("class", "labelDiscount");
    ui->label_invoice_discount_amount->setProperty("class", "labelDiscount");

    ui->label_invoice_additives_overview->setStyleSheet(styleSheet);
    ui->label_invoice_coupon_title->setStyleSheet(styleSheet);
    ui->label_invoice_size_title->setStyleSheet(styleSheet);
    ui->label_invoice_name->setStyleSheet(styleSheet);
    ui->label_invoice_price->setStyleSheet(styleSheet);
    ui->label_invoice_price_total->setStyleSheet(styleSheet);
    ui->label_discount_tag->setStyleSheet(styleSheet);
    ui->label_invoice_discount_amount->setStyleSheet(styleSheet);
    ui->label_invoice_box->setStyleSheet(styleSheet);

    ui->label_invoice_bottle->setProperty("class", "labelOrderOverview");
    ui->label_invoice_bottle_price->setProperty("class", "labelOrderOverview");
    ui->label_invoice_bottle->setStyleSheet(styleSheet);
    ui->label_invoice_bottle_price->setStyleSheet(styleSheet);

    ui->label_invoice_bottle->hide();
    ui->label_invoice_bottle_price->hide();
    if (p_page_idle->thisMachine->hasSelectedBottle())
    {
        // if customer seleted bottle, update bottle price and label for invoice
        ui->label_invoice_bottle->show();
        ui->label_invoice_bottle_price->show();

        QString productName = p_page_idle->thisMachine->getSelectedBottle()->getProductName();
        QString volume;
        QString unit = p_page_idle->thisMachine->getSizeUnit();
        if(unit == "oz"){
            volume = p_page_idle->thisMachine->getSelectedBottle()->getSizeAsVolumeWithCorrectUnits(1, true, true);
        }else{
            volume = QString::number(p_page_idle->thisMachine->getSelectedBottle()->getVolumeOfSelectedBottle()) + unit;
        }
        ui->label_invoice_bottle->setText(productName + " " + volume);
        
        ui->label_invoice_bottle_price->setText("$" + QString::number(p_page_idle->thisMachine->getSelectedBottle()->getPriceOfSelectedBottle(), 'f', 2));
    }

    ui->label_invoice_discount_name->setProperty("class", "labelDiscountName");
    ui->label_invoice_discount_name->setStyleSheet(styleSheet);
    ui->label_total->setStyleSheet(styleSheet);
    ui->label_gif->setStyleSheet(styleSheet);
    ui->line_invoice->setStyleSheet(styleSheet);
    ui->pushButton_select_product_page->setStyleSheet(styleSheet);

    if (p_page_idle->thisMachine->hasMixing())
    {
        QString picturePath = p_page_idle->thisMachine->getSelectedProduct()->getProductPicturePath();
        styleSheet.replace("%IMAGE_PATH%", picturePath);
    }
    ui->label_product_photo->setStyleSheet(styleSheet);
    /* Hacky transparent button */
    ui->pushButton_previous_page->setProperty("class", "buttonBGTransparent");
    ui->pushButton_previous_page->setStyleSheet(styleSheet);
    ui->pushButton_continue->setStyleSheet(styleSheet);
    ui->pushButton_continue_additional->setStyleSheet(styleSheet);

    // p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_continue, "offline");
    // p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_continue, "offline");

    ui->pushButton_to_help->setProperty("class", "buttonBGTransparent");
    ui->pushButton_to_help->setStyleSheet(styleSheet);

    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_invoice_coupon_title);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_invoice_size_title);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_page_title);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_checkout_title);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_select_product_page);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_discount_tag);
    QString product_additives_overview;
    if (additivies_overview(product_additives_overview) == "")
    {
        ui->label_invoice_additives_overview->setText("Non-customizable product");
    }
    else
    {
        ui->label_invoice_additives_overview->setText(additivies_overview(product_additives_overview));
    }

    QString keyboard = KEYBOARD_IMAGE_PATH;
    QString keyboard_picture_path = p_page_idle->thisMachine->getTemplatePathFromName(KEYBOARD_IMAGE_PATH);
    p_page_idle->thisMachine->addPictureToLabel(ui->label_keyboard_background, keyboard_picture_path);
    ui->label_keyboard_background->lower();

    _selectIdleTimeoutSec = 400;
    selectIdleTimer->start(1000);
    reset_and_show_page_elements();
    check_to_page_email();
    std::vector<ActivePaymentMethod> paymentMethods = p_page_idle->thisMachine->getAllowedPaymentMethods();
    size_t numberOfPaymentMethods = paymentMethods.size();
    switch (paymentMethods[0])
    {
    case 0:
    {
        // Set the first payment to Pay With QR
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_continue, "proceed_pay_qr");
        break;
    }
    case 1:
    case 2:
    {
        // Set the first payment to Pay with TAP
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_continue, "proceed_pay_tap");
        break;
    }
    default:
    {
        // Set the first payment to Continue
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_continue, "proceed_free");
        break;
    }
    }
    if (numberOfPaymentMethods == 1)
    {
        ui->pushButton_continue->raise();
        ui->pushButton_continue_additional->lower();
        ui->pushButton_continue->setFixedSize(QSize(740, 100));
        ui->pushButton_continue->setProperty("activePaymentMethod", paymentMethods[0]);
    }
    else if (numberOfPaymentMethods == 2)
    {
        ui->pushButton_continue->raise();
        ui->pushButton_continue_additional->raise();
        ui->pushButton_continue->setFixedSize(QSize(360, 100));
        ui->pushButton_continue->setProperty("activePaymentMethod", paymentMethods[0]);
        ui->pushButton_continue_additional->setFixedSize(QSize(360, 100));
        ui->pushButton_continue_additional->setProperty("activePaymentMethod", paymentMethods[1]);
        switch (paymentMethods[1])
        {
        case 0:
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_continue_additional, "proceed_pay_qr");
            break;
        case 1:
        case 2:
        {
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_continue_additional, "proceed_pay_tap");
            break;
        }
        default:
        {
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_continue_additional, "proceed_free");
            break;
        }
        }
    }

    // for (const auto& method : paymentMethods) {
    //     qDebug()<< "Payment Method: " << methods[method];
    // }
    // if( p_page_idle->thisMachine->getPaymentMethod()==PAYMENT_TAP_CANADA){
    //     ui->pushButton_continue->raise();
    //     // ui->pushButton_continue_additional->show();
    //     ui->pushButton_continue_additional->raise();

    //     ui->pushButton_continue->setFixedSize(QSize(360,92));
    //     p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_continue, "proceed_pay_tap");
    //     p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_continue_additional, "proceed_pay_qr");

    // }
    // else{
    //     ui->pushButton_continue->raise();
    //     ui->pushButton_continue_additional->lower();

    //     ui->pushButton_continue->setFixedSize(QSize(740,92));
    //     p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_continue, "proceed_pay");

    // }
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
    qDebug() << "Reset and show page elements";

    QString bitmap_location;
    if (!p_page_idle->thisMachine->hasMixing())
    {
        p_page_idle->thisMachine->addPictureToLabel(ui->label_product_photo, p_page_idle->thisMachine->getSelectedProduct()->getProductPicturePath());
    }
    // ui->label_selected_price->setText("$" + QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize(), 'f', 2));
    QString full_path = p_page_idle->thisMachine->getTemplatePathFromName(IMAGE_BUTTON_HELP);
    p_page_idle->thisMachine->addPictureToLabel(ui->label_help, full_path);

    ui->label_invoice_name->setText(p_page_idle->thisMachine->getSelectedProduct()->getProductName());

    // by default hide all coupon and discount elements.
    ui->promoKeyboard->hide();
    ui->pushButton_promo_input->hide();
    ui->lineEdit_promo_code->hide();
    ui->label_invoice_discount_amount->hide();
    ui->label_invoice_discount_name->hide();
    ui->label_discount_tag->hide();

    m_readyToSendCoupon = false;

    switch (p_page_idle->thisMachine->getCouponState())
    {
    case (enabled_invalid_input):
    {
        qDebug() << "Coupon state: Invalid coupon input";
        p_page_idle->thisMachine->resetCouponDiscount();
        p_page_idle->thisMachine->addCssClassToObject(ui->lineEdit_promo_code, "promoCode_invalid", PAGE_PRODUCT_OVERVIEW_CSS);
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
        p_page_idle->thisMachine->addCssClassToObject(ui->lineEdit_promo_code, "promoCode_invalid", PAGE_PRODUCT_OVERVIEW_CSS);
        QString promo_code_input_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "lineEdit_promo_code->ineligible");
        ui->lineEdit_promo_code->setText(promo_code_input_text);
        ui->lineEdit_promo_code->show();
        ui->pushButton_promo_input->show();
    }
    break;
    case (network_error):
    {
        qDebug() << "Coupon state: Network error";
        p_page_idle->thisMachine->addCssClassToObject(ui->lineEdit_promo_code, "promoCode_network_error", PAGE_PRODUCT_OVERVIEW_CSS);
        QString promo_code_input_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "lineEdit_promo_code->network_error");
        ui->lineEdit_promo_code->setText(promo_code_input_text);
        ui->lineEdit_promo_code->show();
        ui->pushButton_promo_input->show();
    }
    break;
    case (enabled_valid_active):
    {
        qDebug() << "Coupon state: Coupon valid";
        p_page_idle->thisMachine->addCssClassToObject(ui->lineEdit_promo_code, "promoCode_valid", PAGE_PRODUCT_OVERVIEW_CSS);
        QString promo_code_input_text = p_page_idle->thisMachine->getTemplateTextByPage(this, "lineEdit_promo_code->valid");
        QString entered_coupon_code = ui->lineEdit_promo_code->text().toUpper();
        // ui->lineEdit_promo_code->setText(promo_code_input_text);
        ui->lineEdit_promo_code->setText(entered_coupon_code);
        QString coupon_icon_path = p_page_idle->thisMachine->getTemplatePathFromName(COUPON_ICON_AVAILABLE_PATH);
        if (p_page_idle->thisMachine->hasMixing())
        {
            p_page_idle->thisMachine->addPictureToLabel(ui->label_coupon_icon, coupon_icon_path);
        }
        ui->label_invoice_discount_amount->show();
        ui->label_invoice_discount_name->hide();
        ui->label_discount_tag->show();
        ui->lineEdit_promo_code->show();
        ui->pushButton_promo_input->show();
        check_to_page_email();
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
        p_page_idle->thisMachine->addCssClassToObject(ui->lineEdit_promo_code, "promoCode_processing", PAGE_PRODUCT_OVERVIEW_CSS);
    }
    break;
    case (enabled_show_keyboard):
    {
        qDebug() << "Coupon state: Show keyboard";
        ui->promoKeyboard->show();
        // p_keyboard->registerCallBack(std::bind(&page_product_overview::enterButtonPressed, this));
        // p_keyboard->initializeKeyboard(true, ui->lineEdit_promo_code);
        ui->lineEdit_promo_code->clear();
        ui->lineEdit_promo_code->show();
        p_page_idle->thisMachine->addCssClassToObject(ui->lineEdit_promo_code, "promoCode", PAGE_PRODUCT_OVERVIEW_CSS);
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

    updatePriceLabel();

    ui->pushButton_previous_page->setEnabled(true);
    ui->pushButton_to_help->setEnabled(true);

    _selectIdleTimeoutSec = 400;
}

void page_product_overview::hideCurrentPageAndShowProvided(QWidget *pageToShow)
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
    // statusbarLayout->removeWidget(p_keyboard); // Only one instance can be shown. So, has to be added/removed per page.

    p_page_idle->thisMachine->pageTransition(this, pageToShow);
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
    QString selected_volume = p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(p_page_idle->thisMachine->getSelectedProduct()->getSelectedSize(), true, true);

    if (p_page_idle->thisMachine->getSelectedProduct()->getSelectedSize() == SIZE_CUSTOM_INDEX)
    {
        QString base = p_page_idle->thisMachine->getTemplateTextByElementNameAndPageAndIdentifier(ui->label_selected_volume, "custom_volume");
        ui->label_selected_volume->setText(base.arg(selected_volume));

        double selectedPrice = p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize();
        double selectedPriceCorrected = p_page_idle->thisMachine->getPriceWithDiscount(selectedPrice);
        double discount = p_page_idle->thisMachine->getDiscountAmount(selectedPrice);
        double discountFraction = p_page_idle->thisMachine->getDiscountPercentageFraction();
        qDebug() << "Discount" << discountFraction;

        QString units = p_page_idle->thisMachine->getSizeUnit();
        if (units == "ml")
        {
            units = "100ml";
            selectedPrice = selectedPrice * 100;
        }
        else if (units == "g")
        {
            if (p_page_idle->thisMachine->getSelectedProduct()->getVolumeBySize(SIZE_CUSTOM_INDEX) == VOLUME_TO_TREAT_CUSTOM_DISPENSE_AS_PER_100G)
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
        p_page_idle->thisMachine->getTemplateTextByElementNameAndPageAndIdentifier(ui->label_total, "non_custom_volume");

        base = p_page_idle->thisMachine->getTemplateTextByElementNameAndPageAndIdentifier(ui->label_total, "custom_volume");
        ui->label_total->setText(base.arg(selected_volume));
    }
    else
    {
        // The label_invoice_price total displays the discounted total even when the user goes back to the select_product page.
        // It's intended behaviour so user doesnt have to retype the promo-code
        // promo codes get reset when going to idle page.
        double selectedPrice = p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize();
        qDebug() << "Selcted price" << selectedPrice;
        double selectedPriceCorrected;
        double selectedBottlePrice;
        double discountAmount;
        double discountFraction = p_page_idle->thisMachine->getDiscountPercentageFraction();
        // checking if either bottle option is enabled
        if (p_page_idle->thisMachine->hasBuyBottleOption())
        {
            // check if customer has selected a bottle to purchase
            if (p_page_idle->thisMachine->hasSelectedBottle())
            {
                selectedBottlePrice = p_page_idle->thisMachine->getSelectedBottle()->getPriceOfSelectedBottle();
                selectedPriceCorrected = p_page_idle->thisMachine->getPriceWithDiscount(selectedPrice) + selectedBottlePrice;
                qDebug() << "Selcted price corrected" << selectedPriceCorrected;
                discountAmount = selectedPrice - (selectedPriceCorrected - selectedBottlePrice);
            }
            else
            {
                selectedPriceCorrected = p_page_idle->thisMachine->getPriceWithDiscount(selectedPrice);
                qDebug() << "Selcted price corrected" << selectedPriceCorrected;
                discountAmount = selectedPrice - selectedPriceCorrected;
                // do hide labels for bottle
            }
        }
        else
        {
            selectedPriceCorrected = p_page_idle->thisMachine->getPriceWithDiscount(selectedPrice);
            qDebug() << "Selcted price corrected" << selectedPriceCorrected;
            discountAmount = selectedPrice - selectedPriceCorrected;
        }

        ui->label_invoice_discount_amount->setText("-$" + QString::number(discountAmount, 'f', 2));
        ui->label_selected_volume->setText(selected_volume);
        ui->label_invoice_price->setText("$" + QString::number(selectedPrice, 'f', 2));
        ui->label_invoice_price_total->setText("$" + QString::number(selectedPriceCorrected, 'f', 2));

        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_total, "non_custom_volume");
    }
}

void page_product_overview::apply_promo_code(QString promocode)
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
    QString portal_base_url = p_page_idle->thisMachine->getPortalBaseUrl();
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
                qDebug() << "page_product_overview: apply promo cURL failed init";
                p_page_idle->thisMachine->setCouponState(network_error);
                return;
            }
            curl_easy_setopt(curl, CURLOPT_URL, (portal_base_url+"api/coupon/find/" + promocode + "/" + machine_id + "/" + product_serial).toUtf8().constData());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback_coupon1);
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
                    if (coupon_obj["active"] && coupon_obj["discount_amount"] != 0)
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

void page_product_overview::enterButtonPressed()
{
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

void page_product_overview::keyboardButtonPressed(int buttonID)
{
    QAbstractButton *buttonpressed = ui->buttonGroup->button(buttonID);
    QString buttonText = buttonpressed->objectName();

    if (buttonText == "backspace")
    {
        ui->lineEdit_promo_code->backspace();
    }
    else if (buttonText == "done")
    {
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
    else if (buttonText.mid(0, 3) == "num")
    {
        ui->lineEdit_promo_code->setText(ui->lineEdit_promo_code->text() + buttonText.mid(3, 1));
    }
    else
    {
        ui->lineEdit_promo_code->setText(ui->lineEdit_promo_code->text() + buttonText);
    }

    qDebug() << "Promo code input field: " << ui->lineEdit_promo_code->text();
}

void page_product_overview::on_pushButton_previous_page_clicked()
{

    this->return_to_selectProductPage();
}

void page_product_overview::on_lineEdit_promo_codeInput_clicked()
{
    p_page_idle->thisMachine->setCouponState(enabled_show_keyboard);
    statusbarLayout->removeWidget(p_statusbar);
    reset_and_show_page_elements();
}

void page_product_overview::on_pushButton_continue(int buttonID)
{

    ui->pushButton_to_help->setEnabled(false);
    ui->pushButton_previous_page->setEnabled(false);
    QAbstractButton *buttonpressed = ui->buttonGroup_continue->button(buttonID);
    int activePaymentMethod = buttonpressed->property("activePaymentMethod").toInt();
    QString portal_base_url = p_page_idle->thisMachine->getPortalBaseUrl();
    // QString paymentMethod = p_page_idle->thisMachine->getPaymentMethod();
    // if(buttonName=="paymentMethodAdditional"){
    //     paymentMethod = PAYMENT_QR;
    // }
    double selectedPrice = p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize();
    double finalPrice = p_page_idle->thisMachine->getPriceWithDiscount(selectedPrice);

    if (p_page_idle->thisMachine->isAelenPillarElseSoapStand())
    {
        if (selectedPrice == 0.0 || finalPrice == 0.0)
        {
            hideCurrentPageAndShowProvided(p_page_email);
            return;
        }
    }

    switch (activePaymentMethod)
    {
    case 0:
    {
        p_page_idle->thisMachine->setActivePaymentMethod(ActivePaymentMethod::qr);
        CURL *curl;
        CURLcode res;
        curl = curl_easy_init();

        if (!curl)
        {
            qDebug() << "page_product_overview: cURL failed init";
            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, (portal_base_url+"api/machine_data/ping").toUtf8().constData());
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
            hideCurrentPageAndShowProvided(p_page_payment_qr);
        }
        curl_easy_cleanup(curl);
        readBuffer = "";
        break;
    }
    case 1:
    {
        p_page_idle->thisMachine->setActivePaymentMethod(ActivePaymentMethod::tap_canada);
        hideCurrentPageAndShowProvided(p_page_payment_tap_serial);
        break;
    }
    case 2:
    {
        p_page_idle->thisMachine->setActivePaymentMethod(ActivePaymentMethod::tap_usa);
        hideCurrentPageAndShowProvided(p_page_payment_tap_tcp);
        break;
    }
    default:
    {
        p_page_idle->thisMachine->setActivePaymentMethod(ActivePaymentMethod::receipt_printer);
        hideCurrentPageAndShowProvided(p_page_dispense);
        break;
    }
    }
    // else if (paymentMethod == PAYMENT_QR)
    // {
    //     CURL *curl;
    //     CURLcode res;
    //     curl = curl_easy_init();

    //     if (!curl)
    //     {
    //         qDebug() << "page_product_overview: cURL failed init";
    //         return;
    //     }

    //     curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/ping");
    //     curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);

    //     res = curl_easy_perform(curl);
    //     if (res != CURLE_OK)
    //     {
    //         qDebug() << "ERROR: Failed to reach soapstandportal. error code: " + QString::number(res);
    //         hideCurrentPageAndShowProvided(p_page_wifi_error);
    //     }
    //     else
    //     {
    //         QString feedback = QString::fromUtf8(readBuffer.c_str());
    //         qDebug() << "Server feedback readbuffer: " << feedback;

    //         ui->label_invoice_price->text();
    //         hideCurrentPageAndShowProvided(p_page_payment_qr);
    //     }
    //     curl_easy_cleanup(curl);
    //     readBuffer = "";
    // }
    // else if (paymentMethod == PAYMENT_TAP_USA)
    // {
    //     hideCurrentPageAndShowProvided(p_page_payment_tap_tcp);
    // }
    // else if (paymentMethod == PAYMENT_TAP_CANADA)
    // {
    //     hideCurrentPageAndShowProvided(p_page_payment_tap_serial);
    // }
    // else if (paymentMethod == "plu" || paymentMethod == "barcode" || paymentMethod == "barcode_EAN-2 " || paymentMethod == "barcode_EAN-13")
    // {
    //     hideCurrentPageAndShowProvided(p_page_dispense);
    // }
    // else
    // {
    //     qDebug() << "WARNING: No payment method detected.";
    //     hideCurrentPageAndShowProvided(p_page_dispense);
    // }
}

void page_product_overview::return_to_selectProductPage()
{
    p_page_idle->thisMachine->hasMixing() ? hideCurrentPageAndShowProvided(p_page_product_mixing) : hideCurrentPageAndShowProvided(p_page_product);
}

void page_product_overview::on_pushButton_select_product_page_clicked()
{
    this->return_to_selectProductPage();
}

void page_product_overview::check_to_page_email()
{
    double selectedPrice = p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize();
    double finalPrice = p_page_idle->thisMachine->getPriceWithDiscount(selectedPrice);
    if (finalPrice == 0.0 || selectedPrice == 0.0)
    {
        ui->pushButton_continue_additional->lower();
        ui->pushButton_continue->setFixedSize(QSize(740, 100));
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_continue, "proceed_free");
    }
    // else{
    // p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->pushButton_continue, "proceed_pay");
    // }
}

QString page_product_overview::additivies_overview(QString product_additives_overview)
{
    for (int i = 0; i < p_page_idle->thisMachine->getSelectedProduct()->getMixPNumbers().size() - 1; i++)
    { // first pnumber in mixpnumber is the base product, so ignore
        int additivePNumber = p_page_idle->thisMachine->getSelectedProduct()->getMixPNumbers()[i + 1];
        product_additives_overview = product_additives_overview + p_page_idle->thisMachine->getProductByPNumber(additivePNumber)->getProductName() + " ";
        double additivePRatio = p_page_idle->thisMachine->getSelectedProduct()->getAdditivesRatioModifier(i);
        QString additivePRatio_string = QString::number(p_page_product_mixing->convertAdditivePRatioToPercentage(additivePRatio));
        if (i + 1 == p_page_idle->thisMachine->getSelectedProduct()->getMixPNumbers().size() - 1)
        {
            product_additives_overview = product_additives_overview + additivePRatio_string + "%";
        }
        else
        {
            product_additives_overview = product_additives_overview + additivePRatio_string + "%, ";
        }
    }
    qDebug() << product_additives_overview;
    return product_additives_overview;
}
