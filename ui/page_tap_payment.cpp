//***************************************
//
// page_tap_payment.cpp
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

#include "page_tap_payment.h"
#include "ui_page_tap_payment.h"

#include "page_product.h"
#include "page_dispenser.h"
#include "page_idle.h"
#include "payment/commands.h"
#include "payment/setup_Tap.h"

extern QString transactionLogging;
extern std::string CTROUTD;
extern std::string MAC_KEY;
extern std::string MAC_LABEL;
extern std::string AUTH_CODE;
extern std::string SAF_NUM;
extern std::string socketAddr;
std::thread cardTapThread;
std::thread dataThread;
int numberOfTapAttempts = 0;
// CTOR

page_tap_payment::page_tap_payment(QWidget *parent) : QWidget(parent),
                                                      ui(new Ui::page_tap_payment)
{
    // Fullscreen background setup
    ui->setupUi(this);
    qDebug() << "Payment page";
    // ui->pushButton_previous_page->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->pushButton_previous_page->setStyleSheet(
        "QPushButton {"
        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 32px;"
        "line-height: 99px;"
        "letter-spacing: 1.5px;"
        "text-transform: lowercase;"
        "color: #003840;"
        "text-align: cCenter;"
        "qproperty-alignment: AlignCenter;"
        "border: none;"
        "}");
    ui->pushButton_previous_page->setText("<- Back");
    ui->pushButton_to_idle->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->pushButton_payment_bypass->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    ui->pushButton_payment_bypass->setEnabled(false);

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

    ui->pushButton_payment_bypass->setEnabled(false);
    ui->label_title->hide();

    // ui->order_total_amount->hide();
    DbManager db(DB_PATH);

    if (db.getPaymentMethod(1) == "tapTcp")
    {
        qDebug() << "InitializingTap payment";
        tap_payment = true;
        std::map<std::string, std::string> configMap = readConfigFile();
        std::map<std::string, std::string> deviceStatus = checkDeviceStatus(connectSecondarySocket());
        if (deviceStatus["MACLABEL_IN_SESSION"] != "")
        {
            // finishSession(connectSocket(), configMap["MAC_KEY"], configMap["MAC_LABEL"]);
        }
        cancelTransaction(connectSecondarySocket());
        qDebug() << "Transaction cancelled";
        if (configMap["MAC_KEY"] != "")
        {
            std::map<std::string, std::string> testResponse = testMac(connectSocket(), configMap["MAC_KEY"], configMap["MAC_LABEL"]);
            if (testResponse["RESPONSE_TEXT"] == "Match")
            {
                qDebug() << "Test Mac Command Matched" << endl;
            }
            else
            {
                qDebug() << "Re-registration of the device";
                registerDevice(connectSocket());
            }
        }
        else
        {
            qDebug() << "No file config" << endl;
            registerDevice(connectSocket());
        }
    }
}

QString page_tap_payment::getPaymentMethodForConstructorTime(uint8_t slot)
{
    qDebug() << "db open245";
    DbManager db2(DB_PATH);
    QString payment_method = db2.getPaymentMethod(slot);
    db2.closeDB();
    return payment_method;
}

void page_tap_payment::stopPayTimers()
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
}

/*
 * Page Tracking reference
 */
void page_tap_payment::setPage(pageProduct *pageSizeSelect, page_error_wifi *pageWifiError, page_dispenser *page_dispenser, page_idle *pageIdle, page_help *pageHelp)
{
    tmpCounter = 0;
    this->p_pageProduct = pageSizeSelect;
    this->p_page_wifi_error = pageWifiError;
    this->p_page_dispense = page_dispenser;
    this->p_page_idle = pageIdle;
    this->p_page_help = pageHelp;
}

// DTOR
page_tap_payment::~page_tap_payment()
{
    delete ui;
}

/* ----- GUI ----- */

// Labels and button for tapping payment
void page_tap_payment::displayPaymentPending(bool isVisible)
{
}

void page_tap_payment::on_pushButton_payment_bypass_clicked()
{
    hideCurrentPageAndShowProvided(p_page_dispense);
}

void page_tap_payment::updateTotals(string drinkDescription, string drinkAmount, string orderTotal)
{
}

/*Cancel any previous payment*/
void page_tap_payment::cancelPayment()
{
    // finishSession(std::stoi(socketAddr), MAC_LABEL, MAC_KEY);
    checkPacketReceivedTimer->stop();
}

void page_tap_payment::resizeEvent(QResizeEvent *event)
{
}

void page_tap_payment::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Tap Payment >>>>>>>>>";
    QWidget::showEvent(event);
    state_tap_payment = s_tap_init;

    qDebug() << "Init tap";
    ui->pushButton_payment_bypass->setEnabled(false);
    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_PAY);
    ui->productLabel->hide();
    ui->order_drink_amount->hide();
    // ui->order_total_amount->hide();

    qDebug() << "Prepare tap order";
    tapPaymentHandler();
}

void page_tap_payment::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{

    resetPaymentPage();
    p_page_idle->pageTransition(this, pageToShow);
}

bool page_tap_payment::setpaymentProcess(bool status)
{
    return (paymentProcessing = status);
}

void page_tap_payment::setProgressLabel(QLabel *label, int dot)
{
}

// Local storge for now.  Will need to refactor logger to do a nightly push to AWS
void page_tap_payment::storePaymentEvent(QSqlDatabase db, QString event)
{
}

void page_tap_payment::tapPaymentHandler()
{
    ui->animated_Label->move(221, 327);
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

void page_tap_payment::startPaymentProcess()
{
    if (numberOfTapAttempts < 3)
    {
        numberOfTapAttempts += 1;
        double price = p_page_idle->selectedProduct->getPriceCorrected();
        if (p_page_idle->selectedProduct->getSizeAsChar() == 'c')
        {
            price = p_page_idle->selectedProduct->getPriceCustom();
        }
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
        ui->preauthLabel->setText("You are being pre-authorized for maximum volume " + p_page_idle->selectedProduct->getSizeToVolumeWithCorrectUnits(true, true) +
                                  " with amount of:");
        ui->order_total_amount->setText("$ " + QString::number(price, 'f', 2));
    }
    else
    {
        numberOfTapAttempts = 0;
        // stopPayTimers();
        on_pushButton_previous_page_clicked();
    }
}

void page_tap_payment::check_packet_available()
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
void page_tap_payment::check_card_tapped()
{
    std::map<std::string, std::string> xml_packet_dict_tapped;
    bool isPacketReceived;
    std::string card_tap_status;
    auto output = checkCardTapped(true, &xml_packet_dict_tapped);

    isPacketReceived = std::get<0>(output);
    card_tap_status = std::get<1>(output);

    if (isPacketReceived && card_tap_status == "Success")
    {
        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_PAY);
        qDebug() << "Packet received true";
        ui->label_title->setText("Processing Payment");
        // ui->label_title->show();
        checkCardTappedTimer->stop();
        QMovie *currentGif = ui->animated_Label->movie();
        if (currentGif)
        {
            currentGif->stop();
            delete currentGif;
        }

        // p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_GENERIC);
        ui->animated_Label->move(410, 480);
        QMovie *movie = new QMovie("/home/df-admin/production/references/templates/default/soapstandspinner.gif");
        ui->animated_Label->setMovie(movie);
        movie->start();
    }
    else if (card_tap_status == "Failed")
    {
        on_pushButton_previous_page_clicked();
    }
}

void page_tap_payment::authorized_transaction(std::map<std::string, std::string> responseObj)
{
    stopPayTimers();
    QMovie *currentGif = ui->animated_Label->movie();
    currentGif->stop();
    delete currentGif;
    if (responseObj["RESULT"] == "APPROVED")
    {

        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_PAY_SUCCESS);
        CTROUTD = responseObj["CTROUTD"];
        AUTH_CODE = responseObj["AUTH_CODE"];
        hideCurrentPageAndShowProvided(p_page_dispense);
    }
    else if (responseObj["RESULT"] == "APPROVED/STORED")
    {
        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_PAY_SUCCESS);
        CTROUTD = responseObj["CTROUTD"];
        AUTH_CODE = responseObj["AUTH_CODE"];
        SAF_NUM = responseObj["SAF_NUM"];
        hideCurrentPageAndShowProvided(p_page_dispense);
    }
    else if (responseObj["RESULT"] == "DECLINED")
    {

        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_PAY_FAIL);
        startPaymentProcess();
    }
}
void page_tap_payment::declineTimer_start()
{
}

bool page_tap_payment::exitConfirm()
{
    qDebug() << "In exit confirm";
    if (state_tap_payment == s_tap_payment_processing || state_tap_payment == s_tap_payment_done)
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
            // resetPaymentPage();
            // transactionLogging = "";
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
        // resetPaymentPage();
        // transactionLogging = "";
        return true;
    }
}

// Navigation: Back to Drink Size Selection
void page_tap_payment::on_pushButton_previous_page_clicked()
{
    qDebug() << "In previous page button" << endl;

    if (exitConfirm())
    {
        if (tap_payment)
        {
            stop_tap_action_thread = true;
            stop_authorization_thread = true;
            // stopPayTimers();
            qDebug() << "Stopping the threads";
            std::map<std::string, std::string> cancelResp = cancelTransaction(connectSecondarySocket());
            qDebug() << "My socket address is " << QString::fromStdString(socketAddr) << endl;
            if (cancelResp["RESULT"] == "OK")
            {
                qDebug() << QString::fromUtf8(cancelResp["RESULT"].c_str());
                // sleep(3);
                // finishSession(std::stoi(socketAddr), MAC_LABEL, MAC_KEY);
                qDebug() << "Session finished sent";
            }
        }
        hideCurrentPageAndShowProvided(p_pageProduct);
    }
}

void page_tap_payment::on_pushButton_to_idle_clicked()
{
    if (exitConfirm())
    {
        hideCurrentPageAndShowProvided(p_page_help);
    }
}

void page_tap_payment::idlePaymentTimeout()
{
    hideCurrentPageAndShowProvided(p_page_idle);
}
void page_tap_payment::resetPaymentPage()
{
    ui->label_title->hide();

    stopPayTimers();
    transactionLogging = "";
    response = true;
    qDebug() << "Cancelled";
}

/* ----- Payment ----- */

bool page_tap_payment::tap_init()
{
    return true;
}
