//***************************************
//
// page_help.cpp
// GUI class for user to get drinkfill
//  page_help and contactact info
//
//
// created: 28-05-2021
// by: Paddy Riley
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "page_help.h"
#include "ui_page_help.h"
#include "df_util.h"
#include "page_select_product.h"
#include "page_product.h"
#include "pagepayment.h"
#include "page_idle.h"


// CTOR
page_help::page_help(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_help)
{
    // Fullscreen background setup
    ui->setupUi(this);

    ui->previousPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->previousPage_Button_2->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->refreshButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    QPixmap background(PAGE_HELP_BACKGROUND_PATH);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    helpIdleTimer = new QTimer(this);
    helpIdleTimer->setInterval(1000);
    connect(helpIdleTimer, SIGNAL(timeout()), this, SLOT(onHelpTimeoutTick()));
}

// DTOR
page_help::~page_help()
{
    delete ui;
}

void page_help::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< PPPage Enter: Help >>>>>>>>>";
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
void page_help::setPage(page_select_product *pageSelect, pageProduct* pageProduct, page_idle* pageIdle, pagePayment *pagePayment)
{
    this->p_page_idle = pageIdle;
    this->paymentPage = pagePayment;
    this->selectPage = pageProduct;
    this->p_page_select_product = pageSelect;
}

void page_help::on_previousPage_Button_clicked(){
    helpIdleTimer->stop();
    // qDebug() << "help3 to idle";
    p_page_idle->showFullScreen();
    this->hide();
}

void page_help::on_previousPage_Button_2_clicked(){
    helpIdleTimer->stop();
    // qDebug() << "help2 to idle";
    p_page_idle->showFullScreen();
    this->hide();
}

void page_help::onHelpTimeoutTick(){
    if(-- _helpIdleTimeoutSec >= 0) {
//        qDebug() << "Help Tick Down: " << _helpIdleTimeoutSec << endl;
    } else {
//        qDebug() << "Help Timer Done!" << _helpIdleTimeoutSec << endl;

        helpIdleTimer->stop();
        // qDebug() << "help to idle";
        p_page_idle->showFullScreen();
        this->hide();
    }
    if(_helpIdleTimeoutSec<10){
        ui->refreshLabel->show();
    }
}

void page_help::on_refreshButton_clicked(){
    _helpIdleTimeoutSec = 60;
    ui->refreshLabel->hide();
}
