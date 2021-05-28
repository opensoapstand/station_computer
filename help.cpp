//***************************************
//
// help.cpp
// GUI class for user to get drinkfill
//  help and contactact info
//
//
// created: 28-05-2021
// by: Paddy Riley
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "help.h"
#include "ui_help.h"
#include "df_util.h"
#include "productpage_1.h"
#include "payselect.h"
#include "paypage.h"
#include "idle.h"


// CTOR
help::help(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::help)
{
    // Fullscreen background setup
    ui->setupUi(this);

    ui->previousPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

   // background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, Qt::white);
    this->setPalette(palette);

    helpIdleTimer = new QTimer(this);
    helpIdleTimer->setInterval(1000);
    connect(helpIdleTimer, SIGNAL(timeout()), this, SLOT(onHelpTimeoutTick()));

}

// DTOR
help::~help()
{
    delete ui;
}

void help::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    DbManager db(DB_PATH);
    db.addPageClick("HELP PAGE ENTERED");

    if(helpIdleTimer == nullptr){
        helpIdleTimer = new QTimer(this);
        helpIdleTimer->setInterval(1000);
        connect(helpIdleTimer, SIGNAL(timeout()), this, SLOT(onHelpTimeoutTick()));
    }

    helpIdleTimer->start(1000);
    _helpIdleTimeoutSec = 15;

}

/*
 * Page Tracking reference
 */
void help::setPage(productPage_1 *pageSelect, paySelect* paySelect, idle* pageIdle, payPage *pagePayment)
{
    this->idlePage = pageIdle;
    this->paymentPage = pagePayment;
    this->selectPage = paySelect;
    this->firstProductPage = pageSelect;
}

void help::on_previousPage_Button_clicked(){
    qDebug() << "Previoue Page button clicked" << endl;

    DbManager db(DB_PATH);
    db.addPageClick("HELP PAGE EXITED");

    helpIdleTimer->stop();
    //idlePage->showFullScreen();
    this->hide();
}

void help::onHelpTimeoutTick(){
    if(-- _helpIdleTimeoutSec >= 0) {
        qDebug() << "Help Tick Down: " << _helpIdleTimeoutSec << endl;
    } else {
        qDebug() << "Help Timer Done!" << _helpIdleTimeoutSec << endl;

        //Update Click DB
        DbManager db(DB_PATH);
        db.addPageClick("HELP PAGE TIME OUT");

        helpIdleTimer->stop();
        this->hide();
        //idlePage->showFullScreen();
    }
}
