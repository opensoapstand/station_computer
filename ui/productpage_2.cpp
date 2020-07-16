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

    /*hacky transparent button*/
    ui->previousPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->selection7_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
}

void productPage_2::setPage(productPage_1 *pageOneProducts, paySelect *pageSizeSelect)
{
    this->firstProductPage = pageOneProducts;
    this->paySelectPage = pageSizeSelect;
}

productPage_2::~productPage_2()
{
    delete ui;
}

void productPage_2::on_previousPage_Button_clicked()
{
    firstProductPage->showFullScreen();
    this->hide();
}

void productPage_2::on_selection7_Button_clicked()
{
    paySelectPage->showFullScreen();
    this->hide();
}
