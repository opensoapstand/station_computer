//***************************************
//
// page_idle.cpp
// GUI class while machine is page_idle.
//
// Display Fullscreen DF branded Wallpaper
// Listen for User interaction to load
// Product Page1
//
// created: 05-04-2022
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "page_idle.h"
#include "ui_page_idle.h"
#include "page_idle_products.h"
#include "page_maintenance.h"
#include "page_maintenance_general.h"
#include "dispenser_slot.h"
#include "dbmanager.h"
#include "statusbar.h"

#include <cstdlib> // For rand() function

#include <QStringList>
#include <QMediaPlayer>
#include <QGraphicsVideoItem>
// #include <QMainWindow>
#include <QtWidgets>
#include <QtMultimediaWidgets>

#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>

//    #define PLAY_VIDEO
// CTOR

page_idle::page_idle(QWidget *parent) : QWidget(parent),
                                        ui(new Ui::page_idle)
{

    // Background Set here; Inheritance on forms places image on all elements otherwise.
    ui->setupUi(this);

    idlePageTypeSelectorTimer = new QTimer(this);
    idlePageTypeSelectorTimer->setInterval(1000);
    connect(idlePageTypeSelectorTimer, SIGNAL(timeout()), this, SLOT(onIdlePageTypeSelectorTimerTick()));

    pollTemperatureTimer = new QTimer(this);
    pollTemperatureTimer->setInterval(1000);
    connect(pollTemperatureTimer, SIGNAL(timeout()), this, SLOT(onPollTemperatureTimerTick()));

    testForFrozenScreenTimer = new QTimer(this);
    testForFrozenScreenTimer->setInterval(1000);
    connect(testForFrozenScreenTimer, SIGNAL(timeout()), this, SLOT(onTestForFrozenScreenTick()));

    userRoleTimeOutTimer = new QTimer(this);
    userRoleTimeOutTimer->setInterval(1000);
    connect(userRoleTimeOutTimer, SIGNAL(timeout()), this, SLOT(onUserRoleTimeOutTimerTick()));

    stateScreenCheck = state_screen_check_not_initiated;
    statusbarLayout = new QVBoxLayout(this);

    tappingBlockedUntilPrinterReply = false; 
}

void page_idle::setPage(page_select_product *p_page_select_product, page_maintenance *pageMaintenance, page_maintenance_general *pageMaintenanceGeneral, page_idle_products *p_page_idle_products, page_error_wifi *p_page_error_wifi, statusbar *p_statusbar)
{
    // Chained to KB Listener
    this->p_pageSelectProduct = p_page_select_product;
    this->p_page_maintenance = pageMaintenance;
    this->p_page_maintenance_general = pageMaintenanceGeneral;
    this->p_page_idle_products = p_page_idle_products;
    this->p_page_error_wifi = p_page_error_wifi;
    this->p_statusbar = p_statusbar;
}

// DTOR
page_idle::~page_idle()
{
    delete ui;
}

void page_idle::setMachine(machine *pmachine)
{
    thisMachine = pmachine;
}

void page_idle::showEvent(QShowEvent *event)
{
    thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);
    thisMachine->applyDynamicPropertiesFromTemplateToWidgetChildren(this); // this is the 'page', the central or main widget

    this->installEventFilter(this); // catches all events.

    // p_statusbar->autoSetVisibility();
    p_statusbar->refresh();

    statusbarLayout->addWidget(p_statusbar);            // Only one instance can be shown. So, has to be added/removed per page.
    statusbarLayout->setContentsMargins(0, 1860, 0, 0); // int left, int top, int right, int bottom);

    thisMachine->loadDynamicContent();

    thisMachine->dispenseButtonLightsAnimateState(true);

    if (!thisMachine->isSessionLocked())
    {
        thisMachine->resetSessionId();
    }

    if (thisMachine->getCouponState() == StateCoupon::no_state) // at startup
    {
        thisMachine->initCouponState();
    }

    thisMachine->setSelectedProduct(0); // default selected product is necessary to deal with things if no product is chosen yet e.g. show transaction history

    thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_IDLE_BACKGROUND_PATH);

    QString styleSheet = thisMachine->getCSS(PAGE_IDLE_CSS);
    ui->pushButton_to_select_product_page->hide(); // disable the button.
    ui->label_welcome_message->setStyleSheet(styleSheet);
    ui->pushButton_test->setStyleSheet(styleSheet);
    ui->pushButton_test->hide();

    thisMachine->setTemplateTextToObject(ui->label_welcome_message);
    thisMachine->addClientLogoToLabel(ui->label_client_logo);
    // QString machine_logo_full_path = thisMachine->getTemplatePathFromName(MACHINE_LOGO_PATH);
    // thisMachine->addPictureToLabel(ui->label_manufacturer_logo, machine_logo_full_path);
    ui->label_manufacturer_logo->setStyleSheet(styleSheet);
    ui->label_printer_status->setStyleSheet(styleSheet);
    ui->label_temperature_warning->setStyleSheet(styleSheet);
    ui->label_show_temperature->setStyleSheet(styleSheet);

    // ui->pushButton_to_select_product_page->setStyleSheet(styleSheet);
    // ui->pushButton_test->raise();

    // template text with argument example
    // QString base_text = getTemplateTextByElementNameAndPageAndIdentifier(ui->label_welcome_message, "testargument" );
    // ui->label_welcome_message->setText(base_text.arg("SoAp")); // will replace %1 character in string by the provide text
    // ui->pushButton_to_select_product_page->raise();

    thisMachine->setTemplateTextToObject(ui->label_welcome_message);
    thisMachine->addClientLogoToLabel(ui->label_client_logo);

    ui->label_printer_status->hide(); // always hide here, will show if enabled and has problems.

    if (thisMachine->hasReceiptPrinter())
    {
        checkReceiptPrinterStatus();
    }

    QString machine_logo_full_path = thisMachine->getTemplatePathFromName(MACHINE_LOGO_PATH);
    thisMachine->addPictureToLabel(ui->label_manufacturer_logo, machine_logo_full_path);
    ui->label_manufacturer_logo->setStyleSheet(styleSheet);

    idlePageTypeSelectorTimer->start(1000);
    _idlePageTypeSelectorTimerTimeoutSec = PAGE_IDLE_DELAY_BEFORE_ENTERING_IDLE_PRODUCTS;

    pollTemperatureTimer->start(1000);
    _pollTemperatureTimerTimeoutSec = PAGE_IDLE_POLL_TEMPERATURE_PERIOD_SECONDS;

    testForFrozenScreenTimer->start(1000);
    _testForFrozenScreenTimerTimeoutSec = PAGE_IDLE_TEST_FOR_FROZEN_SCREEN_PERIOD_SECONDS;

    userRoleTimeOutTimer->start(1000);
    _userRoleTimeOutTimerSec = PAGE_IDLE_USER_ROLE_TIMEOUT_SECONDS;

// #define PLAY_VIDEO
#ifdef PLAY_VIDEO
    // player = new QMediaPlayer(this);

    // QGraphicsVideoItem *item = new QGraphicsVideoItem;
    // player->setVideoOutput(item);
    // graphicsView->scene()->addItem(item);
    // graphicsView->show();

    // player->setMedia(QUrl("http://example.com/myclip4.ogv"));
    // player->play();

    // QMainWindow w;
    QVideoWidget *videoWidget = new QVideoWidget(ui->video_player);
    QMediaPlayer *player = new QMediaPlayer(this);

    videoWidget->setGeometry(QRect(0, 0, 1000, 1000));

    // test text label
    video_label = new QLabel(this);
    // video_label = new QLabel(ui->video_player);
    video_label->setObjectName(QStringLiteral("video testset"));
    video_label->setGeometry(QRect(0, 0, 100, 31));
    video_label->setText("test");
    video_label->raise();
    video_label->show();

    // ui->video_player
    // w.setCentralWidget(&videoWidget);

    ui->media_player->show();
    ui->media_player->raise();

    player->setMedia(QUrl::fromLocalFile("/home/df-admin/production/references/media/ttFILEREMOVEDITSTOOBIGt.mp4"));
    player->setVideoOutput(videoWidget);

    // #define VIDEO_IN_WIDGET
    player->play();

#ifdef VIDEO_IN_WIDGET
    // helpful?! https://stackoverflow.com/questions/65910004/qvideowidget-doesnt-display-frames

    videoWidget->show(); // needed to display in non full screen.
    // ui->video_player->raise();
    // ui->video_player->show();

#else
    qDebug() << "Video player is fullscreen? : " << videoWidget->isFullScreen();
    videoWidget->setFullScreen(true);

#endif
    qDebug() << "Video player. Is fullscreen? : " << videoWidget->isFullScreen();
#endif
}

bool page_idle::eventFilter(QObject *object, QEvent *event)
{
    // catches all events
    // qDebug() << "global event trigger......" << event->type();

    if (object == this) // for 'this' widget.  (e.g. if a button from another widget is pressed, it will not pass)
    {
        // catches all events on this widget
        // qDebug() << "Event trigger for idle_page widget" << event->type();
        if (event->type() == QEvent::TouchBegin ||
            event->type() == QEvent::TouchUpdate ||
            event->type() == QEvent::TouchEnd)
        {
            // QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
            qDebug() << "touch event!!!!";
        }
        else if (event->type() == QEvent::MouseButtonPress)
        {
            // QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            // if (mouseEvent->button() == Qt::LeftButton) {
            //     // Handle mouse click event here (page change or any other action)
            //     emit pageChangeRequested();
            //     return true; // Event handled, stop further processing
            // }
            // qDebug() << "Mouse Clicked in idle page (can be virtual)"; // leave this for a while to investigate frozen screens in the field.
            if (stateScreenCheck == state_screen_check_clicked_and_wait)
            {
                stateScreenCheck = state_screen_check_clicked_and_succes;
                // qDebug() << "Mouse Clicked in idle page (can be virtual): Frozen screen test. Will not proceed."; // leave this for a while to investigate frozen screens in the field.
            }
            else
            {
                qDebug() << "Mouse Clicked in idle page (can be virtual): Go to page select products"; // leave this for a while to investigate frozen screens in the field.
                this->hideCurrentPageAndShowProvided(p_pageSelectProduct, true);
            }
        }
        else
        {
            // qDebug() << "other event.... ";
        }
    }
    // return false; // Event not handled, continue processing
    return false;
}

void page_idle::changeToIdleProductsIfSet()
{
    if (thisMachine->getIdlePageType() == "static_products" || thisMachine->getIdlePageType() == "dynamic" || thisMachine->getIdlePageType() == "dynamic_products")
    {
        hideCurrentPageAndShowProvided(this->p_page_idle_products, false);
    }
}

void page_idle::onTestForFrozenScreenTick()
{
    // sometimes the screen is frozen. It is not a hardware issue.
    // virtual mouse clicks also have no effect when frozen.
    // Nobody knows what causes it. Maybe a doubling up of the page? But no evidence of this is in any log.
    // So, we perform periodically a virtual mouse click. If it is caught by the 'next page' button, that means all is fine.
    // If not, we will go to select products page and automatically revert after some seconds. I hope that by reloading idle page, the 'freezing issue' is solved.

    // Only poll temperature after every PAGE_IDLE_TEST_FOR_FROZEN_SCREEN_PERIOD_SECONDS seconds
    _testForFrozenScreenTimerTimeoutSec--;
    if (_testForFrozenScreenTimerTimeoutSec >= 0)
    {
        return;
    }
    _testForFrozenScreenTimerTimeoutSec = PAGE_IDLE_TEST_FOR_FROZEN_SCREEN_PERIOD_SECONDS;

    // we will test the previous cycle's outcome. If there is a fail, that means it will take at least one whole period that it's undetected (for the sake of simplicity of implementation)

    if (stateScreenCheck == state_screen_check_not_initiated)
    {
    }
    else if (stateScreenCheck == state_screen_check_clicked_and_succes)
    {
        stateScreenCheck = state_screen_check_not_initiated;
    }
    else if (stateScreenCheck == state_screen_check_clicked_and_wait)
    {
        // still in this state? This means we have a fail!
        stateScreenCheck = state_screen_check_fail;

        qDebug() << "ERROR: Idle Screen Not resposive to click test. (or program lost focus?!...). Will automatically go to 'select products page'. Screen freeze test is only active on idle page. If this is ennoying you while working in Ubuntu, put the program in maintenance mode. ";
        hideCurrentPageAndShowProvided(p_pageSelectProduct, true); // will go to select products page and automatically revert after some seconds. I hope that by reloading idle page, the 'freezing issue' is solved.
        return;                                                    // we would create a monster if we continue, with multiple clicks and doubled up pages...
    }

    // prepare for next cycle
    stateScreenCheck = state_screen_check_clicked_and_wait;
    df_util::executeVirtualClick(200, 500);

    if (tappingBlockedUntilPrinterReply)
    {
        qDebug() << "ERROR: receipt printer did not respond in time! Unblock at frozen screen test.  ";
        tappingBlockedUntilPrinterReply = false; // if printer not responsive, unblock here. s
    }
}

void page_idle::onUserRoleTimeOutTimerTick()
{
    _userRoleTimeOutTimerSec--;
    if (_userRoleTimeOutTimerSec >= 0)
    {
        // set colour in red
        p_statusbar->setRoleTimeOutTrailingText("(" + QString::number(_userRoleTimeOutTimerSec) + "s)");
        p_statusbar->refresh();

        return;
    }
    else
    {
        userRoleTimeOutTimer->stop();
        p_statusbar->setRoleTimeOutTrailingText("");
        thisMachine->resetUserState();
        p_statusbar->refresh();

        _userRoleTimeOutTimerSec = PAGE_IDLE_USER_ROLE_TIMEOUT_SECONDS;
    }
}

void page_idle::refreshTemperature()
{
    //  QString base_text = "Current Temperature: %1 °C"; //Assuming you have the base_text defined somewhere else or you can define it here.
    if (thisMachine->isAelenPillarElseSoapStand())
    {
        QString base_text = thisMachine->getTemplateTextByElementNameAndPage(ui->label_show_temperature);
        float temperature = thisMachine->getTemperature_1();
        QString temperatureStr = QString::number(temperature, 'f', 1);

        ui->label_show_temperature->setText(base_text.arg(temperatureStr));
        ui->label_show_temperature->show();

        if (thisMachine->isTemperatureTooHigh_1())
        {
            thisMachine->checkForHighTemperatureAndDisableProducts(); // todo test if it works
            qDebug() << "Temperature too high, disable all slots.";

            // Update temperature status label
            QString base = thisMachine->getTemplateTextByElementNameAndPageAndIdentifier(ui->label_temperature_warning, "temperature_too_high");
            ui->label_temperature_warning->setText(base.arg(temperatureStr));
            ui->label_temperature_warning->show();
        }
        else
        {
            ui->label_temperature_warning->hide();
        }
    }
    else
    {
        ui->label_temperature_warning->hide();
        ui->label_show_temperature->hide();
    }
}

// periodical temperature check initiated
void page_idle::onPollTemperatureTimerTick()
{
    // Only poll temperature after every PAGE_IDLE_POLL_TEMPERATURE_PERIOD_SECONDS seconds
    if (--_pollTemperatureTimerTimeoutSec >= 0)
    {
        return;
    }

    _pollTemperatureTimerTimeoutSec = PAGE_IDLE_POLL_TEMPERATURE_PERIOD_SECONDS;
    refreshTemperature();
}

// periodical check to transition to other idle page type
void page_idle::onIdlePageTypeSelectorTimerTick()
{
    _idlePageTypeSelectorTimerTimeoutSec -= 1;
    if (_idlePageTypeSelectorTimerTimeoutSec >= 0)
    {
    }
    else
    {
        changeToIdleProductsIfSet();
    }
}

void page_idle::checkReceiptPrinterStatus()
{
    ui->label_printer_status->hide(); // always hide here, will show if enabled and has problems.
    tappingBlockedUntilPrinterReply = false;

    if (thisMachine->hasReceiptPrinter())
    {
        qDebug() << "Check receipt printer functionality.";
        this->p_page_maintenance_general->send_check_printer_status_command();
        // ui->pushButton_to_select_product_page->hide(); // when printer needs to be restarted, it can take some time. Make sure nobody presses the button in that interval (to prevent crashes)
        tappingBlockedUntilPrinterReply = true;
    }
}

void page_idle::printerStatusFeedback(bool isOnline, bool hasPaper)
{
    qDebug() << "Printer feedback received from fsm";

    if (!isOnline)
    {
        ui->label_printer_status->raise();
        thisMachine->setTemplateTextWithIdentifierToObject(ui->label_printer_status, "offline");
        ui->label_printer_status->show();
    }
    else if (!hasPaper)
    {
        ui->label_printer_status->raise();
        thisMachine->setTemplateTextWithIdentifierToObject(ui->label_printer_status, "nopaper");
        ui->label_printer_status->show();
    }
    else
    {
        ui->label_printer_status->hide();
    }
    // ui->pushButton_to_select_product_page->show();
    tappingBlockedUntilPrinterReply = false;
}

void page_idle::on_pushButton_to_select_product_page_clicked()
{

    // Handled by catching a global click event on the widget.

    //     qDebug() << "Clicked on to_product_page button."; // leave this for a while to investigate frozen screens in the field.
    //     if (stateScreenCheck == state_screen_check_clicked_and_wait)
    //     {
    //         stateScreenCheck = state_screen_check_clicked_and_succes;
    //     }
    //     else
    //     {
    //         // this->hideCurrentPageAndShowProvided(p_pageSelectProduct, true);
    //     }
}

void page_idle::hideCurrentPageAndShowProvided(QWidget *pageToShow, bool createNewSessionId)
{
    // the moment there is a user interaction, a new session ID is created.
    // will only be relevant when user goes to select_products

    if (!tappingBlockedUntilPrinterReply)
    {

        if (createNewSessionId)
        {
            if (!thisMachine->isSessionLocked())
            {
                thisMachine->createSessionId();
            }
        }

        thisMachine->pageTransition(this, pageToShow);
        idlePageTypeSelectorTimer->stop();
        pollTemperatureTimer->stop();
        testForFrozenScreenTimer->stop();
        userRoleTimeOutTimer->stop();
        statusbarLayout->removeWidget(p_statusbar); // Only one instance can be shown. So, has to be added/removed per page.
    }else{
        qDebug() << "Tapping blocked until receipt printer reply. ";
    }
}

void page_idle::on_pushButton_test_clicked()
{
    qDebug() << "pushButton_test clicked.. ";
}
