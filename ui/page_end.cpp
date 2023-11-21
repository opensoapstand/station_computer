//***************************************
//
// page_end.cpp
// GUI class to show user dispense has been
// completed and route back to page_idle
//
// created: 05-04-2022
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "page_end.h"
#include "ui_page_end.h"
#include "page_sendFeedback.h"

extern QString transactionLogging;

// CTOR
page_end::page_end(QWidget *parent) : QWidget(parent),
                                      ui(new Ui::page_end)
{
    ui->setupUi(this);

    connect(ui->pushButton_contact, SIGNAL(clicked()), this, SLOT(on_pushButton_contact_clicked()));

    thankYouEndTimer = new QTimer(this);
    thankYouEndTimer->setInterval(1000);
    connect(thankYouEndTimer, SIGNAL(timeout()), this, SLOT(onThankyouTimeoutTick()));

    is_in_state_thank_you = false;
    statusbarLayout = new QVBoxLayout(this);
}

/*
 * Page Tracking reference
 */
void page_end::setPage(page_dispenser *page_dispenser, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_sendFeedback *page_sendFeedback, statusbar *p_statusbar)
{
    this->p_page_idle = pageIdle;
    this->p_page_dispense = page_dispenser;
    this->paymentPage = page_qr_payment;
    this->p_page_sendFeedback = page_sendFeedback;
    this->p_statusbar = p_statusbar;
}

// DTOR
page_end::~page_end()
{
    delete ui;
}

void page_end::showEvent(QShowEvent *event)
{
    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    statusbarLayout->addWidget(p_statusbar);            // Only one instance can be shown. So, has to be added/removed per page.
    statusbarLayout->setContentsMargins(0, 1874, 0, 0); // int left, int top, int right, int bottom);

    p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_END_BACKGROUND_PATH);
    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget

    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_END_CSS);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_contact);

    ui->pushButton_to_idle->setStyleSheet(styleSheet);
    ui->label_message->setStyleSheet(styleSheet);
    ui->label_message_2->setStyleSheet(styleSheet);

    ui->label_volume_dispensed_ml->setProperty("class", "volumeDispensedStylesheet"); // set property goes first!!
    ui->label_volume_dispensed->setProperty("class", "volumeDispensedStylesheet");    // set property goes first!!

    ui->label_volume_dispensed_ml->setStyleSheet(styleSheet);
    ui->label_volume_dispensed->setStyleSheet(styleSheet);
    ui->pushButton_contact->setStyleSheet(styleSheet);

    ui->pushButton_to_idle->setEnabled(true);
    ui->pushButton_to_idle->raise();
    ui->pushButton_contact->raise();
    ui->label_client_logo->hide();

    p_page_idle->thisMachine->addClientLogoToLabel(ui->label_client_logo);

    QString paymentMethod = p_page_idle->thisMachine->getPaymentMethod();

    ui->label_volume_dispensed_ml->setText("");

    if (p_page_idle->thisMachine->hasReceiptPrinter())
    {
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message, "hasReceiptPrinter");
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message_2, "hasReceiptPrinter2");
    }
    else if (paymentMethod == PAYMENT_QR || paymentMethod == PAYMENT_TAP_TCP)
    {
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message, "qr");
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message_2, "qr2");
    }
    else
    {
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message, "any_pay");
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message_2, "any_pay2");
    }

    is_in_state_thank_you = true;

    is_controller_finished = false;
    is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = false;
    exitIsForceable = false;

    if (paymentMethod == PAYMENT_QR)
    {
        sendDispenseEndToCloud();
    }
    else
    {
        is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
    }
    _thankYouTimeoutSec = PAGE_THANK_YOU_TIMEOUT_SECONDS;
    thankYouEndTimer->start();

    QString machine_logo_full_path = p_page_idle->thisMachine->getTemplatePathFromName(MACHINE_LOGO_PATH);
    p_page_idle->thisMachine->addPictureToLabel(ui->label_manufacturer_logo, machine_logo_full_path);
    ui->label_manufacturer_logo->setStyleSheet(styleSheet);

    // p_page_idle->setDiscountPercentage(0.0);
}

size_t WriteCallback2(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void page_end::fsmReceiveFinalDispensedVolume(double dispensed)
{
    qDebug() << "Updated dispensed volume" << dispensed;
    p_page_idle->thisMachine->getSelectedProduct()->setVolumeDispensedMl(dispensed);
    QString units = p_page_idle->thisMachine->getSelectedProduct()->getUnitsForSlot();
    QString dispensed_correct_units = df_util::getConvertedStringVolumeFromMl(p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl(), units, false, true);

    double price = p_page_idle->thisMachine->getPriceWithDiscount(p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize());

    if (p_page_idle->thisMachine->getSelectedProduct()->getSelectedSize() == SIZE_CUSTOM_INDEX)
    {
    price = p_page_idle->thisMachine->getPriceWithDiscount(p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize()*p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl());
    }
    ui->label_volume_dispensed_ml->setText(dispensed_correct_units + " ( $" + QString::number(price, 'f', 2) + " )");
    
}

// void page_end::fsmReceiveFinalDispensedVolume(double dispensed)
// {
//     qDebug() << "Updated dispensed volume" << dispensed;
//     p_page_idle->thisMachine->selectedProduct->setVolumeDispensedMl(dispensed);
//     QString units = p_page_idle->thisMachine->selectedProduct->getUnitsForSlot();
//     QString dispensed_correct_units = df_util::getConvertedStringVolumeFromMl(p_page_idle->thisMachine->selectedProduct->getVolumeDispensedMl(), units, false, true);

//     double price = p_page_idle->thisMachine->getPriceWithDiscount(p_page_idle->thisMachine->selectedProduct->getBasePrice());

//     if (p_page_idle->thisMachine->selectedProduct->getSize() == SIZE_CUSTOM_INDEX)
//     {
//         price = p_page_idle->thisMachine->getPriceWithDiscount(p_page_idle->thisMachine->selectedProduct->getBasePrice() * p_page_idle->thisMachine->selectedProduct->getVolumeDispensedMl());
//     }
//     ui->label_volume_dispensed_ml->setText(dispensed_correct_units + " ( $" + QString::number(price, 'f', 2) + " )");
//      qDebug() << "End of fsm received dispensed volume";
// }

void page_end::sendDispenseEndToCloud()
{
    QString order_id = this->paymentPage->getOID();

    QString units = p_page_idle->thisMachine->getSelectedProduct()->getUnitsForSlot();
    QString dispensed_correct_units = df_util::getConvertedStringVolumeFromMl(p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl(), units, false, false);
    QString volume_remaining = p_page_idle->thisMachine->getSelectedProduct()->getVolumeRemainingCorrectUnits(false);
    QString soapstand_product_serial = p_page_idle->thisMachine->getSelectedProduct()->getPNumberAsPString();
    QString promoCode = this->p_page_idle->thisMachine->getCouponCode();
    qDebug() << "Send data at finish of order : " << order_id << ". Total dispensed: " << dispensed_correct_units << "corrected units send to soapstandportal: " << dispensed_correct_units;
    if (dispensed_correct_units == 0)
    {
        transactionLogging += "\n ERROR: No Volume dispensed";
    }
    // std::string curl_param = "contents=" + product + "&quantity_requested=" + target_volume + "&quantity_dispensed=" + dispensed_volume_units_converted + "&size_unit=" + units + "&price=" + price_string + "&productId=" + pid + "&start_time=" + start_time + "&end_time=" + end_time + "&MachineSerialNumber=" + machine_id + "&paymentMethod=Printer&volume_remaining_ml=" + to_string(volume_remaining) + "&quantity_dispensed_ml=" + to_string(productDispensers[slot_index].getVolumeDispensed()) + "&volume_remaining=" + volume_remaining_units_converted_string + "&coupon=" + coupon + "&buttonDuration=" + button_press_duration + "&buttonTimes=" + dispense_button_count + "&soapstand_product_serial=" + soapstand_product_serial;

    QString curl_param = "oid=" + order_id + "&dispensed_amount=" + dispensed_correct_units + "&coupon=" + promoCode + "&logging=" + transactionLogging + "&volume_remaining_ml=" + volume_remaining + "&soapstand_product_serial=" + soapstand_product_serial;
    qDebug() << "Curl params" << curl_param << endl;
    curl_param_array = curl_param.toLocal8Bit();
    curl_data = curl_param_array.data();

    curl = curl_easy_init();
    if (!curl)
    {
        qDebug() << "page_end: cURL failed to init. parameters:" + curl_param;
        transactionToFile(curl_data);
        is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
        return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/updateOrder");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curl_param_array.data());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback2);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);
    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        qDebug() << "ERROR: Transaction NOT sent to cloud. cURL fail. Error code: " + QString::number(res);

        transactionToFile(curl_data);
        is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
    }
    else
    {
        QString feedback = QString::fromUtf8(readBuffer.c_str());
        qDebug() << "Transaction sent to cloud. cURL success. Server feedback readbuffer: " << feedback;

        // readbuffer is a string. "true" or "false"
        if (readBuffer == "true" || "Order Completed")
        {
            // return data
            is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
        }
    }
    curl_easy_cleanup(curl);
    readBuffer = "";
}

void page_end::controllerFinishedTransaction()
{
    if (is_in_state_thank_you)
    {
        qDebug() << "Thank you page: Controller msg: All done for transaction.";
        is_controller_finished = true;
        _thankYouTimeoutSec = PAGE_THANK_YOU_TIMEOUT_SECONDS;
    }
    else
    {
        qDebug() << "WARNING: Transaction end received while not in thank you page.";
    }
}



void page_end::transactionToFile(char *curl_params)
{
    QString data_out = curl_params;
    p_page_idle->thisMachine->dfUtility->write_to_file(TRANSACTION_DISPENSE_END_OFFINE_PATH, data_out);
}

void page_end::onThankyouTimeoutTick()
{
    if (--_thankYouTimeoutSec >= 0)
    {
        // qDebug() << QString::number(_thankYouTimeoutSec);
    }
    else
    {
        finishHandler();
        // once finishHandler is activated "unsuccessfully" make sure, next time it will finish for sure!
        exitIsForceable = true;
        _thankYouTimeoutSec = PAGE_THANK_YOU_TIMEOUT_SECONDS;
    }
}

void page_end::on_pushButton_to_idle_clicked()
{
    finishHandler();
}

void page_end::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{

    is_in_state_thank_you = false;
    // p_page_idle->setCouponCode("");

    thankYouEndTimer->stop();
    //  qDebug() << "Thank you timer stopped. ";
    statusbarLayout->removeWidget(p_statusbar); // Only one instance can be shown. So, has to be added/removed per page.
    //  qDebug() << "statusbar hidden ";
    p_page_idle->thisMachine->pageTransition(this, pageToShow);
    //  qDebug() << "page transition done.  ";
}

void page_end::finishHandler()
{
    transactionLogging = "";
    if ((is_controller_finished && is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER) || exitIsForceable)
    {
        if (exitIsForceable)
        {
            qDebug() << "ERROR?!:Forced exit. controller ok?: " << is_controller_finished << " is payment finished?:" << is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER;
        }

        hideCurrentPageAndShowProvided(p_page_idle);
    }
    else
    {

        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message, "finish_transaction");
        ui->label_message_2->hide();
    }
}

void page_end::on_pushButton_contact_clicked()
{
    hideCurrentPageAndShowProvided(p_page_sendFeedback);
}
