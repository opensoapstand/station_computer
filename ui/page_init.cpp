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
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
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
    connect(this, SIGNAL(tapSetupInitialized()), this, SLOT(showIdlePage()));
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
    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    // template content
    p_page_idle->thisMachine->loadDynamicContent();

    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget
    p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_INIT_BACKGROUND_IMAGE_PATH);

    initIdleTimer->start(1000);
    paymentMethod = p_page_idle->thisMachine->getPaymentMethod();
    if (paymentMethod == PAYMENT_TAP_CANADA_QR){
        p_page_idle->thisMachine->setActivePaymentMethod(ActivePaymentMethod::tap_canada);
        p_page_idle->thisMachine->setAllowedPaymentMethods(ActivePaymentMethod::tap_canada);
        p_page_idle->thisMachine->setAllowedPaymentMethods(ActivePaymentMethod::qr);
    }
    else if (paymentMethod == PAYMENT_TAP_CANADA){
        p_page_idle->thisMachine->setActivePaymentMethod(ActivePaymentMethod::tap_canada);
        p_page_idle->thisMachine->setAllowedPaymentMethods(ActivePaymentMethod::tap_canada);
    }
    else if (paymentMethod == PAYMENT_TAP_USA_QR){
        p_page_idle->thisMachine->setActivePaymentMethod(ActivePaymentMethod::tap_usa);
        p_page_idle->thisMachine->setAllowedPaymentMethods(ActivePaymentMethod::tap_usa);
        p_page_idle->thisMachine->setAllowedPaymentMethods(ActivePaymentMethod::qr);
    }
    else if(paymentMethod == PAYMENT_TAP_USA){
        p_page_idle->thisMachine->setActivePaymentMethod(ActivePaymentMethod::tap_usa);
        p_page_idle->thisMachine->setAllowedPaymentMethods(ActivePaymentMethod::tap_usa);
    }
    else if(paymentMethod == PAYMENT_QR){
        p_page_idle->thisMachine->setActivePaymentMethod(ActivePaymentMethod::qr);
        p_page_idle->thisMachine->setAllowedPaymentMethods(ActivePaymentMethod::qr);
    }
    else{
        p_page_idle->thisMachine->setActivePaymentMethod(ActivePaymentMethod::receipt_printer);
        p_page_idle->thisMachine->setAllowedPaymentMethods(ActivePaymentMethod::receipt_printer);
    }
    activePaymentMethod = p_page_idle->thisMachine->getActivePaymentMethod();
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
        QString command = "Ping";
        p_page_idle->thisMachine->dfUtility->send_command_to_FSM(command, true);
        
        switch(activePaymentMethod){
            case tap_canada:
            case tap_usa:{
                // Thread setup for non-blocking tap payment initialization
                // Using bind for non-static functions
                auto bindFn = std::bind(&page_init::initiateTapPayment, this);
                tapInitThread = std::thread(bindFn);
                tapInitThread.detach();
                break;
            }
        }
        // if (paymentMethod == PAYMENT_TAP_USA || paymentMethod == PAYMENT_TAP_CANADA)
        // {
        //     // Thread setup for non-blocking tap payment initialization
        //     // Using bind for non-static functions
        //     auto bindFn = std::bind(&page_init::initiateTapPayment, this);
        //     tapInitThread = std::thread(bindFn);
        //     tapInitThread.detach();
        // }
    }
}

void page_init::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    p_page_idle->thisMachine->pageTransition(this, pageToShow);
    initIdleTimer->stop();
    rebootTimer->stop();
}

void page_init::initReadySlot(void)
{
    qDebug() << "Signal: init ready from fsm";
    switch(activePaymentMethod){
        case qr:
        case receipt_printer:{
            hideCurrentPageAndShowProvided(p_page_idle);
            break;
        }
    }
    // if (paymentMethod != PAYMENT_TAP_USA && paymentMethod != PAYMENT_TAP_CANADA)
    // {
    //     hideCurrentPageAndShowProvided(p_page_idle);
    // }
}

void page_init::onInitTimeoutTick()
{
    if (--_initIdleTimeoutSec >= 0)
    {
        ui->label_init_message->setText(ui->label_init_message->text() + ".");
        QString command = "Ping";
        p_page_idle->thisMachine->dfUtility->send_command_to_FSM(command, true);
        
    }
    else
    {
        initIdleTimer->stop();
        qDebug() << "init: No connection with controller. Timeout trying.";
        switch(activePaymentMethod){
        case qr:
        case receipt_printer:{
            hideCurrentPageAndShowProvided(p_page_idle);
            break;
        }
        }
        // if (paymentMethod != PAYMENT_TAP_USA && paymentMethod != PAYMENT_TAP_CANADA)
        // {
        //     hideCurrentPageAndShowProvided(p_page_idle);
        // }
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

void page_init::initiateTapPayment()
{
    // executed in a separate thread. It shows the init screen until the code is initialized. 

    this->showFullScreen();
    // Waiting for payment label setup
    QString waitingForPayment = p_page_idle->thisMachine->getTemplateText("page_init->label_fail_message->tap_payment");
    p_page_idle->thisMachine->setTextToObject(ui->label_fail_message, waitingForPayment);
    switch(activePaymentMethod){
        case tap_usa:{
            page_payment_tap_tcp paymentObject;
            paymentObject.initiate_tap_setup();
            break;
        }
        case tap_canada:{
            page_payment_tap_serial paymentSerialObject;
            paymentSerialObject.tap_serial_initiate();
            break;
        }
    }
    // if (paymentMethod == PAYMENT_TAP_USA)
    // {
    //     page_payment_tap_tcp paymentObject;
    //     paymentObject.initiate_tap_setup();
    // }
    // else if (paymentMethod == PAYMENT_TAP_CANADA)
    // {
    //     page_payment_tap_serial paymentSerialObject;
    //     paymentSerialObject.tap_serial_initiate();
    // }

    emit tapSetupInitialized(); 
}

void page_init::showIdlePage()
{
    hideCurrentPageAndShowProvided(p_page_idle);
}