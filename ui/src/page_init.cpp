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

    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_INIT_CSS);
    ui->pushButton_continue->setStyleSheet(styleSheet);
    ui->pushButton_continue->hide();
    ui->pushButton_reboot->setStyleSheet(styleSheet);
    ui->pushButton_reboot->hide();
    if(p_page_idle->thisMachine->hasMixing()){
        ui->label_init_message->setStyleSheet(styleSheet);
    }

    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_continue);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->pushButton_reboot);

    if (!p_page_idle->thisMachine->isMachineDBLoaded())
    {
        qDebug() << "Init: ERROR: Configuration Db not loaded. Probably wrong layout. Please repair and restart config.db";
    }

    // template content
    p_page_idle->thisMachine->loadDynamicContent();

    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget
    qDebug() << "setting background in page init";
    p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_INIT_BACKGROUND_IMAGE_PATH);

    _initIdleTimeoutSec = PAGE_INIT_READY_TIMEOUT_SECONDS;
    initIdleTimer->start(1000);

    _rebootTimeoutSec = PAGE_INIT_REBOOT_TIMEOUT_SECONDS;
    rebootTimer->start(1000);

    paymentMethod = p_page_idle->thisMachine->getPaymentMethod();
    if (paymentMethod == PAYMENT_TAP_CANADA_QR)
    {
        p_page_idle->thisMachine->setActivePaymentMethod(ActivePaymentMethod::tap_canada);
        p_page_idle->thisMachine->setAllowedPaymentMethods(ActivePaymentMethod::tap_canada);
        p_page_idle->thisMachine->setAllowedPaymentMethods(ActivePaymentMethod::qr);
    }
    else if (paymentMethod == PAYMENT_TAP_CANADA)
    {
        p_page_idle->thisMachine->setActivePaymentMethod(ActivePaymentMethod::tap_canada);
        p_page_idle->thisMachine->setAllowedPaymentMethods(ActivePaymentMethod::tap_canada);
    }
    else if (paymentMethod == PAYMENT_TAP_USA_QR)
    {
        p_page_idle->thisMachine->setActivePaymentMethod(ActivePaymentMethod::tap_usa);
        p_page_idle->thisMachine->setAllowedPaymentMethods(ActivePaymentMethod::tap_usa);
        p_page_idle->thisMachine->setAllowedPaymentMethods(ActivePaymentMethod::qr);
    }
    else if (paymentMethod == PAYMENT_TAP_USA)
    {
        p_page_idle->thisMachine->setActivePaymentMethod(ActivePaymentMethod::tap_usa);
        p_page_idle->thisMachine->setAllowedPaymentMethods(ActivePaymentMethod::tap_usa);
    }
    else if (paymentMethod == PAYMENT_QR)
    {
        p_page_idle->thisMachine->setActivePaymentMethod(ActivePaymentMethod::qr);
        p_page_idle->thisMachine->setAllowedPaymentMethods(ActivePaymentMethod::qr);
    }
    else
    {
        p_page_idle->thisMachine->setActivePaymentMethod(ActivePaymentMethod::receipt_printer);
        p_page_idle->thisMachine->setAllowedPaymentMethods(ActivePaymentMethod::receipt_printer);
    }
    activePaymentMethod = p_page_idle->thisMachine->getActivePaymentMethod();
    QString command = "Ping";
    p_page_idle->thisMachine->dfUtility->send_command_to_FSM(command, true);

    switch (activePaymentMethod)
    {
    case tap_canada:
    case tap_usa:
    {
        // Thread setup for non-blocking tap payment initialization
        // Using bind for non-static functions
        auto bindFn = std::bind(&page_init::initiateTapPayment, this);
        tapInitThread = std::thread(bindFn);
        tapInitThread.detach();
        break;
    }
    }
}

void page_init::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    p_page_idle->thisMachine->pageTransition(this, pageToShow);
    initIdleTimer->stop();
    rebootTimer->stop();
}

void page_init::controllerReadySlot(void)
{
    qDebug() << "Signal: init ready from fsm";
    m_controller_ready = true;
}

void page_init::onInitTimeoutTick()
{
    if (--_initIdleTimeoutSec >= 0)
    {
        m_label_init_status_text = "";

        bool ready = true;

        if (!p_page_idle->thisMachine->isMachineDBLoaded())
        {

            qDebug() << "init: Machine db table not loaded. Probably corrupt or incorrect database layout. Please fix.";
            ready = false;
            m_label_init_status_text += "Load machine parameters. ERROR<br>";
        }
        else
        {
            m_label_init_status_text += "Load machine parameters. OK<br>";
        }

        if (!p_page_idle->thisMachine->isSlotsLoaded())
        {

            qDebug() << "init: Slots not loaded. Probably corrupt or incorrect database layout. Please fix.";
            m_label_init_status_text += "Load slot parameters. ERROR<br>";
            ready = false;
        }
        else
        {
            m_label_init_status_text += "Load slot parameters. OK<br>";
        }

        if (!p_page_idle->thisMachine->isProductsLoaded())
        {

            qDebug() << "init: Products not loaded. Probably corrupt or incorrect database layout. Please fix.";
            m_label_init_status_text += "Load products. ERROR<br>";
            ready = false;
        }
        else
        {
            m_label_init_status_text += "Load products. OK<br>";
        }

        switch (activePaymentMethod)
        {
        case tap_canada:
        case tap_usa:
        {
            if (!m_tap_payment_ready)
            {
                ready = false;
                qDebug() << "init: Waiting for tap payment ready";
                m_label_init_status_text += "Load Tap payment. INIT ERROR<br>";
            }
            else
            {
                m_label_init_status_text += "Load Tap payment. INIT OK<br>";
            }
            break;
        }
        default:
        {
        }
        }

        if (!m_controller_ready)
        {
            qDebug() << "init: Waiting for controller ready.";
            QString command = "Ping";
            p_page_idle->thisMachine->dfUtility->send_command_to_FSM(command, true);
            ready = false;
            m_label_init_status_text += "Controller status. WAITING<br>";
        }
        else
        {
            m_label_init_status_text += "Controller status. OK<br>";
        }
        QString timeout_message = QString("Timeout in %1s<br>").arg(QString::number(_initIdleTimeoutSec));
        // label_text += timeout_message;

        ui->label_init_message->setText(m_label_init_status_text + timeout_message);

        if (ready)
        {
            hideCurrentPageAndShowProvided(p_page_idle);
        }
    }
    else
    {
        ui->label_init_message->setText(m_label_init_status_text);
        initIdleTimer->stop();

        ui->pushButton_continue->show();
        ui->pushButton_reboot->show();
        qDebug() << "init: Timed out while waiting to be ready. Restart application manually, or wait for auto reboot.";
    }
}

void page_init::onRebootTimeoutTick()
{
    if (--_rebootTimeoutSec >= 0)
    {
        // qDebug() << "init: Reboot Tick Down - " << _rebootTimeoutSec;
    }
    else
    {
        qDebug() << "Reboot Timer elapsed. (should reboot computer now)" << _rebootTimeoutSec;
        rebootTimer->stop();

        // REBOOT!
        p_page_idle->thisMachine->reboot();
    }
}

void page_init::initiateTapPayment()
{
    // executed in a separate thread. It shows the init screen until the code is initialized.

    this->showFullScreen();
    // Waiting for payment label setup
    // QString waitingForPayment = p_page_idle->thisMachine->getTemplateText("page_init->label_fail_message->tap_payment");
    // p_page_idle->thisMachine->setTextToObject(ui->label_fail_message, waitingForPayment);
    switch (activePaymentMethod)
    {
    case tap_usa:
    {
        page_payment_tap_tcp paymentObject;
        paymentObject.initiate_tap_setup();
        break;
    }
    case tap_canada:
    {
        qDebug() << "In tap canada";
        page_payment_tap_serial paymentSerialObject;
        paymentSerialObject.tap_serial_initiate();
        break;
    }
    }
    m_tap_payment_ready = true;
}

void page_init::on_pushButton_continue_clicked()
{
    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_init::on_pushButton_reboot_clicked()
{
    p_page_idle->thisMachine->reboot();
}
