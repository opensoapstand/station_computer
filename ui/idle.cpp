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
#include "maintenancePage.h"

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
    ui->nextPageButton->raise();

    // TODO: Hold and pass DrinkOrder Object
    userDrinkOrder = new DrinkOrder();
    userDrinkOrder->setDrinkOption(OPTION_SLOT);

    // IPC Networking
    dfUtility = new df_util();
    dfUtility->m_IsSendingFSM = false;

}

/*
 * Navigation to Product item
 */
void idle::setPage(productPage_1 *pageProduct, maintenancePage *pageMaintenance)
{
    // Chained to KB Listener
    this->selection_PageOne = pageProduct;
    this->maintenanceMode = pageMaintenance;
}

// DTOR
idle::~idle()
{
    delete ui;
}

void idle::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
//    DbManager db(DB_PATH);
    //ui->savedBottles_label->setText("THANKS TO YOU, THIS MACHINE HAS SAVED<br>OVER " + QString::number(db.getTotalTransactions()) + " PLASTIC CONTAINERS<br>FROM THE LANDFILL");
}

/*
 * Screen click shows product page as full screen and hides idle screen
 */

void idle::on_nextPageButton_clicked()
{
    selection_PageOne->showFullScreen();
    this->hide();
}

bool idle::isEnough(int p){
    switch(p){
        case(1):
            return p1;
            break;
        case(2):
            return p2;
            break;
        case(3):
            return p3;
            break;
        case(4):
            return p4;
            break;
    }
}

void idle::MMSlot(){
//    qDebug() << "HERE I AM I KNOW I MUIST ENTER MM" << endl;
    maintenanceMode->showFullScreen();
    this->hide();
    this->selection_PageOne->hide();
}
