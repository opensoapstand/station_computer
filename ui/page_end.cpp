//***************************************
//
// page_end.cpp
// GUI class to show user dispense has been
// completed and route back to page_idle
//
// created: 05-04-2022
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
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
}

/*
 * Page Tracking reference
 */
void page_end::setPage(page_dispenser *page_dispenser, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_sendFeedback *page_sendFeedback)
{
    this->p_page_idle = pageIdle;
    this->p_page_dispense = page_dispenser;
    this->paymentPage = page_qr_payment;
    this->p_page_sendFeedback = page_sendFeedback;
}

// DTOR
page_end::~page_end()
{
    delete ui;
}

void page_end::showEvent(QShowEvent *event)
{

    p_page_idle->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    p_page_idle->setTemplateTextToObject(ui->pushButton_contact);

    QString styleSheet = p_page_idle->getCSS(PAGE_END_CSS);

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
    ui->label_customer_logo->hide();

    p_page_idle->addCustomerLogoToLabel(ui->label_customer_logo);
    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_END_BACKGROUND_PATH);

    QString paymentMethod = p_page_idle->selectedProduct->getPaymentMethod();

    if (p_page_idle->thisMachine.hasReceiptPrinter())
    {
        p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_message, "hasReceiptPrinter");
        p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_message_2, "hasReceiptPrinter2");
    }
    else if (paymentMethod == "qr" || paymentMethod == "tapTcp")
    {
        p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_message, "qr");
        p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_message_2, "qr2");
    }
    else
    {
        p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_message, "any_pay");
        p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_message_2, "any_pay2");
    }

    is_in_state_thank_you = true;

    is_controller_finished = false;
    is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = false;
    exitIsForceable = false;

    if (paymentMethod == "qr")
    {
        sendDispenseEndToCloud();
    }
    else
    {
        is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
    }
    _thankYouTimeoutSec = PAGE_THANK_YOU_TIMEOUT_SECONDS;
    thankYouEndTimer->start();

    QString machine_logo_full_path = p_page_idle->thisMachine.getTemplatePathFromName(MACHINE_LOGO_PATH);
    p_page_idle->addPictureToLabel(ui->label_manufacturer_logo, machine_logo_full_path);
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
    p_page_idle->selectedProduct->setVolumeDispensedMl(dispensed);
    QString units = p_page_idle->selectedProduct->getUnitsForSlot();
    QString dispensed_correct_units = df_util::getConvertedStringVolumeFromMl(p_page_idle->selectedProduct->getVolumeDispensedMl(), units, false, true);

    double price = p_page_idle->thisMachine.getPriceWithDiscount(p_page_idle->selectedProduct->getPriceCorrected());

    if (p_page_idle->selectedProduct->getSize() == SIZE_CUSTOM_INDEX)
    {
        price = p_page_idle->selectedProduct->getVolumeDispensedMl() * price;
    }
    ui->label_volume_dispensed_ml->setText(dispensed_correct_units + " ( $" + QString::number(price, 'f', 2) + " )");
    
}

void page_end::sendDispenseEndToCloud()
{
    QString order_id = this->paymentPage->getOID();

    QString units = p_page_idle->selectedProduct->getUnitsForSlot();
    QString dispensed_correct_units = df_util::getConvertedStringVolumeFromMl(p_page_idle->selectedProduct->getVolumeDispensedMl(), units, false, false);

    QString promoCode = this->p_page_idle->thisMachine.getPromoCode();
    qDebug() << "Send data at finish of order : " << order_id << ". Total dispensed: " << dispensed_correct_units << "corrected units send to soapstandportal: " << dispensed_correct_units;
    if (dispensed_correct_units == 0)
    {
        transactionLogging += "\n ERROR: No Volume dispensed";
    }
    QString curl_param = "oid=" + order_id + "&dispensed_amount=" + dispensed_correct_units + "&coupon=" + promoCode + "&logging=" + transactionLogging;
    ;
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
    p_page_idle->thisMachine.dfUtility->write_to_file(TRANSACTION_DISPENSE_END_OFFINE_PATH, data_out);
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
    // p_page_idle->setPromoCode("");

    thankYouEndTimer->stop();
    p_page_idle->pageTransition(this, pageToShow);
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

        p_page_idle->setTemplateTextWithIdentifierToObject(ui->label_message, "finish_transaction");
        ui->label_message_2->hide();
    }
}

void page_end::on_pushButton_contact_clicked()
{
    hideCurrentPageAndShowProvided(p_page_sendFeedback);
}
