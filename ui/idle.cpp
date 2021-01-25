//***************************************
//
// idle.cpp
// GUI class while machine is idle.
//
// Display Fullscreen DF branded Wallpaper
// Listen for User interaction to load
// Product Page1
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "idle.h"
#include "ui_idle.h"

// CTOR
idle::idle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::idle)
{
    // Background Set here; Inheritance on forms places image on all elements otherwise.
    ui->setupUi(this);
    QPixmap background(":/light/1_welcome_panel.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /* Issues with QT buttons with images.  Button behind transparent image hack.
       TODO: find a way to make the button transparent/clickable image */
    ui->nextPageButton->setAttribute(Qt::WA_TranslucentBackground);
    ui->nextPageButton->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    setStyleSheet("QPushButton{background: transparent;}");

    // TODO: Hold and pass DrinkOrder Object
    userDrinkOrder = new DrinkOrder();
    userDrinkOrder->setDrinkOption(OPTION_SLOT);

    // IPC Networking
    dfUtility = new df_util();
    dfUtility->m_IsSendingFSM = false;

    // TODO: Will need to determine standard path in future; Could skip if going with Postgres
    // this->dfUtility->getVendorDetails();


}

/*
 * Navigation to Product item
 */
void idle::setPage(productPage_1 *pageProduct)
{
    // Chained to KB Listener
    this->selection_PageOne = pageProduct;
}

// DTOR
idle::~idle()
{
    delete ui;
}

/*
 * Screen click shows product page as full screen and hides idle screen
 */
void idle::on_nextPageButton_clicked()
{

    // UPDATE DB
    DbManager db(DB_PATH);
    db.addPageClick("Main Page -> Product Page");

//    DbManager db_product(DB_PRODUCTS_PATH);
//    db_product.getProductName(1);
//    db_product.getProductPrice(1);
//    db_product.getProductVolume(1);

//    db_product.getProductName(2);
//    db_product.getProductPrice(2);
//    db_product.getProductVolume(2);

//    db_product.getProductName(3);
//    db_product.getProductPrice(3);
//    db_product.getProductVolume(3);

//    db_product.getProductName(4);
//    db_product.getProductPrice(4);
//    db_product.getProductVolume(4);

    // Connect to KB Listener
    selection_PageOne->showFullScreen();
    this->hide();
}
