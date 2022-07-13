//***************************************
//
// page_payment.cpp
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

#include "page_payment.h"
#include "ui_page_payment.h"

#include "page_product.h"
#include "page_dispenser.h"
#include "page_idle.h"

// CTOR
page_payment::page_payment(QWidget *parent) : QWidget(parent),
                                              ui(new Ui::page_payment)
{
    // Fullscreen background setup
    ui->setupUi(this);
    qDebug() << "Payment page" << endl;
    ui->previousPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->mainPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->payment_bypass_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->refreshButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    ui->payment_bypass_Button->setEnabled(false);

    state_payment = s_init;

    ui->title_Label->setText("pay by phone");
    QString css_title = "QLabel{"
        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 500;"
        "font-size: 64px;"
        "line-height: 86px;"
        "text-align: center;"
        "text-transform: lowercase;"
        "color: #003840;"
        "text-shadow: 0px 4px 4px rgba(0, 0, 0, 0.25);"
        "}";
    ui->title_Label->setStyleSheet(css_title);

    ui->scan_Label->setText(
            "Scan to Pay");
    QString css_scan = "QLabel{"
        "text-align: center;"
        "font-family: 'Montserrat';"
        "font-style: normal;"
        "font-weight: 600;"
        "font-size: 48px;"
        "color: #5E8580;"
            "} .tab {"
            "display: inline-block;"
            "margin-left: 40px;"
        "}";
    ui->scan_Label->setStyleSheet(css_scan);

    ui->steps_Label->setText(
            "<style>"
                "li:{margin-top:10px;}"
            "</style>"
            "<ol>"
            "<li><span class='tab'></span>Scan QR code with phone camera<br></li>"
            "<li><span class='tab'></span>Click to open the link that appears<br></li>"
            "<li><span class='tab'></span>Follow payment instructions on phone<br></li>"
            "<li><span class='tab'></span>The station will proceed after payment<br></li>"
            "<li><span class='tab'></span>Refill your soap!</li>"
            "</ol>"
            );
    QString css_steps = "QLabel{"
            "position: absolute;"
            "width: 777px;"
            "height: 306px;"
            "left: 143px;"
            "top: 1029px;"
            "font-family: 'Montserrat';"
            "font-style: normal;"
            "font-weight: 600;"
            "font-size: 36px;"
            "line-height: 51px;"
            "color: #003840;"
            "}";
    ui->steps_Label->setStyleSheet(css_steps);

    ui->processing_Label->setText(
            "it can take a few moments for the station to<br>continue after your payment is confirmed"
            );
    QString css_processing = "QLabel{"
            "position: absolute;"
            "width: 777px;"
            "height: 306px;"
            "left: 143px;"
            "top: 1029px;"
            "font-family: 'Montserrat';"
            "font-style: normal;"
            "font-weight: 600;"
            "font-size: 36px;"
            "line-height: 51px;"
            "color: #003840;"
            "}";
    ui->processing_Label->setStyleSheet(css_processing);

    ui->order_total_amount->hide();

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

    qrPeriodicalCheckTimer = new QTimer(this);
    connect(qrPeriodicalCheckTimer, SIGNAL(timeout()), this, SLOT(qrProcessedPeriodicalCheck()));
    // while(!tap_init());

    // // need to install a plugin to get animated gifs to play
    // QString gif_path = DRINKFILL_LOGO_ANIMATED_PATH;
    // QMovie *movie = new QMovie(gif_path);
    // ui->animated_Label->setMovie(movie);
    // movie->start();
    //  while (!tap_init());
            
    
    // QString tap = "tap";
    // if (getPaymentMethod() == tap)
    // {
    //     ui->payment_bypass_Button->setEnabled(false);
    //     qDebug() << "In tap";
    //     // while (!tap_init())
    //     //     ;
    // }
    // else
    // {
    //     ui->payment_bypass_Button->setEnabled(false);
    // }
    // XXX: Comment on/off for Bypassing payment testing
    // qDebug() << "constructor Payment page. Check for tap.";
    // tap_payment = false;
    // DbManager db(DB_PATH);

    // for (int i = 0; i < SLOT_COUNT; i++)
    // {
    //  if (db.getPaymentMethod(i) == "tap")
    //     {
    //         tap_payment = true;
    //         ui->payment_bypass_Button->setEnabled(false);
    //     }
    //     else
    //     {
    //         ui->payment_bypass_Button->setEnabled(false);
    //     }
    // }
    // db.closeDB();
    // if (tap_payment)
    // {
    //     while (!tap_init())
    //         ;
    // }
}

void page_payment::stopPayTimers()
{
    //    readTimer->stop();
    //    qDebug() << "page_payment: Stop Timers" << endl;
    if (paymentProgressTimer != nullptr)
    {
               qDebug() << "cancel payment progress Timer" << endl;
        paymentProgressTimer->stop();
    }

    if (declineTimer != nullptr)
    {
               qDebug() << "cancel decline Timer" << endl;
        declineTimer->stop();
    }

    if (idlePaymentTimer != nullptr)
    {
               qDebug() << "cancel page_idle payment Timer" << endl;
        idlePaymentTimer->stop();
    }

    if (paymentEndTimer != nullptr)
    {
               qDebug() << "cancel page_idle payment END Timer" << endl;
        paymentEndTimer->stop();
    }

    if (readTimer != nullptr)
    {
               qDebug() << "cancel readTimer" << endl;
        readTimer->stop();
    }

    if (qrPeriodicalCheckTimer != nullptr)
    {
        // qDebug() << "*************************cancel qrPeriodicalCheckTimer" << endl;
        qrPeriodicalCheckTimer->stop();
    }
    //    qDebug() << "page_payment: Stopped Timers" << endl;
}

/*
 * Page Tracking reference
 */
void page_payment::setPage(pageProduct *pageSizeSelect, page_dispenser *page_dispenser, page_idle *pageIdle, page_help *pageHelp)
{
    tmpCounter = 0;
    this->p_pageProduct = pageSizeSelect;
    this->p_page_dispense = page_dispenser;
    this->p_page_idle = pageIdle;
    this->helpPage = pageHelp;
}

// DTOR
page_payment::~page_payment()
{
    delete ui;
}

/* ----- GUI ----- */

// Labels and button for tapping payment
void page_payment::displayPaymentPending(bool isVisible)
{
}

void page_payment::on_payment_bypass_Button_clicked()
{
    proceed_to_dispense();
}

void page_payment::proceed_to_dispense()
{
    stopPayTimers();
    // p_page_dispense->showEvent(dispenseEvent);
    p_page_dispense->showFullScreen();
    this->hide();
}

void page_payment::updateTotals(string drinkDescription, string drinkAmount, string orderTotal)
{
}

/*Cancel any previous payment*/
void page_payment::cancelPayment()
{

    com.flushSerial();
    /*Cancel any previous payment*/
    pktToSend = paymentPacket.purchaseCancelPacket();
    if (sendToUX410())
    {
        waitForUX410();
        pktResponded.clear();
    }
    com.flushSerial();
}

size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

QString page_payment::getPaymentMethod()
{
    qDebug() << "db open245";
    int product_slot___ = p_page_idle->currentProductOrder->getSelectedSlot();
    DbManager db2(DB_PATH);
    QString payment_method = db2.getPaymentMethod(product_slot___);
    db2.closeDB();
    return payment_method;
}

void page_payment::resizeEvent(QResizeEvent *event)
{
    // QString bitmap_location;

    // if (getPaymentMethod() == "tap")
    // {
    //     bitmap_location = PAGE_QR_PAY_BACKGROUND_PATH;
    // }
    // else
    // {
    //     qDebug("ERROR: No tap payment available yet.");
    // }

    // QPixmap background(bitmap_location);
    // background = background.scaled(this->size(), Qt::IgnoreAspectRatio);

    // QPalette palette;
    // palette.setBrush(QPalette::Background, background);
    // this->setPalette(palette);

    // ui->order_total_amount->setText("Total: $" + QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2));
    // ui->order_drink_amount->setText("");

    // ui->productLabel->setText(p_page_idle->currentProductOrder->getSelectedProductName() + " " + p_page_idle->currentProductOrder->getSelectedSizeToVolumeWithCorrectUnits(true, true));

    // response = false;
    // ui->refreshLabel->hide();
}

void page_payment::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Payment >>>>>>>>>";
    QWidget::showEvent(event);

    // QPixmap background(PAGE_QR_PAY_BACKGROUND_PATH);
    // background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    // QPalette palette;
    // palette.setBrush(QPalette::Background, background);
    // this->setPalette(palette);

    state_payment = s_init;
    ui->qrCode->show();
    ui->productLabel->show();
    ui->order_drink_amount->show();
    // ui->order_total_amount->show();
    ui->steps_Label->show();
            
    ui->processing_Label->hide();
    


     ui->title_Label->setText("pay by phone");
     ui->scan_Label->setText("Scan to Pay");


    if (getPaymentMethod() == "tap")
    {
        qDebug() << "Init tap";
        ui->payment_bypass_Button->setEnabled(false);
        // while (!tap_init())
            // ;
    }
    else
    {
        ui->payment_bypass_Button->setEnabled(false);
    }

    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_QR_PAY_BACKGROUND_PATH);

    // QString bitmap_location;

    // bitmap_location = "/home/df-admin/production/references/5_background_pay_qr.png";

    // QPixmap background(bitmap_location);
    // background = background.scaled(this->size(), Qt::IgnoreAspectRatio);

    // QPalette palette;
    // palette.setBrush(QPalette::Background, background);
    // this->setPalette(palette);

    paymentEndTimer = new QTimer(this);
    paymentEndTimer->setInterval(1000);
    connect(paymentEndTimer, SIGNAL(timeout()), this, SLOT(onTimeoutTick()));
    paymentEndTimer->start(1000);
    _pageTimeoutCounterSecondsLeft = QR_PAGE_TIMEOUT_SECONDS;
    this->ui->payment_countdownLabel->setText("");

    ui->refreshLabel->hide();

    ui->productLabel->setText(p_page_idle->currentProductOrder->getSelectedProductName() + " " + p_page_idle->currentProductOrder->getSelectedSizeToVolumeWithCorrectUnits(true, true));
    ui->order_drink_amount->setText("$" + QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2));
    ui->order_total_amount->setText("Total: $" + QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2));

    QString payment_method = getPaymentMethod();
    if (payment_method == "tap")
    {
        qDebug() << "Prepare tap order";
        // pktResponded = com.readForAck();
        // readPacket.packetReadFromUX(pktResponded);
        // pktResponded.clear();
        // response = false;

        if (readPacket.getAckOrNak() == communicationPacketField::ACK)
        {
            timerEnabled = true;
        }
        readTimer->start(1000);
    }
    else if (payment_method == "qr")
    {
        setupQrOrder();
    }
    else
    {
        qDebug() << "Payment method not valid " << payment_method;
    }
}

void page_payment::setupQrOrder()
{

    createOrderIdAndSendToBackend();

    QPixmap map(360, 360);
    map.fill(QColor("black"));
    QPainter painter(&map);

    // build up qr content (link)
    QString qrdata = "https://soapstandportal.com/payment?oid=" + orderId;

    // create qr code graphics
    paintQR(painter, QSize(360, 360), qrdata, QColor("white"));
    ui->qrCode->setPixmap(map);
    // _paymentTimeoutSec = QR_PAGE_TIMEOUT_SECONDS;
    _paymentTimeLabel = QR_PAGE_TIMEOUT_SECONDS;

    _pageTimeoutCounterSecondsLeft = QR_PAGE_TIMEOUT_SECONDS;

    _qrProcessedPeriodicalCheckSec = QR_PROCESSED_PERIODICAL_CHECK_SECONDS;
    qrPeriodicalCheckTimer->start(1000);
}

void page_payment::createOrderIdAndSendToBackend()
{
    // an order Id is generated locally and the order is sent to the cloud.

    qDebug() << "Get cloud to create an order and retrieve the order id";
    QString MachineSerialNumber = p_page_idle->currentProductOrder->getMachineId();
    QString productUnits = p_page_idle->currentProductOrder->getUnitsForSelectedSlot();
    QString productId = p_page_idle->currentProductOrder->getSelectedProductId();
    QString contents = p_page_idle->currentProductOrder->getSelectedProductName();
    QString quantity_requested = p_page_idle->currentProductOrder->getSelectedSizeToVolumeWithCorrectUnits(false, false);
    char drinkSize = p_page_idle->currentProductOrder->getSelectedSizeAsChar();
    QString price = QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2);

    // create a unique order id locally
    orderId = QUuid::createUuid().QUuid::toString();
    orderId = orderId.remove("{");
    orderId = orderId.remove("}");

    // send order details to backend
    QString curl_order_parameters_string = "orderId=" + orderId + "&size=" + drinkSize + "&MachineSerialNumber=" + MachineSerialNumber +
                                           "&contents=" + contents + "&price=" + price + "&productId=" + productId + "&quantity_requested=" + quantity_requested + "&size_unit=" + productUnits;

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

    _pageTimeoutCounterSecondsLeft = QR_PAGE_TIMEOUT_SECONDS;
    _qrProcessedPeriodicalCheckSec = QR_PROCESSED_PERIODICAL_CHECK_SECONDS;
    qrPeriodicalCheckTimer->start(1000);
}

void page_payment::isQrProcessedCheckOnline()
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
        qDebug() << "ERROR: cURL fail at page_payment. Error code: " + QString::number(res);
    }
    else
    {
        QString feedback = QString::fromUtf8(readBuffer.c_str());
        qDebug() << "curl success. Server feedback readbuffer: " << feedback;

        if (readBuffer == "true")
        {
            qDebug() << "QR processed. It's time to dispense.";
            proceed_to_dispense();
            state_payment = s_payment_done;
        }
        else if (readBuffer == "Not paid")
        {
            qDebug() << "Initial page, user has not yet scanned qr code (or it is scanned, but not yet confirmed by backed)";
            // do nothing
            state_payment = s_init;
        }
        else if (readBuffer == "In progress")
        {
            qDebug() << "Wait for QR processed. User must have finished transaction to continue.";
            // user scanned qr code and is processing transaction. Delete qr code and make it harder for user to leave page.
            state_payment = s_payment_processing;
            ui->qrCode->hide();
            ui->productLabel->hide();
            ui->order_drink_amount->hide();
            ui->order_total_amount->hide();
            ui->steps_Label->hide();
            
            ui->processing_Label->show();
            ui->scan_Label->setText("Please finalize transaction");
            ui->title_Label->setText("almost there");
        }
        else
        {
            qDebug() << "ASSERT ERROR: Unknown message from Server";
        }
    }
    curl_easy_cleanup(curl);
    readBuffer = "";
}

void page_payment::qrProcessedPeriodicalCheck()
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

void page_payment::on_refreshButton_clicked()
{
    ui->refreshLabel->hide();
    _pageTimeoutCounterSecondsLeft = QR_PAGE_TIMEOUT_SECONDS;
}

// XXX: Remove this when interrupts and flow sensors work!
void page_payment::onTimeoutTick()
{
    if (--_pageTimeoutCounterSecondsLeft >= 0)
    {

        QString label_text = "Transaction will be cancelled in " + QString::number(_pageTimeoutCounterSecondsLeft) + "s.\nTOUCH THE SCREEN\n if you need more time \n";
        ui->refreshLabel->setText(label_text);
    }
    else
    {
               qDebug() << "Timer Done!" << _pageTimeoutCounterSecondsLeft << endl;
        idlePaymentTimeout();
    }
    if (_pageTimeoutCounterSecondsLeft < QR_PAGE_TIMEOUT_WARNING_SECONDS)
    {
        ui->refreshLabel->show();
    }
}

bool page_payment::setpaymentProcess(bool status)
{
    return (paymentProcessing = status);
}

void page_payment::setProgressLabel(QLabel *label, int dot)
{
}

// Local storge for now.  Will need to refactor logger to do a nightly push to AWS
void page_payment::storePaymentEvent(QSqlDatabase db, QString event)
{
}

void page_payment::progressStatusLabel()
{
}

void page_payment::declineTimer_start()
{
}

bool page_payment::exitConfirm()
{
    if (state_payment == s_payment_processing || state_payment == s_payment_done)
    {
        // ARE YOU SURE YOU WANT TO EXIT?
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::FramelessWindowHint); // do not show messagebox header with program name

        msgBox.setText("<p align=center><br><br>Cancel transaction and exit page?<br><br>It can take up to 30 seconds for dispensing to start after a payment is completed. <br></p>");
        msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px; font-weight: bold; font-style: normal;  font-family: 'Montserrat';} QPushButton{font-size: 24px; min-width: 300px; min-height: 300px;}");

        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();
        bool success;
        switch (ret)
        {
        case QMessageBox::Yes:
        {
            resetPaymentPage();
            return true;
        }
        break;
        case QMessageBox::No:
        {
            return false;
        }
        break;
        }
    }
    else
    {
        // exit, no questions asked.
        resetPaymentPage();
        return true;
    }
}

// Navigation: Back to Drink Size Selection
void page_payment::on_previousPage_Button_clicked()
{
    if (exitConfirm())
    {
        // p_pageProduct->resizeEvent(pageProductResize);
        p_pageProduct->showFullScreen();
        this->hide();
    }
}

void page_payment::on_mainPage_Button_clicked()
{
    if (exitConfirm())
    {
        helpPage->showFullScreen();
        this->hide();
    }
}

void page_payment::idlePaymentTimeout()
{
    resetPaymentPage();
    p_page_idle->showFullScreen();
    this->hide();
}
void page_payment::resetPaymentPage()
{

    stopPayTimers();
    response = true;
    readTimer->stop();
    if (getPaymentMethod() == "tap")
    {
        cancelPayment();
    }
}

/* ----- Payment ----- */

void page_payment::stayAliveLogon()
{
    
    cout << "Getting Lan Info" << endl;
    pktToSend = paymentPacket.ppPosStatusCheckPkt(StatusType::GetLanInfo);
    if (sendToUX410())
    {
        waitForUX410();
    }

    pktResponded.clear();

    com.flushSerial();
    
    
}

void page_payment::batchClose()
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

bool page_payment::sendToUX410()
{
    int waitForAck = 0;
    while (waitForAck < 3)
    {
        cout << "Wait for ACK counter: " << waitForAck << endl;
        com.sendPacket(pktToSend, uint(pktToSend.size()));
        std::cout << "sendtoUX410 Electronic Card Reader: " << paymentPacket.getSendPacket() << endl;
        sleep(1);
        // read back what is responded
        pktResponded = com.readForAck();
        readPacket.packetReadFromUX(pktResponded);
        pktResponded.clear();
        waitForAck++;

        // if(isReadyForTap) {
        cout<< readPacket.getAckOrNak();
        if (readPacket.getAckOrNak() == communicationPacketField::ACK)
        {
            return true;
        }
    }
    return false;
}

bool page_payment::tap_init()
{
    paymentConnected = com.page_init();

    while (!paymentConnected)
    {
        paymentConnected = com.page_init();
        sleep(1);
    }

    // This is super shitty - there must be a better way to find out when the green light starts flashing on the UX420 but it was 35
    // sleep(35);

    cout << "_----_-----__------_-----";
   
    // stayAliveLogon();
      cout << "Sending Device Reset packet..." << endl;
    pktToSend = paymentPacket.resetDevice();
    if (sendToUX410())
    {
        cout << "Receiving Device Reset response" << endl;
        isInitBatched = true;
        waitForUX410();
        pktResponded.clear();
    }
    else
    {
        return false;
    }
    com.flushSerial();
    cout << "-----------------------------------------------" << endl;
    
    
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
    cout << "-----------------------------------------------" << endl;


    /*batch close packet to send*/
    // cout << "Sending Batch close packet..." << endl;
    // pktToSend = paymentPacket.batchClosePkt();
    // if (sendToUX410())
    // {
    //     cout << "Receiving Batch Close response" << endl;
    //     isInitBatched = true;
    //     waitForUX410();
    //     pktResponded.clear();
    // }
    // else
    // {
    //     return false;
    // }
    // com.flushSerial();
    // cout << "-----------------------------------------------" << endl;
    
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
    cout << "-----------------------------------------------" << endl;

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
    cout << "-----------------------------------------------" << endl;

    /*getConfiguration packet to send*/
    cout << "Sending Merchant Address query..." << endl;
    pktToSend = paymentPacket.ppPosGetConfigPkt(CONFIG_ID::URL1);
    if (sendToUX410())
    {
        cout << "Receiving Merchant Address" << endl;
        waitForUX410();
        isInitAddress = true;
        // merchantAddress = paymentPktInfo.dataField(readPacket.getPacket().data).substr(2);
        merchantAddress = paymentPktInfo.dataField(readPacket.getPacket().data);

        std::cout << merchantAddress << endl;
        pktResponded.clear();
    }
    else
    {
        return false;
    }
    com.flushSerial();
    cout << "-----------------------------------------------" << endl;

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

bool page_payment::waitForUX410()
{
    bool waitResponse = false;
    while (!waitResponse)
    {
        // sleep(1);
            //    QCoreApplication::processEvents();
        // cout << readPacket << endl;
        if (pktResponded[0] != 0x02)
        {
            pktResponded.clear();
            pktResponded = com.readPacket();
            usleep(10);
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

void page_payment::readTimer_loop()
{
    cout << p_page_idle->currentProductOrder->getSelectedPriceCorrected() << endl;
    pktToSend = paymentPacket.purchasePacket((QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2)).QString::toStdString());
    cout << "to PAY: " << ((QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2)).QString::toStdString());
    response = getResponse();

    if (sendToUX410())
    {
        waitForUX410();

        while (!response)
        {
            response = getResponse();

            // QCoreApplication::processEvents();

            if (pktResponded[0] != 0x02)
            {
                pktResponded.clear();
                pktResponded = com.readPacket();
                usleep(10);
                response = getResponse();

                cout << "Polling Timer" << endl;
                // readTimer->start(1000);
            }
            else if (pktResponded[10] == 0x33)
            {
                pktResponded.clear();
                pktResponded = com.readPacket();
                // usleep(100);
                response = getResponse();
            }
            else
            {

                if (!response)
                {

                    cout << "HIT: pktResponded: " << to_string(pktResponded[0]) << endl;

                    readPacket.packetReadFromUX(pktResponded);
                    std::cout << readPacket;
                    com.sendAck();

                    if (pktResponded[19] == 0x41)
                    { // Host Response 41 = A "Approved"
                        purchaseEnable = true;
                        approved = true;
                        cout << "Approval Packet 41" << endl;
                        // this->ui->payment_countdownLabel->setText("Approved");
                        paymentPktInfo.transactionID(readPacket.getPacket().data);
                        paymentPktInfo.makeReceipt(getTerminalID(), getMerchantName(), getMerchantAddress());
                        response = true;
                        on_payment_bypass_Button_clicked();
                    }
                    else if (pktResponded[19] == 0x44)
                    { // Host Response 44 = D "Declined"
                        purchaseEnable = true;
                        approved = false;
                        cout << "Declined Packet 32" << endl;
                        // this->ui->payment_countdownLabel->setText("Declined");
                        paymentPktInfo.transactionID(readPacket.getPacket().data);
                        paymentPktInfo.makeReceipt(getTerminalID(), getMerchantName(), getMerchantAddress());
                        pktResponded.clear();
                        // QCoreApplication::processEvents();
                        sleep(3);
                        idlePaymentTimeout();
                    }
                   
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
                     
                    }
                    else
                    {
                        pktResponded.clear();
                        pktResponded = com.readPacket();
                        sleep(5);
                        response = getResponse();
                        // readTimer->start(10);
                    }
                }

        }
    }
}
}

std::string page_payment::toSvgString(const QrCode &qr, int border)
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
void page_payment::printQr(const QrCode &qr)
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

void page_payment::testQRgen()
{

    QPixmap map(400, 400);
    QPainter painter(&map);

    paintQR(painter, QSize(400, 400), "SoapStand RULES", QColor("white"));
    ui->qrCode->setPixmap(map);
}

void page_payment::paintQR(QPainter &painter, const QSize sz, const QString &data, QColor fg)
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
