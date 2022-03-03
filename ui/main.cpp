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
#include "page_help.h"
#include "page_init.h"
#include "page_idle.h"
#include "page_select_product.h"
#include "page_product.h"
#include "pagepayment.h"
#include "page_dispenser.h"
#include "page_error_wifi.h"
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
    page_help* helpPage = new page_help();
    page_init* initPage = new page_init();
    page_idle* idlePage = new page_idle();
    page_select_product* firstSelectPage = new page_select_product();
    pageProduct* p_pageProduct = new pageProduct();
    pagePayment* paymentPage = new pagePayment();
    page_dispenser* dispensingPage = new page_dispenser();
    page_error_wifi* wifiError = new page_error_wifi();
    pagethankyou* lastPage = new pagethankyou();
    page_maintenance* p_page_maintenance = new page_maintenance();
    page_maintenance_dispenser* maintainPage = new page_maintenance_dispenser();

    // TODO: Instantiate a DrinkSelection[] Array
    // TODO: Create Query to populate DrinkSelection[0-12]
    // TODO: Instantiate a DrinkOrder Object

    // Page pathing references to function calls.
    helpPage->setPage(firstSelectPage, p_pageProduct, idlePage, paymentPage);
    initPage->setPage(idlePage);
    maintainPage->setPage(p_page_maintenance, idlePage);
    p_page_maintenance->setPage(idlePage, maintainPage, firstSelectPage, p_pageProduct);
    idlePage->setPage(firstSelectPage, p_page_maintenance);
    firstSelectPage->setPage(p_pageProduct, idlePage, p_page_maintenance, helpPage);
    p_pageProduct->setPage(firstSelectPage, dispensingPage,wifiError, idlePage, paymentPage, helpPage);
    paymentPage->setPage(p_pageProduct, dispensingPage, idlePage, helpPage);
    dispensingPage->setPage(paymentPage, lastPage, idlePage);
    lastPage->setPage(dispensingPage, idlePage, paymentPage);
    wifiError->setPage(paymentPage, lastPage, idlePage);
    initPage->showFullScreen();

    DfUiServer dfUiServer;
    dfUiServer.startServer();

    QObject::connect(&dfUiServer, &DfUiServer::pleaseReset, dispensingPage, &page_dispenser::PleaseResetTimerSlot);
    QObject::connect(&dfUiServer, &DfUiServer::updateVolume, dispensingPage, &page_dispenser::updateVolumeDisplayed);
    QObject::connect(&dfUiServer, &DfUiServer::targetHit, dispensingPage, &page_dispenser::targetHitDisplay);
    QObject::connect(&dfUiServer, &DfUiServer::initReady, initPage, &page_init::initReadySlot);
    QObject::connect(&dfUiServer, &DfUiServer::MM, idlePage, &page_idle::MMSlot);
    QObject::connect(&dfUiServer, &DfUiServer::updateVolume, maintainPage, &page_maintenance_dispenser::updateVolumeDisplayed);

    return mainApp.exec();
}

