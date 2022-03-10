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

static QPointer<QFile> log_file = nullptr;

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(type)
    Q_UNUSED(context)

    QString time_stamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString time_stamp_time = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString time_stamp_date = QDateTime::currentDateTime().toString("yyyy-MM-dd");

    // print to screen
    QString line_terminal = "";
    line_terminal.append(time_stamp);
    line_terminal.append(" : ");
    line_terminal.append(msg);
    printf("%s\n", qPrintable(line_terminal));
    fflush(stdout);

    // create a new log file daily
    QString log_file_base_path = "/home/df-admin/drinkfill/production/logging/ui_%1.txt"; // https://stackoverflow.com/questions/4784155/how-to-format-a-qstring
    QString log_file_path = QString(log_file_base_path).arg(time_stamp_date);
    QFile file(log_file_path);
    log_file = &file;
    if (!log_file->open(QFile::WriteOnly | QFile::Text | QFile::Append))
    {
        qInstallMessageHandler(nullptr);
        qDebug() << "Couldn't log to file!";
    }

    // write to log
    if (log_file)
    {
        QString line_out = "";
        line_out.append(time_stamp_time);
        line_out.append(" : ");
        line_out.append(msg);
        // log_file->write(msg.toLatin1());
        log_file->write(line_out.toUtf8());
        log_file->write("\n");
        log_file->flush();
    }
}

int main(int argc, char *argv[])
{
    // set up logging
    qInstallMessageHandler(myMessageHandler); // Install the handler

    qDebug() << "***************************************************************************";
    qDebug() << "****************************** START SOAPSTAND UI *************************";
    qDebug() << "***************************************************************************";

    // Fire up QT GUI Thread
    QApplication mainApp(argc, argv);

    // Instantiate object to hold drink order

    // Build objects to hold navigation (pages)
    // Linking resources and Function definitions for pathing
    page_help *helpPage = new page_help();
    page_init *initPage = new page_init();
    page_idle *idlePage = new page_idle();
    page_select_product *firstSelectPage = new page_select_product();
    pageProduct *p_pageProduct = new pageProduct();
    pagePayment *paymentPage = new pagePayment();
    page_dispenser *dispensingPage = new page_dispenser();
    page_error_wifi *wifiError = new page_error_wifi();
    pagethankyou *lastPage = new pagethankyou();
    page_maintenance *p_page_maintenance = new page_maintenance();
    page_maintenance_dispenser *p_page_maintenance_product = new page_maintenance_dispenser();

    // TODO: Instantiate a DrinkSelection[] Array
    // TODO: Create Query to populate DrinkSelection[0-12]
    // TODO: Instantiate a DrinkOrder Object

    // Page pathing references to function calls.
    helpPage->setPage(firstSelectPage, p_pageProduct, idlePage, paymentPage);
    initPage->setPage(idlePage);
    p_page_maintenance_product->setPage(p_page_maintenance, idlePage);
    p_page_maintenance->setPage(idlePage, p_page_maintenance_product, firstSelectPage, p_pageProduct);
    idlePage->setPage(firstSelectPage, p_page_maintenance);
    firstSelectPage->setPage(p_pageProduct, idlePage, p_page_maintenance, helpPage);
    p_pageProduct->setPage(firstSelectPage, dispensingPage, wifiError, idlePage, paymentPage, helpPage);
    paymentPage->setPage(p_pageProduct, dispensingPage, idlePage, helpPage);
    dispensingPage->setPage(paymentPage, lastPage, idlePage);
    lastPage->setPage(dispensingPage, idlePage, paymentPage);
    wifiError->setPage(paymentPage, lastPage, idlePage);
    initPage->showFullScreen();

    DfUiServer dfUiServer;
    dfUiServer.startServer();

    QObject::connect(&dfUiServer, &DfUiServer::pleaseReset, dispensingPage, &page_dispenser::resetDispenseTimeout);
    QObject::connect(&dfUiServer, &DfUiServer::signalUpdateVolume, dispensingPage, &page_dispenser::updateVolumeDisplayed);
    QObject::connect(&dfUiServer, &DfUiServer::signalUpdateVolume, p_page_maintenance_product, &page_maintenance_dispenser::updateVolumeDisplayed);
    QObject::connect(&dfUiServer, &DfUiServer::targetHit, dispensingPage, &page_dispenser::fsmReceiveTargetVolumeReached);
    QObject::connect(&dfUiServer, &DfUiServer::initReady, initPage, &page_init::initReadySlot);
    QObject::connect(&dfUiServer, &DfUiServer::MM, idlePage, &page_idle::MMSlot);

    return mainApp.exec();
}
