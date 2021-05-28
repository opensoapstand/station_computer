
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

    QString bitmap_location;

        if(checkOption > 0 && checkOption <= 6) {
            bitmap_location.append(":/light/4_pay_select_page_l_");
            bitmap_location.append(QString::number(idlePage->userDrinkOrder->getOption()));
            bitmap_location.append(".png");
        } else {
            bitmap_location = ":/light/4_pay_select_page_l_1.png";
        }

    QPixmap background(bitmap_location);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);

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


    {
        selectIdleTimer = new QTimer(this);
        selectIdleTimer->setInterval(1000);
//        selectIdleTimer->isSingleShot();
        connect(selectIdleTimer, SIGNAL(timeout()), this, SLOT(onSelectTimeoutTick()));
    }

}

/*
 * Page Tracking reference to Select Drink, Payment Page and Idle page
 */
void paySelect::setPage(productPage_1 *pageSelect, dispensePage* pageDispense, idle* pageIdle, payPage* pagePayment)
{
    this->firstProductPage = pageSelect;
    this->paymentPage = pagePayment;
    this->idlePage = pageIdle;
    this->dispensingPage = pageDispense;
}

// DTOR
paySelect::~paySelect()
{
    delete ui;
}

/* GUI */
void paySelect::on_previousPage_Button_clicked()
{
    //Update Click DB
    DbManager db(DB_PATH);
    db.addPageClick("Pay Select -> Product Page");

    qDebug() << "paySelect: Previous button" << endl;
    while(!stopSelectTimers()){

    };
    selectIdleTimer->stop();
    firstProductPage->showFullScreen();
    this->hide();

}

void paySelect::on_payPage_Button_clicked()
{
    qDebug() << "paySelect: Pay button" << endl;

    //Update Click DB
    DbManager db(DB_PATH);
    db.addPageClick("Pay Select -> Dispense Page");

    //double drinkAmountDbl = idlePage->userDrinkOrder->getPrice();
    //QString qs = QString::number(drinkAmountDbl, 'f', 2);

    //string drinkAmount = qs.toUtf8().constData();

    //drinkAmount.insert(0,"$ ");

    //string drinkTotal = drinkAmount;

    char drinkSize;

    if (idlePage->userDrinkOrder->getSizeOption() == SMALL_DRINK){
        drinkSize = 's';
        paymentPage->resizeEvent(paySelectResize, drinkSize);
    }
    if (idlePage->userDrinkOrder->getSizeOption() == LARGE_DRINK){
        drinkSize = 'l';
        paymentPage->resizeEvent(paySelectResize, drinkSize);
    }


    this->stopSelectTimers();
    selectIdleTimer->stop();

    if (db.getPaymentMethod(idlePage->userDrinkOrder->getOption()) == "tap"){
        //paymentPage->resizeEvent(paySelectResize, drinkSize);
        paymentPage->showFullScreen();
        this->hide();
    }else if (db.getPaymentMethod(idlePage->userDrinkOrder->getOption()) == "barcode" || db.getPaymentMethod(idlePage->userDrinkOrder->getOption()) == "plu"){
        dispensingPage->showEvent(dispenseEvent);
        this->hide();
        dispensingPage->showFullScreen();
    }

    //qDebug() << idlePage->userDrinkOrder->getPrice();
    //qDebug() << idlePage->userDrinkOrder->getSize();

}

void paySelect::resizeEvent(QResizeEvent *event){
    int checkOption = idlePage->userDrinkOrder->getOption();

    QString bitmap_location;

    if(checkOption > 0 && checkOption <= 9) {
        bitmap_location.append(":/light/4_pay_select_page_l_");
        bitmap_location.append(QString::number(idlePage->userDrinkOrder->getOption()));
        bitmap_location.append(".png");
    } else {
        bitmap_location = ":/light/4_pay_select_page_l_1.png";
    }

    QPixmap background(bitmap_location);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);

    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);
    idlePage->userDrinkOrder->setDrinkSize(LARGE_DRINK);

    if(selectIdleTimer == nullptr) {
        selectIdleTimer = new QTimer(this);
        selectIdleTimer->setInterval(1000);
        connect(selectIdleTimer, SIGNAL(timeout()), this, SLOT(onSelectTimeoutTick()));
    }

    qDebug() << "Start paySelect Timers" << endl;
    selectIdleTimer->start(1000);
    _selectIdleTimeoutSec = 40;
}

void paySelect::onSelectTimeoutTick(){
    if(-- _selectIdleTimeoutSec >= 0) {
        qDebug() << "paySelect: Tick Down - " << _selectIdleTimeoutSec << endl;
    } else {
        qDebug() << "Timer Done!" << _selectIdleTimeoutSec << endl;
        selectIdleTimer->stop();

        //Update Click DB
        DbManager db(DB_PATH_CLICKS);
        db.addPageClick("PAY SELECT TIME OUT");

        mainPage();
    }
}

bool paySelect::stopSelectTimers(){
    qDebug() << "Stop paySelect Timers" << endl;
    if(selectIdleTimer != nullptr) {
       // qDebug() << "Enter Stop" << endl;
        selectIdleTimer->stop();
        return true;
    } else {
        return false;
    }
}

void paySelect::mainPage()
{

    //Update Click DB
    DbManager db(DB_PATH);
    db.addPageClick("Pay Select -> Main Page");

    qDebug() << "paySelect: mainPage button" << endl;
    this->stopSelectTimers();
    selectIdleTimer->stop();
    idlePage->showFullScreen();
    this->hide();
}

void paySelect::on_mainPage_Button_clicked()
{

    //Update Click DB
//    DbManager db(DB_PATH);
//    db.addPageClick("Pay Select -> Main Page");

//    qDebug() << "paySelect: mainPage button" << endl;
//    this->stopSelectTimers();
//    selectIdleTimer->stop();
//    idlePage->showFullScreen();
//    this->hide();

    qDebug() << "Main Page Button pressed" << endl;

    selectIdleTimer->stop();

    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::FramelessWindowHint);
    //msgBox.setText("<p align=center>How Can We Help You?<br><br></p>");
    msgBox.setInformativeText("<p align=center>Phone: (604) 837-5066<br></p><p align=center>Email: hello@drinkfill.com<br></p>");
    msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-height: 30px; align: center}");

    msgBox.setStandardButtons(QMessageBox::Close);
    int ret = msgBox.exec();

    switch(ret){
        case QMessageBox::Close:
            qDebug() << "Return CLICKED" << endl;
            msgBox.hide();
            selectIdleTimer->start(1000);
            _selectIdleTimeoutSec = 40;
        break;

    }

}

// on_Small_Order button listener
void paySelect::on_orderSmall_Button_clicked()
{
    QString bitmap_location;

    bitmap_location.append(":/light/4_pay_select_page_s_");
    bitmap_location.append(QString::number(idlePage->userDrinkOrder->getOption()));
    bitmap_location.append(".png");

    QPixmap background(bitmap_location);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    idlePage->userDrinkOrder->setDrinkSize(SMALL_DRINK);
    _selectIdleTimeoutSec = 40;

    DbManager db(DB_PATH);
    db.addPageClick("Small Drink Size Selected");
}

// on_Large_Order button listener
void paySelect::on_orderBig_Button_clicked()
{
    QString bitmap_location;

    bitmap_location.append(":/light/4_pay_select_page_l_");
    bitmap_location.append(QString::number(idlePage->userDrinkOrder->getOption()));
    bitmap_location.append(".png");

    QPixmap background(bitmap_location);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    idlePage->userDrinkOrder->setDrinkSize(LARGE_DRINK);
    _selectIdleTimeoutSec = 40;

    DbManager db(DB_PATH);
    db.addPageClick("Large Drink Size Selected");
}
