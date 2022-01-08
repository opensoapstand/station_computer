//***************************************
//
// wifiErrorPage.cpp
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

#include "wifiErrorPage.h"
#include "ui_wifiErrorPage.h"
#include "includefiles.h"
#include "idle.h"
#include "thankyoupage.h"

// CTOR
wifiErrorPage::wifiErrorPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wifiErrorPage)
{
    ui->setupUi(this);

    ui->finish_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

    dispenseIdleTimer = new QTimer(this);
    dispenseIdleTimer->setInterval(20);
    connect(dispenseIdleTimer, SIGNAL(timeout()), this, SLOT(onDispenseIdleTick()));

       ui->finish_Button->setEnabled(true);
}

/*
 * Page Tracking reference to Payment page and completed payment
 */
void wifiErrorPage::setPage(payPage *pagePayment, thankYouPage* pageThankYou, idle* pageIdle)
{
    this->thanksPage = pageThankYou;
    this->paymentPage = pagePayment;
    this->idlePage = pageIdle;
}

// DTOR
wifiErrorPage::~wifiErrorPage()
{
    delete ui;
}

void wifiErrorPage::showEvent(QShowEvent *event)
{
    QPixmap background(":/light/oops.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    ui->widget->hide();
    ui->filler->hide();

    QWidget::showEvent(event);

    ui->finish_Button->setEnabled(false);

      if(nullptr == dispenseIdleTimer){
        dispenseIdleTimer = new QTimer(this);
        dispenseIdleTimer->setInterval(1000);
        connect(dispenseIdleTimer, SIGNAL(timeout()), this, SLOT(onDispenseIdleTick()));
    }

    dispenseIdleTimer->start(1000);
    _dispenseIdleTimeoutSec = 10;
}



/*
 * Page Tracking reference to Payment page and completed payment
 */
void wifiErrorPage::on_finish_Button_clicked()
{
    DbManager db(DB_PATH);

    stopDispenseTimer();
     db.closeDB();
     idlePage->showFullScreen();
     this->hide();
}


void wifiErrorPage::stopDispenseTimer(){
//    qDebug() << "wifiErrorPage: Stop Timers" << endl;
    if(dispenseIdleTimer != nullptr){
        dispenseIdleTimer->stop();
    }
    dispenseIdleTimer = nullptr;
}


void wifiErrorPage::onDispenseIdleTick(){
    if(-- _dispenseIdleTimeoutSec >= 0) {
//        qDebug() << "dispensePage: Idle Tick Down: " << _dispenseIdleTimeoutSec << endl;
    } else {
//        qDebug() << "Timer Done!" << _dispenseIdleTimeoutSec << endl;
//        dispenseIdleTimer->stop();
        on_finish_Button_clicked();
    }
}


void wifiErrorPage::targetHitDisplay(){
    on_finish_Button_clicked();
}
