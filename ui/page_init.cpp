//***************************************
//
// page_init.cpp
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

#include "page_init.h"
#include "page_idle.h"
#include "ui_page_init.h"

// CTOR
page_init::page_init(QWidget *parent) : QWidget(parent),
                                        ui(new Ui::page_init)
{
    // Background Set here; Inheritance on forms places image on all elements otherwise.
    ui->setupUi(this);
   

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
void page_init::setPage(page_idle *pageIdle)
{
    // Chained to KB Listener
    this->p_page_idle = pageIdle;
}

// DTOR
page_init::~page_init()
{
    delete ui;
}

void page_init::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Init >>>>>>>>>";
    QWidget::showEvent(event);

     QPixmap background(PAGE_INIT_BACKGROUND_PATH);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);
    
    //    qDebug() << "Start init Timers" << endl;
    initIdleTimer->start(1000);
#ifdef START_FSM_FROM_UI
    start_controller = true;
#else
    start_controller = false;
#endif

    if (start_controller)
    {
        system("DISPLAY=:0 xterm -hold  /release/fsm/controller &");
        _initIdleTimeoutSec = 20;
    }
    else
    {
        ui->init_label->setText("Wait for controller signal.");
#ifdef WAIT_FOR_CONTROLLER_READY
        _initIdleTimeoutSec = 20;
#else
        _initIdleTimeoutSec = 1;
#endif
    }
}

void page_init::initReadySlot(void)
{
    qDebug() << "Signal: init ready from fsm";
    initIdleTimer->stop();
    rebootTimer->stop();
    // qDebug() << "init to idle";
    // p_page_idle->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_idle);
}

void page_init::onInitTimeoutTick()
{
    if (--_initIdleTimeoutSec >= 0)
    {
        //        qDebug() << "init: Tick Down - " << _initIdleTimeoutSec << endl;
        ui->init_label->setText(ui->init_label->text() + ".");
    }
    else
    {
        //        qDebug() << "Timer Done!" << _initIdleTimeoutSec << endl;
        initIdleTimer->stop();

        ui->fail_label->setText("Init Timeout. No response from controller.");

#ifdef WAIT_FOR_CONTROLLER_READY
        // if (!start_controller){
        // }
        ui->fail_label->setText("No response from controller. Will reboot.");
        _rebootTimeoutSec = 5;
        rebootTimer->start(1000);
#else
        ui->fail_label->setText("Will start standalone mode. If controller becomes active, commands will be executed. If not, no commands will be executed.");

        initReadySlot();
#endif
    }
}

void page_init::onRebootTimeoutTick()
{
    if (--_rebootTimeoutSec >= 0)
    {
        qDebug() << "init: Reboot Tick Down - " << _rebootTimeoutSec << endl;
        ui->fail_label->setText(ui->fail_label->text() + ".");
    }
    else
    {
        qDebug() << "Reboot Timer elapsed. (should reboot computer now)" << _rebootTimeoutSec << endl;
        rebootTimer->stop();

        // REBOOT!
        system("./release/reboot.sh");
    }
}
