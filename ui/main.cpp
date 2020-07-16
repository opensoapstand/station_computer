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

#include "idle.h"
#include "productpage_1.h"
#include "productpage_2.h"
#include "payselect.h"
#include "paypage.h"
#include "dispensepage.h"
#include "thankyoupage.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // Fire up QT GUI Thread
    QApplication mainApp(argc, argv);

    // Build objects to hold navigation (pages)
    // Linking resources and Function definitions for pathing
    idle* idlePage = new idle();
    productPage_1* firstSelectPage = new productPage_1();
    productPage_2* secondSelectPage = new productPage_2();
    paySelect* paySelectPage = new paySelect();
    payPage* paymentPage = new payPage();
    dispensePage* dispensingPage = new dispensePage();
    thankYouPage* lastPage = new thankYouPage();

    // Function Calls
    idlePage->setPage(firstSelectPage);
    firstSelectPage->setPage(secondSelectPage, paySelectPage);
    secondSelectPage->setPage(firstSelectPage, paySelectPage);
    paySelectPage->setPage(firstSelectPage, paymentPage, idlePage);
    paymentPage->setPage(paySelectPage, dispensingPage, idlePage);
    dispensingPage->setPage(paymentPage, lastPage);
    lastPage->setPage(dispensingPage, idlePage);

    idlePage->showFullScreen();

    return mainApp.exec();
}

