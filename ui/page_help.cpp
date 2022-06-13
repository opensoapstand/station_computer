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
#include "page_payment.h"
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

    // view transactions button
    QFont font;
    font.setFamily(QStringLiteral("Brevia"));
    font.setPointSize(20);
    // font.setBold(true);
    // font.setWeight(75);
    font.setWeight(50);
    ui->transactions_Button->setStyleSheet("QPushButton { color:#003840; background-color: transparent; border: 0px }");
    // ui->transactions_Button->setStyleSheet("QPushButton { color:#FFFFFF;background-color: #5E8580; border: 1px solid #3D6675;box-sizing: border-box;border-radius: 20px;}");
    ui->transactions_Button->setFont(font);
    ui->transactions_Button->setText("Transaction History ->");

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
    qDebug() << "<<<<<<< Page Enter: Help >>>>>>>>>";
    QWidget::showEvent(event);

    // QPixmap background(PAGE_HELP_BACKGROUND_PATH);
    // background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    // QPalette palette;
    // palette.setBrush(QPalette::Background, background);
    // this->setPalette(palette);
    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_HELP_BACKGROUND_PATH);
    

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
void page_help::setPage(page_select_product *pageSelect, pageProduct* pageProduct, page_idle* pageIdle, page_payment *page_payment, page_transactions *pageTransactions)
{
    this->p_page_idle = pageIdle;
    this->paymentPage = page_payment;
    this->selectPage = pageProduct;
    this->p_page_select_product = pageSelect;
    this->p_page_transactions = pageTransactions;
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
       qDebug() << "Help Tick Down: " << _helpIdleTimeoutSec;
    } else {
       qDebug() << "Help Timer Done!" << _helpIdleTimeoutSec;

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

void page_help::on_transactions_Button_clicked()
{
    helpIdleTimer->stop();
    p_page_transactions->showFullScreen();
    this->hide();
}
