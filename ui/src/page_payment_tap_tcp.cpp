//***************************************
//
// page_payment_tap_tcp.cpp
// GUI class while machine is processing
// payment.
//
// Coordinates User input from payment select
// class then communcates results to page_dispenser.
//
// created: 05-04-2022
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "page_payment_tap_tcp.h"
#include "ui_page_payment_tap_tcp.h"

#include "page_product.h"
#include "page_product_mixing.h"
#include "page_dispenser.h"
#include "page_idle.h"
#include "payment/commands.h"
#include "payment/setup_Tap.h"

// extern QString transactionLogging;
extern std::string CTROUTD;
extern std::string MAC_KEY;
extern std::string MAC_LABEL;
extern std::string AUTH_CODE;
extern std::string SAF_NUM;
extern std::string socketAddr;
extern std::map<std::string, std::string> tapPaymentObject;
std::thread cardTapThread;
std::thread dataThread;
int numberOfTapAttempts = 0;
// CTOR

page_payment_tap_tcp::page_payment_tap_tcp(QWidget *parent) : QWidget(parent),
                                                              ui(new Ui::page_payment_tap_tcp)
{
    // Fullscreen background setup
    ui->setupUi(this);

    std::atomic<bool> stop_tap_action_thread(false);
    std::atomic<bool> stop_authorization_thread(false);
    // Receive packets from tap device checker
    checkPacketReceivedTimer = new QTimer(this);
    connect(checkPacketReceivedTimer, SIGNAL(timeout()), this, SLOT(check_packet_available()));
    checkPacketReceivedTimer->setInterval(500); // was 500

    checkCardTappedTimer = new QTimer(this);
    connect(checkCardTappedTimer, SIGNAL(timeout()), this, SLOT(check_card_tapped()));
    checkCardTappedTimer->setInterval(500); // was 500

    // Idle Payment reset
    idlePaymentTimer = new QTimer(this);
    idlePaymentTimer->setInterval(1000);
    connect(idlePaymentTimer, SIGNAL(timeout()), this, SLOT(idlePaymentTimeout()));

    ui->pushButton_payment_bypass->setEnabled(false);
    ui->label_title->hide();
    statusbarLayout = new QVBoxLayout(this);
}

void page_payment_tap_tcp::initiate_tap_setup()
{
    qDebug() << "InitializingTap payment";
    std::map<std::string, std::string> configMap = readConfigFile();
    std::map<std::string, std::string> deviceStatus = checkDeviceStatus(connectSecondarySocket());
    if (deviceStatus["MACLABEL_IN_SESSION"] != "")
    {   
        qDebug() << "Finishing session";
        // finishSession(connectSocket(), configMap["MAC_KEY"], configMap["MAC_LABEL"]);
    }
    cancelTransaction(connectSecondarySocket());
    qDebug() << "Transaction cancelled";
    if (configMap["MAC_KEY"] != "")
    {
        std::map<std::string, std::string> testResponse = testMac(connectSocket(), configMap["MAC_KEY"], configMap["MAC_LABEL"]);
        if (testResponse["RESPONSE_TEXT"] == "Match")
        {
            qDebug() << "Test Mac Command Matched";
        }
        else
        {
            qDebug() << "Re-registration of the device";
            registerDevice(connectSocket());
        }
    }
    else
    {
        qDebug() << "No file config";
        registerDevice(connectSocket());
    }
}
void page_payment_tap_tcp::stopPayTimers()
{

    if (checkPacketReceivedTimer != nullptr)
    {
        qDebug() << "cancel payment progress Timer";
        checkPacketReceivedTimer->stop();
    }
    if (checkCardTappedTimer != nullptr)
    {
        qDebug() << "cancel TAP progress Timer";
        checkCardTappedTimer->stop();
    }

    if (idlePaymentTimer != nullptr)
    {
        qDebug() << "cancel page_idle payment Timer";
        idlePaymentTimer->stop();
    }
}

/*
 * Page Tracking reference
 */
void page_payment_tap_tcp::setPage(page_product *p_page_product, page_product_mixing *p_page_product_mixing, page_error_wifi *pageWifiError, page_dispenser *page_dispenser, page_idle *pageIdle, page_help *pageHelp, statusbar *p_statusbar)
{
    this->p_page_product = p_page_product;
    this->p_page_product_mixing = p_page_product_mixing;
    this->p_page_wifi_error = pageWifiError;
    this->p_page_dispense = page_dispenser;
    this->p_page_idle = pageIdle;
    this->p_page_help = pageHelp;
    this->p_statusbar = p_statusbar;
}

// DTOR
page_payment_tap_tcp::~page_payment_tap_tcp()
{
    delete ui;
}

void page_payment_tap_tcp::on_pushButton_payment_bypass_clicked()
{
    hideCurrentPageAndShowProvided(p_page_dispense);
}

/*Cancel any previous payment*/
void page_payment_tap_tcp::cancelPayment()
{
    // finishSession(std::stoi(socketAddr), MAC_LABEL, MAC_KEY);
    // checkPacketReceivedTimer->stop();
}

void page_payment_tap_tcp::showEvent(QShowEvent *event)
{
    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);
    idlePaymentTimer->start(1000);
    _pageTimeoutCounterSecondsLeft = PAGE_PAYMENT_TAP_TCP_PAGE_TIMEOUT_SECONDS;

    statusbarLayout->addWidget(p_statusbar);            // Only one instance can be shown. So, has to be added/removed per page.
    statusbarLayout->setContentsMargins(0, 1874, 0, 0); // int left, int top, int right, int bottom);

    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget

    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_TAP_PAYMENT_CSS);

    ui->pushButton_previous_page->setStyleSheet(styleSheet);

    ui->pushButton_to_idle->setProperty("class", "invisible_button");
    ui->pushButton_payment_bypass->setProperty("class", "invisible_button");
    ui->pushButton_to_idle->setStyleSheet(styleSheet);
    ui->pushButton_payment_bypass->setStyleSheet(styleSheet);

    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_previous_page);

    ui->pushButton_payment_bypass->setEnabled(false);

    state_tap_payment = s_tap_init;
    ui->pushButton_payment_bypass->setEnabled(false);
    p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_PAY);
    ui->productLabel->hide();
    ui->order_drink_amount->hide();

    msgBox = nullptr;

    qDebug() << "Prepare tap order";
    tapPaymentHandler();
}

void page_payment_tap_tcp::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{

    resetPaymentPage();
    statusbarLayout->removeWidget(p_statusbar); // Only one instance can be shown. So, has to be added/removed per page.
    idlePaymentTimer->stop();

    p_page_idle->thisMachine->pageTransition(this, pageToShow);

    if (msgBox != nullptr)
    {
        msgBox->hide();
        msgBox->deleteLater();
        msgBox = nullptr;
    }
}

bool page_payment_tap_tcp::setpaymentProcess(bool status)
{
    return (paymentProcessing = status);
}

void page_payment_tap_tcp::tapPaymentHandler()
{
    if(p_page_idle->thisMachine->hasMixing()){
        ui->animated_Label->hide();
    }
    ui->animated_Label->move(221, 327);
    QString image_path = p_page_idle->thisMachine->getTemplatePathFromName("tap.gif");
    QMovie *tapGif = new QMovie(image_path);

    ui->animated_Label->setMovie(tapGif);
    tapGif->start();

    int socket = connectSocket();
    socketAddr = to_string(socket);
    qDebug() << "Tap terminal Socket Connected to " << QString::fromStdString(socketAddr);
    std::map<std::string, std::string> configMap = readConfigFile();
    MAC_KEY = configMap["MAC_KEY"];
    MAC_LABEL = configMap["MAC_LABEL"];
    lastTransactionId = std::stoi(configMap["INVOICE"]);

    startSession(socket, MAC_LABEL, MAC_KEY, lastTransactionId + 1);
    tapPaymentObject["mac_label"] = MAC_LABEL;

    startPaymentProcess();
}

void page_payment_tap_tcp::startPaymentProcess()
{
    state_tap_payment = s_tap_payment_processing;
    int pnumber_selected = p_page_idle->thisMachine->getSelectedProduct()->getPNumber();
    double price = p_page_idle->thisMachine->getSelectedProduct()->getBasePriceSelectedSize();
    if (p_page_idle->thisMachine->getSelectedProduct()->getSelectedSize() == SIZE_CUSTOM_INDEX)
    {
        double max_custom_volume = p_page_idle->thisMachine->getSelectedProduct()->getVolumeOfSelectedSize();
        price = price*max_custom_volume;
    }
    price = p_page_idle->thisMachine->getPriceWithDiscount(price);

    if (numberOfTapAttempts < 3)
    {
        numberOfTapAttempts += 1;
        
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2) << price;
        std::string authCommand = authorizationCommand(std::stoi(socketAddr), MAC_LABEL, MAC_KEY, stream.str());

        std::string packetSent = sendPacket(authCommand, std::stoi(socketAddr), true);
        stop_tap_action_thread = false;
        stop_authorization_thread = false;
        cardTapThread = std::thread(receiveCardTapAction);
        cardTapThread.detach();
        checkCardTappedTimer->start();

        dataThread = std::thread(receiveAuthorizationThread, std::stoi(socketAddr));
        dataThread.detach();
        checkPacketReceivedTimer->start();
        QString base_text = p_page_idle->thisMachine->getTemplateTextByElementNameAndPage(ui->preauthLabel);
        ui->preauthLabel->setText(base_text.arg(p_page_idle->thisMachine->getSelectedProduct()->getSizeAsVolumeWithCorrectUnits(true, true)));
        ui->order_total_amount->setText("$ " + QString::number(price, 'f', 2));
    }
    else
    {
        numberOfTapAttempts = 0;
        on_pushButton_previous_page_clicked();
    }
}

void page_payment_tap_tcp::check_packet_available()
{
    std::map<std::string, std::string> xml_packet_dict;
    bool isPacketReceived;
    isPacketReceived = checkPacketReceived(true, &xml_packet_dict);
    if (isPacketReceived)
    {
        checkPacketReceivedTimer->stop();
        authorized_transaction(xml_packet_dict);
    }
}
void page_payment_tap_tcp::check_card_tapped()
{
    std::map<std::string, std::string> xml_packet_dict_tapped;
    bool isPacketReceived;
    std::string card_tap_status;
    auto output = checkCardTapped(true, &xml_packet_dict_tapped);

    isPacketReceived = std::get<0>(output);
    card_tap_status = std::get<1>(output);

    if (isPacketReceived && card_tap_status == "Success")
    {
        p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_PAY);
        qDebug() << "Packet received true";
        p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->label_title, "processing");

        // ui->label_title->setText("Processing Payment");
        // ui->label_title->show();
        checkCardTappedTimer->stop();
        QMovie *currentGif = ui->animated_Label->movie();
        if (currentGif)
        {
            currentGif->stop();
            delete currentGif;
        }

        ui->animated_Label->move(410, 480);
        QString image_path = p_page_idle->thisMachine->getTemplatePathFromName("soapstandspinner.gif");
        QMovie *movie = new QMovie(image_path);
        ui->animated_Label->setMovie(movie);
        movie->start();
    }
    else if (card_tap_status == "Failed")
    {
        on_pushButton_previous_page_clicked();
    }
}

void page_payment_tap_tcp::authorized_transaction(std::map<std::string, std::string> responseObj)
{
    state_tap_payment = s_tap_payment_done;
    stopPayTimers();
    QMovie *currentGif = ui->animated_Label->movie();
    currentGif->stop();
    delete currentGif;
    if (responseObj["RESULT"] == "APPROVED")
    {

        p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_PAY_SUCCESS);
        // CTROUTD = responseObj["CTROUTD"];
        // AUTH_CODE = responseObj["AUTH_CODE"];
        tapPaymentObject["ctroutd"] = responseObj["CTROUTD"];
        tapPaymentObject["auth_code"] = responseObj["AUTH_CODE"];
        tapPaymentObject["amount"] = responseObj["TRANS_AMOUNT"];
        tapPaymentObject["date"] = responseObj["TRANS_DATE"];
        tapPaymentObject["time"] = responseObj["TRANS_TIME"];
        tapPaymentObject["card_number"] = responseObj["ACCT_NUM"];
        tapPaymentObject["card_type"] = responseObj["PAYMENT_MEDIA"];
        tapPaymentObject["status"] = "Authorized";
        hideCurrentPageAndShowProvided(p_page_dispense);
    }
    else if (responseObj["RESULT"] == "APPROVED/STORED")
    {
        p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_PAY_SUCCESS);

        tapPaymentObject["ctroutd"] = responseObj["CTROUTD"];
        tapPaymentObject["auth_code"] = responseObj["AUTH_CODE"];
        tapPaymentObject["saf_num"] = responseObj["SAF_NUM"];
        tapPaymentObject["amount"] = responseObj["TRANS_AMOUNT"];
        tapPaymentObject["date"] = responseObj["TRANS_DATE"];
        tapPaymentObject["time"] = responseObj["TRANS_TIME"];
        tapPaymentObject["card_number"] = responseObj["ACCT_NUM"];
        tapPaymentObject["card_type"] = responseObj["PAYMENT_MEDIA"];
        tapPaymentObject["status"] = "Authorized Offline";
        hideCurrentPageAndShowProvided(p_page_dispense);
    }
    else if (responseObj["RESULT"] == "DECLINED")
    {

        p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_PAY_FAIL);
        startPaymentProcess();
    }
}

bool page_payment_tap_tcp::exitConfirm()
{
    qDebug() << "In exit confirm";
    if (state_tap_payment == s_tap_payment_processing || state_tap_payment == s_tap_payment_done)
    {
        // ARE YOU SURE YOU WANT TO EXIT?
        msgBox = new QMessageBox();
        msgBox->setWindowFlags(Qt::FramelessWindowHint| Qt::Dialog); // do not show messagebox header with program name
        p_page_idle->thisMachine->addCssClassToObject(msgBox, "msgBoxbutton msgBox", PAGE_TAP_PAYMENT_CSS);
        QString searchString = this->objectName() + "->msgBox_cancel";
        // p_page_idle->thisMachine->setTextToObject(&msgBox, p_page_idle->thisMachine->getTemplateText(searchString));

        int remainingTime = MESSAGE_BOX_TIMEOUT_EXIT_TAP_CONFIRM_SECONDS; // Initial value in seconds
        QString templateText = p_page_idle->thisMachine->getTemplateText(searchString);
        QString autoCloseText = QString("Closing in %1 seconds...").arg(remainingTime);
        QString messageBoxText = templateText + "\n" + autoCloseText;

        msgBox->setText(messageBoxText);

        QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_QR_PAYMENT_CSS);
        msgBox->setProperty("class", "msgBoxbutton msgBox");
        msgBox->setStyleSheet(styleSheet);

        QTimer *timeoutTimer = new QTimer(msgBox);
        QObject::connect(timeoutTimer, &QTimer::timeout, [this, &remainingTime, &templateText, timeoutTimer]()
                         {
            remainingTime--;
            QString autoCloseText = QString("Closing in %1 seconds...").arg(remainingTime);
            QString messageBoxText = templateText + "\n" + autoCloseText;
            msgBox->setText(messageBoxText);
            msgBox->raise(); // not reproducable bug for Lode but, for UBC machine, the msgbox keeps focus but is set to background, so it's unclickable
 
            if (remainingTime <= 0) {
                timeoutTimer->stop();
                msgBox->hide();
                msgBox->deleteLater();
                msgBox = nullptr;
            } });

        timeoutTimer->start(1000); // Update every 1000 milliseconds (1 second)
        
        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox->exec();
        msgBox = nullptr;
        timeoutTimer->stop();
        bool exitPage = false; //return false as default e.g. if message box timed out
        switch (ret)
        {
        case QMessageBox::Yes:
            exitPage = true;
            break;
            // return true;
        case QMessageBox::No:
            exitPage = false;
            break;
            // return false;
        }

        // WARNING: if messagbox still active while TAP page times out, it will still run in the background. 
        return exitPage; // return false as default e.g. if message box timed out
    }
}

// Navigation: Back to Product Size Selection
void page_payment_tap_tcp::on_pushButton_previous_page_clicked()
{
    qDebug() << "In previous page button";

    if (exitConfirm())
    {
        stop_tap_action_thread = true;
        stop_authorization_thread = true;
        std::map<std::string, std::string> cancelResp = cancelTransaction(connectSecondarySocket());
        qDebug() << "My socket address is " << QString::fromStdString(socketAddr);
        if (cancelResp["RESULT"] == "OK")
        {
            qDebug() << QString::fromUtf8(cancelResp["RESULT"].c_str());
            // It needs around 2-3 seconds before sending finish session. Tried without sleep and/or changed sleep time to 1 and 2 second, it crashes
            sleep(3);
            finishSession(std::stoi(socketAddr), MAC_LABEL, MAC_KEY);
            qDebug() << "Session finished sent";
        }
    p_page_idle->thisMachine->hasMixing() ? hideCurrentPageAndShowProvided(p_page_product_mixing) : hideCurrentPageAndShowProvided(p_page_product);
    }
}

void page_payment_tap_tcp::on_pushButton_to_idle_clicked()
{
    if (exitConfirm())
    {
        hideCurrentPageAndShowProvided(p_page_help);
    }
}

void page_payment_tap_tcp::rebootTapTcpDevice(){
    rebootDevice(connectSocket());
}

void page_payment_tap_tcp::idlePaymentTimeout()
{
    if (--_pageTimeoutCounterSecondsLeft >= 0)
    {
        // qDebug() << "Tick Down: " << _pageTimeoutCounterSecondsLeft;
    }
    else
    {
        // qDebug() << "Timer Done!" << _pageTimeoutCounterSecondsLeft;
        _pageTimeoutCounterSecondsLeft = PAGE_PAYMENT_TAP_TCP_PAGE_TIMEOUT_SECONDS;
        p_page_idle->thisMachine->hasMixing() ? hideCurrentPageAndShowProvided(p_page_product_mixing) : hideCurrentPageAndShowProvided(p_page_product);
    }
}

void page_payment_tap_tcp::resetPaymentPage()
{
    ui->label_title->hide();

    stopPayTimers();
    p_page_idle->thisMachine->resetTransactionLogging();
    qDebug() << "Reset Payment Page";
}

// void page_payment_tap_tcp::authorizeTestTransaction()
// {
//     std::map<std::string, std::string> configMap = readConfigFile();
//     MAC_KEY = configMap["MAC_KEY"];
//     MAC_LABEL = configMap["MAC_LABEL"];
//     lastTransactionId = std::stoi(configMap["INVOICE"]);

//     startSession(socket, MAC_LABEL, MAC_KEY, lastTransactionId + 1);
//     // tapPaymentObject["session_id"] = std::to_string(lastTransactionId+1);
//     tapPaymentObject["mac_label"] = MAC_LABEL;
//     double price = 1;
//     std::ostringstream stream;
//     stream << std::fixed << std::setprecision(2) << price;
//     std::string authCommand = authorizationCommand(std::stoi(socketAddr), MAC_LABEL, MAC_KEY, stream.str());

//     std::string packetSent = sendPacket(authCommand, std::stoi(socketAddr), true);
//     stop_tap_action_thread = false;
//     stop_authorization_thread = false;
//     cardTapThread = std::thread(receiveCardTapAction);
//     cardTapThread.detach();
//     checkCardTappedTimer->start();

//     dataThread = std::thread(receiveAuthorizationThread, std::stoi(socketAddr));
//     dataThread.detach();
//     checkPacketReceivedTimer->start();
    
// }
