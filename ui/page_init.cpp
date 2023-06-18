//***************************************
//
// page_init.cpp
// GUI class while machine is initializing.
//
// Display Fullscreen DF branded Wallpaper
// Listen for User interaction to load
// Idle Page
//
// created: 16-06-2023
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
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
    p_page_idle->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    // load template texts
    p_page_idle->loadTextsFromTemplateCsv();
    p_page_idle->loadTextsFromDefaultCsv();

    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_INIT_BACKGROUND_IMAGE_PATH);

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
        QString paymentMethod = p_page_idle->products[0].getPaymentMethod();
        if(paymentMethod == "tapTcp"){
            QCoreApplication::processEvents();
            page_tap_payment paymentObject;
            paymentObject.initiate_tap_setup();
        }
        else if(paymentMethod=="tapSerial"){
            QCoreApplication::processEvents();
            page_tap_payment_serial paymentSerialObject;
            paymentSerialObject.tap_serial_initiate();
        }
        p_page_idle->setTemplateTextToObject(ui->label_init_message);
        _initIdleTimeoutSec = 1;
    }
}

void page_init::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    p_page_idle->pageTransition(this, pageToShow);
    initIdleTimer->stop();
    rebootTimer->stop();
}

void page_init::initReadySlot(void)
{
    qDebug() << "Signal: init ready from fsm";
    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_init::onInitTimeoutTick()
{
    if (--_initIdleTimeoutSec >= 0)
    {
        ui->label_init_message->setText(ui->label_init_message->text() + ".");
    }
    else
    {
        qDebug() << "No response from controller. Will reboot";
        initIdleTimer->stop();
        p_page_idle->setTemplateTextToObject(ui->label_fail_message);
        hideCurrentPageAndShowProvided(p_page_idle);
    }
}

void page_init::onRebootTimeoutTick()
{
    if (--_rebootTimeoutSec >= 0)
    {
        qDebug() << "init: Reboot Tick Down - " << _rebootTimeoutSec;
    }
    else
    {
        qDebug() << "Reboot Timer elapsed. (should reboot computer now)" << _rebootTimeoutSec;
        rebootTimer->stop();

        // REBOOT!
        system("./release/reboot.sh");
    }
}

