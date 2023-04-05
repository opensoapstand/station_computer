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
#include "payment/commands.h"
#include "payment/setup_Tap.h"

extern QString transactionLogging;
std::string CTROUTD = "";
std::string MAC_KEY = "";
std::string MAC_LABEL = "";
std::string AUTH_CODE = "";
std::string SAF_NUM = "";
std::string socketAddr;
std::thread cardTapThread;
std::thread dataThread;
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

    // **** Timer and Slot Setup ****

    // Payment Tap Ready
    // readTimer = new QTimer(this);
    // connect(readTimer, SIGNAL(timeout()), this, SLOT(readTimer_loop()));

    std::atomic<bool> stop_tap_action_thread(false);
    std::atomic<bool> stop_authorization_thread(false);
    // Receive packets from tap device checker
    checkPacketReceivedTimer = new QTimer(this);
    connect(checkPacketReceivedTimer, SIGNAL(timeout()), this, SLOT(check_packet_available()));
    checkPacketReceivedTimer->setInterval(500); // was 500


    checkCardTappedTimer = new QTimer(this);
    connect(checkCardTappedTimer, SIGNAL(timeout()), this, SLOT(check_card_tapped()));
    checkCardTappedTimer->setInterval(500); // was 500


    // Payment Declined
    declineTimer = new QTimer(this);
    connect(declineTimer, SIGNAL(timeout()), this, SLOT(declineTimer_start()));

    // Idle Payment reset
    idlePaymentTimer = new QTimer(this);
    connect(idlePaymentTimer, SIGNAL(timeout()), this, SLOT(idlePaymentTimeout()));

    qrPeriodicalCheckTimer = new QTimer(this);
    connect(qrPeriodicalCheckTimer, SIGNAL(timeout()), this, SLOT(qrProcessedPeriodicalCheck()));
    DbManager db(DB_PATH);
    
        
    if (db.getPaymentMethod(1) == "tap")
    {
        tap_payment = true;
        ui->payment_bypass_Button->setEnabled(false);
        ui->title_Label->hide();
        ui->steps_Label->hide();
        ui->qrCode->hide();
        ui->scan_Label->hide();
    }
    else
    {
        ui->payment_bypass_Button->setEnabled(false);
        state_payment = s_init;
        tap_payment = false;
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
        ui->title_Label->setText("pay by phone");
        

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
            "</ol>");
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
            "it can take a few moments for the station to<br>continue after your payment is confirmed");
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
    }
    ui->order_total_amount->hide();

    db.closeDB();
    if (tap_payment)
    {
        qDebug()<< "InitializingTap payment";
        std::map<std::string, std::string> configMap = readConfigFile();
        int socket = connectSocket();
        qDebug() << "Socket connection successful " << socket << endl;
        std::map<std::string, std::string> deviceStatus = checkDeviceStatus(connectSecondarySocket());
        if(deviceStatus["MACLABEL_IN_SESSION"]!=""){
            // finishSession(socket, configMap["MAC_KEY"], configMap["MAC_LABEL"]);
        }
        // cancelTransaction(connectSecondarySocket());
        qDebug() << "Transaction cancelled";
        if(configMap["MAC_KEY"]!=""){
             std::map<std::string, std::string> testResponse = testMac(connectSocket(), configMap["MAC_KEY"], configMap["MAC_LABEL"]);
            if (testResponse["RESPONSE_TEXT"] == "Match")
            {
                qDebug() << "Test Mac Command Matched" << endl;
            }
            else{
                qDebug() << "Re-registration of the device";
                registerDevice(connectSocket());
            }
        }
        else
        {
            qDebug() << "No file config" << endl;
            registerDevice(connectSocket());
        }
        
        // finishSession(socket, configMap["MAC_KEY"], configMap["MAC_LABEL"]);
    }
}

void page_payment::stopPayTimers()
{

    if (checkPacketReceivedTimer != nullptr)
    {
        qDebug() << "cancel payment progress Timer" << endl;
        // stop_tap_action_thread=true;
        checkPacketReceivedTimer->stop();
    }
     if (checkCardTappedTimer != nullptr)
    {
        qDebug() << "cancel TAP progress Timer" << endl;
        checkCardTappedTimer->stop();
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
void page_payment::setPage(pageProduct *pageSizeSelect, page_error_wifi *pageWifiError, page_dispenser *page_dispenser, page_idle *pageIdle, page_help *pageHelp)
{
    tmpCounter = 0;
    this->p_pageProduct = pageSizeSelect;
    this->p_page_wifi_error = pageWifiError;
    this->p_page_dispense = page_dispenser;
    this->p_page_idle = pageIdle;
    this->p_page_help = pageHelp;
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
    ui->title_Label->hide();
    
    stopPayTimers();
    // p_page_dispense->showEvent(dispenseEvent);
    // p_page_dispense->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_dispense);
}

void page_payment::updateTotals(string drinkDescription, string drinkAmount, string orderTotal)
{
}

/*Cancel any previous payment*/
void page_payment::cancelPayment()
{
    // finishSession(std::stoi(socketAddr), MAC_LABEL, MAC_KEY);
    checkPacketReceivedTimer->stop();
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
    state_payment = s_init;

    // ui->order_total_amount->show();

    if (getPaymentMethod() == "tap")
    {
        qDebug() << "Init tap";
        ui->payment_bypass_Button->setEnabled(false);
        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_PAY);
        ui->qrCode->hide();
        ui->productLabel->hide();
        ui->order_drink_amount->hide();
        ui->order_total_amount->hide();
        ui->steps_Label->hide();
        ui->processing_Label->hide();
    }
    else
    {
        ui->qrCode->show();
        ui->productLabel->show();
        ui->order_drink_amount->show();
        ui->title_Label->setText("pay by phone");
        ui->scan_Label->setText("Scan to Pay");
        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_QR_PAY_BACKGROUND_PATH);
        ui->payment_bypass_Button->setEnabled(false);
        ui->productLabel->setText(p_page_idle->currentProductOrder->getSelectedProductName() + " " + p_page_idle->currentProductOrder->getSelectedSizeToVolumeWithCorrectUnits(true, true));
        ui->order_drink_amount->setText("$" + QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2));
        ui->order_total_amount->setText("Total: $" + QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2));
        ui->steps_Label->show();
        ui->processing_Label->hide();
        transactionLogging += "\n 2: QR code - True";
    }

    // p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_QR_PAY_BACKGROUND_PATH);
    paymentEndTimer = new QTimer(this);
    paymentEndTimer->setInterval(1000);
    connect(paymentEndTimer, SIGNAL(timeout()), this, SLOT(onTimeoutTick()));
    paymentEndTimer->start(1000);
    _pageTimeoutCounterSecondsLeft = QR_PAGE_TIMEOUT_SECONDS;
    this->ui->payment_countdownLabel->setText("");

    ui->refreshLabel->hide();

    QString payment_method = getPaymentMethod();

    if (payment_method == "tap")
    {
        // createOrderIdAndSendToBackend();
        qDebug() << "Prepare tap order";
        tapPaymentHandler();

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

    if (createOrderIdAndSendToBackend())
    {

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
    else
    {
        p_page_idle->pageTransition(this, p_page_wifi_error);
    }
}

bool page_payment::createOrderIdAndSendToBackend()
{
    // an order Id is generated locally and the order is sent to the cloud.
    bool shouldShowQR = false;
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
                                           "&contents=" + contents + "&price=" + price + "&productId=" + productId + "&quantity_requested=" + quantity_requested + "&size_unit=" + productUnits + "&logging=" + transactionLogging;
    curl_order_parameters = curl_order_parameters_string.toLocal8Bit();

    curl1 = curl_easy_init();

    if (!curl1)
    {
        qDebug() << "pagepayement cURL Failed to init : " + curl_order_parameters_string + "to: " + "https://soapstandportal.com/api/machine_data/createOrderInDb";
        return shouldShowQR;
    }

    curl_easy_setopt(curl1, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/createOrderInDb");
    curl_easy_setopt(curl1, CURLOPT_POSTFIELDS, curl_order_parameters.data());
    curl_easy_setopt(curl1, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl1, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl1, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);

    int createOrderInDbAttempts = 0;
    QString feedback;
    do
    {
        res1 = curl_easy_perform(curl1);
        feedback = QString::fromUtf8(readBuffer.c_str());
        createOrderInDbAttempts += 1;
    } while (feedback != "true" && createOrderInDbAttempts <= 3);
    if (feedback != "true")
    {
        qDebug() << "ERROR: Problem at create order in the cloud request. error code: " + QString::number(res1);
    }
    else
    {
        qDebug() << "create order in the cloud request sent to soapstandportal (" + curl_order_parameters_string + "). Server feedback: " << feedback;
    }
    if (feedback == "true")
    {
        shouldShowQR = true;
    }
    curl_easy_cleanup(curl1);
    readBuffer = "";
    feedback = "";

    _pageTimeoutCounterSecondsLeft = QR_PAGE_TIMEOUT_SECONDS;
    _qrProcessedPeriodicalCheckSec = QR_PROCESSED_PERIODICAL_CHECK_SECONDS;
    // qrPeriodicalCheckTimer->start(1000);
    return shouldShowQR;
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
            transactionLogging += "\n 4: Order Paid - True";
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
            if(!transactionLogging.contains("\n 3: QR Scanned - True")){
                transactionLogging += "\n 3: QR Scanned - True";
            }
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
        transactionLogging += "\n 5: Timeout - True";

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

void page_payment::tapPaymentHandler()
{
    ui->animated_Label->move(221,327);
    QMovie *tapGif = new QMovie("/home/df-admin/production/references/templates/default/tap.gif");
    ui->animated_Label->setMovie(tapGif);
    tapGif->start();

    int socket = connectSocket();
    socketAddr = to_string(socket);
    qDebug() << "Tap terminal Socket Connected to " << QString::fromStdString(socketAddr) << endl;
    std::map<std::string, std::string> configMap = readConfigFile();
    MAC_KEY = configMap["MAC_KEY"];
    MAC_LABEL = configMap["MAC_LABEL"];
    lastTransactionId = std::stoi(configMap["INVOICE"]);

    startSession(socket, MAC_LABEL, MAC_KEY, lastTransactionId + 1);
    startPaymentProcess();
}

void page_payment::startPaymentProcess(){
     double price = p_page_idle->currentProductOrder->getSelectedPriceCorrected();
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << price;
    std::string authCommand = authorizationCommand(std::stoi(socketAddr), MAC_LABEL, MAC_KEY, stream.str());

    std::string packetSent = sendPacket(authCommand, std::stoi(socketAddr), true);
    stop_tap_action_thread = false;
    stop_authorization_thread=false;
    cardTapThread = std::thread(receiveCardTapAction);
    cardTapThread.detach();
    checkCardTappedTimer->start();  

    dataThread = std::thread(receiveAuthorizationThread, std::stoi(socketAddr));
    dataThread.detach();
    checkPacketReceivedTimer->start();  
}

void page_payment::check_packet_available()
{
    // qDebug() << "Check for received packets from tap terminal.";

    std::map<std::string, std::string> xml_packet_dict;
    bool isPacketReceived;
    isPacketReceived = checkPacketReceived(true, &xml_packet_dict);
    // qDebug() << "Is packet received" << isPacketReceived;
    if (isPacketReceived)
    {
        checkPacketReceivedTimer->stop();
        authorized_transaction(xml_packet_dict);
    }
}
void page_payment::check_card_tapped()
{
    std::map<std::string, std::string> xml_packet_dict_tapped;
    bool isPacketReceived;
    std::string card_tap_status;
    auto output= checkCardTapped(true, &xml_packet_dict_tapped);
    
    isPacketReceived = std::get<0>(output);
    card_tap_status = std::get<1>(output);

    if (isPacketReceived && card_tap_status=="Success")
    {
        qDebug() << "Packet received true";
        // ui->title_Label->setText("Processing Payment");
        // ui->title_Label->show();
        checkCardTappedTimer->stop();
        QMovie* currentGif = ui->animated_Label->movie();
        currentGif->stop();
        delete currentGif;
        // p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_GENERIC);
        ui->animated_Label->move(410,480);
        QMovie *movie = new QMovie("/home/df-admin/production/references/templates/default/soapstandspinner.gif");
        ui->animated_Label->setMovie(movie);
        movie->start(); 
    }
    else if(card_tap_status=="Failed"){
        on_previousPage_Button_clicked();
    }     
    }

void page_payment::authorized_transaction(std::map<std::string, std::string> responseObj)
{
    double price = p_page_idle->currentProductOrder->getSelectedPriceCorrected();
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << price;
    QMovie* currentGif = ui->animated_Label->movie();
    currentGif->stop();
    delete currentGif;
    if (responseObj["RESULT"] == "APPROVED")
    {
        
        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_PAY_SUCCESS);
        CTROUTD = responseObj["CTROUTD"];
        AUTH_CODE = responseObj["AUTH_CODE"];
        proceed_to_dispense();
    }
    else if (responseObj["RESULT"] == "APPROVED/STORED")
    {
        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_PAY_SUCCESS);
        CTROUTD = responseObj["CTROUTD"];
        AUTH_CODE = responseObj["AUTH_CODE"];
        SAF_NUM = responseObj["SAF_NUM"];
        proceed_to_dispense();
    }
    else if (responseObj["RESULT"] == "DECLINED")
    {

        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_PAY_FAIL);
        startPaymentProcess();
        paymentEndTimer->start();
        // std::map<std::string, std::string> responseObjSecond = authorization(socket, MAC_LABEL, MAC_KEY, stream.str());
        // if (responseObjSecond["RESULT"] == "APPROVED")
        // {
        //     CTROUTD = responseObjSecond["CTROUTD"];
        //     proceed_to_dispense();
        // }
        // else
        // {
        //     finishSession(std::stoi(socketAddr), MAC_LABEL, MAC_KEY);
        //     p_page_idle->pageTransition(this, p_pageProduct);
        // }
    }
}
void page_payment::declineTimer_start()
{
}

bool page_payment::exitConfirm()
{
    qDebug() << "In exit confirm";
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
            transactionLogging = "";
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
        transactionLogging = "";
        return true;
    }
}

// Navigation: Back to Drink Size Selection
void page_payment::on_previousPage_Button_clicked()
{
    qDebug() << "In previous page button" << endl;
    if (exitConfirm())
    {
        if (tap_payment)
        {   
            stop_tap_action_thread = true;
            stop_authorization_thread=true;
            stopPayTimers();
            qDebug() << "Stopping the threads";
            cancelTransaction(connectSecondarySocket());
            qDebug()<< "My socket address is "<< QString::fromStdString(socketAddr) << endl;
            // finishSession(std::stoi(socketAddr), MAC_LABEL, MAC_KEY);
            qDebug() << "Session finished sent";
            
        }
        p_page_idle->pageTransition(this, p_pageProduct);
    }
}

void page_payment::on_mainPage_Button_clicked()
{
    if (exitConfirm())
    {
        p_page_idle->pageTransition(this, p_page_help);
    }
}

void page_payment::idlePaymentTimeout()
{
    // resetPaymentPage();
    // p_page_idle->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_idle);
}
void page_payment::resetPaymentPage()
{
    stopPayTimers();
    response = true;
    // readTimer->stop();
    qDebug() << "Cancelled";
}

/* ----- Payment ----- */

bool page_payment::tap_init()
{
    return true;
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
