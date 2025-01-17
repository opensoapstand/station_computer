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

    is_in_page_end = false;
    statusbarLayout = new QVBoxLayout(this);
}

/*
 * Page Tracking reference
 */
void page_end::setPage(page_dispenser *page_dispenser, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_offline_payment *page_offline_payment, page_sendFeedback *page_sendFeedback, statusbar *p_statusbar)
{
    this->p_page_idle = pageIdle;
    this->p_page_dispense = page_dispenser;
    this->paymentPage = page_qr_payment;
    this->paymentOfflinePage = page_offline_payment;
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
    if (p_page_idle->thisMachine->hasMixing())
    {
        ui->label_manufacturer_logo->hide();
    }
    is_in_page_end = true;
    // p_page_idle->setDiscountPercentage(0.0);

    // controller - UI comms

    // UI -> enters page_end (waits for final transaction details)
    //                                 controller -> sends final transaction details
    // UI -> receives final transaction details, (waits for all finished signal)
    //                                 controller -> sends all finish signal.
    // UI -> receives all finished signal
    //
    // if a signal from the controller is missed, at timeout and user action or 2 timeouts, the page will go to idle page.

    qDebug() << "Wait for controller to finish transaction.";

    ActivePaymentMethod paymentMethod = p_page_idle->thisMachine->getSelectedPaymentMethod();

    ui->label_volume_dispensed_ml->setText("");
    switch (paymentMethod)
    {
    case qr:
    case tap_canada:
    case tap_usa:
    {
        // Tap USA Payment - Payment Method
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message, "qr");
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message_2, "qr2");
        break;
    }
    case receipt_printer:
    {
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message, "hasReceiptPrinter");
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message_2, "hasReceiptPrinter2");
        break;
    }
    case none:
    {
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message, "any_pay");
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message_2, "any_pay2");
        break;
    }
    default:
    {
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message, "any_pay");
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message_2, "any_pay2");
        break;
    }
    }

    is_controller_finished = false;
    is_dispense_aftermath_complete = false;
    exitIsForceable = false;

    _thankYouTimeoutSec = PAGE_THANK_YOU_TIMEOUT_SECONDS;
    thankYouEndTimer->start();

    updateDispensedVolumeLabel();
}

void page_end::controllerReceivedDispenseAftermath(QString status, QString start_time, QString end_time, double button_press_duration, double button_press_count, double volume_dispensed, double volume_remaining, QString volumeDispensedMixProduct)
{
    qDebug() << "Received final transaction details from controller. In page end?:" << is_in_page_end;

    if (status == "invalid")
    {
    }
    else if (status == "valid")
    {

        // maintenance mode dispenses also gets processed here... Make sure never to trigger page_end end if in maintenance mode
        p_page_idle->thisMachine->getSelectedSlot()->setDispenseStartTime(start_time);
        p_page_idle->thisMachine->getSelectedSlot()->setDispenseEndTime(end_time);
        p_page_idle->thisMachine->getSelectedSlot()->setButtonPressDuration(button_press_duration);
        p_page_idle->thisMachine->getSelectedSlot()->setButtonPressCount(button_press_count);
        p_page_idle->thisMachine->getSelectedProduct()->setVolumeDispensedMl(volume_dispensed);
        p_page_idle->thisMachine->getSelectedProduct()->setVolumeRemaining(volume_remaining);
        p_page_idle->thisMachine->getSelectedProduct()->setVolumeDispensedMixedProduct(volumeDispensedMixProduct);

        // maintenance mode dispenses also gets processed here... Make sure never to trigger page_end end.
        if (is_in_page_end)
        {
            ActivePaymentMethod paymentMethod = p_page_idle->thisMachine->getSelectedPaymentMethod();
            switch (paymentMethod)
            {
            case qr:
            {
                // QR Payment - Payment Method
                sendDispenseEndToCloud();
            }
            case tap_canada:
            {
                // Tap Canada Payment - Payment Method
                sendCompleteOrderToCloudAndWriteToDatabase(PAYMENT_TAP_CANADA);
                break;
            }
            case tap_usa:
            {
                // Tap USA Payment - Payment Method
                sendCompleteOrderToCloudAndWriteToDatabase(PAYMENT_TAP_USA);
                break;
            }
            case receipt_printer:
            {
                sendCompleteOrderToCloudAndWriteToDatabase(p_page_idle->thisMachine->getPaymentOptions());
                break;
            }
            case none:
            {
                sendCompleteOrderToCloudAndWriteToDatabase("none");
                break;
            }
            default:
            {
                sendCompleteOrderToCloudAndWriteToDatabase(p_page_idle->thisMachine->getPaymentOptions());
                break;
            }
            }
            _thankYouTimeoutSec = PAGE_THANK_YOU_TIMEOUT_SECONDS;
            updateDispensedVolumeLabel();
        }
    }
    is_dispense_aftermath_complete = true;
}

void page_end::updateDispensedVolumeLabel()
{
    QString units = p_page_idle->thisMachine->getSizeUnit();
    QString dispensed_correct_units = df_util::getConvertedStringVolumeFromMl(p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl(), units, false, true);
    double price = p_page_idle->thisMachine->getPriceWithDiscount(p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize());
    if (p_page_idle->thisMachine->getSelectedProduct()->getSelectedSize() == SIZE_CUSTOM_INDEX)
    {
        price = p_page_idle->thisMachine->getPriceWithDiscount(p_page_idle->thisMachine->getSelectedProduct()->getBasePrice(SIZE_CUSTOM_INDEX) * p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl());
    }
    ui->label_volume_dispensed_ml->setText(dispensed_correct_units + " ( $" + QString::number(price, 'f', 2) + " )");
}

void page_end::controllerReceivedFinishedTransaction()
{
    qDebug() << "Received Finished transaction from controller in page end?:" << is_in_page_end;

    if (is_in_page_end)
    {
        qDebug() << "Thank you page: Controller msg: All done for transaction.";
        is_controller_finished = true;
    }
    else
    {
        qDebug() << "WARNING: Transaction end received while not in thank you page.";
    }
}

void page_end::onThankyouTimeoutTick()
{
    if (--_thankYouTimeoutSec >= 0)
    {
        // qDebug() << QString::number(_thankYouTimeoutSec);
    }
    else
    {
        hideCurrentPageAndShowProvided(p_page_idle);

        // once hideCurrentPageAndShowProvided is activated "unsuccessfully" make sure, next time it will finish for sure (in order for the UI never to be stuck)!
        exitIsForceable = true;
        _thankYouTimeoutSec = PAGE_THANK_YOU_TIMEOUT_SECONDS;
    }
}

void page_end::on_pushButton_to_idle_clicked()
{
    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_end::on_pushButton_contact_clicked()
{
    hideCurrentPageAndShowProvided(p_page_sendFeedback);
}

void page_end::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    bool exit_approved = false;

    if (exitIsForceable)
    {
        p_page_idle->thisMachine->resetTransactionLogging();
        qDebug() << "ERROR? This is a forced exit. Is controller finished?: " << is_controller_finished << " Is all handled?:" << is_dispense_aftermath_complete;
        exit_approved = true; // hideCurrentPageAndShowProvided(p_page_idle);
    }
    else if (is_controller_finished && is_dispense_aftermath_complete)
    {
        exit_approved = true; // hideCurrentPageAndShowProvided(p_page_idle);
    }
    else
    {
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_message, "finish_transaction");
        ui->label_message_2->hide();
    }

    if (exit_approved)
    {
        is_in_page_end = false;
        thankYouEndTimer->stop();
        statusbarLayout->removeWidget(p_statusbar); // Only one instance can be shown. So, has to be added/removed per page.
        p_page_idle->thisMachine->pageTransition(this, pageToShow);
    }
}

void page_end::sendDispenseEndToCloud()
{
    // Update the volume dispensed at the end of the order
    QString order_id = this->paymentPage->getOID();

    QString units = p_page_idle->thisMachine->getSizeUnit();
    QString dispensed_correct_units = df_util::getConvertedStringVolumeFromMl(p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl(), units, false, false);
    QString volume_remaining = QString::number(p_page_idle->thisMachine->getSelectedProduct()->getVolumeRemaining());
    QString soapstand_product_serial = p_page_idle->thisMachine->getSelectedProduct()->getPNumberAsPString();
    QString promoCode = this->p_page_idle->thisMachine->getCouponCode();
    QString volume_dispensed_mix_product = this->p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMixedProduct();
    bool processed_by_backend = false;
    qDebug() << "Send data at finish of order : " << order_id << ". Total dispensed: " << dispensed_correct_units << "corrected units send to soapstandportal: " << dispensed_correct_units + "&mixProductInfo=" + volume_dispensed_mix_product;
    if (dispensed_correct_units == 0)
    {
        p_page_idle->thisMachine->addToTransactionLogging("\n ERROR: No Volume dispensed");
    }
    QString curl_params = "oid=" + order_id + "&dispensed_amount=" + dispensed_correct_units + "&coupon=" + promoCode + "&logging=" + p_page_idle->thisMachine->getTransactionLogging() + "&volume_remaining_ml=" + volume_remaining + "&soapstand_product_serial=" + soapstand_product_serial;

    std::tie(res, readBuffer, http_code) = p_page_idle->thisMachine->sendRequestToPortal(PORTAL_UPDATE_ALREADY_CREATED_ORDER, "POST", curl_params, "PAGE_END");

    if (res != CURLE_OK)
    {
        qDebug() << "ERROR: Transaction NOT sent to cloud. cURL fail. Error code: " + QString::number(res);

        transactionToFile(curl_params);
        // is_dispense_aftermath_complete = true;
    }
    else
    {
        QString feedback = QString::fromUtf8(readBuffer.c_str());
        qDebug() << "Transaction sent to cloud. cURL success. Server feedback readbuffer: " << feedback;

        // readbuffer is a string. "true" or "false"
        if (readBuffer == "true" || "Order Completed")
        {
            processed_by_backend = true;
            // return data
            // is_dispense_aftermath_complete = true;
        }
    }
    p_page_idle->thisMachine->updateTransactionInDb(processed_by_backend,volume_dispensed_mix_product);
}

// Push the complete order information to cloud
void page_end::sendCompleteOrderToCloudAndWriteToDatabase(QString paymentMethod)
{
    QString MachineSerialNumber = p_page_idle->thisMachine->getMachineId();
    QString productUnits = p_page_idle->thisMachine->getSizeUnit();

    QString productId = p_page_idle->thisMachine->getSelectedProductAwsProductId();

    QString contents = p_page_idle->thisMachine->getSelectedProduct()->getProductName();
    QString quantity_requested = QString::number(p_page_idle->thisMachine->getSelectedProduct()->getVolumeOfSelectedSize());
    char drinkSize = p_page_idle->thisMachine->getSelectedProduct()->getSelectedSizeAsChar();
    QString originalPrice;
    if (p_page_idle->thisMachine->getSelectedProduct()->getSelectedSize() == SIZE_CUSTOM_INDEX)
    {
        originalPrice = QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize() * p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl());
    }
    else
    {
        originalPrice = QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize());
    }
    QString dispensed_correct_units = df_util::getConvertedStringVolumeFromMl(p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl(), productUnits, false, false);
    QString dispensed_volume_ml = QString::number(p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl());
    QString volume_remaining = QString::number(p_page_idle->thisMachine->getSelectedProduct()->getVolumeRemaining());
    QString soapstand_product_serial = p_page_idle->thisMachine->getSelectedProduct()->getPNumberAsPString();
    QString promoCode = this->p_page_idle->thisMachine->getCouponCode();
    QString startTime = this->p_page_idle->thisMachine->getSelectedSlot()->getDispenseStartTime();
    QString endTime = this->p_page_idle->thisMachine->getSelectedSlot()->getDispenseEndTime();
    QString button_press_duration = QString::number(this->p_page_idle->thisMachine->getSelectedSlot()->getButtonPressDuration());
    QString button_press_count = QString::number(this->p_page_idle->thisMachine->getSelectedSlot()->getButtonPressCount());
    QString volume_dispensed_mix_product = this->p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMixedProduct();
    bool processed_by_backend = 0;
    int slot = p_page_idle->thisMachine->getSelectedSlot()->getSlotId();

    if (dispensed_correct_units == 0)
    {
        p_page_idle->thisMachine->addToTransactionLogging("\n ERROR: No Volume dispensed");
    }
    QString curl_params = "contents=" + contents + "&quantity_requested=" + quantity_requested + "&quantity_dispensed=" + dispensed_volume_ml +
                          "&size_unit=" + productUnits + "&price=" + originalPrice + "&productId=" + productId + "&start_time=" + startTime +
                          "&end_time=" + endTime + "&MachineSerialNumber=" + MachineSerialNumber + "&paymentMethod=" + paymentMethod + "&volume_remaining_ml=" +
                          volume_remaining + "&quantity_dispensed_ml=" + dispensed_volume_ml +
                          "&volume_remaining=" + volume_remaining + "&coupon=" + promoCode + "&buttonDuration=" + button_press_duration +
                          "&buttonTimes=" + button_press_count + "&pnumber=" + soapstand_product_serial + "&mixProductInfo={" + volume_dispensed_mix_product + "}";

    std::tie(res, readBuffer, http_code) = p_page_idle->thisMachine->sendRequestToPortal(PORTAL_SEND_NEW_COMPLETE_ORDER_DEV, "POST", curl_params, "PAGE_END");
    qDebug() << curl_params;
    
    if (res != CURLE_OK)
    {
        qDebug() << "ERROR: Transaction NOT sent to cloud. cURL fail. Error code: " + QString::number(res);

        transactionToFile(curl_params);
        // is_dispense_aftermath_complete = true;
    }
    else
    {
        QString feedback = QString::fromUtf8(readBuffer.c_str());
        qDebug() << "Transaction sent to cloud. cURL success. Server feedback readbuffer: " << feedback;

        // readbuffer is a string. "true" or "false"
        if (readBuffer == "true" || "Order Completed")
        {
            processed_by_backend = 1;
            // return data
            // is_dispense_aftermath_complete = true;
        }
    }
    p_page_idle->thisMachine->updateTransactionInDb(processed_by_backend, volume_dispensed_mix_product);
}

void page_end::transactionToFile(QString curl_params)
{
    p_page_idle->thisMachine->dfUtility->write_to_file(TRANSACTION_DISPENSE_END_OFFLINE_PATH, curl_params);
}

// QMap<int, QPair<double, double>> extractValues(const QString& data) {
    
//     QMap<int, QPair<double, double>> resultMap;
//     // Split the string to multiple key value pairs with delimiter - "],"
//     QStringList pairs = data.split("],");
//     for (const QString& pair : pairs) {
//         QString keyValuePair = pair.trimmed();
//         // Extracting the product number
//         int startIdx = keyValuePair.indexOf("P-");
//         startIdx += 2; // Index after "P-"
//         int endIdx = keyValuePair.indexOf(":", startIdx);
//         int productNumber = keyValuePair.mid(startIdx, endIdx - startIdx).toInt();

//         // Extracting volume dispensed and volume remaining
//         int bracketStart = keyValuePair.indexOf("[");
//         int commaIndex = keyValuePair.indexOf(",", bracketStart);
//         double volumeDispensed = keyValuePair.mid(bracketStart + 1, commaIndex - bracketStart - 1).trimmed().toDouble();
        
//         int endIndex = keyValuePair.indexOf("]", commaIndex);
//         double volumeRemaining = keyValuePair.mid(commaIndex + 1, endIndex - commaIndex - 1).trimmed().toDouble();
//         // Add key-value pair to result map
//         resultMap.insert(productNumber, qMakePair(volumeDispensed, volumeRemaining));
//     }
//     return resultMap;
// }

// void page_end::updateTransactionInDb(bool processed_by_backend, QString volume_dispensed_mix_product){
//     QString productId = p_page_idle->thisMachine->getSelectedProductAwsProductId();
//     QString contents = p_page_idle->thisMachine->getSelectedProduct()->getProductName();
//     QString quantity_requested = QString::number(p_page_idle->thisMachine->getSelectedProduct()->getVolumeOfSelectedSize());
//     QString originalPrice;
//     if (p_page_idle->thisMachine->getSelectedProduct()->getSelectedSize() == SIZE_CUSTOM_INDEX)
//     {
//         originalPrice = QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize() * p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl());
//     }
//     else
//     {
//         originalPrice = QString::number(p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize());
//     }
//     QString dispensed_volume_ml = QString::number(p_page_idle->thisMachine->getSelectedProduct()->getVolumeDispensedMl());
//     QString volume_remaining = QString::number(p_page_idle->thisMachine->getSelectedProduct()->getVolumeRemaining());
//     QString soapstand_product_serial = p_page_idle->thisMachine->getSelectedProduct()->getPNumberAsPString();
//     QString startTime = this->p_page_idle->thisMachine->getSelectedSlot()->getDispenseStartTime();
//     QString endTime = this->p_page_idle->thisMachine->getSelectedSlot()->getDispenseEndTime();
//     QString button_press_duration = QString::number(this->p_page_idle->thisMachine->getSelectedSlot()->getButtonPressDuration());
//     QString button_press_count = QString::number(this->p_page_idle->thisMachine->getSelectedSlot()->getButtonPressCount());
//     int slot = p_page_idle->thisMachine->getSelectedSlot()->getSlotId();
//     QString sqlQuery = QString("INSERT INTO transactions (product,quantity_requested,price,start_time,quantity_dispensed,end_time,volume_remaining,button_duration,button_times,processed_by_backend,product_id, soapstand_product_serial,slot) VALUES ('%1', %2, %3, '%4', %5, '%6', %7, %8, %9, %10, '%11', '%12', %13)")
//                 .arg(contents)
//                 .arg(quantity_requested)
//                 .arg(originalPrice)
//                 .arg(startTime)
//                 .arg(dispensed_volume_ml)
//                 .arg(endTime)
//                 .arg(volume_remaining)
//                 .arg(button_press_duration)
//                 .arg(button_press_count)
//                 .arg(processed_by_backend)
//                 .arg(productId)
//                 .arg(soapstand_product_serial)
//                 .arg(slot);
//     db->executeQuery(sqlQuery, USAGE_DB_PATH);
//     // Clean the string by removing escape chracters
//     volume_dispensed_mix_product = volume_dispensed_mix_product.replace("\\\"", "\"").replace("\"", "");
//     QMap<int, QPair<double, double>> resultMap = extractValues(volume_dispensed_mix_product);

//     // update product table with p Number
//     for (auto it = resultMap.begin(); it != resultMap.end(); ++it) {
//         int pNumber = it.key();
//         double volume_dispensed = it.value().first;
//         double volume_remaining = it.value().second; 
//         p_page_idle->thisMachine->setSelectedProduct(pNumber);
//         QString selected_product_state_str = p_page_idle->thisMachine->getSelectedProduct()->getProductStateAsString();
//         QString sqlQueryProductUpdate = QString("UPDATE products SET volume_dispensed_total=volume_dispensed_total + %1, volume_remaining=%2, volume_dispensed_since_restock=volume_dispensed_since_restock+%3,status_text='%4' WHERE soapstand_product_serial='%5';")
//                .arg(volume_dispensed)
//                .arg(volume_remaining)
//                .arg(volume_dispensed)
//                .arg(selected_product_state_str)
//                .arg(pNumber);        
//         db->executeQuery(sqlQueryProductUpdate, CONFIG_DB_PATH);
//     }

// }

