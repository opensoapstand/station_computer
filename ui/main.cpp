//***************************************
//
// main.cpp
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

#define UI_VERSION "0.1.4"
#include "help.h"
#include "init.h"
#include "idle.h"
#include "pageproductsoverview.h"
#include "payselect.h"
#include "pagepayment.h"
#include "page_dispenser.h"
#include "wifiErrorPage.h"
#include "pagethankyou.h"
#include <QApplication>
#include "df_util.h"
#include "page_maintenance.h"
#include "page_maintenance_dispenser.h"
#include <QCoreApplication>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QQuickView>
#include <QGuiApplication>
#include <QQmlEngine>

int main(int argc, char *argv[])
{
    // Fire up QT GUI Thread
    QApplication mainApp(argc, argv);

    // Instantiate object to hold drink order

    // Build objects to hold navigation (pages)
    // Linking resources and Function definitions for pathing
    help* helpPage = new help();
    init* initPage = new init();
    idle* idlePage = new idle();
    pageproductsoverview* firstSelectPage = new pageproductsoverview();
    paySelect* paySelectPage = new paySelect();
    pagePayment* paymentPage = new pagePayment();
    page_dispenser* dispensingPage = new page_dispenser();
    wifiErrorPage* wifiError = new wifiErrorPage();
    pagethankyou* lastPage = new pagethankyou();
    page_maintenance* p_page_maintenance = new page_maintenance();
    page_maintenance_dispenser* maintainPage = new page_maintenance_dispenser();

    // TODO: Instantiate a DrinkSelection[] Array
    // TODO: Create Query to populate DrinkSelection[0-12]
    // TODO: Instantiate a DrinkOrder Object

    // Page pathing references to function calls.
    helpPage->setPage(firstSelectPage, paySelectPage, idlePage, paymentPage);
    //helpPage->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    initPage->setPage(idlePage);
    //initPage->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    maintainPage->setPage(p_page_maintenance, idlePage);
    //maintainPage->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    p_page_maintenance->setPage(idlePage, maintainPage, firstSelectPage, paySelectPage);
    //p_page_maintenance->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    idlePage->setPage(firstSelectPage, p_page_maintenance);
    //idlePage->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    firstSelectPage->setPage(paySelectPage, idlePage, p_page_maintenance, helpPage);
    //firstSelectPage->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    //secondSelectPage->setPage(firstSelectPage, paySelectPage, idlePage, p_page_maintenance);
    //secondSelectPage->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    paySelectPage->setPage(firstSelectPage, dispensingPage,wifiError, idlePage, paymentPage, helpPage);
    //paySelectPage->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    paymentPage->setPage(paySelectPage, dispensingPage, idlePage, helpPage);
//    paymentPage->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    dispensingPage->setPage(paymentPage, lastPage, idlePage);
//    dispensingPage->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    lastPage->setPage(dispensingPage, idlePage, paymentPage);
//    lastPage->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    wifiError->setPage(paymentPage, lastPage, idlePage);

    //    payOptionToggle->setPage();

    initPage->showFullScreen();

    DfUiServer dfUiServer;
    dfUiServer.startServer();

    QObject::connect(&dfUiServer, &DfUiServer::pleaseReset, dispensingPage, &page_dispenser::PleaseResetTimerSlot);
    QObject::connect(&dfUiServer, &DfUiServer::updateVolume, dispensingPage, &page_dispenser::updateVolumeDisplayed);
    QObject::connect(&dfUiServer, &DfUiServer::targetHit, dispensingPage, &page_dispenser::targetHitDisplay);
    QObject::connect(&dfUiServer, &DfUiServer::initReady, initPage, &init::initReadySlot);
    QObject::connect(&dfUiServer, &DfUiServer::MM, idlePage, &idle::MMSlot);
    QObject::connect(&dfUiServer, &DfUiServer::updateVolume, maintainPage, &page_maintenance_dispenser::updateVolumeDisplayed);

    return mainApp.exec();
}

