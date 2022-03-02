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
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "help.h"
#include "ui_help.h"
#include "df_util.h"
#include "pageproductsoverview.h"
#include "page_product.h"
#include "pagepayment.h"
#include "idle.h"


// CTOR
help::help(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::help)
{
    // Fullscreen background setup
    ui->setupUi(this);

    ui->previousPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->previousPage_Button_2->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->refreshButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    QPixmap background("/release/references/general/3_background_help.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
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

    if(helpIdleTimer == nullptr){
        helpIdleTimer = new QTimer(this);
        helpIdleTimer->setInterval(1000);
        connect(helpIdleTimer, SIGNAL(timeout()), this, SLOT(onHelpTimeoutTick()));
    }

    helpIdleTimer->start(1000);
    _helpIdleTimeoutSec = 60;
    ui->refreshLabel->hide();
}

/*
 * Page Tracking reference
 */
void help::setPage(pageproductsoverview *pageSelect, pageProduct* pageProduct, idle* pageIdle, pagePayment *pagePayment)
{
    this->idlePage = pageIdle;
    this->paymentPage = pagePayment;
    this->selectPage = pageProduct;
    this->firstProductPage = pageSelect;
}

void help::on_previousPage_Button_clicked(){
    helpIdleTimer->stop();
    idlePage->showFullScreen();
    this->hide();
}

void help::on_previousPage_Button_2_clicked(){
    helpIdleTimer->stop();
    idlePage->showFullScreen();
    this->hide();
}

void help::onHelpTimeoutTick(){
    if(-- _helpIdleTimeoutSec >= 0) {
//        qDebug() << "Help Tick Down: " << _helpIdleTimeoutSec << endl;
    } else {
//        qDebug() << "Help Timer Done!" << _helpIdleTimeoutSec << endl;

        helpIdleTimer->stop();
        idlePage->showFullScreen();
        this->hide();
    }
    if(_helpIdleTimeoutSec<10){
        ui->refreshLabel->show();
    }
}

void help::on_refreshButton_clicked(){
    _helpIdleTimeoutSec = 60;
    ui->refreshLabel->hide();
}
