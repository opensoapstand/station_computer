//***************************************
//
// productpage_2.cpp
// TODO: Compress to single Product page class
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "productpage_2.h"
#include "ui_productpage_2.h"

#include "productpage_1.h"
#include "payselect.h"

#include "df_util.h"

// CTOR
productPage_2::productPage_2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::productPage_2)
{
    ui->setupUi(this);
    QPixmap background(":/light/3_drink_page_2.jpg");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    // TODO: Populate Page Drink buttons with DrinkSelection[6-12]

    /*hacky transparent button*/
    ui->previousPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->selection7_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->selection8_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->selection9_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
}

/*
 * Page Tracking reference
 */
void productPage_2::setPage(productPage_1 *pageOneProducts, paySelect *pageSizeSelect)
{
    this->firstProductPage = pageOneProducts;
    this->paySelectPage = pageSizeSelect;
}

// DTOR
productPage_2::~productPage_2()
{
    delete ui;
}

// FIXME: Again...this is terrible...product pages need better inheritance structure!
void productPage_2::on_previousPage_Button_clicked()
{
    qInfo() << "Selection 6" << endl;
    firstProductPage->showFullScreen();
    this->hide();
}

void productPage_2::on_selection7_Button_clicked()
{
    qInfo() << "Selection 7" << endl;
    paySelectPage->showFullScreen();
    this->hide();
}

void productPage_2::on_selection8_Button_clicked()
{
    qInfo() << "Selection 8" << endl;
    paySelectPage->showFullScreen();
    this->hide();
}

void productPage_2::on_selection9_Button_clicked()
{
    qInfo() << "Selection 8" << endl;
    paySelectPage->showFullScreen();
    this->hide();
}
