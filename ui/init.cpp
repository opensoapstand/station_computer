//***************************************
//
// init.cpp
// GUI class while machine is initializing.
//
// Display Fullscreen DF branded Wallpaper
// Listen for User interaction to load
// Idle Page
//
// created: 16-04-2021
// by: Paddy Riley
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "init.h"
#include "idle.h"
#include "ui_init.h"

// CTOR
init::init(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::init)
{
    // Background Set here; Inheritance on forms places image on all elements otherwise.
    ui->setupUi(this);
    QPixmap background("/release/references/general/0_background_init.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    // IPC Networking
    dfUtility = new df_util();
    dfUtility->m_IsSendingFSM = false;

    initIdleTimer = new QTimer(this);
    initIdleTimer->setInterval(1000);
    connect(initIdleTimer, SIGNAL(timeout()), this, SLOT(onInitTimeoutTick()));

    rebootTimer = new QTimer(this);
    rebootTimer->setInterval(1000);
    connect(rebootTimer, SIGNAL(timeout()), this, SLOT(onRebootTimeoutTick()));

}

/*
 * Navigation to Product item
 */
void init::setPage(idle *pageIdle)
{
    // Chained to KB Listener
    this->idlePage = pageIdle;
}

// DTOR
init::~init()
{
    delete ui;
}

void init::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
//    qDebug() << "Start init Timers" << endl;
    initIdleTimer->start(1000);
    _initIdleTimeoutSec = 20;
    system("DISPLAY=:0 xterm -hold  /release/fsm/controller &");
}

void init::initReadySlot(void){
    initIdleTimer->stop();
    idlePage->showFullScreen();
    this->hide();
}

void init::onInitTimeoutTick(){
    if(-- _initIdleTimeoutSec >= 0) {
//        qDebug() << "init: Tick Down - " << _initIdleTimeoutSec << endl;
        ui->init_label->setText(ui->init_label->text() + ".");
    } else {
//        qDebug() << "Timer Done!" << _initIdleTimeoutSec << endl;
        initIdleTimer->stop();

        ui->fail_label->setText("Init failed. Rebooting");

        rebootTimer->start(1000);
        _rebootTimeoutSec = 5;


    }
}

void init::onRebootTimeoutTick(){
    if(-- _rebootTimeoutSec >= 0) {
//        qDebug() << "init: Reboot Tick Down - " << _rebootTimeoutSec << endl;
        ui->fail_label->setText(ui->fail_label->text() + ".");
    } else {
//        qDebug() << "Reboot Timer Done!" << _rebootTimeoutSec << endl;
        rebootTimer->stop();

        //REBOOT!
        system("./release/reboot.sh");
    }
}
