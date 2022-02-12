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
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "productpage_1.h"
#include "ui_productpage_1.h"

#include "productpage_2.h"
#include "payselect.h"
#include "idle.h"

// CTOR
productPage_1::productPage_1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::productPage_1)
{
    ui->setupUi(this);
    QPixmap background(":/light/2_drink_page_1.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    // TODO:  Populate Page Drink buttons with DrinkSelection[0-5]

    /*hacky transparent button*/
    ui->selection1_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->selection2_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->selection3_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->selection4_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->mainPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->maintenanceModeButton->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

    productPageEndTimer = new QTimer(this);
    productPageEndTimer->setInterval(1000);
    connect(productPageEndTimer, SIGNAL(timeout()), this, SLOT(onProductPageTimeoutTick()));
}

/*
 * Page Tracking reference
 */
void productPage_1::setPage(productPage_2 *pageTwoProducts, paySelect *pageSizeSelect, idle* pageIdle, maintenancePage *pageMaintenance, help *pageHelp)
{
    this->selection_PageTwo = pageTwoProducts;
    this->paymentSelectPage = pageSizeSelect;
    this->idlePage = pageIdle;
    this->maintenanceMode = pageMaintenance;
    this->helpPage = pageHelp;
}

// DTOR
productPage_1::~productPage_1()
{
    delete ui;
}

//void productPage_1::on_nextPageButton_clicked()
//{
//    selection_PageTwo->showFullScreen();
//    this->hide();
//}

void productPage_1::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    DbManager db(DB_PATH);

    maintenanceCounter=0;

    if(productPageEndTimer == nullptr){
        productPageEndTimer = new QTimer(this);
        productPageEndTimer->setInterval(1000);
        connect(productPageEndTimer, SIGNAL(timeout()), this, SLOT(onProductPageTimeoutTick()));
    }

    productPageEndTimer->start(1000);
    _productPageTimeoutSec = 15;

    if (!db.checkLevels(1)){
        // Change p1 to Sold Out
//        qDebug() << "Product 1 is Sold Out!" << endl;
        ui->selection1_Button->setStyleSheet("QPushButton { border-image: url(:/light/soldOut.png); }");
    }
    else{
        ui->selection1_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    }
    if (!db.checkLevels(2)){
        // Change p2 to Sold Out
//        qDebug() << "Product 2 is Sold Out!" << endl;
        ui->selection2_Button->setStyleSheet("QPushButton { border-image: url(:/light/soldOut.png); }");
    }
    else{
        ui->selection2_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    }
    if (!db.checkLevels(3)){
        // Change p3 to Sold Out
//        qDebug() << "Product 3 is Sold Out!" << endl;
        ui->selection3_Button->setStyleSheet("QPushButton { border-image: url(:/light/soldOut.png); }");
    }
    else{
        ui->selection3_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    }
    if (!db.checkLevels(4)){
        // Change p4 to Sold Out
//        qDebug() << "Product 4 is Sold Out!" << endl;
        ui->selection4_Button->setStyleSheet("QPushButton { border-image: url(:/light/soldOut.png); }");
    }
    else{
        ui->selection4_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    }

    db.closeDB();
}

void productPage_1::cancelTimers(){
    productPageEndTimer->stop();
}


// FIXME: This is terrible...no time to make array reference to hold button press functions
void productPage_1::on_selection1_Button_clicked()
{
    DbManager db(DB_PATH);
    if(db.checkLevels(1)){
        db.closeDB();
        productPageEndTimer->stop();
        idlePage->userDrinkOrder->setDrinkOption(OPTION_SLOT_1);
        idlePage->userDrinkOrder->setDrinkSize(LARGE_DRINK);
        paymentSelectPage->resizeEvent(productResize);
        paymentSelectPage->showFullScreen();
        this->hide();
    }else{
        db.closeDB();
    }
}

void productPage_1::on_selection2_Button_clicked()
{
    DbManager db(DB_PATH);
    if(db.checkLevels(2)){
        db.closeDB();
        productPageEndTimer->stop();
        idlePage->userDrinkOrder->setDrinkOption(OPTION_SLOT_2);
        idlePage->userDrinkOrder->setDrinkSize(LARGE_DRINK);
        paymentSelectPage->resizeEvent(productResize);
        paymentSelectPage->showFullScreen();
        this->hide();
    }else{
        db.closeDB();
    }
}

void productPage_1::on_selection3_Button_clicked()
{
    DbManager db(DB_PATH);
    if(db.checkLevels(3)){
        db.closeDB();
        productPageEndTimer->stop();
        idlePage->userDrinkOrder->setDrinkOption(OPTION_SLOT_3);
        idlePage->userDrinkOrder->setDrinkSize(LARGE_DRINK);
        paymentSelectPage->resizeEvent(productResize);
        paymentSelectPage->showFullScreen();
        this->hide();
    }else{
        db.closeDB();
    }

}

void productPage_1::on_selection4_Button_clicked()
{
    DbManager db(DB_PATH);
    if(db.checkLevels(4)){
        db.closeDB();
        productPageEndTimer->stop();
        idlePage->userDrinkOrder->setDrinkOption(OPTION_SLOT_4);
        idlePage->userDrinkOrder->setDrinkSize(LARGE_DRINK);
        paymentSelectPage->resizeEvent(productResize);
        paymentSelectPage->showFullScreen();
        this->hide();
    }else{
        db.closeDB();
    }

}

void productPage_1::onProductPageTimeoutTick(){
    if(-- _productPageTimeoutSec >= 0) {
//        qDebug() << "Tick Down: " << _productPageTimeoutSec << endl;
    } else {
//        qDebug() << "Timer Done!" << _productPageTimeoutSec << endl;
        mainPage();
    }
}

void productPage_1::mainPage()
{
    productPageEndTimer->stop();
    idlePage->showFullScreen();
    this->hide();
}

void productPage_1::on_maintenanceModeButton_pressed()
{
//    qDebug() << "Maintenance button pressed" << endl;
    maintenanceCounter++;
    if (maintenanceCounter > 15){
        productPageEndTimer->stop();
        maintenanceMode->showFullScreen();
        this->hide();
    }
}

void productPage_1::on_mainPage_Button_clicked()
{
//    qDebug() << "Main Page Button pressed" << endl;
    productPageEndTimer->stop();
    helpPage->showFullScreen();
    this->hide();
}
