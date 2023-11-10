//***************************************
//
// page_payment_tap_serial.cpp
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

#include "page_payment_tap_serial.h"
#include "ui_page_payment_tap_serial.h"

#include "page_product.h"
#include "page_dispenser.h"
#include "page_idle.h"

// CTOR
page_payment_tap_serial::page_payment_tap_serial(QWidget *parent) : QWidget(parent),
                                                                    ui(new Ui::page_payment_tap_serial)
{
    // Fullscreen background setup
    ui->setupUi(this);
    // Payment Tap Ready
    readTimer = new QTimer(this);
    connect(readTimer, SIGNAL(timeout()), this, SLOT(readTimer_loop()));

    ui->pushButton_payment_bypass->setEnabled(false);
    ui->label_title->hide();
    ui->order_total_amount->hide();
    statusbarLayout = new QVBoxLayout(this);
}


void page_payment_tap_serial::stopPayTimers()
{
    if (readTimer && readTimer->isActive()) {
        qDebug() << "Cancel readTimer" << endl;
        readTimer->stop();
    }
}
/*
 * Page Tracking reference
 */
void page_payment_tap_serial::setPage(page_product *p_page_product, page_error_wifi *pageWifiError, page_dispenser *page_dispenser, page_idle *pageIdle, page_help *pageHelp, statusbar *p_statusbar)
{
    tmpCounter = 0;
    this->p_page_product = p_page_product;
    this->p_page_wifi_error = pageWifiError;
    this->p_page_dispense = page_dispenser;
    this->p_page_idle = pageIdle;
    this->p_page_help = pageHelp;
    this->p_statusbar = p_statusbar;
}

// DTOR
page_payment_tap_serial::~page_payment_tap_serial()
{
    delete ui;
}

void page_payment_tap_serial::on_pushButton_payment_bypass_clicked()
{
    hideCurrentPageAndShowProvided(p_page_dispense,false);
}

/*Cancel any previous payment*/
void page_payment_tap_serial::cancelPayment()
{
    com.flushSerial();
    /*Cancel any previous payment*/
    pktToSend = paymentPacket.purchaseCancelPacket();
    p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_CANCEL);
    p_page_idle->thisMachine->setTemplateTextWithIdentifierToObject(ui->animated_Label, "cancel");
    
    if (sendToUX410())
    {
        waitForUX410();
        // pktResponded.clear();
    }
    com.flushSerial();
    qDebug() << "Cancel payment";
}

// Navigation: Back to Drink Size Selection
void page_payment_tap_serial::on_pushButton_previous_page_clicked()
{
    qDebug() << "In previous page button" << endl;

    if (exitConfirm())
    {
        hideCurrentPageAndShowProvided(p_page_product,true);
    }
}

void page_payment_tap_serial::resizeEvent(QResizeEvent *event)
{
}

void page_payment_tap_serial::showEvent(QShowEvent *event)
{
    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    statusbarLayout->addWidget(p_statusbar);            // Only one instance can be shown. So, has to be added/removed per page.
    statusbarLayout->setContentsMargins(0, 1874, 0, 0); // int left, int top, int right, int bottom);
    
    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget
    
    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_TAP_PAYMENT_SERIAL_CSS);

    ui->pushButton_previous_page->setStyleSheet(styleSheet);

    ui->pushButton_to_idle->setProperty("class", "invisible_button");
    ui->pushButton_payment_bypass->setProperty("class", "invisible_button");
    ui->pushButton_to_idle->setStyleSheet(styleSheet);
    ui->pushButton_payment_bypass->setStyleSheet(styleSheet);
    ui->animated_Label->setProperty("class", "animated_Label");
    ui->animated_Label->setStyleSheet(styleSheet);

    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_previous_page);

    ui->pushButton_payment_bypass->setEnabled(false);

    state_payment = s_serial_init;
    ui->pushButton_payment_bypass->setEnabled(false);

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
    p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_PAY);
    ui->productLabel->hide();
    ui->order_drink_amount->hide();
}

bool page_payment_tap_serial::setpaymentProcess(bool status)
{
    return (paymentProcessing = status);
}


bool page_payment_tap_serial::exitConfirm()
{
    qDebug() << "In exit confirm";
    if (state_payment == s_serial_payment_processing || state_payment == s_serial_payment_done)
    {
        // ARE YOU SURE YOU WANT TO EXIT?
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::FramelessWindowHint); // do not show messagebox header with program name
        p_page_idle->thisMachine->addCssClassToObject(&msgBox, "msgBoxbutton msgBox", PAGE_TAP_PAYMENT_CSS);
        QString searchString = this->objectName() + "->msgBox_cancel";
        p_page_idle->thisMachine->setTextToObject(&msgBox, p_page_idle->thisMachine->getTemplateText(searchString));

        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();
        bool success;
        switch (ret)
        {
        case QMessageBox::Yes:
        {
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
        return true;
    }
}

void page_payment_tap_serial::on_pushButton_to_idle_clicked()
{
    if (exitConfirm())
    {
        hideCurrentPageAndShowProvided(p_page_help,true);
    }
}

void page_payment_tap_serial::hideCurrentPageAndShowProvided(QWidget *pageToShow, bool cancelTapPayment)
{
    resetPaymentPage(cancelTapPayment);
    statusbarLayout->removeWidget(p_statusbar); // Only one instance can be shown. So, has to be added/removed per page.
    p_page_idle->thisMachine->pageTransition(this, pageToShow);
}

void page_payment_tap_serial::idlePaymentTimeout()
{
    resetPaymentPage(true);
}

void page_payment_tap_serial::resetPaymentPage(bool cancelTapPayment)
{
    stopPayTimers();
    if(cancelTapPayment){
        readTimer->stop();
        cancelPayment();
    }
}

bool page_payment_tap_serial::tap_serial_initiate()
{
    while (!paymentConnected)
    {
        paymentConnected = com.page_init();
        sleep(1);
    }
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
    // else
    // {
    //     return false;
    // }
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
    // else
    // {
    //     return false;
    // }
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
    // else
    // {
    //     return false;
    // }
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
    // else
    // {
    //     return false;
    // }
    com.flushSerial();
    /*Cancel any previous payment*/
    pktToSend = paymentPacket.purchaseCancelPacket();

    if (sendToUX410())
    {
        waitForUX410();
        pktResponded.clear();
    }
    com.flushSerial();
    qDebug() << "Cancel payment";
    tapSetupStarted = true;
    qDebug() << "tap init started " << tapSetupStarted;
    com.closeCom();
 
    return true;
}

bool page_payment_tap_serial::sendToUX410()
{
    qDebug() << "Send packet"<< endl;
    int waitForAck = 0;
    while (waitForAck < 3)
    {
        com.sendPacket(pktToSend, uint(pktToSend.size()));
        std::cout << " \n sendtoUX410 Electronic Card Reader: " << paymentPacket.getSendPacket() << endl;
        usleep(100);
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
bool page_payment_tap_serial::waitForUX410()
{
    bool waitResponse = false;
    while (!waitResponse)
    {
        qDebug() << "In wait for packet" << endl;
        sleep(1);
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

void page_payment_tap_serial::readTimer_loop()
{   
    double originalPrice = p_page_idle->thisMachine->selectedProduct->getBasePrice();
    if (p_page_idle->thisMachine->selectedProduct->getSizeAsChar() == 'c')
    {
        originalPrice = p_page_idle->thisMachine->selectedProduct->getPriceCustom();
    }
    pktToSend = paymentPacket.purchasePacket((QString::number(p_page_idle->thisMachine->getPriceWithDiscount(originalPrice), 'f', 2)).QString::toStdString());
    // response = getResponse();
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
                        p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_PAY_SUCCESS);

                        purchaseEnable = true;
                        approved = true;
                        cout << "Approval Packet 41" << endl;
                        paymentPktInfo.transactionID(readPacket.getPacket().data);
                        paymentPktInfo.makeReceipt(getTerminalID(), getMerchantName(), getMerchantAddress());
                        response = true;
                        hideCurrentPageAndShowProvided(p_page_dispense,false);
                    }
                    else if (pktResponded[19] == 0x44)
                    { // Host Response 44 = D "Declined"
                        p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_TAP_PAY_FAIL);

                        purchaseEnable = true;
                        approved = false;
                        cout << "Declined Packet 44" << endl;
                        // this->ui->payment_countdownLabel->setText("Declined");
                        paymentPktInfo.transactionID(readPacket.getPacket().data);
                        paymentPktInfo.makeReceipt(getTerminalID(), getMerchantName(), getMerchantAddress());
                        pktResponded.clear();
                        QCoreApplication::processEvents();
                        sleep(1);
                        pktResponded.clear();
                        pktResponded = com.readPacket();
                        usleep(100);
                        response = getResponse();
                        readTimer->start(10);
                        // idlePaymentTimeout();
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
                        sleep(1);
                        idlePaymentTimeout();
                    }
                    else
                    {
                        pktResponded.clear();
                        pktResponded = com.readPacket();
                        usleep(100);
                        response = getResponse();
                        readTimer->start(10);
                    }
                }
            }
        }
    }
}