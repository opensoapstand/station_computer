//***************************************
//
// thankyoupage.cpp
// GUI class to show user dispense has been
// completed and route back to idle
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "thankyoupage.h"
#include "ui_thankyoupage.h"

#include "idle.h"
#include "dispensepage.h"

// CTOR
thankYouPage::thankYouPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::thankYouPage)
{
    ui->setupUi(this);
    QPixmap background(":/light/7_thank_you_page.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /*hacky transparent button*/
    ui->mainPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->rinse_label->setText("<p align=center>Water rinse coming in<br>5</p>");
    //ui->rinse_label->hide();

    thankYouEndTimer = new QTimer(this);
    thankYouEndTimer->setInterval(1000);
    connect(thankYouEndTimer, SIGNAL(timeout()), this, SLOT(onThankyouTimeoutTick()));

    rinseTimer = new QTimer(this);
    rinseTimer->setInterval(1000);
    connect(rinseTimer, SIGNAL(timeout()), this, SLOT(onRinseTimerTick()));

    //    QString counter = this->idlePage->dfUtility->get_local_db_max_transaction();
    //    ui->Counter->setStyleSheet("background-color : #F1F2F2; color: #CBA580");
    //    ui->Counter->setText(counter);
    //    ui->Counter->setText("0");
}

/*
 * Page Tracking reference
 */
void thankYouPage::setPage(dispensePage *pageDispense, idle *pageIdle)
{
    this->idlePage = pageIdle;
    this->dispensingPage = pageDispense;
}

// DTOR
thankYouPage::~thankYouPage()
{
    delete ui;
}

void thankYouPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    DbManager db(DB_PATH);

    if(thankYouEndTimer == nullptr){
        thankYouEndTimer = new QTimer(this);
        thankYouEndTimer->setInterval(1000);
        connect(thankYouEndTimer, SIGNAL(timeout()), this, SLOT(onThankyouTimeoutTick()));
    }


    // THIS WILL HAVE TO BE CHANGED SO THE SYSTEM CHECKS IF IT IS A DF / SS MACHINE

    if (db.getPaymentMethod(idlePage->userDrinkOrder->getOption()) == "tap"){
        rinse=false;
        rinseTimer->start(1000);
        _rinseTimerTimeoutSec = 5;
        ui->rinse_label->setText("<p align=center>Water rinse coming in<br><br>5</p>");
        ui->rinse_label->show();
        ui->mainPage_Button->setEnabled(false);
    }else{
        ui->rinse_label->hide();
        thankYouEndTimer->start(1000);
        _thankYouTimeoutSec = 5;
        ui->mainPage_Button->setEnabled(true);
    }

//    thankYouEndTimer->start(1000);
//    _thankYouTimeoutSec = 7;



}

void thankYouPage::onThankyouTimeoutTick(){
    if(-- _thankYouTimeoutSec >= 0) {
        qDebug() << "thanksPage: Tick Down: " << _thankYouTimeoutSec << endl;
    } else {
        qDebug() << "thanksPage: Timer Done!" << _thankYouTimeoutSec << endl;

        //Update Click DB
        DbManager db(DB_PATH);
        db.addPageClick("THANK YOU PAGE TIME OUT");
        db.addPageClick("TRANSACTION COMPLETED");
        db.addPageClick("Thank You Page -> Main Page");

        thankYouEndTimer->stop();
        this->hide();
        idlePage->showFullScreen();
    }
}

void thankYouPage::on_mainPage_Button_clicked()
{
   // Update Click DB
   DbManager db(DB_PATH);
   db.addPageClick("TRANSACTION COMPLETED");
   db.addPageClick("Thank You Page -> Main Page");

   thankYouEndTimer->stop();
   this->hide();
   idlePage->showFullScreen();
}

void thankYouPage::onRinseTimerTick(){

    QMessageBox msgBox;
    if (!rinse){
        ui->rinse_label->show();
        ui->rinse_label->setText("<p align=center>Water rinse coming in<br><br>"+ QString::number(_rinseTimerTimeoutSec) +"</p>");
        rinse=true;
    }

    if(-- _rinseTimerTimeoutSec >= 1) {
        qDebug() << "rinseTimer: Tick Down: " << _rinseTimerTimeoutSec << endl;
        ui->rinse_label->setText("<p align=center>Water rinse coming in<br><br>"+ QString::number(_rinseTimerTimeoutSec) +"</p>");
    }else if(_rinseTimerTimeoutSec == 0) {
        ui->rinse_label->setText("<p align=center>Rinsing with water now</p>");
    }
    else if(_rinseTimerTimeoutSec == -1) {
        ui->rinse_label->setText("<p align=center>Rinsing with water now</p>");
    }
    else{
        qDebug() << "rinseTimer Done!" << _rinseTimerTimeoutSec << endl;
        rinseTimer->stop();
        ui->rinse_label->hide();
        thankYouEndTimer->start(1000);
        _thankYouTimeoutSec = 3;
    }
}
