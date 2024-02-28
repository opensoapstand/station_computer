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
extern bool isFreeEmailOrder;

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
    is_in_page_end = false;
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
    ui->label_message_2->show();
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
    QString machine_logo_full_path = p_page_idle->thisMachine->getTemplatePathFromName(MACHINE_LOGO_PATH);
    p_page_idle->thisMachine->addPictureToLabel(ui->label_manufacturer_logo, machine_logo_full_path);
    ui->label_manufacturer_logo->setStyleSheet(styleSheet);
    if(p_page_idle->thisMachine->hasMixing()){
        ui->label_manufacturer_logo->hide();
    }
    is_in_page_end = true;
    // p_page_idle->setDiscountPercentage(0.0);

}

// void page_end::fsmReceiveFinalDispensedVolume(double dispensed)
// {
//     qDebug() << "Updated dispensed volume" << dispensed;
//     p_page_idle->thisMachine->getSelectedProduct()->setVolumeDispensedMl(dispensed);
//     updateDispensedVolumeLabel();    
// }

void page_end::fsmReceiveFinalTransactionMessage(QString start_time, QString end_time, double button_press_duration, double button_press_count, double volume_dispensed,QString volumeDispensedMixProduct)
{
    p_page_idle->thisMachine->getSelectedSlot()->setDispenseStartTime(start_time);
    p_page_idle->thisMachine->getSelectedSlot()->setDispenseEndTime(end_time);
    p_page_idle->thisMachine->getSelectedSlot()->setButtonPressDuration(button_press_duration);
    p_page_idle->thisMachine->getSelectedSlot()->setButtonPressCount(button_press_count);
    p_page_idle->thisMachine->getSelectedProduct()->setVolumeDispensedMl(volume_dispensed);
    p_page_idle->thisMachine->getSelectedProduct()->setVolumeDispensedMixedProduct(volumeDispensedMixProduct);    


    // maintenance mode dispenses also get processed here... Make sure never to trigger page_end end.
    if(is_in_page_end){
        waitToFinishTransactionInFsm();
    }
}

void page_end::updateDispensedVolumeLabel(){
    QString units = p_page_idle->thisMachine->getSizeUnit();
    QString dispensed_correct_units = df_util::getConvertedStringVolumeFromMl(p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl(), units, false, true);
    double price = p_page_idle->thisMachine->getPriceWithDiscount(p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize());
    if (p_page_idle->thisMachine->getSelectedProduct()->getSelectedSize() == SIZE_CUSTOM_INDEX)
    {
    price = p_page_idle->thisMachine->getPriceWithDiscount(p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize()*p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl());
    }
    ui->label_volume_dispensed_ml->setText(dispensed_correct_units + " ( $" + QString::number(price, 'f', 2) + " )");
}

void page_end::sendDispenseEndToCloud()
{
    //Update the volume dispensed at the end of the order 
    QString order_id = this->paymentPage->getOID();

    QString units = p_page_idle->thisMachine->getSizeUnit();
    QString dispensed_correct_units = df_util::getConvertedStringVolumeFromMl(p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl(), units, false, false);
    QString volume_remaining = p_page_idle->thisMachine->getSelectedProduct()->getVolumeRemainingCorrectUnits(false);
    QString soapstand_product_serial = p_page_idle->thisMachine->getSelectedProduct()->getPNumberAsPString();
    QString promoCode = this->p_page_idle->thisMachine->getCouponCode();
    QString volume_dispensed_mix_product = this->p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMixedProduct();
    qDebug() << "Send data at finish of order : " << order_id << ". Total dispensed: " << dispensed_correct_units << "corrected units send to soapstandportal: " << dispensed_correct_units+ "&mixProductInfo=" + volume_dispensed_mix_product;
    if (dispensed_correct_units == 0)
    {
        p_page_idle->thisMachine->addToTransactionLogging("\n ERROR: No Volume dispensed");
    }
    QString curl_params = "oid=" + order_id + "&dispensed_amount=" + dispensed_correct_units + "&coupon=" + promoCode + "&logging=" + p_page_idle->thisMachine->getTransactionLogging() + "&volume_remaining_ml=" + volume_remaining + "&soapstand_product_serial=" + soapstand_product_serial;

    std::tie(res,readBuffer, http_code) = p_page_idle->thisMachine->sendRequestToPortal(PORTAL_UPDATE_ALREADY_CREATED_ORDER, "POST", curl_params, "PAGE_END");

    if (res != CURLE_OK)
    {
        qDebug() << "ERROR: Transaction NOT sent to cloud. cURL fail. Error code: " + QString::number(res);

        transactionToFile(curl_params);
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
}

//Push the complete order information to cloud
void page_end::sendCompleteOrderToCloud(QString paymentMethod)
{
    QString MachineSerialNumber = p_page_idle->thisMachine->getMachineId();
    QString productUnits = p_page_idle->thisMachine->getSizeUnit();
    QString productId = p_page_idle->thisMachine->getSelectedProduct()->getAwsProductId();
    QString contents = p_page_idle->thisMachine->getSelectedProduct()->getProductName();
    QString quantity_requested = p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(false, false);
    char drinkSize = p_page_idle->thisMachine->getSelectedProduct()->getSelectedSizeAsChar();
    QString originalPrice = QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize());  
    QString dispensed_correct_units = df_util::getConvertedStringVolumeFromMl(p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl(), productUnits, false, false);
    QString volume_remaining = p_page_idle->thisMachine->getSelectedProduct()->getVolumeRemainingCorrectUnits(false);
    QString soapstand_product_serial = p_page_idle->thisMachine->getSelectedProduct()->getPNumberAsPString();
    QString promoCode = this->p_page_idle->thisMachine->getCouponCode();  
    QString startTime = this->p_page_idle->thisMachine->getSelectedSlot()->getDispenseStartTime();
    QString endTime = this->p_page_idle->thisMachine->getSelectedSlot()->getDispenseEndTime();
    QString button_press_duration = QString::number(this->p_page_idle->thisMachine->getSelectedSlot()->getButtonPressDuration());
    QString button_press_count = QString::number(this->p_page_idle->thisMachine->getSelectedSlot()->getButtonPressCount());
    QString volume_dispensed_mix_product = this->p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMixedProduct();
    if (dispensed_correct_units == 0)
        {
            p_page_idle->thisMachine->addToTransactionLogging("\n ERROR: No Volume dispensed");
        }
    QString curl_params = "contents=" + contents + "&quantity_requested=" + quantity_requested + "&quantity_dispensed=" + dispensed_correct_units + \
                 "&size_unit=" + productUnits + "&price=" + originalPrice + "&productId=" + productId + "&start_time=" + startTime + 
                 "&end_time=" + endTime + "&MachineSerialNumber=" + MachineSerialNumber + "&paymentMethod="+paymentMethod+"&volume_remaining_ml=" + 
                 volume_remaining + "&quantity_dispensed_ml=" + dispensed_correct_units +
                 "&volume_remaining=" + volume_remaining + "&coupon=" + promoCode +"&buttonDuration=" + button_press_duration + 
                 "&buttonTimes=" + button_press_count + "&pnumber=" + soapstand_product_serial + "&mixProductInfo={" + volume_dispensed_mix_product+"}";
                 
    std::tie(res,readBuffer, http_code) = p_page_idle->thisMachine->sendRequestToPortal(PORTAL_SEND_NEW_COMPLETE_ORDER_DEV, "POST", curl_params, "PAGE_END");

    if (res != CURLE_OK)
    {
        qDebug() << "ERROR: Transaction NOT sent to cloud. cURL fail. Error code: " + QString::number(res);

        transactionToFile(curl_params);
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

void page_end::transactionToFile(QString curl_params)
{
    p_page_idle->thisMachine->dfUtility->write_to_file(TRANSACTION_DISPENSE_END_OFFINE_PATH, curl_params);
}

void page_end::onThankyouTimeoutTick()
{
    if (--_thankYouTimeoutSec >= 0)
    {
        // qDebug() << QString::number(_thankYouTimeoutSec);
    }
    else
    {
        qDebug() << "page end: timemout. forceable exit = true";
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
    thankYouEndTimer->stop();
    statusbarLayout->removeWidget(p_statusbar); // Only one instance can be shown. So, has to be added/removed per page.
    p_page_idle->thisMachine->pageTransition(this, pageToShow);
    is_in_page_end = false;
}

void page_end::finishHandler()
{
    // transactionLogging = "";
    p_page_idle->thisMachine->resetTransactionLogging();
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

void page_end::waitToFinishTransactionInFsm(){
    ActivePaymentMethod paymentMethod = p_page_idle->thisMachine->getActivePaymentMethod();

    ui->label_volume_dispensed_ml->setText("");
    switch(paymentMethod){
        case 0:
        {
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message, "qr");
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message_2, "qr2");
            is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = false;
            sendDispenseEndToCloud();
        }
        case 1:
        case 2:{
            //Tap Payment - Payment Method
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message, "qr");
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message_2, "qr2");
            is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
            break;
        }
        case 3:{
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message, "hasReceiptPrinter");
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message_2, "hasReceiptPrinter2");
            is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = false;
            sendCompleteOrderToCloud(PAYMENT_RECEIPT_PRINTER);
            break;
        }
        default:{
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message, "any_pay");
            p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message_2, "any_pay2");
            is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
            sendCompleteOrderToCloud(PAYMENT_RECEIPT_PRINTER);
            break;
        }
    }

    is_in_state_thank_you = true;

    is_controller_finished = false;
    exitIsForceable = false;
  
    _thankYouTimeoutSec = PAGE_THANK_YOU_TIMEOUT_SECONDS;
    thankYouEndTimer->start();

    updateDispensedVolumeLabel();
}