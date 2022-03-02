//***************************************
//
// pageproductsoverview.cpp
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

#include "pageproductsoverview.h"
#include "ui_pageproductsoverview.h"

#include "payselect.h"
#include "idle.h"

// CTOR
pageproductsoverview::pageproductsoverview(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pageproductsoverview)
{
    ui->setupUi(this);
    QPixmap background("/release/references/general/2_background_products.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    ui->mainPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
    ui->p_page_maintenanceButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
    
    ui->selection1_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
    ui->selection2_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
    ui->selection3_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget
    ui->selection4_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget

    productPageEndTimer = new QTimer(this);
    productPageEndTimer->setInterval(1000);
    connect(productPageEndTimer, SIGNAL(timeout()), this, SLOT(onProductPageTimeoutTick()));
}

/*
 * Page Tracking reference
 */
void pageproductsoverview::setPage(paySelect *pageSizeSelect, idle* pageIdle, page_maintenance *pageMaintenance, help *pageHelp)
{
    //this->selection_PageTwo = pageTwoProducts;
    this->paymentSelectPage = pageSizeSelect;
    this->idlePage = pageIdle;
    this->p_page_maintenance = pageMaintenance;
    this->helpPage = pageHelp;
}

// DTOR
pageproductsoverview::~pageproductsoverview()
{
    delete ui;
}

//void pageproductsoverview::on_nextPageButton_clicked()
//{
//    selection_PageTwo->showFullScreen();
//    this->hide();
//}

void pageproductsoverview::showEvent(QShowEvent *event)
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
        ui->selection1_Button->setStyleSheet("QPushButton { border-image: url(/references/general/soldOut.png); }");
    }
    else{
        ui->selection1_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    }

    if (!db.checkLevels(2)){
        // Change p2 to Sold Out
//        qDebug() << "Product 2 is Sold Out!" << endl;
        ui->selection2_Button->setStyleSheet("QPushButton { border-image: url(/references/general/soldOut.png); }");
    }
    else{
        ui->selection2_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    }
    
    if (!db.checkLevels(3)){
        // Change p3 to Sold Out
//        qDebug() << "Product 3 is Sold Out!" << endl;
        ui->selection3_Button->setStyleSheet("QPushButton { border-image: url(/references/general/soldOut.png); }");
    } 
    else{
        ui->selection3_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    }

    if (!db.checkLevels(4)){
        // Change p4 to Sold Out
//        qDebug() << "Product 4 is Sold Out!" << endl;
        ui->selection4_Button->setStyleSheet("QPushButton { border-image: url(/references/general/soldOut.png); }");
    }
    else{
        ui->selection4_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    }

    db.closeDB();
}

void pageproductsoverview::cancelTimers(){
    productPageEndTimer->stop();
}


// FIXME: This is terrible...no time to make array reference to hold button press functions
void pageproductsoverview::on_selection1_Button_clicked()
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

void pageproductsoverview::on_selection2_Button_clicked()
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

void pageproductsoverview::on_selection3_Button_clicked()
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

void pageproductsoverview::on_selection4_Button_clicked()
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

void pageproductsoverview::onProductPageTimeoutTick(){
    if(-- _productPageTimeoutSec >= 0) {
        qDebug() << "Tick Down: " << _productPageTimeoutSec << endl;
    } else {
        qDebug() << "Timer Done!" << _productPageTimeoutSec << endl;
        mainPage();
    }
}

void pageproductsoverview::mainPage()
{
    productPageEndTimer->stop();
    idlePage->showFullScreen();
    this->hide();
}

void pageproductsoverview::on_p_page_maintenanceButton_pressed()
{
//    qDebug() << "Maintenance button pressed" << endl;
    maintenanceCounter++;
    if (maintenanceCounter > 15){
        productPageEndTimer->stop();
        p_page_maintenance->showFullScreen();
        this->hide();
    }
}

void pageproductsoverview::on_mainPage_Button_clicked()
{
//    qDebug() << "Main Page Button pressed" << endl;
    productPageEndTimer->stop();
    helpPage->showFullScreen();
    this->hide();
}
