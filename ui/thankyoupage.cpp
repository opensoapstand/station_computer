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

    thankYouEndTimer->start(1000);
    _thankYouTimeoutSec = 20;

}

void thankYouPage::onThankyouTimeoutTick(){
    if(-- _thankYouTimeoutSec >= 0) {
        qDebug() << "Tick Down: " << _thankYouTimeoutSec << endl;
    } else {
        qDebug() << "Timer Done!" << _thankYouTimeoutSec << endl;

        //Update Click DB
        DbManager db(DB_PATH);
        db.addPageClick("THANK YOU PAGE TIME OUT");

  //      on_mainPage_Button_clicked();
           //Update Click DB
//        DbManager db(DB_PATH);
        db.addPageClick("TRANSACTION COMPLETED");
        db.addPageClick("Thank You Page -> Main Page");

        thankYouEndTimer->stop();
        this->hide();
        idlePage->showFullScreen();
    }
}

void thankYouPage::on_mainPage_Button_clicked()
{
    //Update Click DB
//    DbManager db(DB_PATH);
//    db.addPageClick("TRANSACTION COMPLETED");
//    db.addPageClick("Thank You Page -> Main Page");

//    thankYouEndTimer->stop();
//    this->hide();
//    idlePage->showFullScreen();
}
