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

    ui->wifi_ack_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    timeoutTimer = new QTimer(this);
    timeoutTimer->setInterval(20);
    connect(timeoutTimer, SIGNAL(timeout()), this, SLOT(onTimeOutTick()));

    ui->wifi_ack_Button->setEnabled(true);
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
    QPixmap background("/home/df-admin/drinkfill/production/references/oops.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    ui->widget->hide();
    ui->filler->hide();

    QWidget::showEvent(event);

    ui->wifi_ack_Button->setEnabled(false);

      if(nullptr == timeoutTimer){
        timeoutTimer = new QTimer(this);
        timeoutTimer->setInterval(1000);
        connect(timeoutTimer, SIGNAL(timeout()), this, SLOT(onTimeOutTick()));
    }

    timeoutTimer->start(1000);
    _goToIdlePageTimeoutSec = 10;
}



/*
 * Page Tracking reference to Payment page and completed payment
 */
void page_error_wifi::on_wifi_ack_Button_clicked()
{
    qDebug() << "call db from wifi error page" << endl;
    DbManager db(DB_PATH);

    //stopDispenseTimer();
     db.closeDB();

     idlePage->showFullScreen();
     this->hide();
}


// void page_error_wifi::stopDispenseTimer(){
// //    qDebug() << "page_error_wifi: Stop Timers" << endl;
//     if(timeoutTimer != nullptr){
//         timeoutTimer->stop();
//     }
//     timeoutTimer = nullptr;
// }


void page_error_wifi::onTimeOutTick(){
    if(-- _goToIdlePageTimeoutSec >= 0) {
//        qDebug() << "page_dispenser: Idle Tick Down: " << _goToIdlePageTimeoutSec << endl;
    } else {
//        qDebug() << "Timer Done!" << _goToIdlePageTimeoutSec << endl;
        qDebug() << "Show wifi error timed out. " << endl;
        timeoutTimer->stop();
        
    }
}

