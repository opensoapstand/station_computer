//***************************************
//
// dispensepage.cpp
// GUI class to dispense drink.
// Coordinates User input to payment class
// then communcates results to FSM IP_thread
//
// Allows navigation to paymentpage and
// thankyou page
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "dispensepage.h"
#include "ui_dispensepage.h"
#include "includefiles.h"
#include "idle.h"
#include "thankyoupage.h"

// CTOR
dispensePage::dispensePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dispensePage)
{
    ui->setupUi(this);

    ui->finish_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

    dispenseIdleTimer = new QTimer(this);
    dispenseIdleTimer->setInterval(1000);
    connect(dispenseIdleTimer, SIGNAL(timeout()), this, SLOT(onDispenseIdleTick()));

}

/*
 * Page Tracking reference to Payment page and completed payment
 */
void dispensePage::setPage(payPage *pagePayment, thankYouPage* pageThankYou, idle* pageIdle)
{
    this->thanksPage = pageThankYou;
    this->paymentPage = pagePayment;
    this->idlePage = pageIdle;
}

// DTOR
dispensePage::~dispensePage()
{
    delete ui;
}

void dispensePage::showEvent(QShowEvent *event)
{
    QPixmap background(":/light/5_dispense_page_before.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    ui->widget->hide();
    ui->filler->hide();

    // FIXME: this is a hack for size changes...
    QString command = QString::number(this->idlePage->userDrinkOrder->getOption());

    if(idlePage->userDrinkOrder->getSizeOption() == SMALL_DRINK){
        command.append('s');

    } else {
        command.append('l');
    }

    volumeDispensed=0;

    this->idlePage->dfUtility->msg = command;

    // Networking
    idlePage->dfUtility->m_IsSendingFSM = true;
    QWidget::showEvent(event);
    idlePage->dfUtility->m_fsmMsg = SEND_DRINK;

    idlePage->dfUtility->send_to_FSM();
    idlePage->dfUtility->m_IsSendingFSM = false;
    ui->finish_Button->setEnabled(false);

    if(nullptr == dispenseIdleTimer){
        dispenseIdleTimer = new QTimer(this);
        dispenseIdleTimer->setInterval(1000);
        connect(dispenseIdleTimer, SIGNAL(timeout()), this, SLOT(onDispenseIdleTick()));
    }

    dispenseIdleTimer->start(1000);
    _dispenseIdleTimeoutSec = 30;

}

bool dispensePage::sendToUX410()
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

        cout << "Waiting for TAP" << endl;
        cout << readPacket << endl;
        if (readPacket.getAckOrNak() == communicationPacketField::ACK)
        {
//            cout << readPacket << endl;
            return true;
        }
        usleep(50000);
    }
    return false;
}

bool dispensePage::waitForUX410()
{
    bool waitResponse = false;
    while (!waitResponse){
//        QCoreApplication::processEvents();
      // cout << readPacket << endl;
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

/*
 * Page Tracking reference to Payment page and completed payment
 */
void dispensePage::on_finish_Button_clicked()
{
    DbManager db(DB_PATH);

    if (volumeDispensed == 0 && (db.getPaymentMethod(idlePage->userDrinkOrder->getOption())=="tap")){
        // REVERSE PAYMENT
        com.init();
        pktToSend = paymentPacket.reversePurchasePacket();
        if (sendToUX410()){
            waitForUX410();
//            qDebug() << "Payment Reversed" << endl;
            pktResponded.clear();
            com.flushSerial();
        }
    }else if ((db.getPaymentMethod(idlePage->userDrinkOrder->getOption())=="tap") && volumeDispensed != 0){
        QMessageBox msgBox;
        msgBox.setText("Complete!");
        msgBox.setInformativeText("Would you like your receipt emailed to you?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();

        switch(ret){
        case QMessageBox::Yes:
//            qDebug() << "This person wants their receipt emailed";
            break;
        case QMessageBox::No:
//            qDebug() << "This person doesn't want a receipt";
            break;
        default:
            break;
        }
    }

    QString command = QString::number(this->idlePage->userDrinkOrder->getOption());

    if(idlePage->userDrinkOrder->getSizeOption() == SMALL_DRINK){
        command.append('s');

    } else {
        command.append('l');
    }

    stopDispenseTimer();

    this->idlePage->dfUtility->msg = command;
    idlePage->dfUtility->m_IsSendingFSM = true;
    idlePage->dfUtility->m_fsmMsg = SEND_CLEAN;

    // Send a Cleanse and TODO: helps FSM onExit...
    idlePage->dfUtility->send_to_FSM();

     db.closeDB();
     thanksPage->showFullScreen();
     this->hide();
}

void dispensePage::onRinseTimerTick(){
}

void dispensePage::stopDispenseTimer(){
//    qDebug() << "dispensePage: Stop Timers" << endl;
    if(dispenseIdleTimer != nullptr){
        dispenseIdleTimer->stop();
    }
    dispenseIdleTimer = nullptr;
}

void dispensePage::onDispenseIdleTick(){
    if(-- _dispenseIdleTimeoutSec >= 0) {
//        qDebug() << "dispensePage: Idle Tick Down: " << _dispenseIdleTimeoutSec << endl;
    } else {
//        qDebug() << "Timer Done!" << _dispenseIdleTimeoutSec << endl;
//        dispenseIdleTimer->stop();
        on_finish_Button_clicked();
    }
}

double dispensePage::getTotalDispensed(){
    return volumeDispensed;
}

void dispensePage::PleaseResetTimerSlot(void){
    //qDebug() << "RESET SIGNAL RECEIVED!" << endl;
    _dispenseIdleTimeoutSec = 30;
}

void dispensePage::updateVolumeDisplayed(double dispensed){

    volumeDispensed = dispensed;
    double target_volume = idlePage->userDrinkOrder->getSize();
    double percentage = dispensed/target_volume*100;

    this->ui->filler->move(380, 590 - 3*percentage);

    ui->widget->show();
    ui->filler->show();

    ui->finish_Button->setEnabled(true);
}

void dispensePage::targetHitDisplay(){
    //this->ui->volumeDispensedLabel->setText(QString::number(volumeDispensed)+ " ml - Target Hit!");
    on_finish_Button_clicked();
}
