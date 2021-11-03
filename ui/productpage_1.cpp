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
    // TODO: Initialize By option i.e. Brand1, BrandImage1, DrinkFlavor1, DrinkImageButton with Array or loop...
    //    ui->nextPageButton->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->selection1_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
//    ui->Brand1->setText("");
//    ui->BrandImage1->clear();
//    ui->DrinkFlavor1->setText("");

    ui->selection2_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->selection3_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->selection4_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->mainPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
//    ui->selection5_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
//    ui->selection6_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    //ui->backButton->setStyleSheet("QPushButton{background: white;}");
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

    maintenanceCounter=0;

    if(productPageEndTimer == nullptr){
        productPageEndTimer = new QTimer(this);
        productPageEndTimer->setInterval(1000);
        connect(productPageEndTimer, SIGNAL(timeout()), this, SLOT(onProductPageTimeoutTick()));
    }

    productPageEndTimer->start(1000);
    _productPageTimeoutSec = 15;

    if (!this->idlePage->isEnough(1)){
        // Change p1 to Sold Out
//        qDebug() << "Product 1 is Sold Out!" << endl;
        ui->selection1_Button->setStyleSheet("QPushButton { border-image: url(:/light/soldOut.png); }");
    }
    else{
        ui->selection1_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    }
    if (!this->idlePage->isEnough(2)){
        // Change p2 to Sold Out
//        qDebug() << "Product 2 is Sold Out!" << endl;
        ui->selection2_Button->setStyleSheet("QPushButton { border-image: url(:/light/soldOut.png); }");
    }
    else{
        ui->selection2_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    }
    if (!this->idlePage->isEnough(3)){
        // Change p3 to Sold Out
//        qDebug() << "Product 3 is Sold Out!" << endl;
        ui->selection3_Button->setStyleSheet("QPushButton { border-image: url(:/light/soldOut.png); }");
    }
    else{
        ui->selection3_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    }
    if (!this->idlePage->isEnough(4)){
        // Change p4 to Sold Out
//        qDebug() << "Product 4 is Sold Out!" << endl;
        ui->selection4_Button->setStyleSheet("QPushButton { border-image: url(:/light/soldOut.png); }");
    }
    else{
        ui->selection4_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    }

}

void productPage_1::cancelTimers(){
    productPageEndTimer->stop();
}


// FIXME: This is terrible...no time to make array reference to hold button press functions
void productPage_1::on_selection1_Button_clicked()
{
    // UPDATE DB
    DbManager db(DB_PATH);
    db.addPageClick("Product Page -> Option 1");

    if(this->idlePage->isEnough(1)){
        productPageEndTimer->stop();

        idlePage->userDrinkOrder->setDrinkOption(OPTION_SLOT_1);
        idlePage->userDrinkOrder->setDrinkSize(LARGE_DRINK);
        //idlePage->userDrinkOrder->setDrinkSize(DRINK1);
        paymentSelectPage->resizeEvent(productResize);
        paymentSelectPage->showFullScreen();
        usleep(100);
        this->hide();
    }

    db.closeDB();
}


void productPage_1::on_selection2_Button_clicked()
{
    DbManager db(DB_PATH);
    db.addPageClick("Product Page -> Option 2");

    if(this->idlePage->isEnough(2)){
        productPageEndTimer->stop();

        idlePage->userDrinkOrder->setDrinkOption(OPTION_SLOT_2);
        idlePage->userDrinkOrder->setDrinkSize(LARGE_DRINK);
        //idlePage->userDrinkOrder->setDrinkSize(DRINK2);
        paymentSelectPage->resizeEvent(productResize);
        paymentSelectPage->showFullScreen();
        usleep(100);
        this->hide();
    }

    db.closeDB();
}

void productPage_1::on_selection3_Button_clicked()
{
    DbManager db(DB_PATH);
    db.addPageClick("Product Page -> Option 3");

    if(this->idlePage->isEnough(3)){
        productPageEndTimer->stop();

        idlePage->userDrinkOrder->setDrinkOption(OPTION_SLOT_3);
        idlePage->userDrinkOrder->setDrinkSize(LARGE_DRINK);
        //idlePage->userDrinkOrder->setDrinkSize(DRINK3);
        paymentSelectPage->resizeEvent(productResize);
        paymentSelectPage->showFullScreen();
        usleep(100);
        this->hide();
    }

    db.closeDB();
}

void productPage_1::on_selection4_Button_clicked()
{
    DbManager db(DB_PATH);
    db.addPageClick("Product Page -> Option 4");

    if(this->idlePage->isEnough(4)){
        productPageEndTimer->stop();

        idlePage->userDrinkOrder->setDrinkOption(OPTION_SLOT_4);
        idlePage->userDrinkOrder->setDrinkSize(LARGE_DRINK);
        //idlePage->userDrinkOrder->setDrinkSize(DRINK4);
        paymentSelectPage->resizeEvent(productResize);
        paymentSelectPage->showFullScreen();
        usleep(100);
        this->hide();
    }

    db.closeDB();
}

//void productPage_1::on_selection5_Button_clicked()
//{
//    // XXX: For BNTL Selection 2 goes to OPTION 9
//    idlePage->userDrinkOrder->setDrinkOption(OPTION_SLOT_5);

//    // idlePage->userDrinkOrder->setDrinkOption(OPTION_SLOT_5);
//    idlePage->userDrinkOrder->setDrinkSize(SMALL_DRINK);
//    paymentSelectPage->resizeEvent(productResize);
//    paymentSelectPage->showFullScreen();
//    this->hide();
//}

//void productPage_1::on_selection6_Button_clicked()
//{
//    idlePage->userDrinkOrder->setDrinkOption(OPTION_SLOT_6);
//    idlePage->userDrinkOrder->setDrinkSize(SMALL_DRINK);
//    paymentSelectPage->resizeEvent(productResize);
//    paymentSelectPage->showFullScreen();
//    this->hide();
//}

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
    this->hide();
    idlePage->showFullScreen();
}

//void productPage_1::on_backButton_clicked()
//{
//    DbManager db(DB_PATH);
//    db.addPageClick("Product Page -> Main Page");

//    mainPage();
//}

void productPage_1::on_maintenanceModeButton_pressed()
{
//    qDebug() << "Maintenance button pressed" << endl;
    maintenanceCounter++;
    if (maintenanceCounter > 15){
        productPageEndTimer->stop();
        maintenanceMode->showFullScreen();
        this->hide();
        //maintenanceCounter=0;
    }

}

void productPage_1::on_mainPage_Button_clicked()
{
//    qDebug() << "Main Page Button pressed" << endl;

    productPageEndTimer->stop();

//    qDebug() << "productPage: helpPage button" << endl;
    helpPage->showFullScreen();
    usleep(100);
    this->hide();

//    QMessageBox msgBox;
//    msgBox.setWindowFlags(Qt::FramelessWindowHint);
//    //msgBox.setText("<p align=center>How Can We Help You?<br><br></p>");
//    msgBox.setInformativeText("<p align=center>Phone: (604) 837-5066<br></p><p align=center>Email: hello@drinkfill.com<br></p>");
//    msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px;} QPushButton{font-size: 18px; min-height: 30px; align: center}");

//    msgBox.setStandardButtons(QMessageBox::Close);
//    int ret = msgBox.exec();

//    switch(ret){
//        case QMessageBox::Close:
//            qDebug() << "Return CLICKED" << endl;
//            msgBox.hide();
//            productPageEndTimer->start(1000);
//            _productPageTimeoutSec = 15;
//        break;

//    }
}
