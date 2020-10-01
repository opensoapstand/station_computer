
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

    int checkOption = 99;

//    qDebug() << idlePage->userDrinkOrder << endl;

//    if(idlePage->userDrinkOrder != nullptr) {
//        checkOption = idlePage->userDrinkOrder->getOption();
//    }

//    cout << checkOption << endl;
////    qDebug() << checkOption << endl;
    QString bitmap_location;

//    if(checkOption > 0 && checkOption <= 6) {
//        bitmap_location.append(":/light/4_pay_select_page_s_");
//        bitmap_location.append(QString::number(idlePage->userDrinkOrder->getOption()));
//        bitmap_location.append(".jpg");
//    } else {
        bitmap_location = ":/light/4_pay_select_page_s.jpg";
//    }
    QPixmap background(bitmap_location);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);

    // background = background.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /* Hacky transparent button */
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
    stopSelectTimers();
}

void paySelect::on_payPage_Button_clicked()
{
    // TODO: Grab from DB description
    string description = "Drink Flavor DrinkSizeOZ (DrinkML)";

    // FIXME: Remove this when DB price referencing/calculations are correct.
    if(idlePage->userDrinkOrder->getOption() == 9) {
        if(idlePage->userDrinkOrder->getSize() == idlePage->userDrinkOrder->SMALL_SIZE_ML) {
            idlePage->userDrinkOrder->setPrice(0.99);
        } else {
            idlePage->userDrinkOrder->setPrice(1.25);
        }
    }

    double drinkAmountDbl = idlePage->userDrinkOrder->getPrice();
    QString qs = QString::number(drinkAmountDbl, 'f', 2);

    string drinkAmount = qs.toUtf8().constData();

    drinkAmount.insert(0,"$ ");

    string drinkTotal = drinkAmount;

    connect(this, SIGNAL(paymentTotal(string,string,string)), this->paymentPage, SLOT(updateTotals(string,string,string)));
    emit(paymentTotal(description, drinkAmount, drinkTotal));

    char drinkSize;
    // FIXME: This is fucking terrible object use and magic values.
    // Cannot be long term fix, need to get rid of fullscreen hackery.
    if(idlePage->userDrinkOrder->getSize() == idlePage->userDrinkOrder->SMALL_SIZE_ML)
    {
        drinkSize = 's';

    } else {
        drinkSize = 'l';
    }

    // FIXME: Remove this when DB price referencing/calculations are correct.
    if(idlePage->userDrinkOrder->getOption() == 9) {
        if(drinkSize =='s') {
            idlePage->userDrinkOrder->setPrice(0.99);
        } else if (drinkSize == 'l') {
            idlePage->userDrinkOrder->setPrice(1.25);
        }
    }

    paymentPage->resizeEvent(paySelectResize, drinkSize);

    this->stopSelectTimers();
    paymentPage->showFullScreen();
    this->hide();
    qDebug() << idlePage->userDrinkOrder->getPrice();
}

void paySelect::resizeEvent(QResizeEvent *event){
    int checkOption = idlePage->userDrinkOrder->getOption();
    cout << checkOption << endl;
//    qDebug() << checkOption << endl;
    QString bitmap_location;

    if(checkOption > 0 && checkOption <= 9) {
        bitmap_location.append(":/light/4_pay_select_page_s_");
        bitmap_location.append(QString::number(idlePage->userDrinkOrder->getOption()));
        bitmap_location.append(".jpg");
    } else {
        bitmap_location = ":/light/4_pay_select_page_s.jpg";
    }

    QPixmap background(bitmap_location);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);

    // background = background.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    idlePage->userDrinkOrder->setDrinkSize(SMALL_DRINK);
    idlePage->userDrinkOrder->setPrice(idlePage->userDrinkOrder->PRICE_SMALL_TEST);
    this->setPalette(palette);

    if(selectIdleTimer == nullptr){
            qDebug() << "Start Select Timers" << endl;
        selectIdleTimer = new QTimer(this);
        selectIdleTimer->setInterval(1000);
        selectIdleTimer->isSingleShot();
        connect(selectIdleTimer, SIGNAL(timeout()), this, SLOT(on_previousPage_Button_clicked()));
        selectIdleTimer->start(10000);
    }
}

void paySelect::stopSelectTimers(){
    qDebug() << "Stop Select Timers" << endl;
    if(selectIdleTimer != nullptr) {
        qDebug() << "Enter Stop" << endl;
        selectIdleTimer->stop();

    }
        selectIdleTimer = nullptr;
}

void paySelect::on_mainPage_Button_clicked()
{

    this->stopSelectTimers();
    idlePage->showFullScreen();
    this->hide();
}

// on_Small_Order button listener
void paySelect::on_orderSmall_Button_clicked()
{
    QString bitmap_location;

    bitmap_location.append(":/light/4_pay_select_page_s_");
    bitmap_location.append(QString::number(idlePage->userDrinkOrder->getOption()));
    bitmap_location.append(".jpg");

    QPixmap background(bitmap_location);
    background = background.scaled(this->size(), Qt::KeepAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    idlePage->userDrinkOrder->setDrinkSize(SMALL_DRINK);
    idlePage->userDrinkOrder->setPrice(idlePage->userDrinkOrder->PRICE_SMALL_TEST);
}

// on_Large_Order button listener
void paySelect::on_orderBig_Button_clicked()
{
    QString bitmap_location;

    bitmap_location.append(":/light/4_pay_select_page_l_");
    bitmap_location.append(QString::number(idlePage->userDrinkOrder->getOption()));
    bitmap_location.append(".jpg");

    QPixmap background(bitmap_location);
    background = background.scaled(this->size(), Qt::KeepAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    idlePage->userDrinkOrder->setDrinkSize(LARGE_DRINK);
    idlePage->userDrinkOrder->setPrice(idlePage->userDrinkOrder->PRICE_LARGE_TEST);
}


/* Models */

// Function setDrinkOrder
