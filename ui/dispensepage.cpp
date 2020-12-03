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
    QPixmap background(":/light/6_dispense_page.jpg");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /*hacky transparent button*/
    ui->finish_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

    //ui->volumeDispensedLabel->setText("");

    this->ui->volumeDispensedLabel->setText("");

    dispenseIdleTimer = new QTimer(this);
    dispenseIdleTimer->setInterval(1000);
    connect(dispenseIdleTimer, SIGNAL(timeout()), this, SLOT(onDispenseIdleTick()));
   // connect(dfuicommthread, &DfUiCommThread::resetTimerSignal, this, &dispensePage::resetTimerSlot);

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

    // FIXME: this is a hack for size changes...
    QString command = QString::number(this->idlePage->userDrinkOrder->getOption());
    if(idlePage->userDrinkOrder->getSize() <= 355){
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
    ui->finish_Button->setEnabled(true);

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
    qDebug() << "dispensePage: finish button clicked" << endl;

    QString command = QString::number(this->idlePage->userDrinkOrder->getOption());
    command.append('s');

    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::FramelessWindowHint);
    msgBox.setText("<p align=center>Are you sure?</p>");
    msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-width: 300px; min-height: 300px;}");

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int ret = msgBox.exec();

    switch(ret){
        case QMessageBox::Yes:
            qDebug() << "YES CLICKED" << endl;
            stopDispenseTimer();

            // TODO: Link to FSM for Dispense

                qDebug() << "dispensePage: Cleanse cycle." << endl;
                //ui->dispense_clean_label->setText("REMOVE BOTTLE!");
                //ui->dispense_progress_label->setText("...");

                dispenseEndTimer = new QTimer(this);
                dispenseEndTimer->setInterval(1000);
                connect(dispenseEndTimer, SIGNAL(timeout()), this, SLOT(onDispenseTick()));
                dispenseEndTimer->start(1000);
                _dispenseTimeoutSec = 5;


            // FIXME: this is a hack for size changes...

            this->idlePage->dfUtility->msg = command;

            qDebug() << this->idlePage->dfUtility->msg << endl;

            idlePage->dfUtility->m_IsSendingFSM = true;

            idlePage->dfUtility->m_fsmMsg = SEND_CLEAN;
            //    this->idlePage->dfUtility->msg = QString::number(this->idlePage->userDrinkOrder->getOption());

            // Send a Cleanse and TODO: helps FSM onExit...
            idlePage->dfUtility->send_to_FSM();

            dispenseEndTimer->stop();
            dispenseEndTimer->deleteLater();

            this->hide();
            thanksPage->showFullScreen();
            this->ui->volumeDispensedLabel->setText("");
            break;

        case QMessageBox::No:
            qDebug() << "NO CLICKED" << endl;
            break;

        default:
            break;
    }


}

void dispensePage::stopDispenseTimer(){
    qDebug() << "dispensePage: Stop Timers" << endl;
    if(dispenseIdleTimer != nullptr){
        dispenseIdleTimer->stop();
    }
    dispenseIdleTimer = nullptr;
}

// XXX: Remove this when interrupts and flow sensors work!

void dispensePage::onDispenseTick(){
    if(-- _dispenseTimeoutSec >= 0) {
        qDebug() << "Tick Down: " << _dispenseTimeoutSec << endl;
        _dispenseTimeLabel.clear();
        QString time = QString::number(_dispenseTimeoutSec);
        _dispenseTimeLabel.append(time);
     //   this->ui->dispense_progress_label->setText(_dispenseTimeLabel);
    } else {
        qDebug() << "Timer Done!" << _dispenseTimeoutSec << endl;
        dispenseEndTimer->stop();
       // this->ui->dispense_progress_label->setText("Finished!");
    }
}

void dispensePage::onDispenseIdleTick(){
    if(-- _dispenseIdleTimeoutSec >= 0) {
        qDebug() << "dispensePage: Idle Tick Down: " << _dispenseIdleTimeoutSec << endl;
    } else {
        qDebug() << "Timer Done!" << _dispenseIdleTimeoutSec << endl;
//        dispenseIdleTimer->stop();
        on_finish_Button_clicked();
    }
}

void dispensePage::PleaseResetTimerSlot(void){
    //qDebug() << "RESET SIGNAL RECEIVED!" << endl;
    _dispenseIdleTimeoutSec = 30;
}

void dispensePage::updateVolumeDisplayed(int dispensed){
    //string disSTRING = "I am in dispensed page and I know that " + to_string(dispensed) + " has been dispensed!";
    //qDebug() << (disSTRING).c_str() << endl;
    volumeDispensed = dispensed;
    //volumeDispensedLabel.clear();
    this->ui->volumeDispensedLabel->setText(QString::number(volumeDispensed)+ "ml dispensed");
}
