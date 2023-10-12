//***************************************
//
// page_error_wifi.cpp
// GUI class to dispense drink.
// Coordinates User input to payment class
// then communcates results to FSM IP_thread
//
// Allows navigation to paymentpage and
// thankyou page
//
// created: 05-04-2022
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "page_error_wifi.h"
#include "ui_page_error_wifi.h"
#include "includefiles.h"
#include "page_idle.h"
#include "page_end.h"

// CTOR
page_error_wifi::page_error_wifi(QWidget *parent) : QWidget(parent),
                                                    ui(new Ui::page_error_wifi)
{
    ui->setupUi(this);

    timeoutTimer = new QTimer(this);
    timeoutTimer->setInterval(20);
    connect(timeoutTimer, SIGNAL(timeout()), this, SLOT(onTimeOutTick()));
}

/*
 * Page Tracking reference to Payment page and completed payment
 */
void page_error_wifi::setPage(page_qr_payment *page_qr_payment, page_end *page_end, page_idle *pageIdle)
{
    this->thanksPage = page_end;
    this->paymentPage = page_qr_payment;
    this->p_page_idle = pageIdle;
}

// DTOR
page_error_wifi::~page_error_wifi()
{
    delete ui;
}

void page_error_wifi::showEvent(QShowEvent *event)
{

    p_page_idle->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;

    QWidget::showEvent(event);
    p_page_idle->thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget
    
    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_ERROR_WIFI_CSS);
    ui->label_error_message->setStyleSheet(styleSheet);
    ui->label_oops->setStyleSheet(styleSheet);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_oops);

    ui->label_error_occured->setStyleSheet(styleSheet);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_error_occured);
    ui->label_check_back->setStyleSheet(styleSheet);
    p_page_idle->thisMachine->setTemplateTextToObject(ui->label_check_back);
    ui->pushButton_mainPage->setStyleSheet(styleSheet);

    QString machine_logo_full_path = p_page_idle->thisMachine->getTemplatePathFromName(MACHINE_LOGO_PATH);
    p_page_idle->thisMachine->addPictureToLabel(ui->label_logo_drinkfill, machine_logo_full_path);
    ui->label_logo_drinkfill->setStyleSheet(styleSheet);

    p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_ERROR_BACKGROUND_PATH);

    timeoutTimer->start(1000);
    _goTop_page_idleTimeoutSec = 10;
}

void page_error_wifi::on_pushButton_mainPage_clicked()
{
    exit_page();
}

void page_error_wifi::exit_page()
{
    //    qDebug() << "page_error_wifi: Stop Timers";
    // if(timeoutTimer != nullptr){
    timeoutTimer->stop();
    // }
    // p_page_idle->showFullScreen();
    // this->hide();
    p_page_idle->thisMachine->pageTransition(this, p_page_idle);
}

void page_error_wifi::onTimeOutTick()
{
    if (--_goTop_page_idleTimeoutSec >= 0)
    {
        // qDebug() << "page_dispenser: Idle Tick Down: " << _goTop_page_idleTimeoutSec;
    }
    else
    {
        //        qDebug() << "Timer Done!" << _goTop_page_idleTimeoutSec;
        qDebug() << "Show wifi error timed out. ";
        // timeoutTimer->stop();
        exit_page();
    }
}
