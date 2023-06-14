//***************************************
//
// page_tap_payment_serial.cpp
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

#include "page_tap_payment_serial.h"
#include "ui_page_tap_payment_serial.h"

#include "page_product.h"
#include "page_dispenser.h"
#include "page_idle.h"
#include "payment/commands.h"
#include "payment/setup_Tap.h"

extern QString transactionLogging;
// CTOR

page_tap_payment_serial::page_tap_payment_serial(QWidget *parent) : QWidget(parent),
                                                                    ui(new Ui::page_tap_payment_serial)
{
    // Fullscreen background setup
    ui->setupUi(this);
    qDebug() << "Payment page" << endl;

    ui->previousPage_Button->setText("<- Back");
    ui->pushButton_payment_bypass->setEnabled(false);

    // Payment Declined
    declineTimer = new QTimer(this);
    connect(declineTimer, SIGNAL(timeout()), this, SLOT(declineTimer_start()));

    // Idle Payment reset
    idlePaymentTimer = new QTimer(this);
    connect(idlePaymentTimer, SIGNAL(timeout()), this, SLOT(idlePaymentTimeout()));

    ui->pushButton_payment_bypass->setEnabled(false);
    ui->label_title->hide();

    DbManager db(DB_PATH);
    if (db.getPaymentMethod(1) == "tapSerial")
    {
        tap_payment = true;
    }
    db.closeDb();
    if (tap_payment)
    {

        // bring out of constructor time please.
        // while (!tap_init());
    }
}

void page_tap_payment_serial::stopPayTimers()
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
void page_tap_payment_serial::setPage(page_product *p_page_product, page_error_wifi *pageWifiError, page_dispenser *page_dispenser, page_idle *pageIdle, page_help *pageHelp)
{
    tmpCounter = 0;
    this->p_page_product = p_page_product;
    this->p_page_wifi_error = pageWifiError;
    this->p_page_dispense = page_dispenser;
    this->p_page_idle = pageIdle;
    this->p_page_help = pageHelp;
}

// DTOR
page_tap_payment_serial::~page_tap_payment_serial()
{
    delete ui;
}

/* ----- GUI ----- */

// Labels and button for tapping payment
void page_tap_payment_serial::displayPaymentPending(bool isVisible)
{
}

void page_tap_payment_serial::on_pushButton_payment_bypass_clicked()
{
    hideCurrentPageAndShowProvided(p_page_dispense);
}

void page_tap_payment_serial::updateTotals(string drinkDescription, string drinkAmount, string orderTotal)
{
}

void page_tap_payment_serial::resizeEvent(QResizeEvent *event)
{
}

void page_tap_payment_serial::showEvent(QShowEvent *event)
{

    QString styleSheet = p_page_idle->getCSS(PAGE_TAP_PAYMENT_SERIAL_CSS);
    
    ui->previousPage_Button->setStyleSheet(styleSheet);
    ui->mainPage_Button->setProperty("class", "transparent");
    ui->pushButton_payment_bypass->setProperty("class", "transparent");
    ui->mainPage_Button->setStyleSheet(styleSheet);
    ui->pushButton_payment_bypass->setStyleSheet(styleSheet);


    qDebug() << "<<<<<<< Page Enter: Tap Serial Payment >>>>>>>>>";

    QWidget::showEvent(event);
    state_payment = s_init;
    if (p_page_idle->selectedProduct->getPaymentMethod() == "tapSerial")
    {
        qDebug() << "Tap initiated";
    }
    paymentEndTimer = new QTimer(this);
    paymentEndTimer->setInterval(1000);
    connect(paymentEndTimer, SIGNAL(timeout()), this, SLOT(onTimeoutTick()));
    paymentEndTimer->start(1000);

    qDebug() << "Prepare tap order";
    pktResponded = com.readForAck();
    readPacket.packetReadFromUX(pktResponded);
    pktResponded.clear();
    response = false;
    qDebug() << "Acknowledgement received";
    if (readPacket.getAckOrNak() == communicationPacketField::ACK)
    {
        timerEnabled = true;
    }
    readTimer->start(10);
}

void page_tap_payment_serial::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{

    resetPaymentPage();
    p_page_idle->pageTransition(this, pageToShow);
}

bool page_tap_payment_serial::setpaymentProcess(bool status)
{
    return (paymentProcessing = status);
}

void page_tap_payment_serial::setProgressLabel(QLabel *label, int dot)
{
}

// Local storge for now.  Will need to refactor logger to do a nightly push to AWS
void page_tap_payment_serial::storePaymentEvent(QSqlDatabase db, QString event)
{
}

void page_tap_payment_serial::declineTimer_start()
{
}

bool page_tap_payment_serial::exitConfirm()
{
    qDebug() << "In exit confirm";
    if (state_tap_payment == s_tap_payment_processing || state_tap_payment == s_tap_payment_done)
    {
        // ARE YOU SURE YOU WANT TO EXIT?
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::FramelessWindowHint); // do not show messagebox header with program name

        msgBox.setText("<p align=center><br><br>Cancel transaction and exit page?<br><br>It can take up to 30 seconds for dispensing to start after a payment is completed. <br></p>");
        msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px; font-weight: bold; font-style: normal;  font-family: 'Montserrat';} QPushButton{font-size: 24px; min-width: 300px; min-height: 300px;}");
        //msgBox.setStyleSheet(styleSheet);

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
void page_tap_payment_serial::on_previousPage_Button_clicked()
{
    qDebug() << "In previous page button" << endl;
}

void page_tap_payment_serial::on_mainPage_Button_clicked()
{
    if (exitConfirm())
    {
        hideCurrentPageAndShowProvided(p_page_help);
    }
}

void page_tap_payment_serial::idlePaymentTimeout()
{
    hideCurrentPageAndShowProvided(p_page_idle);
}
void page_tap_payment_serial::resetPaymentPage()
{
    ui->label_title->hide();

    stopPayTimers();
    transactionLogging = "";
    response = true;
    qDebug() << "Cancelled";
}

/* ----- Payment ----- */
void page_tap_payment_serial::stayAliveLogon()
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

void page_tap_payment_serial::batchClose()
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

bool page_tap_payment_serial::sendToUX410()
{
    int waitForAck = 0;
    while (waitForAck < 3)
    {
        std::cout << "Wait for ACK counter: " << waitForAck << endl;
        std::cout << "sendtoUX410 Electronic Card Reader: " << paymentPacket.getSendPacket() << endl;
        // std::cout << "Packet to send size :" << pktToSend.size() << "\n";
        com.sendPacket(pktToSend, uint(pktToSend.size()));
        sleep(1);
        // read back what is responded
        pktResponded = com.readForAck();
        readPacket.packetReadFromUX(pktResponded);
        pktResponded.clear();
        waitForAck++;

        // if(isReadyForTap) {
        if (readPacket.getAckOrNak() == communicationPacketField::ACK)
        {
            return true;
        }
    }
    return false;
}

bool page_tap_payment_serial::tap_init()
{
    paymentConnected = com.page_init();
    int paymentConnectionFailed = 0;
    while (!paymentConnected)
    {

        paymentConnected = com.page_init();
        sleep(1);
        paymentConnectionFailed += 1;
        if (paymentConnectionFailed == 50)
        {
            // DbManager db3(DB_PATH);
            // db3.setPaymentToQR();
            // QString payment_method = db3.getPaymentMethod(1);
            // qDebug() << "Payment method" << payment_method;
            // db3.closeDb();
            for (int slotIndex = 0; slotIndex < SLOT_COUNT; slotIndex++)
            {
                p_page_idle->products[slotIndex].setPaymentMethod("qr")
            }
            qDebug() << "Change the db to QR";
            exit(1);
        }
    }

    // sleep(35);

    cout << "_----_-----__------_-----";

    // stayAliveLogon();

    /*Cancel any previous payment*/
    // cout << "Sending Cancel payment packet..." << endl;
    // pktToSend = paymentPacket.purchaseCancelPacket();
    // if (sendToUX410())
    // {
    //     cout << "Receiving Cancel payment response" << endl;
    //     isInitCancelled = true;
    //     waitForUX410();
    //     pktResponded.clear();
    // }
    // else
    // {
    //     return false;
    // }
    // com.flushSerial();
    // cout << "-----------------------------------------------" << endl;

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
    // sleep(10);
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

bool page_tap_payment_serial::waitForUX410()
{
    bool waitResponse = false;
    while (!waitResponse)
    {
        usleep(100);
        QCoreApplication::processEvents();
        if (pktResponded[0] != 0x02)
        {
            pktResponded.clear();
            pktResponded = com.readPacket();
            sleep(1);
        }
        else
        {
            readPacket.packetReadFromUX(pktResponded);
            com.sendAck();
            waitResponse = true;
            sleep(1);
        }
    }
    return waitResponse;
}

void page_tap_payment_serial::readTimer_loop()
{
    cout << p_page_idle->currentProductOrder->getSelectedPriceCorrected() << endl;
    pktToSend = paymentPacket.purchasePacket((QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2)).QString::toStdString());
    cout << "to PAY: " << ((QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2)).QString::toStdString());
    response = getResponse();
    // qDebug() << "In read timer";

    if (sendToUX410())
    {
        // qDebug() << "After send";
        waitForUX410();
        // qDebug() << "After wait";
        while (!response)
        {
            response = getResponse();
            QCoreApplication::processEvents();
            if (pktResponded[0] != 0x02)
            {
                pktResponded.clear();
                pktResponded = com.readPacket();
                response = getResponse();
                cout << "Polling Timer" << endl;
                readTimer->start(1000);
            }
            else if (pktResponded[10] == 0x33)
            {
                pktResponded.clear();
                pktResponded = com.readPacket();
                usleep(100);
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
                    {
                        // Host Response 41 = A "Approved"
                        purchaseEnable = true;
                        approved = true;
                        cout << "Approval Packet 41" << endl;
                        this->ui->payment_countdownLabel->setText("Approved");
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
                        this->ui->payment_countdownLabel->setText("Declined");
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
                        readTimer->start(10);
                    }
                }
            }
        }
    }
}
