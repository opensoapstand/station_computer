//***************************************
//
// paypage.cpp
// GUI class while machine is processing
// payment.
//
// Coordinates User input from payment select
// class then communcates results to dispensepage.
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "paypage.h"
#include "ui_paypage.h"

#include "payselect.h"
#include "dispensepage.h"
#include "idle.h"

// CTOR
payPage::payPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::payPage)
{
    ui->setupUi(this);
    QPixmap background(":/light/5_pay_page_blank.jpg");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /*hacky transparent button*/
    ui->previousPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->passPayment_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->mainPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

    // Setup static labels
    ui->order_tax_label->setText("Our Planet");
    ui->order_tax_amount->setText("Priceless");
    ui->order_total_label->setText("Total");
    updateTotals(this->drinkDescription, this->drinkAmount, this->orderTotal);
}
/*
 * Page Tracking reference
 */
void payPage::setPage(paySelect *pageSizeSelect, dispensePage* pageDispense, idle* pageIdle)
{
    this->paySelectPage = pageSizeSelect;
    this->dispensingPage = pageDispense;
    this->idlePage = pageIdle;
}

// DTOR
payPage::~payPage()
{
    delete ui;
}

void payPage::on_previousPage_Button_clicked()
{
    paySelectPage->showFullScreen();
    this->hide();
}

void payPage::on_passPayment_Button_clicked()
{
    // TODO: Moneris Linkage here!
    qDebug() << this->idlePage->userDrinkOrder->getOption();
    qDebug() << this->idlePage->userDrinkOrder->getSize();
    qDebug() << this->idlePage->userDrinkOrder->getPrice();
    dispensingPage->showFullScreen();
    this->hide();
}

void payPage::updateTotals(string drinkDescription, string drinkAmount, string orderTotal)
{
    this->drinkDescription = drinkDescription;
    this->drinkAmount = drinkAmount;
    this->orderTotal = orderTotal;

    ui->order_drink_label->setText(this->drinkDescription.c_str());
    ui->order_drink_amount->setText(this->drinkAmount.c_str());
    ui->order_total_amount->setText(this->orderTotal.c_str());
}

void payPage::on_mainPage_Button_clicked()
{
    this->hide();
    idlePage->showFullScreen();
}
