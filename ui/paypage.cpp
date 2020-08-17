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

extern int optionSelected;
//static string Mid = "0030128909430";
//static string pedID = "U4383005";
//static string URL = "ipgt1.moneris.com:443";

// TODO: Put information into XML

static string Mid = "0030410835171"; //0030128909430 0030410835171
static string pedID = "U4383002"; //U4383017 //U4383144 //U4383005
static string URL = "ipg1.moneris.com:443";
const QString tapDeclined = "Card Declined";
const QString tapAgain = "Try Again";
const QFont newFont("Arial", 30, QFont::Bold);
extern QByteArray DispenseVolume;
bool approved = false;
bool paymentConnected = false;
extern QString machineID;
extern QByteArray tempValue;

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
    ui->passPayment_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->mainPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

    // Setup static labels
    ui->order_tax_label->setText("Our Planet");
    ui->order_tax_amount->setText("Priceless");
    ui->order_total_label->setText("Total");

    // Setup Reciept
    updateTotals(this->drinkDescription, this->drinkAmount, this->orderTotal);

    // Payment
    /*timer and slots setup*/{
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(progressStatusLabel()));
        timer->setInterval(500);

        declineTimer = new QTimer(this);
        connect(declineTimer, SIGNAL(timeout()), this, SLOT(declineTimer_start()));

        pageUpdateTimer = new QTimer(this);
        connect(pageUpdateTimer, SIGNAL(timeout()), this, SLOT(updatePageNumber()));
        pageUpdateTimer->setInterval(10);
        //pageUpdateTimer->start();

        setpaymentProcess(false);
        ui->processLabel->hide();
        ui->declineLabel->setText(tapAgain);
        labelSetup(ui->declineLabel, 40);

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

// DTOR
payPage::~payPage()
{
    delete ui;
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

// Payment Section based on DF001 Prototype
void payPage::showEvent(QShowEvent *event)
{
    enterPage = true;
    progressValue = 0;
    tempValue = "0";
    DispenseVolume = "0";

    mainPage->sendardCommand("Z");

    ui->priceVolume1Button->setStyleSheet("border-image:url(:/assets/V6/ev2_ui_assets_v6-14.png); color: white;");
    ui->priceVolume2Button->setStyleSheet("border-image:url(:/assets/V6/ev2_ui_assets_v6-15.png); color: white;");

    pageUpdateTimer->start();
    goBackTimer->start();//if no one uses the machine go back to first page
    QWidget::showEvent(event);
}

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

void payPage::setProductPrice(QString price)
{
    if(1 == optionSelected)
    {
        productSelectedPrice = price.toUtf8().constData(); //QString convert to string
    }
    else if (2 == optionSelected)
    {
        productSelectedPrice = price.toUtf8().constData();
    }
}

void payPage::storeEvent(database_logger db, QString event)
{
    beverageData* curBev = mainPage->getBeverageData(optionSelected);
    db.event_log(machineID, QDateTime::currentDateTime(), optionSelected, event, curBev->getInventoryVolume());

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
