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
// copyright 2020 by Drinkfill Beverages Ltd
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
//    QPixmap background(":/light/6_dispense_page.png");
//    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
//    QPalette palette;
//    palette.setBrush(QPalette::Background, background);
//    this->setPalette(palette);

    /*hacky transparent button*/
    //ui->finish_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->volumeDispensedLabel->setText("");

    this->ui->volumeDispensedLabel->setText("");

    dispenseIdleTimer = new QTimer(this);
    dispenseIdleTimer->setInterval(1000);
    connect(dispenseIdleTimer, SIGNAL(timeout()), this, SLOT(onDispenseIdleTick()));

//    rinseTimer = new QTimer(this);
//    rinseTimer->setInterval(1000);
//    connect(rinseTimer, SIGNAL(timeout()), this, SLOT(onRinseTimerTick()));
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

    //qDebug() << "getSize: " << idlePage->userDrinkOrder->getSize() << endl;

    if(idlePage->userDrinkOrder->getSizeOption() == SMALL_DRINK){

        command.append('s');

    } else {

        command.append('l');
    }

    this->idlePage->dfUtility->msg = command;

    // Networking
    idlePage->dfUtility->m_IsSendingFSM = true;
    QWidget::showEvent(event);
    idlePage->dfUtility->m_fsmMsg = SEND_DRINK;


    idlePage->dfUtility->send_to_FSM();
    idlePage->dfUtility->m_IsSendingFSM = false;
    ui->finish_Button->setEnabled(false);

//    ui->dispense_clean_label->setText(" ");
//    ui->dispense_progress_label->setText(" ");

    if(nullptr == dispenseIdleTimer){
        dispenseIdleTimer = new QTimer(this);
        dispenseIdleTimer->setInterval(1000);
//        dispenseIdleTimer->isSingleShot();
        connect(dispenseIdleTimer, SIGNAL(timeout()), this, SLOT(onDispenseIdleTick()));
    }

    qDebug() << "Start Dispense Timers" << endl;
    dispenseIdleTimer->start(1000);
    _dispenseIdleTimeoutSec = 30;

}

/*
 * Page Tracking reference to Payment page and completed payment
 */
void dispensePage::on_finish_Button_clicked()
{
    //Update Click DB
    DbManager db(DB_PATH);
    db.addPageClick("Dispense Page -> Thank You Page");

    qDebug() << "dispensePage: finish button clicked" << endl;

    QString command = QString::number(this->idlePage->userDrinkOrder->getOption());


    if(idlePage->userDrinkOrder->getSizeOption() == SMALL_DRINK){

        command.append('s');

    } else {

        command.append('l');
    }

    stopDispenseTimer();

    this->idlePage->dfUtility->msg = command;

    qDebug() << this->idlePage->dfUtility->msg << endl;

    idlePage->dfUtility->m_IsSendingFSM = true;

    idlePage->dfUtility->m_fsmMsg = SEND_CLEAN;

    // Send a Cleanse and TODO: helps FSM onExit...
    idlePage->dfUtility->send_to_FSM();

//    // RINSING MESSAGE

//    rinse=false;
//    rinseTimer->start(1000);
//    _rinseTimerTimeoutSec = 5;

      this->hide();
      thanksPage->showFullScreen();
      this->ui->volumeDispensedLabel->setText("");
}

void dispensePage::onRinseTimerTick(){

//    QMessageBox msgBox;
//    if (!rinse){
//        msgBox.setWindowFlags(Qt::FramelessWindowHint);
//        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
//        msgBox.setText("<p align=center>Water rinse coming in<br>5</p>");
//        msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;}");
//        msgBox.show();
//        msgBox.raise();
//        QCoreApplication::processEvents();
//        rinse=true;
//    }

//    if(-- _rinseTimerTimeoutSec >= 0) {
//        qDebug() << "rinseTimer: Tick Down: " << _rinseTimerTimeoutSec << endl;
//        msgBox.setText("<p align=center>Water rinse coming in<br>"+ QString::number(_rinseTimerTimeoutSec) +"</p>");
//        msgBox.show();
//        msgBox.raise();
//        QCoreApplication::processEvents();
//    } else {
//        qDebug() << "rinseTimer Done!" << _rinseTimerTimeoutSec << endl;
//        rinseTimer->stop();
//        msgBox.hide();
//        this->hide();
//        thanksPage->showFullScreen();
//        this->ui->volumeDispensedLabel->setText("");
//    }
}

void dispensePage::stopDispenseTimer(){
    qDebug() << "dispensePage: Stop Timers" << endl;
    if(dispenseIdleTimer != nullptr){
        dispenseIdleTimer->stop();
    }
    dispenseIdleTimer = nullptr;
}

// XXX: Remove this when interrupts and flow sensors work!

//void dispensePage::onDispenseTick(){
//    if(-- _dispenseTimeoutSec >= 0) {
//        qDebug() << "Tick Down: " << _dispenseTimeoutSec << endl;
//        _dispenseTimeLabel.clear();
//        QString time = QString::number(_dispenseTimeoutSec);
//        _dispenseTimeLabel.append(time);
//     //   this->ui->dispense_progress_label->setText(_dispenseTimeLabel);
//    } else {
//        qDebug() << "Timer Done!" << _dispenseTimeoutSec << endl;
//        dispenseEndTimer->stop();
//       // this->ui->dispense_progress_label->setText("Finished!");
//    }
//}

void dispensePage::onDispenseIdleTick(){
    if(-- _dispenseIdleTimeoutSec >= 0) {
        qDebug() << "dispensePage: Idle Tick Down: " << _dispenseIdleTimeoutSec << endl;
    } else {
        qDebug() << "Timer Done!" << _dispenseIdleTimeoutSec << endl;
//        dispenseIdleTimer->stop();

        //Update Click DB
        DbManager db(DB_PATH);
        db.addPageClick("DISPENSE TIME OUT");

        on_finish_Button_clicked();
    }
}

void dispensePage::PleaseResetTimerSlot(void){
    //qDebug() << "RESET SIGNAL RECEIVED!" << endl;
    _dispenseIdleTimeoutSec = 30;
}

void dispensePage::updateVolumeDisplayed(int dispensed){

    volumeDispensed = dispensed;
    double target_volume = idlePage->userDrinkOrder->getSize();
    double percentage = dispensed/target_volume*100;

    //this->ui->filler->setStyleSheet("QWidget { height: 463px}");
    this->ui->filler->move(416, 978 - 3*percentage);

    QPixmap background(":/light/drink_empty.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);
    ui->widget->show();
    ui->filler->show();

    ui->finish_Button->setEnabled(true);
    //ui->volumeDispensedLabel->setText("");
    //this->ui->volumeDispensedLabel->setText(QString::number(percentage));
}

void dispensePage::targetHitDisplay(){
    //this->ui->volumeDispensedLabel->setText(QString::number(volumeDispensed)+ " ml - Target Hit!");
    //Update Click DB
    DbManager db(DB_PATH);
    db.addPageClick("TARGET HIT");
    on_finish_Button_clicked();
}
