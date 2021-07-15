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
    QPixmap background(":/light/5_pay_page.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /* HACK: transparent button*/
    ui->previousPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    //ui->payment_pass_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->mainPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

    ui->payment_bypass_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
   // ui->payment_pass_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
   // ui->payment_cancel_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

    // Setup static labels
    // TODO: Swap this when modular GUI ready
       // ui->order_tax_label->setText("Our Planet");
       // ui->order_tax_amount->setText("Priceless");
      //  ui->order_total_label->setText("Total");

    //ui->order_total_label->setText(" ");
   // ui->order_drink_amount->setText(" ");

    ui->payment_bypass_Button->setEnabled(false);

    //displayPaymentPending(false);

    // Setup Reciept
    //updateTotals(this->drinkDescription, this->drinkAmount, this->orderTotal);

    // Payment

    /* Create Timeout Interface: Wait for tap; message user; process tap */

        // Mutex
       // setpaymentProcess(false);

        // GUI Setup
        // ui->payment_processLabel->setText(TAP_READY_LABEL);
       // ui->payment_processLabel->setText(" ");

      //  ui->payment_processLabel->show();

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


    // XXX: Comment on/off for Bypassing payment testing
        payment=false;

        DbManager db(DB_PATH);
        for (int i = 1; i<db.getNumberOfProducts(); i++){
            if (db.getPaymentMethod(i) == "tap"){
                payment = true;
                ui->payment_bypass_Button->setEnabled(false);
            }else{
                 ui->payment_bypass_Button->setEnabled(true);
            }

        }
        if (payment){
            while (!paymentInit());
            }


}

void payPage::stopPayTimers(){
    //    readTimer->stop();
    qDebug() << "payPage: Stop Timers" << endl;
    if(paymentProgressTimer != nullptr) {
        qDebug() << "cancel payment progress Timer" << endl;
        paymentProgressTimer->stop();
    }

    if(declineTimer != nullptr) {
        qDebug() << "cancel decline Timer" << endl;
        declineTimer->stop();
    }

    if(idlePaymentTimer != nullptr) {
        qDebug() << "cancel idle payment Timer" << endl;
        idlePaymentTimer->stop();
    }

    if(paymentEndTimer != nullptr) {
        qDebug() << "cancel idle payment END Timer" << endl;
        paymentEndTimer->stop();
    }

    if(readTimer != nullptr) {
        qDebug() << "cancel readTimer" << endl;
        readTimer->stop();
    }
    qDebug() << "payPage: Stopped Timers" << endl;

}

/*
 * Page Tracking reference
 */
void payPage::setPage(paySelect *pageSizeSelect, dispensePage* pageDispense, idle* pageIdle, help* pageHelp)
{
    this->paySelectPage = pageSizeSelect;
    this->dispensingPage = pageDispense;
    this->idlePage = pageIdle;
    this->helpPage = pageHelp;
}

// TODO: Link style to sheet
void payPage::labelSetup(QLabel *label, int fontSize)
{
//    QFont font("Arial", fontSize, QFont::Bold);
//    label->setFont(font);
//    label->setStyleSheet("color: white");
//    label->setAlignment(Qt::AlignCenter);
}

void payPage::resizeEvent(QResizeEvent *event, char drinkSize){
    // FIXME: MAGIC NUMBER!!! UX410 Socket Auto Close time is 60 seconds so timer kills page GUI
    idlePaymentTimer->start(60000);

    int checkOption = idlePage->userDrinkOrder->getOption();

    QString bitmap_location;

    if (!payment){
        bitmap_location = ":/light/5_pay_page.png";
    }
    else if(checkOption > 0 && checkOption <= 9) {
        bitmap_location.append(":/light/5_pay_page_");
        bitmap_location.append(drinkSize);
        bitmap_location.append("_");
        bitmap_location.append(QString::number(idlePage->userDrinkOrder->getOption()));
        bitmap_location.append(".png");
        ui->order_drink_amount->setText("$" + QString::number(idlePage->userDrinkOrder->getPrice(), 'f', 2));
    } else {
        bitmap_location = ":/light/5_pay_page_l_1.png";
    }

    //qDebug() << "BITMAP: " << bitmap_location << endl;

    QPixmap background(bitmap_location);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);

    // background = background.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);
    //this->resize(this->geometry().width(), this->geometry().height());

        ui->order_total_amount->setText("$" + QString::number(idlePage->userDrinkOrder->getPrice(), 'f', 2));

    if (payment){
        readTimer->start(1000);
    }
}


// DTOR
payPage::~payPage()
{
    delete ui;
}

/* ----- GUI ----- */

// Labels and button for tapping payment
void payPage::displayPaymentPending(bool isVisible)
{
//    if(isVisible == false){
//        ui->payment_processLabel->hide();
//    } else {
//        ui->payment_processLabel->show();
//    }
}

// Navigation: Back to Drink Size Selection
void payPage::on_previousPage_Button_clicked()
{
    //Update Click DB
    DbManager db(DB_PATH);
    db.addPageClick("Pay Page -> Pay Select");

    qDebug() << "payPage: previous button" << endl;
    stopPayTimers();
    response = true;
    readTimer->stop();
    if (payment){
        cancelPayment();
    }
    paySelectPage->resizeEvent(paySelectResize);
    paySelectPage->showFullScreen();
    this->hide();
}

void payPage::on_payment_bypass_Button_clicked()
{

    //Update Click DB
    DbManager db(DB_PATH);
    db.addPageClick("Pay Page -> Dispense Page");
    db.addPageClick("TRANSACTION STARTED");

    qDebug() << "ByPass payment to Dispense" << endl;
    //    cancelPayment();
    stopPayTimers();
    //readTimer->stop();
    dispensingPage->showEvent(dispenseEvent);
    this->hide();
    dispensingPage->showFullScreen();
}

// Payment Processing Debug Button
//void payPage::on_payment_pass_Button_clicked()
//{
//    // TODO: Moneris Linkage here!
//    qDebug() << "Payment Pass button" << endl;

//    qDebug() << this->idlePage->userDrinkOrder->getOption();
//    qDebug() << this->idlePage->userDrinkOrder->getSize();


//    //    purchaseEnable = true;

//    if (!paymentConnected){
//        usleep(100000);
//        //mainPage->sendardCommand("Z");
//        //if(!com.init()){

//        // TODO: Replace with an ACK NACK to FSM for Dispenser slot...
//        //mainPage->checkard();

//        // Wait for a Drink Payment
//        // ui->payment_processLabel->show();
//        // labelSetup(ui->payment_processLabel, 50);
//        // ui->payment_processLabel->setText("You get a free drink");

//        // Lock Navigation
//        //ui->payment_pass_Button->hide();
//        ui->previousPage_Button->hide();

//        //paymentInit();

//        // Database log a failed payment
//        storePaymentEvent(db, QString("mpos failed"));
//        paymentProgressTimer->start();

//        dispensingPage->showFullScreen();
//        this->hide();

//    } else if(paymentConnected){

//        QFont warning;
//        warning.setBold(true);
//        warning.setFamily("Arial");
//        warning.setPointSize(30);

//      //  ui->payment_cancel_Button->setFont(warning);
//      //  ui->payment_cancel_Button->setText("CANCEL");
//      //  ui->payment_cancel_Button->show();

//        //ui->tapLabel->show(); //currently replaced with pay button 10.18
//        //ui->payButton->hide();
//        //ui->priceVolume1Button->setEnabled(false);
//        //ui->priceVolume2Button->setEnabled(false);

//        com.flushSerial();

//        // Set the price to send.
//        cout << "Setting price of packet: " << productSelectedPrice << endl;
//        pktToSend = paymentPacket.purchasePacket(productSelectedPrice);

//        purchaseEnable = true;


//        if (sendToUX410())
//        {
//            //            isReadyForTap = true;
//            //            waitForUX410();
//            pktResponded.clear();
//            timerEnabled = true;
//            cout << "From Payment Button" << endl;
//            readTimer->start(10);
//        }
//        else {
//            isReadyForTap = false;
//            std::cout<<"TIME OUT";
//        }
//    }

//    //    com.sendPacket(pktToSend, uint(pktToSend.size()));

//    //    std::cout<<paymentPacket.getSendPacket();

//    //    //read back what is responded

//    //    pktResponded = com.readForAck();

//    //    readPacket.packetReadFromUX(pktResponded);
//    //    pktResponded.clear();

//    //    if (readPacket.getAckOrNak() == communicationPacketField::ACK)
//    //    {
//    //        timerEnabled = true;
//    //        readTimer->start(10);
//    //    }
//}

//void payPage::on_payment_cancel_Button_clicked()
//{
//    qDebug()<< "Cancel Clicked" << endl;
//    cancelPayment();
//}

void payPage::updateTotals(string drinkDescription, string drinkAmount, string orderTotal)
{
//    this->drinkDescription = drinkDescription;
//    this->drinkAmount = drinkAmount;
//    this->orderTotal = orderTotal;

//    //    ui->order_drink_label->setText(this->drinkDescription.c_str());
//   // ui->order_drink_amount->setText(this->drinkAmount.c_str());
//    ui->order_total_amount->setText(this->orderTotal.c_str());

}

void payPage::on_mainPage_Button_clicked()
{
    qDebug() << "Main Button Page" << endl;

    //sleep(1);
    stopPayTimers();
    response = true;
    readTimer->stop();
    if (payment){
        cancelPayment();
    }
    this->hide();
    helpPage->showFullScreen();
}

/*Cancel any previous payment*/
void payPage::cancelPayment()
{


    //waitResponse = true;

    com.flushSerial();
    /*Cancel any previous payment*/
    //if(purchaseEnable){
        pktToSend = paymentPacket.purchaseCancelPacket();
        if (sendToUX410()){
            waitForUX410();
            // If this is not commented out, the GUI crashes! (But it was uncommented when I found it...)
            pktResponded.clear();
        }
        com.flushSerial();

        qDebug() << "Payment cancelled!" << endl;
   // }

}

void payPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

       // ui->payment_countdownLabel->setText(" ");

        paymentEndTimer = new QTimer(this);
        paymentEndTimer->setInterval(1000);
        connect(paymentEndTimer, SIGNAL(timeout()), this, SLOT(onTimeoutTick()));
        paymentEndTimer->start(1000);
        _paymentTimeoutSec = 30;

        ui->order_total_amount->setText("$" + QString::number(idlePage->userDrinkOrder->getPrice(), 'f', 2));
        this->ui->payment_countdownLabel->setText("");


  //  ui->payment_pass_Button->setEnabled(false);
  //  ui->payment_cancel_Button->setEnabled(false);

        if (payment){
            pktResponded = com.readForAck();
            readPacket.packetReadFromUX(pktResponded);
            pktResponded.clear();
            response = false;

            if (readPacket.getAckOrNak() == communicationPacketField::ACK)
            {
                timerEnabled = true;
                //readTimer->start(10);
            }
        }

}


// XXX: Remove this when interrupts and flow sensors work!
void payPage::onTimeoutTick(){
    if(-- _paymentTimeoutSec >= 0) {
        qDebug() << "payPage: Tick Down: " << _paymentTimeoutSec << endl;

//        _paymentTimeLabel.clear();
//        QString time = QString::number(_paymentTimeoutSec);

//        if(_paymentTimeoutSec >= 10) {
//            if(_paymentTimeoutSec % 2 == 0) {
//                _paymentTimeLabel.append("TAP NOW");
//                qDebug() << _paymentTimeLabel << endl;
//            } else {
//                _paymentTimeLabel.append(" ");
//                qDebug() << _paymentTimeLabel << endl;
//            }
//        } else {
//            if(_paymentTimeoutSec % 2 == 0) {
//                _paymentTimeLabel.append("PROCESSING.");
//                qDebug() << _paymentTimeLabel << endl;
//            } else {
//                _paymentTimeLabel.append("PROCESSING..");
//                qDebug() << _paymentTimeLabel << endl;
//            }
//            qDebug() << _paymentTimeLabel << endl;
//        }
//        this->ui->payment_countdownLabel->setText(_paymentTimeLabel);
    } else {
        qDebug() << "Timer Done!" << _paymentTimeoutSec << endl;

        //response = true;
        //stopPayTimers();
        //readTimer->stop();
        //cancelPayment();
        idlePaymentTimeout();
//        on_payment_bypass_Button_clicked();
//        //        paymentEndTimer->stop();
//        //        this->ui->payment_countdownLabel->setText("Finished!");
    }
}

bool payPage::setpaymentProcess(bool status)
{
    return (paymentProcessing = status);
}

void payPage::setProgressLabel(QLabel* label, int dot)
{
//    QString dotString = ".";
//    label->setText("Processing" + dotString.repeated(dot));
}

// Local storge for now.  Will need to refactor logger to do a nightly push to AWS
void payPage::storePaymentEvent(QSqlDatabase db, QString event)
{

    //    beverageData* curBev = mainPage->getBeverageData(optionSelected);
    //    db.event_log(machineID, QDateTime::currentDateTime(), optionSelected, event, curBev->getInventoryVolume());
}

void payPage::progressStatusLabel()
{
//    if (!paymentConnected){
//        //timer->start();
//        //pageUpdateTimer->start();
//        //mainPage->clearArd();
//        //sendCommand();
//        //pageNumber = 1;
//    }
//    else
//    {
//        if (paymentProcessing == true)
//        {
//            // Setup progress dots
//   //         ui->payment_processLabel->setText(TAP_BLANK_LABEL);

//            // Lock down page navigation
//    //        ui->payment_pass_Button->hide();
//     //       ui->previousPage_Button->hide();
//     //       ui->mainPage_Button->hide();
//            //labelSetup(ui->payment_processLabel, 50);
//            //setProgressLabel(ui->payment_processLabel, progressDots);
//            if (progressDots < 3){
//                progressDots++;
//            }
//            else {
//                progressDots = 1;
//            }
//            progressLoopCounter++;
//        }

//        if (progressLoopCounter == 3) {
//            paymentProgressTimer->stop();

//    //        ui->payment_pass_Button->hide();
//      //      ui->previousPage_Button->hide();

//            paymentProcessing = false;

//            if (approved){
//          //      ui->previousPage_Button->setEnabled(false);
//           //     ui->mainPage_Button->setEnabled(false);
//           //     ui->payment_processLabel->setText(TAP_APPROVED_LABEL);
//            }
//            else {
//          //      ui->payment_processLabel->setText(TAP_DECLINED_LABEL);
//          //      ui->payment_processLabel->show();
//                declineTimer->start(2000);
//            }
//        }
//    }
}

void payPage::declineTimer_start()
{
//   // ui->payment_processLabel->setText(TAP_AGAIN_LABEL);
//    declineCounter++;
//    if (declineCounter < 3){
//        this->on_payment_bypass_Button_clicked();
//    } else {
//     //   ui->payment_processLabel->setText(TAP_DECLINED_LABEL);
//    }
//    declineTimer->stop();
}

void payPage::idlePaymentTimeout() {
    stopPayTimers();
    response = true;
    readTimer->stop();
    if (payment){
        cancelPayment();
    }
    this->hide();
    idlePage->showFullScreen();
}

/* ----- Payment ----- */

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
        cout << "Wait for ACK counter: " << waitForAck << endl;
        com.sendPacket(pktToSend, uint(pktToSend.size()));
        std::cout<< "sendtoUX410 Electronic Card Reader: " << paymentPacket.getSendPacket() << endl;

        //read back what is responded
        pktResponded = com.readForAck();
        readPacket.packetReadFromUX(pktResponded);
        pktResponded.clear();
        waitForAck++;

        // if(isReadyForTap) {
        cout << "Waiting for TAP" << endl;
        if (readPacket.getAckOrNak() == communicationPacketField::ACK)
        {
            //cout << readPacket << endl;
            return true;
        }
        // }
        usleep(50000);

//        if(isInitCancelled) {
//            return true;
//        }

//        if(isInitBatched) {
//            return true;
//        }

//        if(isInitLogin) {
//            return true;
//        }

//        if(isInitMerchant) {
//            return true;
//        }

//        if(isInitAddress) {
//            return true;
//        }

//        if(isInitTerminalID) {
//            return true;
//        }

    }
    return false;
}

bool payPage::paymentInit()
{
    paymentConnected = com.init();

    while (!paymentConnected){
        paymentConnected = com.init();
        sleep(1);
    }

    //This is super shitty - there must be a better way to find out when the green light starts flashing on the UX420
    //sleep(35);

    /*Cancel any previous payment*/
    cout << "Sending Cancel payment packet..." << endl;
    pktToSend = paymentPacket.purchaseCancelPacket();
    if (sendToUX410()){
        cout << "Receiving Cancel payment response" << endl;
        isInitCancelled = true;
        waitForUX410();
        pktResponded.clear();
    } else {
        return false;
    }
    com.flushSerial();

    /*batch close packet to send*/
    cout << "Sending Batch close packet..." << endl;
    pktToSend = paymentPacket.batchClosePkt();
    if (sendToUX410()){
        cout << "Receiving Batch Close response" << endl;
        isInitBatched = true;
        waitForUX410();
        pktResponded.clear();
    } else {
        return false;
    }
    com.flushSerial();

    /*logon packet to send*/
    cout << "Sending Logon packet..." << endl;
    pktToSend = paymentPacket.logonPacket();
    if (sendToUX410()){
        cout << "Receiving Logon response" << endl;
        isInitLogin = true;
        waitForUX410();
        pktResponded.clear();
    } else {
        return false;
    }
    com.flushSerial();

    /*getConfiguration packet to send*/
    cout << "Sending Merchant Name query..." << endl;
    pktToSend = paymentPacket.ppPosGetConfigPkt(CONFIG_ID::MERCH_NAME);
    if (sendToUX410()){
        cout << "Receiving Merchant Name" << endl;
        waitForUX410();
        isInitMerchant = true;
        merchantName = paymentPktInfo.dataField(readPacket.getPacket().data).substr(2);
        cout << merchantName<<endl;
        pktResponded.clear();

    } else {
        return false;
    }
    com.flushSerial();

    /*getConfiguration packet to send*/
    cout << "Sending Merchant Address query..." << endl;
    pktToSend = paymentPacket.ppPosGetConfigPkt(CONFIG_ID::MERCH_ADDR);
    if (sendToUX410()){
        cout << "Receiving Merchant Address" << endl;
        waitForUX410();
        isInitAddress = true;
        merchantAddress = paymentPktInfo.dataField(readPacket.getPacket().data).substr(2);
        std::cout << merchantAddress<<endl;
        pktResponded.clear();

    } else {
        return false;
    }
    com.flushSerial();

    /*getConfiguration packet to send*/
    cout << "Sending PTID query..." << endl;
    pktToSend = paymentPacket.ppPosGetConfigPkt(CONFIG_ID::CON_TID);
    if (sendToUX410()){
        cout << "Receiving PTID" << endl;
        waitForUX410();
        isInitTerminalID = true;
        terminalID = paymentPktInfo.dataField(readPacket.getPacket().data).substr(2);
        std::cout << terminalID<<endl;
        pktResponded.clear();

    }else {
        return false;
    }
    com.flushSerial();

    return true;
}

bool payPage::waitForUX410()
{
    bool waitResponse = false;
    while (!waitResponse){
//        QCoreApplication::processEvents();

        if(pktResponded[0] != 0x02){
            pktResponded.clear();
            pktResponded = com.readPacket();
            usleep(10);
        }
        else {
            //  pktResponded = com.readPacket();
            readPacket.packetReadFromUX(pktResponded);
            std::cout << readPacket;
            com.sendAck();
            waitResponse = true;
        }
    }
    return waitResponse;
}

void payPage::readTimer_loop()
{
    qDebug() << "readingTimer_loop" << endl;
    cout << "start loop pktResponded: " << to_string(pktResponded[0]) << endl;

    //response = false;
    com.flushSerial();
    pktToSend = paymentPacket.purchasePacket("0.01");

    //this->ui->payment_countdownLabel->setText("TAP NOW");
    response = getResponse();

    if (sendToUX410()){
//        waitForUX410();

    while (!response){
        response = getResponse();

        QCoreApplication::processEvents();

    if(pktResponded[0] != 0x02){
        qDebug() << "Reading TAP Packet" << endl;

        //std::cout<< "ReadTimer Electronic Card Reader: " << paymentPacket.getSendPacket() << endl;

        // This breaks the polling loop!
        //        pktResponded = com.readPacket();
        //cout << "MISS: pktResponded: " << to_string(pktResponded[0]) << endl;
        pktResponded.clear();
        pktResponded = com.readPacket();
        usleep(10);
        response = getResponse();
        //        com.sendAck();
        //cout << "Polling Timer" << endl;
        //readTimer->start(1000);
    } else {
        if (!response){

        cout << "HIT: pktResponded: " << to_string(pktResponded[0]) << endl;

        this->ui->payment_countdownLabel->setText("Processing...");
        QCoreApplication::processEvents();

        qDebug() << "Check TAP Packet; Sending" << endl;
       // com.sendAck();
        readPacket.packetReadFromUX(pktResponded);
        std::cout << readPacket;
        com.sendAck();

        if (pktResponded[19] == 0x41){      // Host Response 41 = A "Approved"
            purchaseEnable = true;
            approved = true;
            cout << "Approval Packet 41" << endl;
            this->ui->payment_countdownLabel->setText("Approved");
            paymentPktInfo.transactionID(readPacket.getPacket().data);
            paymentPktInfo.makeReceipt(getTerminalID(), getMerchantName(), getMerchantAddress());
            QCoreApplication::processEvents();
            sleep(2);
            response = true;
            on_payment_bypass_Button_clicked();
            //            mainPage->getSurveyPage()->resetSurveyFilled(); //reset the coupon discount
        }
        else if(pktResponded[19] == 0x44){  // Host Response 44 = D "Declined"
            purchaseEnable = true;
            approved = false;
            cout << "Declined Packet 32" << endl;
            this->ui->payment_countdownLabel->setText("Declined");
            paymentPktInfo.transactionID(readPacket.getPacket().data);
            paymentPktInfo.makeReceipt(getTerminalID(), getMerchantName(), getMerchantAddress());
//            pktResponded.clear();
//            pktResponded = com.readPacket();
//            usleep(10);
            pktResponded.clear();
            QCoreApplication::processEvents();
            sleep(3);
            idlePaymentTimeout();
            //sleep(5);
//            on_mainPage_Button_clicked();
        }
        else {
            purchaseEnable = false;
            cout << "No Approval Packet!" << endl;
            this->ui->payment_countdownLabel->setText("Not Approved");
            paymentPktInfo.transactionID(readPacket.getPacket().data);
            paymentPktInfo.makeReceipt(getTerminalID(), getMerchantName(), getMerchantAddress());
            pktResponded.clear();
            QCoreApplication::processEvents();
            sleep(3);
            idlePaymentTimeout();
            //this->ui->payment_countdownLabel->setText("Hmm...");
            //sleep(5);
            //on_mainPage_Button_clicked();
        }
        }

//        readPacket.packetReadFromUX(pktResponded);
//        std::cout << readPacket;


//        if (purchaseEnable == true){
//            //once purchase successed create a receipt and store into database
//            paymentPktInfo.transactionID(readPacket.getPacket().data);
//            paymentPktInfo.makeReceipt(mainPage->getDatabase());


//            paymentProcessing = false;
//            progressLoopCounter = 0;
//        }
//        timerEnabled = false;
    }

//    if (timerEnabled == false){
//        qDebug() << "Timer has been disabled" << endl;
//        //        if (purchaseEnable == false){
//        //            pageNumber = 0;
//        //            mainPage->getSurveyPage()->resetSurveyFilled(); //reset the coupon discount
//        //        }
//        readTimer->stop();
//        purchaseEnable = false;
//    }

//    if (pktResponded.size() > 100)
//    {
//        if (progressLoopCounter == 0){
// //           ui->payment_processLabel->setText(TAP_PROCESSING_LABEL);
//            //            ui->payment_declineLabel->hide();
//            paymentProcessing = true;
//            paymentProgressTimer->start();
//        }
//    }
//        }
    }
    }
}
