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

    if(thankYouEndTimer == nullptr){
        thankYouEndTimer = new QTimer(this);
        thankYouEndTimer->setInterval(1000);
        connect(thankYouEndTimer, SIGNAL(timeout()), this, SLOT(onThankyouTimeoutTick()));
    }

    //    // RINSING MESSAGE

        rinse=false;
        rinseTimer->start(1000);
        _rinseTimerTimeoutSec = 5;



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
//   // Update Click DB
//   DbManager db(DB_PATH);
//   db.addPageClick("TRANSACTION COMPLETED");
//   db.addPageClick("Thank You Page -> Main Page");

//   thankYouEndTimer->stop();
//   this->hide();
//   idlePage->showFullScreen();
}

void thankYouPage::onRinseTimerTick(){

    QMessageBox msgBox;
    if (!rinse){
        msgBox.setWindowFlags(Qt::FramelessWindowHint);
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
        msgBox.setText("<p align=center>Water rinse coming in<br>5</p>");
        msgBox.setStyleSheet("QMessageBox{min-width: 7000px; min-height:2000px; font-size: 24px;}");
        msgBox.show();
        msgBox.raise();
        QCoreApplication::processEvents();
        rinse=true;
    }

    if(-- _rinseTimerTimeoutSec >= 0) {
        qDebug() << "rinseTimer: Tick Down: " << _rinseTimerTimeoutSec << endl;
        msgBox.setText("<p align=center>Water rinse coming in<br>"+ QString::number(_rinseTimerTimeoutSec) +"</p>");
        //msgBox.show();
        //msgBox.raise();
        //QCoreApplication::processEvents();
    } else {
        qDebug() << "rinseTimer Done!" << _rinseTimerTimeoutSec << endl;
        rinseTimer->stop();
        msgBox.hide();
        thankYouEndTimer->start(1000);
        _thankYouTimeoutSec = 7;
        //this->ui->volumeDispensedLabel->setText("");
    }
}
