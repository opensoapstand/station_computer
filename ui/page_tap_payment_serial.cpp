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

// CTOR
page_tap_payment_serial::page_tap_payment_serial(QWidget *parent) : QWidget(parent),
                                              ui(new Ui::page_tap_payment_serial)
{
    // Fullscreen background setup
    ui->setupUi(this);
    qDebug() << "Payment page" << endl;
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
    ui->payment_bypass_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

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
  
    // while (!tap_init());

    ui->payment_bypass_Button->setEnabled(false);
    ui->title_Label->hide();
}

void page_tap_payment_serial::stopPayTimers()
{
    //    readTimer->stop();
    //    qDebug() << "page_tap_payment_serial: Stop Timers" << endl;
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

  
    //    qDebug() << "page_tap_payment_serial: Stopped Timers" << endl;
}

/*
 * Page Tracking reference
 */
void page_tap_payment_serial::setPage(page_idle *pageIdle,pageProduct *pageSizeSelect, page_dispenser *page_dispenser)
{
    tmpCounter = 0;
    this->p_pageProduct = pageSizeSelect;
    this->p_page_dispense = page_dispenser;
    this->p_page_idle = pageIdle;
    // this->helpPage = pageHelp;
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

void page_tap_payment_serial::updateTotals(string drinkDescription, string drinkAmount, string orderTotal)
{
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
        ui->payment_bypass_Button->setEnabled(false);
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

void page_tap_payment_serial::setProgressLabel(QLabel *label, int dot)
{
}

// Local storge for now.  Will need to refactor logger to do a nightly push to AWS
void page_tap_payment_serial::storePaymentEvent(QSqlDatabase db, QString event)
{
}

void page_tap_payment_serial::progressStatusLabel()
{
}

void page_tap_payment_serial::declineTimer_start()
{
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
void page_tap_payment_serial::on_previousPage_Button_clicked()
{
    if (exitConfirm())
    {
        hideCurrentPageAndShowProvided(p_pageProduct);
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
        // helpPage->showFullScreen();
        this->hide();
    }
}

void page_tap_payment_serial::idlePaymentTimeout()
{
    resetPaymentPage();
    // p_page_idle->showFullScreen();
    // this->hide();
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
bool page_tap_payment_serial::waitForUX410()
{
    bool waitResponse = false;
    while (!waitResponse)
    {
        sleep(1);
               QCoreApplication::processEvents();
        cout << readPacket << endl;
        if (pktResponded[0] != 0x02)
        {
            pktResponded.clear();
            pktResponded = com.readPacket();
            usleep(10);
        }
        else
        {
             pktResponded = com.readPacket();
            readPacket.packetReadFromUX(pktResponded);
            std::cout << readPacket;
            com.sendAck();
            waitResponse = true;
        }
    }
    return waitResponse;
}

// bool page_tap_payment_serial::tap_init()
// {
//     paymentConnected = com.page_init();

//     while (!paymentConnected)
//     {
//         paymentConnected = com.page_init();
//     }
//     sleep(35);

//     // This is super shitty - there must be a better way to find out when the green light starts flashing on the UX420 but it was 35

//     cout << "_----_-----__------_-----";
   
//     // stayAliveLogon();
//     //   cout << "Sending Device Reset packet..." << endl;
//     // pktToSend = paymentPacket.resetDevice();
//     // if (sendToUX410())
//     // {
//     //     cout << "Receiving Device Reset response" << endl;
//     //     isInitBatched = true;
//     //     waitForUX410();
//     //     pktResponded.clear();
//     // }
//     // else
//     // {
//     //     return false;
//     // }
//     // com.flushSerial();
//     // cout << "-----------------------------------------------" << endl;
    
    
//     /*Cancel any previous payment*/
//     // cout << "Sending Cancel payment packet..." << endl;
//     // pktToSend = paymentPacket.purchaseCancelPacket();
//     // if (sendToUX410())
//     // {
//     //     cout << "Receiving Cancel payment response" << endl;
//     //     isInitCancelled = true;
//     //     waitForUX410();
//     //     pktResponded.clear();
//     // }
//     // else
//     // {
//     //     return false;
//     // }
//     // com.flushSerial();
//     // cout << "-----------------------------------------------" << endl;


//     /*batch close packet to send*/
//     // cout << "Sending Batch close packet..." << endl;
//     // pktToSend = paymentPacket.batchClosePkt();
//     // if (sendToUX410())
//     // {
//     //     cout << "Receiving Batch Close response" << endl;
//     //     isInitBatched = true;
//     //     waitForUX410();
//     //     pktResponded.clear();
//     // }
//     // else
//     // {
//     //     return false;
//     // }
//     // com.flushSerial();
//     // cout << "-----------------------------------------------" << endl;
    
//     /*logon packet to send*/
//     cout << "Sending Logon packet..." << endl;
//     pktToSend = paymentPacket.logonPacket();
//     if (sendToUX410())
//     {
//         cout << "Receiving Logon response" << endl;
//         isInitLogin = true;
//         waitForUX410();
//         pktResponded.clear();
//     }
//     else
//     {
//         return false;
//     }
//     com.flushSerial();
//     cout << "-----------------------------------------------" << endl;

//     /*getConfiguration packet to send*/
//     cout << "Sending Merchant Name query..." << endl;
//     pktToSend = paymentPacket.ppPosGetConfigPkt(CONFIG_ID::MERCH_NAME);
//     if (sendToUX410())
//     {
//         cout << "Receiving Merchant Name" << endl;
//         waitForUX410();
//         isInitMerchant = true;
//         merchantName = paymentPktInfo.dataField(readPacket.getPacket().data).substr(2);
//         cout << merchantName << endl;
//         pktResponded.clear();
//     }
//     else
//     {
//         return false;
//     }
//     com.flushSerial();
//     cout << "-----------------------------------------------" << endl;

//     /*getConfiguration packet to send*/
//     cout << "Sending Merchant Address query..." << endl;
//     pktToSend = paymentPacket.ppPosGetConfigPkt(CONFIG_ID::MERCH_ADDR);
//     if (sendToUX410())
//     {
//         cout << "Receiving Merchant Address" << endl;
//         waitForUX410();
//         isInitAddress = true;
//         // merchantAddress = paymentPktInfo.dataField(readPacket.getPacket().data).substr(2);
//         merchantAddress = paymentPktInfo.dataField(readPacket.getPacket().data);

//         std::cout << merchantAddress << endl;
//         pktResponded.clear();
//     }
//     else
//     {
//         return false;
//     }
//     com.flushSerial();
//     cout << "-----------------------------------------------" << endl;

//     /*getConfiguration packet to send*/
//     cout << "Sending PTID query..." << endl;
//     pktToSend = paymentPacket.ppPosGetConfigPkt(CONFIG_ID::CON_TID);
//     if (sendToUX410())
//     {
//         cout << "Receiving PTID" << endl;
//         waitForUX410();
//         isInitTerminalID = true;
//         terminalID = paymentPktInfo.dataField(readPacket.getPacket().data).substr(2);
//         std::cout << terminalID << endl;
//         pktResponded.clear();
//     }
//     else
//     {
//         return false;
//     }
//     com.flushSerial();

//     return true;
// }



void page_tap_payment_serial::readTimer_loop()
{
    // pktToSend = paymentPacket.purchasePacket((QString::number(p_page_idle->currentProductOrder->getSelectedPriceCorrected(), 'f', 2)).QString::toStdString());
    pktToSend = paymentPacket.purchasePacket("1.00");
    response = getResponse();

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
                usleep(10);
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