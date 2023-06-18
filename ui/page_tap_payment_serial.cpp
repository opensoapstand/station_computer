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
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "page_tap_payment_serial.h"
#include "ui_page_tap_payment_serial.h"

#include "page_product.h"
#include "page_dispenser.h"
#include "page_idle.h"

// CTOR
page_tap_payment_serial::page_tap_payment_serial(QWidget *parent) : QWidget(parent),
                                                                    ui(new Ui::page_tap_payment_serial)
{
    // Fullscreen background setup
    ui->setupUi(this);
    ui->previousPage_Button->setStyleSheet(
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
    ui->previousPage_Button->setText("<- Back");
    ui->mainPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    ui->previousPage_Button->setText("<- Back");

    // Payment Tap Ready
    readTimer = new QTimer(this);
    connect(readTimer, SIGNAL(timeout()), this, SLOT(readTimer_loop()));

    // Idle Payment reset
    idlePaymentTimer = new QTimer(this);
    connect(idlePaymentTimer, SIGNAL(timeout()), this, SLOT(idlePaymentTimeout()));
}

void page_tap_payment_serial::stopPayTimers()
{

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
}

/*
 * Page Tracking reference
 */
void page_tap_payment_serial::setPage(page_idle *pageIdle, page_product *pageSizeSelect, page_dispenser *page_dispenser)
{
    tmpCounter = 0;
    this->p_page_product = pageSizeSelect;
    this->p_page_dispense = page_dispenser;
    this->p_page_idle = pageIdle;
}

// DTOR
page_tap_payment_serial::~page_tap_payment_serial()
{
    delete ui;
}

void page_tap_payment_serial::on_payment_bypass_Button_clicked()
{
    proceed_to_dispense();
}

void page_tap_payment_serial::proceed_to_dispense()
{
    stopPayTimers();
    // p_page_dispense->showEvent(dispenseEvent);
    p_page_dispense->showFullScreen();
    this->hide();
}

/*Cancel any previous payment*/
void page_tap_payment_serial::cancelPayment()
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

QString page_tap_payment_serial::getPaymentMethod()
{
    return "tapSerial";
}

void page_tap_payment_serial::resizeEvent(QResizeEvent *event)
{
}

void page_tap_payment_serial::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Serial Payment >>>>>>>>>";
    QWidget::showEvent(event);

    state_payment = s_serial_init;

    QString payment_method = getPaymentMethod();
    if (payment_method == "tapSerial")
    {

        qDebug() << "Prepare tap order";
        paymentConnected = com.page_init();

        while (!paymentConnected)
        {
            paymentConnected = com.page_init();
        }
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
        // ui->payment_bypass_Button->setEnabled(false);
        p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_PAY);
        ui->productLabel->hide();
        ui->order_drink_amount->hide();
    }
    else
    {
        qDebug() << "Payment method not valid " << payment_method;
    }
}

bool page_tap_payment_serial::setpaymentProcess(bool status)
{
    return (paymentProcessing = status);
}

bool page_tap_payment_serial::exitConfirm()
{
    if (state_payment == s_serial_payment_processing || state_payment == s_serial_payment_done)
    {
        // ARE YOU SURE YOU WANT TO EXIT?
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::FramelessWindowHint); // do not show messagebox header with program name

        msgBox.setText("<p align=center><br><br>Cancel transaction and exit page?<br><br>It can take up to 30 seconds for dispensing to start after a payment is completed. <br></p>");
        msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px; font-weight: bold; font-style: normal;  font-family: 'Montserrat';} QPushButton{font-size: 24px; min-width: 300px; min-height: 300px;}");
        // msgBox.setStyleSheet(styleSheet);

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

void page_tap_payment_serial::on_previousPage_Button_clicked()
{
    if (exitConfirm())
    {
        hideCurrentPageAndShowProvided(p_page_product);
    }
}

void page_tap_payment_serial::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{

    resetPaymentPage();
    p_page_idle->pageTransition(this, pageToShow);
}

void page_tap_payment_serial::on_mainPage_Button_clicked()
{
    if (exitConfirm())
    {
        this->hide();
    }
}

void page_tap_payment_serial::idlePaymentTimeout()
{
    resetPaymentPage();
}
void page_tap_payment_serial::resetPaymentPage()
{

    stopPayTimers();
    response = true;
    readTimer->stop();
    if (getPaymentMethod() == "tapSerial")
    {
        cancelPayment();
    }
}

bool page_tap_payment_serial::tap_serial_initiate()
{
    paymentConnected = com.page_init();
    while (!paymentConnected)
    {
        paymentConnected = com.page_init();
    }
    sleep(35);
    cout << "_----_-----__------_-----";
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
    pktToSend = paymentPacket.ppPosGetConfigPkt(CONFIG_ID::MERCH_ADDR);
    if (sendToUX410())
    {
        cout << "Receiving Merchant Address" << endl;
        waitForUX410();
        isInitAddress = true;
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
        std::cout << terminalID << endl;
        pktResponded.clear();
    }
    else
    {
        return false;
    }
    com.flushSerial();
    tapSetupStarted = true;
    return true;
}

bool page_tap_payment_serial::sendToUX410()
{
    int waitForAck = 0;
    while (waitForAck < 3)
    {
        cout << "Wait for ACK counter: " << waitForAck << endl;
        com.sendPacket(pktToSend, uint(pktToSend.size()));
        std::cout << " \n sendtoUX410 Electronic Card Reader: " << paymentPacket.getSendPacket() << endl;
        sleep(1);
        // read back what is responded
        pktResponded = com.readForAck();
        readPacket.packetReadFromUX(pktResponded);
        pktResponded.clear();
        waitForAck++;
        if (readPacket.getAckOrNak() == communicationPacketField::ACK)
        {
            return true;
        }
    }
    return false;
}
bool page_tap_payment_serial::waitForUX410()
{
    bool waitResponse = false;
    while (!waitResponse)
    {
        usleep(100);
        QCoreApplication::processEvents();
        cout << readPacket << endl;
        if (pktResponded[0] != 0x02)
        {
            pktResponded.clear();
            pktResponded = com.readPacket();
            sleep(1);
        }
        else
        {
            readPacket.packetReadFromUX(pktResponded);
            std::cout << readPacket;
            com.sendAck();
            waitResponse = true;
            sleep(1);
        }
    }
    return waitResponse;
}

void page_tap_payment_serial::readTimer_loop()
{
    // pktToSend = paymentPacket.purchasePacket((QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2)).QString::toStdString());
    pktToSend = paymentPacket.purchasePacket("1.00");
    response = getResponse();
    qDebug() << "Packet sent for payment";
    if (sendToUX410())
    {
        waitForUX410();
        while (!response)
        {
            response = getResponse();
            QCoreApplication::processEvents();
            if (pktResponded[0] != 0x02)
            {
                pktResponded.clear();
                pktResponded = com.readPacket();
                response = getResponse();
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
                    { // Host Response 41 = A "Approved"
                        purchaseEnable = true;
                        approved = true;
                        cout << "Approval Packet 41" << endl;
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
                        // this->ui->payment_countdownLabel->setText("Not Approved");
                        paymentPktInfo.transactionID(readPacket.getPacket().data);
                        paymentPktInfo.makeReceipt(getTerminalID(), getMerchantName(), getMerchantAddress());
                        pktResponded.clear();
                        QCoreApplication::processEvents();
                        sleep(3);
                        idlePaymentTimeout();
                    }
                    else
                    {
                        qDebug() << "In else condition";
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
