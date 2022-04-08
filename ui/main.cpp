//***************************************
//
// main.cpp
// GUI class while machine is idle.
//
// Display Fullscreen DF branded Wallpaper
// Listen for User interaction to load
// Product Page1
//
// created: 05-04-2022
// by: Lode Ameije & Ash Singla
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
    QString log_file_base_path = "/home/df-admin/production/logging/ui/ui_%1.txt"; // https://stackoverflow.com/questions/4784155/how-to-format-a-qstring
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
    page_idle *p_page_idle = new page_idle();
    page_select_product *firstSelectPage = new page_select_product();
    pageProduct *p_pageProduct = new pageProduct();
    pagePayment *paymentPage = new pagePayment();
    page_dispenser *p_page_dispense = new page_dispenser();
    page_error_wifi *wifiError = new page_error_wifi();
    pagethankyou *p_page_thank_you = new pagethankyou();
    page_maintenance *p_page_maintenance = new page_maintenance();
    page_maintenance_dispenser *p_page_maintenance_product = new page_maintenance_dispenser();

    // TODO: Instantiate a DrinkSelection[] Array
    // TODO: Create Query to populate DrinkSelection[0-12]
    // TODO: Instantiate a DrinkOrder Object

    qDebug()<< "Check image paths.... (all paths resolved if nothing shows up).";

    df_util::fileExists(PAGE_INIT_BACKGROUND_PATH);
    df_util::fileExists(PAGE_IDLE_BACKGROUND_PATH);
    df_util::fileExists(PAGE_SELECT_PRODUCT_BACKGROUND_PATH);
    df_util::fileExists(PAGE_HELP_BACKGROUND_PATH);
    // df_util::fileExists(PAGE_PRODUCT_1_L_BACKGROUND_PATH);
    // df_util::fileExists(PAGE_PRODUCT_2_L_BACKGROUND_PATH);
    // df_util::fileExists(PAGE_PRODUCT_3_L_BACKGROUND_PATH);
    // df_util::fileExists(PAGE_PRODUCT_4_L_BACKGROUND_PATH);
    // df_util::fileExists(PAGE_PRODUCT_1_S_BACKGROUND_PATH);
    // df_util::fileExists(PAGE_PRODUCT_2_S_BACKGROUND_PATH);
    // df_util::fileExists(PAGE_PRODUCT_3_S_BACKGROUND_PATH);
    // df_util::fileExists(PAGE_PRODUCT_4_S_BACKGROUND_PATH);
    df_util::fileExists(PAGE_DISPENSE_INSTRUCTIONS_BACKGROUND_PATH);
    df_util::fileExists(PAGE_QR_PAY_BACKGROUND_PATH);
    // df_util::fileExists(PRODUCT_1_IMAGE_PATH);
    // df_util::fileExists(PRODUCT_2_IMAGE_PATH);
    // df_util::fileExists(PRODUCT_3_IMAGE_PATH);
    // df_util::fileExists(PRODUCT_4_IMAGE_PATH);
    df_util::fileExists(PAGE_THANK_YOU_BACKGROUND_PATH);
    df_util::fileExists(PAGE_WIFI_ERROR_BACKGROUND_PATH);
    df_util::fileExists(BOTTLE_FILL_FOR_ANIMATION_IMAGE_PATH);
    df_util::fileExists(SOLD_OUT_IMAGE_PATH);
    df_util::fileExists(KEYBOARD_IMAGE_PATH);
    df_util::fileExists(FULL_TRANSPARENT_IMAGE_PATH);

    // Page pathing references to function calls.
    helpPage->setPage(firstSelectPage, p_pageProduct, p_page_idle, paymentPage);
    initPage->setPage(p_page_idle);
    p_page_maintenance_product->setPage(p_page_maintenance, p_page_idle);
    p_page_maintenance->setPage(p_page_idle, p_page_maintenance_product, firstSelectPage, p_pageProduct);
    p_page_idle->setPage(firstSelectPage, p_page_maintenance);
    firstSelectPage->setPage(p_pageProduct, p_page_idle, p_page_maintenance, helpPage);
    p_pageProduct->setPage(firstSelectPage, p_page_dispense, wifiError, p_page_idle, paymentPage, helpPage);
    paymentPage->setPage(p_pageProduct, p_page_dispense, p_page_idle, helpPage);
    p_page_dispense->setPage(paymentPage, p_page_thank_you, p_page_idle);
    p_page_thank_you->setPage(p_page_dispense, p_page_idle, paymentPage);
    wifiError->setPage(paymentPage, p_page_thank_you, p_page_idle);
    
    initPage->showFullScreen();

    DfUiServer dfUiServer;
    dfUiServer.startServer();
 
    QObject::connect(&dfUiServer, &DfUiServer::controllerFinishedAck, p_page_thank_you, &pagethankyou::controllerFinishedTransaction);
    QObject::connect(&dfUiServer, &DfUiServer::printerStatus, p_page_maintenance, &page_maintenance::printerStatusFeedback);
    QObject::connect(&dfUiServer, &DfUiServer::pleaseReset, p_page_dispense, &page_dispenser::resetDispenseTimeout);
    
    QObject::connect(&dfUiServer, &DfUiServer::signalUpdateVolume, p_page_dispense, &page_dispenser::updateVolumeDisplayed);
    QObject::connect(&dfUiServer, &DfUiServer::targetHit, p_page_dispense, &page_dispenser::fsmReceiveTargetVolumeReached);
    
    QObject::connect(&dfUiServer, &DfUiServer::noFlowAbort, p_page_dispense, &page_dispenser::fsmReceiveNoFlowAbort);
    
    QObject::connect(&dfUiServer, &DfUiServer::signalUpdateVolume, p_page_maintenance_product, &page_maintenance_dispenser::updateVolumeDisplayed);
    QObject::connect(&dfUiServer, &DfUiServer::targetHit, p_page_maintenance_product, &page_maintenance_dispenser::fsmReceiveTargetVolumeReached);
    QObject::connect(&dfUiServer, &DfUiServer::noFlowAbort, p_page_maintenance_product, &page_maintenance_dispenser::fsmReceiveNoFlowAbort);
    
    QObject::connect(&dfUiServer, &DfUiServer::initReady, initPage, &page_init::initReadySlot);
    QObject::connect(&dfUiServer, &DfUiServer::MM, p_page_idle, &page_idle::MMSlot);

    return mainApp.exec();
}
