//***************************************
//
// page_idle.cpp
// GUI class while machine is page_idle.
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

#include "page_idle.h"
#include "ui_page_idle.h"
#include "page_maintenance.h"

// CTOR
page_idle::page_idle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_idle)
{
    // Background Set here; Inheritance on forms places image on all elements otherwise.
    ui->setupUi(this);
    QPixmap background("/home/df-admin/drinkfill/production/references/1_background_welcome.png");
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

bool page_idle::isSlotAvailable(int slot){
    return this->slotIndexAvailable[slot - 1];
}
void page_idle::setSlotAvailability(int slot, bool isEnabled){
    this->slotIndexAvailable[slot - 1] = isEnabled;
}

/*
 * Navigation to Product item
 */
void page_idle::setPage(page_select_product *p_pageProduct, page_maintenance *pageMaintenance)
{
    // Chained to KB Listener
    this->p_pageSelectProduct = p_pageProduct;
    this->p_page_maintenance = pageMaintenance;
}

// DTOR
page_idle::~page_idle()
{
    delete ui;
}

void page_idle::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
//    DbManager db(DB_PATH);
    //ui->savedBottles_label->setText("THANKS TO YOU, THIS MACHINE HAS SAVED<br>OVER " + QString::number(db.getTotalTransactions()) + " PLASTIC CONTAINERS<br>FROM THE LANDFILL");
}

/*
 * Screen click shows product page as full screen and hides page_idle screen
 */

void page_idle::on_nextPageButton_clicked()
{

    // qDebug() << "SEND LARGE FILL DEBUG TEST" << endl;
    // this->dfUtility->msg = "1dl;";
    // this->dfUtility->m_IsSendingFSM = true;
    // this->dfUtility->m_fsmMsg = SEND_DISPENSE_STOP;
    // this->dfUtility->send_to_FSM();

    p_pageSelectProduct->showFullScreen();

    // DO NOT HIDE IDLE PAGE 
    // it's staying in the background to counter a hack UBC students found (when changing screens and tapping during the swap, they could get a hold of the machine)
    // Tapping on on the desktop wallpaper minimizes the application.
    // If the idle page is not hidden, and always on the background, there is never a wall paper showing. Effectively preventing this vulnerability to be exploited.
    //this->hide(); 
}

bool page_idle::isEnough(int p){
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

void page_idle::MMSlot(){
    qDebug() << "Signal: Enter maintenance mode" << endl;
    p_page_maintenance->showFullScreen();
    qDebug() << "123aefas";
    this->hide();
    qDebug() << "123fh4f";
    this->p_pageSelectProduct->hide();
    qDebug() << "123gheeeef";
}
