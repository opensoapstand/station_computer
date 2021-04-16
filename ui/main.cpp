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
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#define UI_VERSION "0.1.4"
#include "init.h"
#include "idle.h"
#include "productpage_1.h"
#include "productpage_2.h"
#include "payselect.h"
#include "paypage.h"
#include "dispensepage.h"
#include "thankyoupage.h"
#include <QApplication>
#include "df_util.h"
#include "maintenancePage.h"
#include "maintain_product.h"

int main(int argc, char *argv[])
{
    // Fire up QT GUI Thread
    QApplication mainApp(argc, argv);

    // Instantiate object to hold drink order

    // Build objects to hold navigation (pages)
    // Linking resources and Function definitions for pathing
    init* initPage = new init();
    idle* idlePage = new idle();
    productPage_1* firstSelectPage = new productPage_1();
    productPage_2* secondSelectPage = new productPage_2();
    paySelect* paySelectPage = new paySelect();
    payPage* paymentPage = new payPage();
    dispensePage* dispensingPage = new dispensePage();
    thankYouPage* lastPage = new thankYouPage();
    maintenancePage* maintenanceMode = new maintenancePage();
    maintain_product* maintainPage = new maintain_product();

    // TODO: Instantiate a DrinkSelection[] Array
    // TODO: Create Query to populate DrinkSelection[0-12]
    // TODO: Instantiate a DrinkOrder Object

    // Page pathing references to function calls.
    initPage->setPage(idlePage);
    initPage->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    maintainPage->setPage(maintenanceMode, idlePage);
    maintainPage->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    maintenanceMode->setPage(idlePage, maintainPage, firstSelectPage);
    maintenanceMode->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    idlePage->setPage(firstSelectPage);
    idlePage->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    firstSelectPage->setPage(secondSelectPage, paySelectPage, idlePage, maintenanceMode);
    firstSelectPage->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    secondSelectPage->setPage(firstSelectPage, paySelectPage, idlePage, maintenanceMode);
    secondSelectPage->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    paySelectPage->setPage(firstSelectPage, dispensingPage, idlePage);
    paySelectPage->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    paymentPage->setPage(paySelectPage, dispensingPage, idlePage);
    paymentPage->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    dispensingPage->setPage(paymentPage, lastPage, idlePage);
    dispensingPage->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    lastPage->setPage(dispensingPage, idlePage);
    lastPage->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    //    payOptionToggle->setPage();

    initPage->showFullScreen();

    DfUiServer dfUiServer;
    dfUiServer.startServer();

    QObject::connect(&dfUiServer, &DfUiServer::pleaseReset, dispensingPage, &dispensePage::PleaseResetTimerSlot);
    QObject::connect(&dfUiServer, &DfUiServer::updateVolume, dispensingPage, &dispensePage::updateVolumeDisplayed);
    QObject::connect(&dfUiServer, &DfUiServer::targetHit, dispensingPage, &dispensePage::targetHitDisplay);
    QObject::connect(&dfUiServer, &DfUiServer::initReady, initPage, &init::initReadySlot);


    //Update Click DB
    DbManager db(DB_PATH);
    db.addPageClick("INITIALIZED");

    return mainApp.exec();
}

