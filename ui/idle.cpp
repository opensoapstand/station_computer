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
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "idle.h"
#include "ui_idle.h"
#include "page_maintenance.h"

// CTOR
idle::idle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::idle)
{
    // Background Set here; Inheritance on forms places image on all elements otherwise.
    ui->setupUi(this);
    QPixmap background("/release/references/general/1_background_welcome.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    ui->nextPageButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }"); // flat transparent button  https://stackoverflow.com/questions/29941464/how-to-add-a-button-with-image-and-transparent-background-to-qvideowidget

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
void idle::setPage(pageproductsoverview *p_pageProduct, page_maintenance *pageMaintenance)
{
    // Chained to KB Listener
    this->selection_PageOne = p_pageProduct;
    this->p_page_maintenance = pageMaintenance;
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

    // qDebug() << "SEND LARGE FILL DEBUG TEST" << endl;
    // this->dfUtility->msg = "1dl;";
    // this->dfUtility->m_IsSendingFSM = true;
    // this->dfUtility->m_fsmMsg = SEND_CLEAN;
    // this->dfUtility->send_to_FSM();

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
    p_page_maintenance->showFullScreen();
    this->hide();
    this->selection_PageOne->hide();
}
