//***************************************
//
// paypage.cpp
// GUI class while machine is processing
// payment.
//
// Coordinates User input from payment select
// class then communcates results to dispensepage.
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "paypage.h"
#include "ui_paypage.h"

#include "payselect.h"
#include "dispensepage.h"
#include "idle.h"

// CTOR
payPage::payPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::payPage)
{
    // Fullscreen background setup
    ui->setupUi(this);
    QPixmap background(":/light/5_pay_page_blank.jpg");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /* HACK: transparent button*/
    ui->previousPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->payment_pass_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->mainPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

    // Setup static labels
    ui->order_tax_label->setText("Our Planet");
    ui->order_tax_amount->setText("Priceless");
    ui->order_total_label->setText("Total");

    displayPaymentPending(false);

    // Setup Reciept
    updateTotals(this->drinkDescription, this->drinkAmount, this->orderTotal);

    // Payment

    cancelPayment();

    /* Create Timeout Interface: Wait for tap; message user; process tap*/
    {
        // Timer and Slot Setup
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(progressStatusLabel()));
        timer->setInterval(500);

        declineTimer = new QTimer(this);
        connect(declineTimer, SIGNAL(timeout()), this, SLOT(declineTimer_start()));

        pageUpdateTimer = new QTimer(this);
        connect(pageUpdateTimer, SIGNAL(timeout()), this, SLOT(updatePageNumber()));
        pageUpdateTimer->setInterval(10);
        //pageUpdateTimer->start();

        // Mutex
        setpaymentProcess(false);
        ui->payment_processLabel->hide();
        ui->payment_declineLabel->setText(TAP_AGAIN);
        labelSetup(ui->payment_declineLabel, 40);

        //pageSetup("Kombucha", ":/assets/kombucha.png", 5.95);

        readTimer = new QTimer(this);
        connect (readTimer, SIGNAL(timeout()), this, SLOT(readTimer_loop()));

        goBackTimer = new QTimer(this);
        connect(goBackTimer, SIGNAL(timeout()), this, SLOT(goBack()));
        goBackTimer->start(60000);
    }

    paymentInit();
}
/*
 * Page Tracking reference
 */
void payPage::setPage(paySelect *pageSizeSelect, dispensePage* pageDispense, idle* pageIdle)
{
    this->paySelectPage = pageSizeSelect;
    this->dispensingPage = pageDispense;
    this->idlePage = pageIdle;
}

// TODO: Link style to sheet
void payPage::labelSetup(QLabel *label, int fontSize)
{
    QFont font("Arial", fontSize, QFont::Bold);
    label->setFont(font);
    label->setStyleSheet("color: white");
    label->setAlignment(Qt::AlignCenter);
}

// DTOR
payPage::~payPage()
{
    delete ui;
}

// Labels and button for tapping payment
void payPage::displayPaymentPending(bool isVisible)
{
    if(isVisible = false){
        ui->payment_declineLabel->hide();
        ui->payment_processLabel->hide();

    } else {

    }
}

void payPage::on_previousPage_Button_clicked()
{
    paySelectPage->showFullScreen();
    this->hide();
}

void payPage::on_passPayment_Button_clicked()
{
    // TODO: Moneris Linkage here!
    qDebug() << this->idlePage->userDrinkOrder->getOption();
    qDebug() << this->idlePage->userDrinkOrder->getSize();
    qDebug() << this->idlePage->userDrinkOrder->getPrice();

    purchaseEnable = true;

    if (!paymentConnected){
        //mainPage->sendardCommand("Z");
        //if(!com.init()){

        // TODO: Replace with an ACK NACK to FSM for Dispenser slot...
//        mainPage->checkard();

        // Wait for a Drink Payment
        ui->payment_processLabel->show();
        labelSetup(ui->payment_processLabel, 50);
        ui->payment_processLabel->setText("You get a free drink");

        // Lock Navigation
        ui->payment_pass_Button->hide();
        ui->previousPage_Button->hide();

//        paymentInit();

        // Database log a failed payment
        storePaymentEvent(db, QString("mpos failed"));
        timer->start();
        //}
    } else {
        purchaseEnable = true;
        QFont warning;
        warning.setBold(true);
        warning.setFamily("Arial");
        warning.setPointSize(30);

        ui->payment_cancel_Button->setFont(warning);
        ui->payment_cancel_Button->setText("CANCEL");
        ui->payment_cancel_Button->show();


//        ui->tapLabel->show(); //currently replaced with pay button 10.18
//        ui->payButton->hide();

//        ui->priceVolume1Button->setEnabled(false);
//        ui->priceVolume2Button->setEnabled(false);

        com.flushSerial();

        pktToSend = paymentPacket.purchasePacket(productSelectedPrice);

        if (sendToUX410())
        {
            timerEnabled = true;
            readTimer->start(10);
        }
        else {
            //std::cout<<"Jason: I wanna die!! :(";
        }
    }

    dispensingPage->showFullScreen();
    this->hide();
}

void payPage::updateTotals(string drinkDescription, string drinkAmount, string orderTotal)
{
    this->drinkDescription = drinkDescription;
    this->drinkAmount = drinkAmount;
    this->orderTotal = orderTotal;

    ui->order_drink_label->setText(this->drinkDescription.c_str());
    ui->order_drink_amount->setText(this->drinkAmount.c_str());
    ui->order_total_amount->setText(this->orderTotal.c_str());
}

void payPage::on_mainPage_Button_clicked()
{
    this->hide();
    idlePage->showFullScreen();
}

/*Cancel any previous payment*/
void payPage::cancelPayment()
{
        /*Cancel any previous payment*/
        if(purchaseEnable){
            pktToSend = paymentPacket.purchaseCancelPacket();
            if (sendToUX410()){
                waitForUX410();
                pktResponded.clear();
            }
            com.flushSerial();
        }
}

// Payment Section based on DF001 Prototype

// XXX: Old implementation of Dispensing.  ARDUINO SHOULD NOT BE THIS TIGHTLY COUPLED TO UI!!!
//void payPage::showEvent(QShowEvent *event)
//{
//    enterPage = true;
//    progressValue = 0;
//    tempValue = "0";
//    DispenseVolume = "0";

//    mainPage->sendardCommand("Z");

//    ui->priceVolume1Button->setStyleSheet("border-image:url(:/assets/V6/ev2_ui_assets_v6-14.png); color: white;");
//    ui->priceVolume2Button->setStyleSheet("border-image:url(:/assets/V6/ev2_ui_assets_v6-15.png); color: white;");

//    pageUpdateTimer->start();
//    goBackTimer->start();//if no one uses the machine go back to first page
//    QWidget::showEvent(event);
//}

// HACK: This seems to do nothing...Could mask for GUI thread pausing?
void payPage::paintEvent(QPaintEvent *p)
{
    QWidget::paintEvent(p);
}


bool payPage::setpaymentProcess(bool status)
{
    return (paymentProcessing = status);
}

void payPage::setProgressLabel(QLabel* label, int dot)
{
    QString dotString = ".";
    label->setText("Processing" + dotString.repeated(dot));
}

// Local storge for now.  Will need to refactor logger to do a nightly push to AWS
void payPage::storePaymentEvent(QSqlDatabase db, QString event)
{

//    beverageData* curBev = mainPage->getBeverageData(optionSelected);
//    db.event_log(machineID, QDateTime::currentDateTime(), optionSelected, event, curBev->getInventoryVolume());
}

void payPage::stayAliveLogon()
{
    com.flushSerial();
    /*logon packet to send*/
    pktToSend = paymentPacket.ppPosGetConfigPkt(StatusType::GetLanStatus);
    if (sendToUX410()){
        waitForUX410();
    }

    pktResponded.clear();
}

void payPage::batchClose()
{
    com.flushSerial();
    /*logon packet to send*/
    pktToSend = paymentPacket.batchClosePkt();
    if (sendToUX410()){
        waitForUX410();
    }

    pktResponded.clear();
}

bool payPage::sendToUX410()
{
    int waitForAck = 0;
    while (waitForAck < 3){
        com.sendPacket(pktToSend, uint(pktToSend.size()));

        //std::cout<<paymentPacket.getSendPacket();

        //read back what is responded
        pktResponded = com.readForAck();
        readPacket.packetReadFromUX(pktResponded);
        pktResponded.clear();
        waitForAck++;
        if (readPacket.getAckOrNak() == communicationPacketField::ACK)
        {
            return true;
        }
        usleep(500000);
    }
    return false;
}

bool payPage::paymentInit()
{
    paymentConnected = com.init();

    /*Cancel any previous payment*/
    pktToSend = paymentPacket.purchaseCancelPacket();
    if (sendToUX410()){
        waitForUX410();
        pktResponded.clear();
    } else {
        return false;
    }
    com.flushSerial();

    /*batch close packet to send*/
    pktToSend = paymentPacket.batchClosePkt();
    if (sendToUX410()){
        waitForUX410();
        pktResponded.clear();
    } else {
        return false;
    }
    com.flushSerial();

    /*logon packet to send*/
    pktToSend = paymentPacket.logonPacket();
    if (sendToUX410()){
        waitForUX410();
        pktResponded.clear();
    } else {
        return false;
    }
    com.flushSerial();

    /*getConfiguration packet to send*/
    pktToSend = paymentPacket.ppPosGetConfigPkt(CONFIG_ID::MERCH_NAME);
    if (sendToUX410()){
        waitForUX410();
        //merchantName = paymentPktInfo.dataField(readPacket.getPacket().data).substr(2);
        //std::cout << merchantName<<endl;
        pktResponded.clear();

    } else {
        return false;
    }
    com.flushSerial();

    /*getConfiguration packet to send*/
    pktToSend = paymentPacket.ppPosGetConfigPkt(CONFIG_ID::MERCH_ADDR);
    if (sendToUX410()){
        waitForUX410();
        //merchantAddress = paymentPktInfo.dataField(readPacket.getPacket().data).substr(2);
        //std::cout << merchantAddress<<endl;
        pktResponded.clear();

    } else {
        return false;
    }
    com.flushSerial();

    /*getConfiguration packet to send*/
    pktToSend = paymentPacket.ppPosGetConfigPkt(CONFIG_ID::CON_TID);
    if (sendToUX410()){
        waitForUX410();
        terminalID = paymentPktInfo.dataField(readPacket.getPacket().data).substr(2);
        //std::cout << terminalID<<endl;
        pktResponded.clear();

    } else {
        return false;
    }

    return true;
}

bool payPage::waitForUX410()
{
    bool waitResponse = false;
    while (!waitResponse){

        if(pktResponded[0] != 0x02){
            pktResponded.clear();
            pktResponded = com.readPacket();
            usleep(10);
        }
        else {
            //  pktResponded = com.readPacket();
            readPacket.packetReadFromUX(pktResponded);
            //std::cout << readPacket;
            com.sendAck();
            waitResponse = true;
        }
    }
    return waitResponse;
}

void payPage::readTimer_loop()
{
    if(pktResponded[0] != 0x02){
        pktResponded = com.readPacket();
        com.sendAck();

        readTimer->start(10);
    }
    else {
        com.sendAck();

        readTimer->stop();

        if (pktResponded[10] == 0x31){
            purchaseEnable = true;
            approved = true;
            mainPage->getSurveyPage()->resetSurveyFilled(); //reset the coupon discount
        }
        else if(pktResponded[10] == 0x32){
            purchaseEnable = true;
            approved = true; //should be false
        }
        else {
            purchaseEnable = false;
        }

        readPacket.packetReadFromUX(pktResponded);
        //std::cout << readPacket;

        if (purchaseEnable == true){//once purchase successed create a receipt and store into database

            paymentPktInfo.transactionID(readPacket.getPacket().data);
            paymentPktInfo.makeReceipt(mainPage->getDatabase());

            paymentProcessing = false;
            counter = 0;
        }
        timerEnabled = false;
    }

    if (timerEnabled == false){
//        if (purchaseEnable == false){
//            pageNumber = 0;
//            mainPage->getSurveyPage()->resetSurveyFilled(); //reset the coupon discount
//        }
//        purchaseEnable = false;
    }

    if (pktResponded.size() > 100)
    {
        if (counter == 0){
            ui->declineLabel->hide();
            paymentProcessing = true;
            timer->start();
        }
    }
}
