//***************************************
//
// pagepayment.cpp
// GUI class while machine is processing
// payment.
//
// Coordinates User input from payment select
// class then communcates results to page_dispenser.
//
// created: 05-04-2022
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "pagepayment.h"
#include "ui_pagepayment.h"

#include "page_product.h"
#include "page_dispenser.h"
#include "page_idle.h"

// CTOR
pagePayment::pagePayment(QWidget *parent) : QWidget(parent),
                                            ui(new Ui::pagePayment)
{
    // Fullscreen background setup
    ui->setupUi(this);
   
    ui->previousPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->mainPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->payment_bypass_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->refreshButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    ui->payment_bypass_Button->setEnabled(false);

    // **** Timer and Slot Setup ****

    // Payment Tap Ready
    readTimer = new QTimer(this);
    connect(readTimer, SIGNAL(timeout()), this, SLOT(readTimer_loop()));

    // Payment Progress
    paymentProgressTimer = new QTimer(this);
    connect(paymentProgressTimer, SIGNAL(timeout()), this, SLOT(progressStatusLabel()));
    paymentProgressTimer->setInterval(500);

    // Payment Declined
    declineTimer = new QTimer(this);
    connect(declineTimer, SIGNAL(timeout()), this, SLOT(declineTimer_start()));

    // Idle Payment reset
    idlePaymentTimer = new QTimer(this);
    connect(idlePaymentTimer, SIGNAL(timeout()), this, SLOT(idlePaymentTimeout()));

    qrTimer = new QTimer(this);
    connect(qrTimer, SIGNAL(timeout()), this, SLOT(qrProcessedPeriodicalCheck()));

    // XXX: Comment on/off for Bypassing payment testing
    qDebug() << "constructor Payment page. Check for tap.";
    tap_payment = false;
    DbManager db(DB_PATH);

    for (int i = 0; i < SLOT_COUNT; i++)
    {
        if (db.getPaymentMethod(i) == "tap")
        {
            tap_payment = true;
            ui->payment_bypass_Button->setEnabled(false);
        }
        else
        {
            ui->payment_bypass_Button->setEnabled(false);
        }
    }
    db.closeDB();
    if (tap_payment)
    {
        while (!tap_init())
            ;
    }
}

void pagePayment::stopPayTimers()
{
    //    readTimer->stop();
    //    qDebug() << "pagePayment: Stop Timers" << endl;
    if (paymentProgressTimer != nullptr)
    {
        //        qDebug() << "cancel payment progress Timer" << endl;
        paymentProgressTimer->stop();
    }

    if (declineTimer != nullptr)
    {
        //        qDebug() << "cancel decline Timer" << endl;
        declineTimer->stop();
    }

    if (idlePaymentTimer != nullptr)
    {
        //        qDebug() << "cancel page_idle payment Timer" << endl;
        idlePaymentTimer->stop();
    }

    if (paymentEndTimer != nullptr)
    {
        //        qDebug() << "cancel page_idle payment END Timer" << endl;
        paymentEndTimer->stop();
    }

    if (readTimer != nullptr)
    {
        //        qDebug() << "cancel readTimer" << endl;
        readTimer->stop();
    }

    if (qrTimer != nullptr)
    {
        //        qDebug() << "cancel qrTimer" << endl;
        qrTimer->stop();
    }
    //    qDebug() << "pagePayment: Stopped Timers" << endl;
}

/*
 * Page Tracking reference
 */
void pagePayment::setPage(pageProduct *pageSizeSelect, page_dispenser *page_dispenser, page_idle *pageIdle, page_help *pageHelp)
{
    tmpCounter = 0;
    this->p_pageProduct = pageSizeSelect;
    this->p_page_dispense = page_dispenser;
    this->p_page_idle = pageIdle;
    this->helpPage = pageHelp;
}

void pagePayment::resizeEvent(QResizeEvent *event)
{
    // FIXME: MAGIC NUMBER!!! UX410 Socket Auto Close time is 60 seconds so timer kills page GUI
    //    idlePaymentTimer->start(60000);

    int product_slot___ = p_page_idle->currentProductOrder->getSelectedSlot();
    char drinkSize;
    if (p_page_idle->currentProductOrder->getSelectedSize() == SIZE_SMALL_INDEX)
    {
        drinkSize = 's';
    }
    if (p_page_idle->currentProductOrder->getSelectedSize() == SIZE_LARGE_INDEX)
    {
        drinkSize = 'l';
    }

    QString bitmap_location;

    if (!tap_payment)
    {
        bitmap_location = PAGE_QR_PAY_BACKGROUND_PATH;
    }

    else
    {
        qDebug("ERROR: No tap payment available yet.");
    }

    QPixmap background(bitmap_location);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);

    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    ui->order_total_amount->setText("Total: $" + QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2));
    ui->order_drink_amount->setText("");

    ui->productLabel->setText(p_page_idle->currentProductOrder->getSelectedProductName() + " " + p_page_idle->currentProductOrder->getSelectedSizeToVolumeWithCorrectUnits(true, true));

    response = false;
    ui->refreshLabel->hide();
}

// DTOR
pagePayment::~pagePayment()
{
    delete ui;
}

/* ----- GUI ----- */

// Labels and button for tapping payment
void pagePayment::displayPaymentPending(bool isVisible)
{
}

// Navigation: Back to Drink Size Selection
void pagePayment::on_previousPage_Button_clicked()
{
    stopPayTimers();
    response = true;
    readTimer->stop();
    if (tap_payment)
    {
        cancelPayment();
    }
    p_pageProduct->resizeEvent(pageProductResize);
    p_pageProduct->showFullScreen();
    //    usleep(100);
    this->hide();
}

void pagePayment::on_payment_bypass_Button_clicked()
{
    proceed_to_dispense();
}

void pagePayment::proceed_to_dispense()
{
    stopPayTimers();
    // p_page_dispense->showEvent(dispenseEvent);
    p_page_dispense->showFullScreen();
    this->hide();
}

void pagePayment::updateTotals(string drinkDescription, string drinkAmount, string orderTotal)
{
}

void pagePayment::on_mainPage_Button_clicked()
{
    stopPayTimers();
    response = true;
    readTimer->stop();
    if (tap_payment)
    {
        cancelPayment();
    }
    helpPage->showFullScreen();
    this->hide();
}

/*Cancel any previous payment*/
void pagePayment::cancelPayment()
{

    com.flushSerial();
    /*Cancel any previous payment*/
    // if(purchaseEnable){
    pktToSend = paymentPacket.purchaseCancelPacket();
    if (sendToUX410())
    {
        waitForUX410();
        // If this is not commented out, the GUI crashes! (But it was uncommented when I found it...)
        pktResponded.clear();
    }
    com.flushSerial();
}

size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void pagePayment::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Payment >>>>>>>>>";

     // QPixmap background(PAGE_QR_PAY_BACKGROUND_PATH);
    // background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    // QPalette palette;
    // palette.setBrush(QPalette::Background, background);
    // this->setPalette(palette);
    
    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_QR_PAY_BACKGROUND_PATH);



    int product_slot___ = p_page_idle->currentProductOrder->getSelectedSlot();
    char drinkSize;
    if (p_page_idle->currentProductOrder->getSelectedSize() == SIZE_SMALL_INDEX)
    {
        drinkSize = 's';
    }
    if (p_page_idle->currentProductOrder->getSelectedSize() == SIZE_LARGE_INDEX)
    {
        drinkSize = 'l';
    }

    QString bitmap_location;

    if (!tap_payment)
    {
        bitmap_location = "/home/df-admin/production/references/5_background_pay_qr.png";
    }
    else if (product_slot___ > 0 && product_slot___ <= 9)
    {
        bitmap_location.append("/home/df-admin/production/references/5_pay_page_");
        bitmap_location.append(drinkSize);
        bitmap_location.append("_");
        bitmap_location.append(QString::number(p_page_idle->currentProductOrder->getSelectedSlot()));
        bitmap_location.append(".png");
        ui->order_drink_amount->setText("$" + QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2));
    }
    else
    {
        bitmap_location = "/home/df-admin/production/references/5_pay_page_l_1.png";
    }

    QPixmap background(bitmap_location);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);

    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    paymentEndTimer = new QTimer(this);
    paymentEndTimer->setInterval(1000);
    connect(paymentEndTimer, SIGNAL(timeout()), this, SLOT(onTimeoutTick()));
    paymentEndTimer->start(1000);
    _paymentTimeoutSec = QR_PAGE_TIMEOUT_SECONDS;

    ui->order_total_amount->setText("Total: $" + QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2));
    this->ui->payment_countdownLabel->setText("");
    ui->refreshLabel->hide();

    QWidget::showEvent(event);

   
    ui->productLabel->setText(p_page_idle->currentProductOrder->getSelectedProductName() + " " + p_page_idle->currentProductOrder->getSelectedSizeToVolumeWithCorrectUnits(true, true));

    ui->order_drink_amount->setText("$" + QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2));

    //  ui->payment_pass_Button->setEnabled(false);
    //  ui->payment_cancel_Button->setEnabled(false);

    if (tap_payment)
    {
        pktResponded = com.readForAck();
        readPacket.packetReadFromUX(pktResponded);
        pktResponded.clear();
        response = false;

        if (readPacket.getAckOrNak() == communicationPacketField::ACK)
        {
            timerEnabled = true;
        }
        cancelPayment();
        com.flushSerial();
        readTimer->start();
    }

    qDebug() << "ahoyy245";
    DbManager db2(DB_PATH);
    QString payment_method = db2.getPaymentMethod(product_slot___);
    db2.closeDB();

    if (payment_method == "qr")
    {
        setupQrOrder();
    }
}

void pagePayment::setupQrOrder()
{

    createQrOrder();

    QPixmap map(360, 360);
    map.fill(QColor("black"));
    QPainter painter(&map);
    //    ui->qrCode->setPixmap(map);

    // QString qrdata_amount = QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2);
    //  QString machine_id = db.getMachineID();
    //  QString product_id = db.getProductID(product_slot___);
    //  order_id = QUuid::createUuid().QUuid::toString();
    //  order_id = order_id.remove("{");
    //  order_id = order_id.remove("}");
    // qDebug() << "ORDER ID: " << order_id << endl;
    //  QString qrdata = "https://soapstandportal.com/payment?mid="+machine_id+"&pid="+product_id+"&size="+drinkSize+"&oid="+order_id;
    QString qrdata = "https://soapstandportal.com/payment?oid=" + orderId;

    paintQR(painter, QSize(360, 360), qrdata, QColor("white"));
    ui->qrCode->setPixmap(map);

    // QString curl_param = "oid=" + orderId;
    // curl_param_array = curl_param.toLocal8Bit();
    // curl_data = curl_param_array.data();

    _paymentTimeoutSec = QR_PAGE_TIMEOUT_SECONDS;

    _qrProcessedPeriodicalCheckSec = QR_PROCESSED_PERIODICAL_CHECK_SECONDS;
    qrTimer->start(1000);
}

void pagePayment::createQrOrder()
{
    // an order Id is generated locally and the order is sent to the cloud.

    qDebug() << "Create order in the cloud";
    QString MachineSerialNumber = p_page_idle->currentProductOrder->getMachineId();
    QString productId = p_page_idle->currentProductOrder->getSelectedProductId();
    QString contents = p_page_idle->currentProductOrder->getSelectedProductName();
    QString quantity_requested = p_page_idle->currentProductOrder->getSelectedSizeToVolumeWithCorrectUnits(false, false);
    char drinkSize = p_page_idle->currentProductOrder->getSelectedSizeAsChar();
    QString price = QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2);

    orderId = QUuid::createUuid().QUuid::toString();
    orderId = orderId.remove("{");
    orderId = orderId.remove("}");

    QString curl_order_parameters_string = "orderId=" + orderId + "&size=" + drinkSize + "&MachineSerialNumber=" + MachineSerialNumber +
                                           "&contents=" + contents + "&price=" + price + "&productId=" + productId + "&quantity_requested=" + quantity_requested;

    curl_order_parameters = curl_order_parameters_string.toLocal8Bit();

    curl1 = curl_easy_init();

    if (!curl1)
    {
        qDebug() << "pagepayement cURL Failed to init : " + curl_order_parameters_string + "to: " + "https://soapstandportal.com/api/machine_data/createOrderInDb";
        return;
    }

    curl_easy_setopt(curl1, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/createOrderInDb");
    curl_easy_setopt(curl1, CURLOPT_POSTFIELDS, curl_order_parameters.data());
    curl_easy_setopt(curl1, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl1, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl1, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);

    res1 = curl_easy_perform(curl1);
    if (res1 != CURLE_OK)
    {
        qDebug() << "ERROR: Problem at create order in the cloud request. error code: " + QString::number(res1);
    }
    else
    {
        QString feedback = QString::fromUtf8(readBuffer.c_str());
        qDebug() << "create order in the cloud request sent to soapstandportal (" + curl_order_parameters_string + "). Server feedback: " << feedback;
    }
    curl_easy_cleanup(curl1);
    readBuffer = "";

    _paymentTimeoutSec = QR_PAGE_TIMEOUT_SECONDS;
    _qrProcessedPeriodicalCheckSec = QR_PROCESSED_PERIODICAL_CHECK_SECONDS;
    qrTimer->start(1000);
}

void pagePayment::isQrProcessedCheckOnline()
{

    curl = curl_easy_init();
    if (!curl)
    {
        qDebug() << "cURL failed to page_init";
        return;
    }

    QString curl_param = "oid=" + orderId;
    curl_param_array = curl_param.toLocal8Bit();

    curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/check_order_status");

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curl_param_array.data());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);

    res = curl_easy_perform(curl);
    qDebug() << "Send check qr status for order " << orderId;

    if (res != CURLE_OK)
    {
        // string test = std::string(curl_easy_strerror(res));
        qDebug() << "ERROR: cURL fail at pagepayment. Error code: " + QString::number(res);
    }
    else
    {
        QString feedback = QString::fromUtf8(readBuffer.c_str());
        qDebug() << "curl success. Server feedback readbuffer: " << feedback;

        if (readBuffer == "true")
        {
            qDebug() << "QR processed. It's time to dispense.";
            proceed_to_dispense();
        }
        else
        {
            qDebug() << "Wait for QR processed. Nothing received";
        }
    }
    curl_easy_cleanup(curl);
    readBuffer = "";
}

void pagePayment::qrProcessedPeriodicalCheck()
{
    if (--_qrProcessedPeriodicalCheckSec >= 0)
    {
    }
    else
    {
        qDebug() << "Periodical check timer trigger. Check API endpoint for handled QR transaction.";
        isQrProcessedCheckOnline();
        _qrProcessedPeriodicalCheckSec = QR_PROCESSED_PERIODICAL_CHECK_SECONDS;
    }
}

void pagePayment::on_refreshButton_clicked()
{
    ui->refreshLabel->hide();
    _paymentTimeoutSec = QR_PAGE_TIMEOUT_SECONDS;
}

// XXX: Remove this when interrupts and flow sensors work!
void pagePayment::onTimeoutTick()
{
    if (--_paymentTimeoutSec >= 0)
    {

        QString label_text = "Transaction will be cancelled in " + QString::number(_paymentTimeoutSec) + "s.\nTOUCH THE SCREEN\n if you need more time \n";
        ui->refreshLabel->setText(label_text);
    }
    else
    {
        //        qDebug() << "Timer Done!" << _paymentTimeoutSec << endl;
        idlePaymentTimeout();
    }
    if (_paymentTimeoutSec < QR_PAGE_TIMEOUT_WARNING_SECONDS)
    {
        ui->refreshLabel->show();
    }
}

bool pagePayment::setpaymentProcess(bool status)
{
    return (paymentProcessing = status);
}

void pagePayment::setProgressLabel(QLabel *label, int dot)
{
}

// Local storge for now.  Will need to refactor logger to do a nightly push to AWS
void pagePayment::storePaymentEvent(QSqlDatabase db, QString event)
{
}

void pagePayment::progressStatusLabel()
{
}

void pagePayment::declineTimer_start()
{
}

void pagePayment::idlePaymentTimeout()
{
    stopPayTimers();
    response = true;
    readTimer->stop();
    if (tap_payment)
    {
        cancelPayment();
    }
    // qDebug() << "payment to idle";
    p_page_idle->showFullScreen();
    //    usleep(100);
    this->hide();
}

/* ----- Payment ----- */

void pagePayment::stayAliveLogon()
{
    com.flushSerial();
    /*logon packet to send*/
    pktToSend = paymentPacket.ppPosGetConfigPkt(StatusType::GetLanStatus);
    if (sendToUX410())
    {
        waitForUX410();
    }

    pktResponded.clear();
}

void pagePayment::batchClose()
{
    com.flushSerial();
    /*logon packet to send*/
    pktToSend = paymentPacket.batchClosePkt();
    if (sendToUX410())
    {
        waitForUX410();
    }

    pktResponded.clear();
}

bool pagePayment::sendToUX410()
{
    int waitForAck = 0;
    while (waitForAck < 3)
    {
        cout << "Wait for ACK counter: " << waitForAck << endl;
        com.sendPacket(pktToSend, uint(pktToSend.size()));
        std::cout << "sendtoUX410 Electronic Card Reader: " << paymentPacket.getSendPacket() << endl;

        // read back what is responded
        pktResponded = com.readForAck();
        readPacket.packetReadFromUX(pktResponded);
        pktResponded.clear();
        waitForAck++;

        // if(isReadyForTap) {
        cout << "Waiting for TAP" << endl;
        if (readPacket.getAckOrNak() == communicationPacketField::ACK)
        {
            // cout << readPacket << endl;
            return true;
        }
        // }
        usleep(50000);

        //        if(isInitCancelled) {
        //            return true;
        //        }

        //        if(isInitBatched) {
        //            return true;
        //        }

        //        if(isInitLogin) {
        //            return true;
        //        }

        //        if(isInitMerchant) {
        //            return true;
        //        }

        //        if(isInitAddress) {
        //            return true;
        //        }

        //        if(isInitTerminalID) {
        //            return true;
        //        }
    }
    return false;
}

bool pagePayment::tap_init()
{
    paymentConnected = com.page_init();

    while (!paymentConnected)
    {
        paymentConnected = com.page_init();
        sleep(1);
    }

    // This is super shitty - there must be a better way to find out when the green light starts flashing on the UX420
    sleep(35);

    /*Cancel any previous payment*/
    cout << "Sending Cancel payment packet..." << endl;
    pktToSend = paymentPacket.purchaseCancelPacket();
    if (sendToUX410())
    {
        cout << "Receiving Cancel payment response" << endl;
        isInitCancelled = true;
        waitForUX410();
        pktResponded.clear();
    }
    else
    {
        return false;
    }
    com.flushSerial();

    /*batch close packet to send*/
    cout << "Sending Batch close packet..." << endl;
    pktToSend = paymentPacket.batchClosePkt();
    if (sendToUX410())
    {
        cout << "Receiving Batch Close response" << endl;
        isInitBatched = true;
        waitForUX410();
        pktResponded.clear();
    }
    else
    {
        return false;
    }
    com.flushSerial();

    /*logon packet to send*/
    cout << "Sending Logon packet..." << endl;
    pktToSend = paymentPacket.logonPacket();
    if (sendToUX410())
    {
        cout << "Receiving Logon response" << endl;
        isInitLogin = true;
        waitForUX410();
        pktResponded.clear();
    }
    else
    {
        return false;
    }
    com.flushSerial();

    /*getConfiguration packet to send*/
    cout << "Sending Merchant Name query..." << endl;
    pktToSend = paymentPacket.ppPosGetConfigPkt(CONFIG_ID::MERCH_NAME);
    if (sendToUX410())
    {
        cout << "Receiving Merchant Name" << endl;
        waitForUX410();
        isInitMerchant = true;
        merchantName = paymentPktInfo.dataField(readPacket.getPacket().data).substr(2);
        cout << merchantName << endl;
        pktResponded.clear();
    }
    else
    {
        return false;
    }
    com.flushSerial();

    /*getConfiguration packet to send*/
    cout << "Sending Merchant Address query..." << endl;
    pktToSend = paymentPacket.ppPosGetConfigPkt(CONFIG_ID::MERCH_ADDR);
    if (sendToUX410())
    {
        cout << "Receiving Merchant Address" << endl;
        waitForUX410();
        isInitAddress = true;
        merchantAddress = paymentPktInfo.dataField(readPacket.getPacket().data).substr(2);
        std::cout << merchantAddress << endl;
        pktResponded.clear();
    }
    else
    {
        return false;
    }
    com.flushSerial();

    /*getConfiguration packet to send*/
    cout << "Sending PTID query..." << endl;
    pktToSend = paymentPacket.ppPosGetConfigPkt(CONFIG_ID::CON_TID);
    if (sendToUX410())
    {
        cout << "Receiving PTID" << endl;
        waitForUX410();
        isInitTerminalID = true;
        terminalID = paymentPktInfo.dataField(readPacket.getPacket().data).substr(2);
        std::cout << terminalID;
        pktResponded.clear();
    }
    else
    {
        return false;
    }
    com.flushSerial();

    return true;
}

bool pagePayment::waitForUX410()
{
    bool waitResponse = false;
    while (!waitResponse)
    {
        //        QCoreApplication::processEvents();
        // cout << readPacket << endl;
        if (pktResponded[0] != 0x02)
        {
            pktResponded.clear();
            pktResponded = com.readPacket();
            usleep(1000);
        }
        else
        {
            //  pktResponded = com.readPacket();
            readPacket.packetReadFromUX(pktResponded);
            std::cout << readPacket;
            com.sendAck();
            waitResponse = true;
        }
    }
    return waitResponse;
}

void pagePayment::readTimer_loop()
{
    //    qDebug() << "readingTimer_loop" << endl;
    // cout << "start loop pktResponded: " << to_string(pktResponded[0]) << endl;

    // response = false;
    // pktResponded = com.readPacket();
    pktResponded.clear();
    // com.clearBuffer();

    com.flushSerial();
    cout << p_page_idle->currentProductOrder->getSelectedPriceCorrected() << endl;
    pktToSend = paymentPacket.purchasePacket((QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2)).QString::toStdString());
    cout << "to PAY: " << ((QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2)).QString::toStdString());
    //        pktToSend = paymentPacket.purchasePacket("0.01");

    // this->ui->payment_countdownLabel->setText("TAP NOW");
    response = getResponse();

    if (sendToUX410())
    {
        //        waitForUX410();

        while (!response)
        {
            response = getResponse();

            QCoreApplication::processEvents();

            if (pktResponded[0] != 0x02)
            {
                //       qDebug() << "Reading TAP Packet" << endl;
                // cout << readPacket << endl;

                // std::cout<< "ReadTimer Electronic Card Reader: " << paymentPacket.getSendPacket() << endl;

                // This breaks the polling loop!
                //        pktResponded = com.readPacket();
                // cout << "MISS: pktResponded: " << to_string(pktResponded[0]) << endl;
                pktResponded.clear();
                pktResponded = com.readPacket();
                usleep(10000);
                response = getResponse();

                //        com.sendAck();
                // cout << "Polling Timer" << endl;
                // readTimer->start(1000);
            }
            else if (pktResponded[10] == 0x33)
            {
                pktResponded.clear();
                pktResponded = com.readPacket();
                usleep(100);
                response = getResponse();
                // cancelPayment();
                // readTimer->start(1000);
            }
            else
            {

                if (!response)
                {

                    cout << "HIT: pktResponded: " << to_string(pktResponded[0]) << endl;

                    // this->ui->payment_countdownLabel->setText("Processing...");
                    QCoreApplication::processEvents();

                    //        qDebug() << "Check TAP Packet; Sending" << endl;

                    // com.sendAck();
                    readPacket.packetReadFromUX(pktResponded);
                    std::cout << readPacket;
                    com.sendAck();

                    if (pktResponded[19] == 0x41)
                    { // Host Response 41 = A "Approved"
                        purchaseEnable = true;
                        approved = true;
                        cout << "Approval Packet 41" << endl;
                        this->ui->payment_countdownLabel->setText("Approved");
                        paymentPktInfo.transactionID(readPacket.getPacket().data);
                        paymentPktInfo.makeReceipt(getTerminalID(), getMerchantName(), getMerchantAddress());
                        QCoreApplication::processEvents();
                        sleep(2);
                        response = true;
                        on_payment_bypass_Button_clicked();
                        //            mainPage->getSurveyPage()->resetSurveyFilled(); //reset the coupon discount
                    }
                    else if (pktResponded[19] == 0x44)
                    { // Host Response 44 = D "Declined"
                        purchaseEnable = true;
                        approved = false;
                        cout << "Declined Packet 32" << endl;
                        this->ui->payment_countdownLabel->setText("Declined");
                        paymentPktInfo.transactionID(readPacket.getPacket().data);
                        paymentPktInfo.makeReceipt(getTerminalID(), getMerchantName(), getMerchantAddress());
                        pktResponded.clear();
                        QCoreApplication::processEvents();
                        sleep(3);
                        idlePaymentTimeout();
                        // sleep(5);
                        //            on_mainPage_Button_clicked();
                    }
                    /* else if (pktResponded[10] == 0x33){
                         pktResponded.clear();
                         pktResponded = com.readPacket();
                         usleep(100);
                         response = getResponse();
                         this->ui->payment_countdownLabel->setText("Try again");
                         //        com.sendAck();
                         //cout << "Polling Timer" << endl;
                         readTimer->start(1000);

                     }*/
                    else if (pktResponded[19] == 0x4e)
                    {
                        purchaseEnable = false;
                        cout << "No Approval Packet!" << endl;
                        this->ui->payment_countdownLabel->setText("Not Approved");
                        paymentPktInfo.transactionID(readPacket.getPacket().data);
                        paymentPktInfo.makeReceipt(getTerminalID(), getMerchantName(), getMerchantAddress());
                        pktResponded.clear();
                        QCoreApplication::processEvents();
                        sleep(3);
                        idlePaymentTimeout();
                        // this->ui->payment_countdownLabel->setText("Hmm...");
                        // sleep(5);
                        // on_mainPage_Button_clicked();
                    }
                    else
                    {
                        pktResponded.clear();
                        pktResponded = com.readPacket();
                        usleep(100);
                        response = getResponse();
                        // readTimer->start(1000);
                    }
                }

                //        readPacket.packetReadFromUX(pktResponded);
                //        std::cout << readPacket;

                //        if (purchaseEnable == true){
                //            //once purchase successed create a receipt and store into database
                //            paymentPktInfo.transactionID(readPacket.getPacket().data);
                //            paymentPktInfo.makeReceipt(mainPage->getDatabase());

                //            paymentProcessing = false;
                //            progressLoopCounter = 0;
                //        }
                //        timerEnabled = false;
            }

            //    if (timerEnabled == false){
            //        qDebug() << "Timer has been disabled" << endl;
            //        //        if (purchaseEnable == false){
            //        //            pageNumber = 0;
            //        //            mainPage->getSurveyPage()->resetSurveyFilled(); //reset the coupon discount
            //        //        }
            //        readTimer->stop();
            //        purchaseEnable = false;
            //    }

            //    if (pktResponded.size() > 100)
            //    {
            //        if (progressLoopCounter == 0){
            // //           ui->payment_processLabel->setText(TAP_PROCESSING_LABEL);
            //            //            ui->payment_declineLabel->hide();
            //            paymentProcessing = true;
            //            paymentProgressTimer->start();
            //        }
            //    }
            //        }
        }
    }
}

std::string pagePayment::toSvgString(const QrCode &qr, int border)
{
    if (border < 0)
        throw std::domain_error("Border must be non-negative");
    if (border > INT_MAX / 2 || border * 2 > INT_MAX - qr.getSize())
        throw std::overflow_error("Border too large");

    std::ostringstream sb;
    sb << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    sb << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
    sb << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" viewBox=\"0 0 ";
    sb << (qr.getSize() + border * 2) << " " << (qr.getSize() + border * 2) << "\" stroke=\"none\">\n";
    sb << "\t<rect width=\"100%\" height=\"100%\" fill=\"#FFFFFF\"/>\n";
    sb << "\t<path d=\"";
    for (int y = 0; y < qr.getSize(); y++)
    {
        for (int x = 0; x < qr.getSize(); x++)
        {
            if (qr.getModule(x, y))
            {
                if (x != 0 || y != 0)
                    sb << " ";
                sb << "M" << (x + border) << "," << (y + border) << "h1v1h-1z";
            }
        }
    }
    sb << "\" fill=\"#000000\"/>\n";
    sb << "</svg>\n";
    return sb.str();
}

// Prints the given QrCode object to the console.
void pagePayment::printQr(const QrCode &qr)
{
    int border = 4;
    for (int y = -border; y < qr.getSize() + border; y++)
    {
        for (int x = -border; x < qr.getSize() + border; x++)
        {
            std::cout << (qr.getModule(x, y) ? "##" : "  ");
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void pagePayment::QRgen()
{

    QPixmap map(400, 400);
    QPainter painter(&map);

    paintQR(painter, QSize(400, 400), "SoapStand RULES", QColor("white"));
    ui->qrCode->setPixmap(map);
}

void pagePayment::paintQR(QPainter &painter, const QSize sz, const QString &data, QColor fg)
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
                const double rx1 = (x + 1) * scale, ry1 = (y + 1) * scale;
                QRectF r(rx1, ry1, scale, scale);
                painter.drawRects(&r, 1);
            }
        }
    }
}
