//***************************************
//
// payselect.cpp
// GUI class for user to select size and
// payment for drink.
//
// Allows navigation to First product page
// payment page and idle page
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "payselect.h"
#include "ui_payselect.h"

#include "paypage.h"
#include "productpage_1.h"
#include "idle.h"

// CTOR
paySelect::paySelect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::paySelect)
{
    ui->setupUi(this);
    QPixmap background(":/light/4_pay_select_page.jpg");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /*hacky transparent button*/
    ui->previousPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->payPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->mainPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

    // TODO: ADD buttons to select size/price of drink
    ui->orderSmall_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->orderBig_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

    // TODO: Set up functions to manipulate DrinkOrder Object
}

/*
 * Page Tracking reference to Select Drink, Payment Page and Idle page
 */
void paySelect::setPage(productPage_1 *pageSelect, payPage *pagePayment, idle* pageIdle)
{
    this->firstProductPage = pageSelect;
    this->paymentPage = pagePayment;
    this->idlePage = pageIdle;
}

// DTOR
paySelect::~paySelect()
{
    delete ui;
}

/* GUI */
void paySelect::on_previousPage_Button_clicked()
{
    firstProductPage->showFullScreen();
    this->hide();
}

void paySelect::on_payPage_Button_clicked()
{
    paymentPage->showFullScreen();
    this->hide();
}

void paySelect::on_mainPage_Button_clicked()
{
    idlePage->showFullScreen();
    this->hide();
}

// on_Small_Order button listener
void paySelect::on_orderSmall_Button_clicked()
{
    qInfo() << "Existing Size" << idlePage->userDrinkOrder->getSize() << endl;
    idlePage->userDrinkOrder->setSize(355.00);
    qInfo() << "Small Order" << idlePage->userDrinkOrder->getSize() << endl;
}

// on_Large_Order button listener
void paySelect::on_orderBig_Button_clicked()
{
    qInfo() << "Big Order" << endl;
}

/* Models */

// Function setDrinkOrder
