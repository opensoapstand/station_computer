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
    setStyleSheet("QPushButton{background: transparent;}");
    ui->maintenanceModeButton->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

    // TODO: Hold and pass DrinkOrder Object
    userDrinkOrder = new DrinkOrder();
    userDrinkOrder->setDrinkOption(OPTION_SLOT);

    // IPC Networking
    dfUtility = new df_util();
    dfUtility->m_IsSendingFSM = false;

    // TODO: Will need to determine standard path in future; Could skip if going with Postgres
    // this->dfUtility->getVendorDetails();

    maintenanceTimer = new QTimer(this);
    maintenanceTimer->setInterval(1000);
    connect(maintenanceTimer, SIGNAL(timeout()), this, SLOT(onMaintenanceTimeoutTick()));
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

/*
 * Screen click shows product page as full screen and hides idle screen
 */
void idle::on_nextPageButton_clicked()
{

    // UPDATE DB
    DbManager db(DB_PATH);
    db.addPageClick("Main Page -> Product Page");

    //Check product levels
    p1 = db.checkLevels(1);
    p2 = db.checkLevels(2);
    p3 = db.checkLevels(3);
    p4 = db.checkLevels(4);

    // Connect to KB Listener
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
    default:
            return false;
            break;
    }
}


void idle::on_maintenanceModeButton_pressed()
{
    qDebug() << "Maintenance button pressed" << endl;
    if(maintenanceTimer == nullptr){
        maintenanceTimer = new QTimer(this);
        maintenanceTimer->setInterval(1000);
        connect(maintenanceTimer, SIGNAL(timeout()), this, SLOT(onMaintenanceTimeoutTick()));
    }

    maintenanceTimer->start(1000);
    _maintenanceTimeoutSec = 10;
}

void idle::on_maintenanceModeButton_released()
{
    qDebug() << "Maintenance button released" << endl;
    if (_maintenanceTimeoutSec > 0){
        qDebug() << "Sec: " << _maintenanceTimeoutSec << endl;
        maintenanceTimer->stop();
        _maintenanceTimeoutSec = 10;
        on_nextPageButton_clicked();
    }
    else{
        maintenanceTimer->stop();
        _maintenanceTimeoutSec = 10;
    }
}

void idle::open_maintenance_mode()
{
    maintenanceTimer->stop();
    maintenanceMode->showFullScreen();
    this->hide();
}

void idle::onMaintenanceTimeoutTick(){
    if(-- _maintenanceTimeoutSec >= 0) {
        qDebug() << "Maintenance Tick Down: " << _maintenanceTimeoutSec << endl;
    } else {
        qDebug() << "Maintenace Timer Done!" << _maintenanceTimeoutSec << endl;
        open_maintenance_mode();
    }
}
