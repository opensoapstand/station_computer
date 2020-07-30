//***************************************
//
// productpage_1.cpp
// GUI class for user to browse up to
// 6 drinks.
//
// Allows navigation to First product page
// payment selection page
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "productpage_1.h"
#include "ui_productpage_1.h"

#include "productpage_2.h"
#include "payselect.h"

// CTOR
productPage_1::productPage_1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::productPage_1)
{
    ui->setupUi(this);
    QPixmap background(":/light/2_drink_page_1.jpg");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    // TODO:  Populate Page Drink buttons with DrinkSelection[0-5]

    /*hacky transparent button*/
    ui->nextPageButton->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->selection1_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->selection2_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->selection3_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->selection4_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->selection5_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->selection6_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
}

/*
 * Page Tracking reference
 */
void productPage_1::setPage(productPage_2 *pageTwoProducts, paySelect *pageSizeSelect)
{
    this->selection_PageTwo = pageTwoProducts;
    this->paymentSelectPage = pageSizeSelect;
}

// DTOR
productPage_1::~productPage_1()
{
    delete ui;
}

void productPage_1::on_nextPageButton_clicked()
{
    selection_PageTwo->showFullScreen();
    this->hide();
}

// FIXME: This is terrible...no time to make array reference to hold button press functions
void productPage_1::on_selection1_Button_clicked()
{    
    qInfo() << "Selection 1" << endl;
    this->selectedDrink->brandName = "ok";
    qInfo() << selectedDrink->brandName << endl;

    paymentSelectPage->showFullScreen();
    this->hide();
}

void productPage_1::on_selection2_Button_clicked()
{
    qInfo() << "Selection 2" << endl;
    paymentSelectPage->showFullScreen();
    this->hide();
}

void productPage_1::on_selection3_Button_clicked()
{
    qInfo() << "Selection 3" << endl;
    paymentSelectPage->showFullScreen();
    this->hide();
}

void productPage_1::on_selection4_Button_clicked()
{
    qInfo() << "Selection 4" << endl;
    paymentSelectPage->showFullScreen();
    this->hide();
}

void productPage_1::on_selection5_Button_clicked()
{
    qInfo() << "Selection 5" << endl;
    paymentSelectPage->showFullScreen();
    this->hide();
}

void productPage_1::on_selection6_Button_clicked()
{
    qInfo() << "Selection 6" << endl;
    paymentSelectPage->showFullScreen();
    this->hide();
}
