//***************************************
//
// payment diplay window
// update scheme for to work with payment device
//
// created: 03-SEP-2019
// by: Jason Wang
//
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#include "payment.h"
#include "ui_payment.h"
#include "dispensewindow.h"
#include "mainwindow.h"
std::string merchantName;
std::string merchantAddress;
std::string terminalID;

extern int pageNumber;
extern bool enterPage;
extern int progressValue;

extern int optionSelected;
//static string Mid = "0030128909430";
//static string pedID = "U4383005";
//static string URL = "ipgt1.moneris.com:443";
const QString tapDeclined = "Card Declined";
const QString tapAgain = "Try Again";
const QFont newFont("Arial", 30, QFont::Bold);
extern QByteArray DispenseVolume;
bool approved = false;
bool paymentConnected = false;
extern QString machineID;
extern QByteArray tempValue;

payment::payment(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::payment)
{
    ui->setupUi(this);
    /*background setup*/
    QPixmap background(":/assets/V6/ev2_ui_assets_v6-07.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /*pushbutton setup*/{
        ui->goBackButton->setStyleSheet("border-image:url(:/assets/V6/ev2_ui_assets_v6-16.png); color: white;");

        ui->goBackButton->setFont(newFont);
        ui->goBackButton->setText("GO BACK");

        ui->payButton->setStyleSheet("border-image:url(:/assets/V6/ev2_ui_assets_v6-16.png); color: white;");
        ui->payButton->setFont(newFont);
        ui->payButton->setText("Pay");

        QFont priceFont("Arial", 20, QFont::Bold);
        ui->priceVolume1Button->setStyleSheet("border-image:url(:/assets/V6/ev2_ui_assets_v6-14.png); color: white;");
        ui->priceVolume1Button->setFont(priceFont);
        //ui->priceVolume1Button->setText("12 oz (355mL) \n $3.00");


        ui->priceVolume2Button->setStyleSheet("border-image:url(:/assets/V6/ev2_ui_assets_v6-15.png); color: white;");
        ui->priceVolume2Button->setFont(priceFont);
        //ui->priceVolume2Button->setText("16 oz (473mL) \n $4.00");
    }

    /*page label setup*/{
        labelSetup(ui->orLabel, 30);
        ui->orLabel->setText("or");
        labelSetup(ui->tapLabel, 30);
        ui->tapLabel->setText("TAP on the\n right to pay");
        ui->tapLabel->hide(); //currently replaced with pay button 10.18
    }

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

payment::~payment()
{
    //delete ui;
}

void payment::pageSetup(QString optionTitle, QByteArray imgData, bool survey)
{
    labelSetup(ui->productLabel, 60);
    ui->productLabel->setWordWrap(true);
    ui->productLabel->setText(optionTitle);

    priceSelect = 1;

//    /*Cancel any previous payment*/
//    pktToSend = paymentPacket.purchaseCancelPacket();
//    com.sendPacket(pktToSend, uint(pktToSend.size()));
//    pktResponded.clear();
//    com.flushSerial();

//    /*Cancel any previous payment*/
//    if(purchaseEnable){
//        pktToSend = paymentPacket.purchaseCancelPacket();
//        if (sendToUX410()){
//            waitForUX410();
//            pktResponded.clear();
//        }
//        com.flushSerial();
//    }

    surveyBool = survey;
    /*image use for the payment page*/
    QPixmap logo;
    logo.loadFromData(imgData);
    logo = logo.scaled(ui->imageLabel->size(), Qt::IgnoreAspectRatio); //logo = logo.scaled(ui->imageLabel->size());
    ui->imageLabel->setPixmap(logo);

    ui->priceVolume1Button->setEnabled(true);
    ui->priceVolume2Button->setEnabled(true);


    ui->declineLabel->hide();
    purchaseEnable= false;
    paymentProcessing = false;
    declineCounter = 0;
    counter = 0;
    pageNumber = 4;

    /*unit price label and tax*/
    ui->priceLabel->hide();

    ui->priceVolume1Button->setText("12 oz (355mL) \n $ " + mainPage->getPrice_1());
    ui->priceVolume2Button->setText("16 oz (473mL) \n $ " + mainPage->getPrice_2());

    ui->orLabel->show();
    if (paymentProcessing == false){
        ui->tapLabel->hide(); //currently replaced with pay button 10.18
        ui->goBackButton->setFont(newFont);
        ui->goBackButton->setText("GO BACK");
        ui->goBackButton->show();
        ui->orLabel->show();
        ui->payButton->show();
    }
    else {
        ui->tapLabel->show();
    }

    ui->goBackButton->setEnabled(true);
    ui->goBackButton->show();

    ui->processLabel->hide();

    QString price = mainPage->getPrice_1();
    labelSetup(ui->priceLabel, 35);

    double priceDouble;
    {
        priceDouble = price.toDouble();
        //ui->priceLabel->setText("$" + price + "\n +$" + QString::number(tax, 'f', 2) + " tax" );
    }

    /*overall amount label*/
    labelSetup(ui->totalLabel, 60);
    double total = priceDouble;// + tax;
    setProductPrice(QString::number(total, 'f', 2)); //to preset the product price
    ui->totalLabel->setText("TOTAL: $" + QString::number(total, 'f', 2));
    productSelectedPrice = price.toUtf8().constData();

}

void payment::labelSetup(QLabel *label, int fontSize)
{
    QFont font("Arial", fontSize, QFont::Bold);
    label->setFont(font);
    label->setStyleSheet("color: white");
    label->setAlignment(Qt::AlignCenter);
}

void payment::setPages(MainWindow *main, DispenseWindow *dispense)
{
    this->mainPage = main;
    this->dispensePage = dispense;
}

bool payment::setpaymentProcess(bool status)
{
    return (paymentProcessing = status);
}

void payment::setProgressLabel(QLabel* label, int dot)
{
    QString dotString = ".";
    label->setText("Processing" + dotString.repeated(dot));
}

void payment::setProductPrice(QString price)
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

void payment::storeEvent(database_logger db, QString event)
{
    beverageData* curBev = mainPage->getBeverageData(optionSelected);
    db.event_log(machineID, QDateTime::currentDateTime(), optionSelected, event, curBev->getInventoryVolume());

}

void payment::stayAliveLogon()
{
    com.flushSerial();
    /*logon packet to send*/
    pktToSend = paymentPacket.ppPosGetConfigPkt(StatusType::GetLanStatus);
    if (sendToUX410()){
        waitForUX410();
    }

    pktResponded.clear();
}

void payment::batchClose()
{
    com.flushSerial();
    /*logon packet to send*/
    pktToSend = paymentPacket.batchClosePkt();
    if (sendToUX410()){
        waitForUX410();
    }

    pktResponded.clear();
}

bool payment::sendToUX410()
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

bool payment::paymentInit()
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

bool payment::waitForUX410()
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

void payment::showEvent(QShowEvent *event)
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

void payment::paintEvent(QPaintEvent *p)
{
    QWidget::paintEvent(p);
}

void payment::declineTimer_start()
{
    ui->declineLabel->setText(tapAgain);
    declineCounter++;
    if (declineCounter<3){
        on_payButton_clicked();
    } else {
        pageNumber = 0;
    }
    declineTimer->stop();
}

void payment::on_goBackButton_clicked()
{
    //if (purchaseEnable)
    {
        //ui->goBackButton->setEnabled(false);
        readTimer->stop();
        /*Cancel any previous payment*/
        pktToSend = paymentPacket.purchaseCancelPacket();
        if (sendToUX410()){
            waitForUX410();
            pktResponded.clear();
        } else {
            //return false;
        }
        com.flushSerial();


//        com.flushSerial();
//        pktToSend = paymentPacket.purchaseCancelPacket();

//        if (sendToUX410())
//        {
//            timerEnabled = true;
//            readTimer->start(10);
//        }
//        ui->goBackButton->setEnabled(false);
//        readTimer->stop();
//        /*Cancel any previous payment*/
//        pktToSend = paymentPacket.purchaseCancelPacket();
//        com.sendPacket(pktToSend, uint(pktToSend.size()));
//        pktResponded.clear();
//        com.flushSerial();

//        /*Cancel any previous payment*/
//        pktToSend = paymentPacket.purchaseCancelPacket();
//        if (sendToUX410()){
//            waitForUX410();
//            pktResponded.clear();
//        }
//        com.flushSerial();

//        ui->goBackButton->setEnabled(true);
        //mainPage->showFullScreen();
        pageNumber =  0;

        //return;
    }
//    else{
//        ui->goBackButton->setEnabled(true);
//        //mainPage->showFullScreen();
//        pageNumber =  0;
//        //return;
//    }
}

void payment::progressStatusLabel()
{
    //timer->stop();
    if (!paymentConnected){

//        mainPage->clearArd();

//        if(optionSelected == 1 && priceSelect == 1)
//        {
//            mainPage->sendardCommand("a");
//        }
//        else if(optionSelected == 1 && priceSelect == 2)
//        {
//            mainPage->sendardCommand("b");
//        }
//        else if(optionSelected == 2 && priceSelect == 1)
//        {
//            mainPage->sendardCommand("c");
//        }
//        else if(optionSelected == 2 && priceSelect == 2)
//        {
//            mainPage->sendardCommand("d");
//        }
        sendCommand();
        pageNumber = 1;
    }
    else
    {
        if (paymentProcessing == true)
        {
            ui->orLabel->hide();
            ui->tapLabel->hide();
            ui->goBackButton->hide();
            ui->payButton->hide();
            ui->processLabel->show();
            labelSetup(ui->processLabel, 50);
            setProgressLabel(ui->processLabel, progressDots);
            if (progressDots < 3){
                progressDots++;
            }
            else {
                progressDots = 1;
            }
            counter++;
        }

        if (counter == 3) {
            timer->stop();

            ui->orLabel->hide();
            ui->goBackButton->hide();
            ui->processLabel->hide();
            paymentProcessing = false;
            if (approved){
                mainPage->clearArd();
                sendCommand();
//                if(optionSelected == 1 && priceSelect == 1)
//                {
//                    mainPage->sendardCommand("a");
//                }
//                else if(optionSelected == 1 && priceSelect == 2)
//                {
//                    mainPage->sendardCommand("b");
//                }
//                else if(optionSelected == 2 && priceSelect == 1)
//                {
//                    mainPage->sendardCommand("c");
//                }
//                else if(optionSelected == 2 && priceSelect == 2)
//                {
//                    mainPage->sendardCommand("d");
//                }

                pageNumber = 1;

            }
            else {
                ui->declineLabel->setText(tapDeclined);
                ui->declineLabel->show();
                declineTimer->start(2000);
            }
        }
    }
}

void payment::updatePageNumber()
{
    pageUpdateTimer->stop();
    this->showFullScreen();
    if (enterPage){
        //usleep(200000);
        mainPage->hide();
        dispensePage->hide();
        enterPage = false;
    }
    if (pageNumber == 1)
    {
        dispensePage->showFullScreen();
        timer->stop();
        paymentProcessing = false;
        return;
    }
    else if(pageNumber == 0){
        mainPage->showFullScreen();
        timer->stop();
        paymentProcessing = false;
        return;
    } else {
        pageUpdateTimer->start();
    }
}

void payment::readTimer_loop()
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

void payment::goBack()
{
    pageNumber = 0;
    goBackTimer->stop();

    if (purchaseEnable){
        ui->goBackButton->setEnabled(false);
        readTimer->stop();
        com.flushSerial();
        pktToSend = paymentPacket.purchaseCancelPacket();

        if (sendToUX410())
        {
            timerEnabled = true;
            readTimer->start(10);
        }

    }
}

void payment::on_payButton_clicked()
{
    purchaseEnable = true;
    if (!paymentConnected){
        //mainPage->sendardCommand("Z");
        //if(!com.init()){
        mainPage->checkard();
        ui->processLabel->show();
        labelSetup(ui->processLabel, 50);
        ui->processLabel->setText("You get a free drink");

        ui->payButton->hide();
        ui->orLabel->hide();
        ui->goBackButton->hide();

        //paymentInit();
        storeEvent(mainPage->getDatabase(), QString("mpos failed"));
        timer->start();
        //}
    } else {
        //purchaseEnable = true;
        ui->goBackButton->setFont(newFont);
        ui->goBackButton->setText("CANCEL");
        ui->goBackButton->show();
        ui->tapLabel->show(); //currently replaced with pay button 10.18
        ui->payButton->hide();

        ui->priceVolume1Button->setEnabled(false);
        ui->priceVolume2Button->setEnabled(false);

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

}

void payment::on_priceVolume1Button_clicked()
{
    ui->priceVolume1Button->setStyleSheet("border-image:url(:/assets/V6/ev2_ui_assets_v6-14.png); color: white;");
    ui->priceVolume2Button->setStyleSheet("border-image:url(:/assets/V6/ev2_ui_assets_v6-15.png); color: white;");

    priceSelect = 1;

    QString price = mainPage->getPrice_1();
    labelSetup(ui->priceLabel, 35);
    double priceDouble;
    double tax;
//    if(surveyBool)
//    {
//        priceDouble = price.toDouble();
//        tax = priceDouble*0;//*0.05;
//        //ui->priceLabel->setText(" $" + price + "\n-$1.00" + "\n+$" + QString::number(tax, 'f', 2) + " tax" );
//    }
//    else
    {
        priceDouble = price.toDouble();
        tax = priceDouble*0;//*0.05;
        ui->priceLabel->setText("$" + price + "\n +$" + QString::number(tax, 'f', 2) + " tax" );
    }

    /*overall amount label*/
    labelSetup(ui->totalLabel, 60);
    double total = priceDouble;// + tax;
    setProductPrice(QString::number(total, 'f', 2)); //to preset the product price
    ui->totalLabel->setText("TOTAL: $" + QString::number(total, 'f', 2));
    productSelectedPrice = price.toUtf8().constData();

    storeEvent(mainPage->getDatabase(), QString(price + " Clicked"));
}

void payment::on_priceVolume2Button_clicked()
{
    ui->priceVolume2Button->setStyleSheet("border-image:url(:/assets/V6/ev2_ui_assets_v6-14.png); color: white;");
    ui->priceVolume1Button->setStyleSheet("border-image:url(:/assets/V6/ev2_ui_assets_v6-15.png); color: white;");

    priceSelect = 2;
    QString price = mainPage->getPrice_2();
    labelSetup(ui->priceLabel, 35);
    double priceDouble;
    double tax;
//    if(surveyBool)
//    {
//        priceDouble = price.toDouble();
//        tax = priceDouble*0;//*0.05;
//        //ui->priceLabel->setText(" $" + price + "\n-$1.00" + "\n+$" + QString::number(tax, 'f', 2) + " tax" );
//    }
//    else
    {
        priceDouble = price.toDouble();
        tax = priceDouble*0;//*0.05;
        ui->priceLabel->setText("$" + price + "\n +$" + QString::number(tax, 'f', 2) + " tax" );
    }

    ui->priceLabel->setText("$" + price + "\n +$" + QString::number(tax, 'f', 2) + " tax" );

    /*overall amount label*/
    labelSetup(ui->totalLabel, 60);
    double total = priceDouble;// + tax;
    setProductPrice(QString::number(total, 'f', 2)); //to preset the product price
    ui->totalLabel->setText("TOTAL: $" + QString::number(total, 'f', 2));
    productSelectedPrice = price.toUtf8().constData();
    storeEvent(mainPage->getDatabase(),  QString(price + " Clicked"));
}

int payment::getPriceSelect()
{
    return priceSelect;
}

void payment::sendCommand()
{

    switch (optionSelected){

    case 1:
    {
        if(priceSelect == 1)
            mainPage->sendardCommand("a");
        else if(priceSelect == 2)
            mainPage->sendardCommand("b");
        break;
    }
    case 2:
    {
        if(priceSelect == 1)
            mainPage->sendardCommand("c");
        else if(priceSelect == 2)
            mainPage->sendardCommand("d");
        break;
    }
    case 3:
    {
        if(priceSelect == 1)
            mainPage->sendardCommand("e");
        else if(priceSelect == 2)
            mainPage->sendardCommand("f");
        break;
    }
    case 4:
    {
        if(priceSelect == 1)
            mainPage->sendardCommand("g");
        else if(priceSelect == 2)
            mainPage->sendardCommand("h");
        break;
    }
    case 5:
    {
        if(priceSelect == 1)
            mainPage->sendardCommand("e");
        else if(priceSelect == 2)
            mainPage->sendardCommand("f");
        break;
    }
    case 6:
    {
        if(priceSelect == 1)
            mainPage->sendardCommand("e");
        else if(priceSelect == 2)
            mainPage->sendardCommand("f");
        break;
    }
    case 7:
    {
        if(priceSelect == 1)
            mainPage->sendardCommand("g");
        else if(priceSelect == 2)
            mainPage->sendardCommand("h");
        break;
    }
    case 8:
    {
        if(priceSelect == 1)
            mainPage->sendardCommand("g");
        else if(priceSelect == 2)
            mainPage->sendardCommand("h");

        break;
    }
    case 9:
    {
        if(priceSelect == 1)
            mainPage->sendardCommand("g");
        else if(priceSelect == 2)
            mainPage->sendardCommand("h");

        break;
    }

    }
}
