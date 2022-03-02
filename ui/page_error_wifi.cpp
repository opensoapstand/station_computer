//***************************************
//
// page_error_wifi.cpp
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

#include "page_error_wifi.h"
#include "ui_page_error_wifi.h"
#include "includefiles.h"
#include "page_idle.h"
#include "pagethankyou.h"

// CTOR
page_error_wifi::page_error_wifi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_error_wifi)
{
    ui->setupUi(this);

    ui->finish_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    dispenseIdleTimer = new QTimer(this);
    dispenseIdleTimer->setInterval(20);
    connect(dispenseIdleTimer, SIGNAL(timeout()), this, SLOT(onDispenseIdleTick()));

       ui->finish_Button->setEnabled(true);
}

/*
 * Page Tracking reference to Payment page and completed payment
 */
void page_error_wifi::setPage(pagePayment *pagePayment, pagethankyou* pageThankYou, page_idle* pageIdle)
{
    this->thanksPage = pageThankYou;
    this->paymentPage = pagePayment;
    this->idlePage = pageIdle;
}

// DTOR
page_error_wifi::~page_error_wifi()
{
    delete ui;
}

void page_error_wifi::showEvent(QShowEvent *event)
{
    QPixmap background("/release/references/general/oops.png");
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
void page_error_wifi::on_finish_Button_clicked()
{
    DbManager db(DB_PATH);

    stopDispenseTimer();
     db.closeDB();
     idlePage->showFullScreen();
     this->hide();
}


void page_error_wifi::stopDispenseTimer(){
//    qDebug() << "page_error_wifi: Stop Timers" << endl;
    if(dispenseIdleTimer != nullptr){
        dispenseIdleTimer->stop();
    }
    dispenseIdleTimer = nullptr;
}


void page_error_wifi::onDispenseIdleTick(){
    if(-- _dispenseIdleTimeoutSec >= 0) {
//        qDebug() << "page_dispenser: Idle Tick Down: " << _dispenseIdleTimeoutSec << endl;
    } else {
//        qDebug() << "Timer Done!" << _dispenseIdleTimeoutSec << endl;
//        dispenseIdleTimer->stop();
        on_finish_Button_clicked();
    }
}


void page_error_wifi::targetHitDisplay(){
    on_finish_Button_clicked();
}
