//***************************************
//
// page_dispenser.cpp
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

#include "page_dispenser.h"
#include "ui_page_dispenser.h"
#include "includefiles.h"
#include "page_idle.h"
#include "pagethankyou.h"

// CTOR
page_dispenser::page_dispenser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_dispenser)
{
    this->isDispensing = false;
    ui->setupUi(this);

    // ui->finish_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->abortButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    dispenseIdleTimer = new QTimer(this);
    dispenseIdleTimer->setInterval(1000);
    connect(dispenseIdleTimer, SIGNAL(timeout()), this, SLOT(onDispenseIdleTick()));

}

/*
 * Page Tracking reference to Payment page and completed payment
 */
void page_dispenser::setPage(pagePayment *pagePayment, pagethankyou* pageThankYou, page_idle* pageIdle)
{
    this->thanksPage = pageThankYou;
    this->paymentPage = pagePayment;
    this->idlePage = pageIdle;
}

// DTOR
page_dispenser::~page_dispenser()
{
    delete ui;
}

void page_dispenser::showEvent(QShowEvent *event)
{
    this->isDispensing = false;
    qDebug()<<"Enter dispense page." << endl;
    QPixmap background("/home/df-admin/production/references/5_background_dispense_instructions.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    ui->widget->hide();
    ui->filler->hide();

    QWidget::showEvent(event);

    // FIXME: this is a hack for size changes...
    fsmSendStartDispensing();
    
    // ui->abortButton->setEnabled(false);

    if(nullptr == dispenseIdleTimer){
        dispenseIdleTimer = new QTimer(this);
        dispenseIdleTimer->setInterval(1000);
        connect(dispenseIdleTimer, SIGNAL(timeout()), this, SLOT(onDispenseIdleTick()));
    }

    dispenseIdleTimer->start(1000);
    _dispenseIdleTimeoutSec = 30;

}

bool page_dispenser::sendToUX410()
{
    int waitForAck = 0;
    while (waitForAck < 3){
        cout << "Wait for ACK counter: " << waitForAck << endl;
        qDebug()<<"Wait for ACK counter: " <<  endl;
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

bool page_dispenser::waitForUX410()
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
void page_dispenser::dispensing_end_admin()
{
    this->isDispensing = false;
    sleep(1);
    qDebug() << "call db from dispense end" << endl;
    DbManager db(DB_PATH);
    qDebug() << "call db2 from dispense end" << endl;

    if (volumeDispensed == 0 && (db.getPaymentMethod(idlePage->currentProductOrder->getSelectedSlot())=="tap")){
        qDebug() << "dispense end: tap payment No volume dispensed.";
        // REVERSE PAYMENT
        com.page_init();
        pktToSend = paymentPacket.reversePurchasePacket();
        if (sendToUX410()){
            waitForUX410();
//            qDebug() << "Payment Reversed" << endl;
            pktResponded.clear();
            com.flushSerial();
        }
    }else if ((db.getPaymentMethod(idlePage->currentProductOrder->getSelectedSlot())=="tap") && volumeDispensed != 0){
        qDebug() << "dispense end: tap payment. Some volume dispensed.";
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
qDebug() << "cefesfsef" << endl;
    stopDispenseTimer();

qDebug() << "cefes99999999999fsef" << endl;
     db.closeDB();
qDebug() << "11111111111111111fsef" << endl;
     thanksPage->showFullScreen();
qDebug() << "11144444fsef" << endl;
     this->hide();
qDebug() << "6626622fsef" << endl;
    qDebug() << "Finished dispense admin handling";
}

void page_dispenser::force_finish_dispensing(){

    fsmSendStopDispensing();
    dispensing_end_admin();
}

void page_dispenser::fsmSendStartDispensing(){
    QString command = QString::number(this->idlePage->currentProductOrder->getSelectedSlot());

    if(idlePage->currentProductOrder->getSelectedSizeOption() == SMALL_DRINK){
        command.append('s');

    } else {
        command.append('l');
    }

    volumeDispensed=0;

    this->idlePage->dfUtility->msg = command;

    // Networking
    idlePage->dfUtility->m_IsSendingFSM = true;
    idlePage->dfUtility->m_fsmMsg = SEND_DISPENSE_START;
    idlePage->dfUtility->send_to_FSM();
    idlePage->dfUtility->m_IsSendingFSM = false;
    this->isDispensing = true;
}

void page_dispenser::fsmSendStopDispensing(){
    this->isDispensing = false;
    QString command = QString::number(this->idlePage->currentProductOrder->getSelectedSlot());

    if(idlePage->currentProductOrder->getSelectedSizeOption() == SMALL_DRINK){
        command.append('s');

    } else {
        command.append('l');
    }


    this->idlePage->dfUtility->msg = command;
    idlePage->dfUtility->m_IsSendingFSM = true;
    idlePage->dfUtility->m_fsmMsg = SEND_DISPENSE_STOP;

    // Send a Cleanse and TODO: helps FSM onExit...
    idlePage->dfUtility->send_to_FSM();
    idlePage->dfUtility->m_IsSendingFSM = false;
     
}


void page_dispenser::onRinseTimerTick(){
}

void page_dispenser::stopDispenseTimer(){
    this->isDispensing = false;
//    qDebug() << "page_dispenser: Stop Timers" << endl;
    if(dispenseIdleTimer != nullptr){
        dispenseIdleTimer->stop();
    }
    dispenseIdleTimer = nullptr;
}

void page_dispenser::onDispenseIdleTick(){
    if(-- _dispenseIdleTimeoutSec >= 0) {
//        qDebug() << "page_dispenser: Idle Tick Down: " << _dispenseIdleTimeoutSec << endl;
    } else {
//        qDebug() << "Timer Done!" << _dispenseIdleTimeoutSec << endl;
//        dispenseIdleTimer->stop();
        qDebug() << "Dispensing timeout. End dispensing."  << endl;
        force_finish_dispensing();
    }
}

double page_dispenser::getTotalDispensed(){
    return volumeDispensed;
}

void page_dispenser::resetDispenseTimeout(void){
    _dispenseIdleTimeoutSec = 30;
}

void page_dispenser::updateVolumeDisplayed(double dispensed, bool isFull){
    if (this->isDispensing){
        //qDebug() << "Signal: update vol in dispenser!" << endl;
        resetDispenseTimeout();

        volumeDispensed = dispensed;
        double target_volume = idlePage->currentProductOrder->getSelectedVolume();
        double percentage = dispensed/target_volume*100;
        if (isFull){
            percentage = 100;
        }

        this->ui->filler->move(380, 590 - 3*percentage);

        ui->widget->show();
        ui->filler->show();

        // ui->abortButton->setEnabled(true);
    }else{

       qDebug() << "Signal: volume update in dispenser while not dispensing." << endl;
    }
}

void page_dispenser::fsmReceiveTargetVolumeReached(){
    if (this->isDispensing){
        this->isDispensing = false;
        // qDebug() << "Signal: Target volume reached."  << endl;
        updateVolumeDisplayed(1.0, true); // make sure the fill bottle graphics are completed

        dispensing_end_admin();
        // qDebug() << "Finish dispense end admin."  << endl;
    }else{
        qDebug() << "target volumne reached signal received while not dispensing.";
    }
}


void page_dispenser::on_abortButton_clicked()
{
    qDebug() << "Pressed dispense complete.";
    if(this->isDispensing){
        force_finish_dispensing();
    }
}
