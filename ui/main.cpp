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
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "df_util.h"
#include "page_transactions.h"
#include "page_help.h"
#include "page_init.h"
#include "page_idle.h"
#include "page_idle_products.h"
#include "page_select_product.h"
#include "page_product.h"
#include "page_qr_payment.h"
#include "page_payment_tap_tcp.h"
#include "page_payment_tap_serial.h"
#include "page_dispenser.h"
#include "page_error_wifi.h"
#include "page_productOverview.h"

#include "page_end.h"
#include <QApplication>
#include "page_maintenance.h"
#include "page_maintenance_dispenser.h"
#include "page_maintenance_general.h"
#include <QCoreApplication>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QQuickView>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QDBusConnection>

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

    QString logging_controller_base_path = "/home/df-admin/production/logging/controller";
    QString logging_wifi_base_path = "/home/df-admin/production/logging/wifi";
    QString logging_transactions_base_path = "/home/df-admin/production/logging/transactions";
    QString logging_ui_base_path = "/home/df-admin/production/logging/ui";

    // // Check if the directory exists, create it if not
    // QDir uidir(QFileInfo(logging_ui_base_path).absolutePath());
    // if (!uidir.exists())
    // {
    //     uidir.mkpath(".");
    // }

    // Check logging_controller_base_path
    QDir uidir(logging_ui_base_path);
    if (!uidir.exists())
    {
        uidir.mkpath(".");
    }
    // Check logging_controller_base_path
    QDir controllerDir(logging_controller_base_path);
    if (!controllerDir.exists())
    {
        controllerDir.mkpath(".");
    }

    // Check logging_wifi_base_path
    QDir wifiDir(logging_wifi_base_path);
    if (!wifiDir.exists())
    {
        wifiDir.mkpath(".");
    }

    // Check logging_transactions_base_path
    QDir transactionsDir(logging_transactions_base_path);
    if (!transactionsDir.exists())
    {
        transactionsDir.mkpath(".");
    }

    qInstallMessageHandler(myMessageHandler); // Install the handler
    QString title = QString("********** START SOAPSTAND UI v%1 *********************************").arg(QString(UI_VERSION));

    qDebug() << "***************************************************************************";
    qDebug() << title;
    qDebug() << "***************************************************************************";

    // Fire up QT GUI Thread
    qDebug() << "Start main thread";
    QApplication mainApp(argc, argv);

    // Instantiate object to hold drink order

    // Build objects to hold navigation (pages)
    // Linking resources and Function definitions for pathing
    qDebug() << "Constructor page_idle";
    page_idle *p_page_idle = new page_idle();
    qDebug() << "Constructor page_help";
    page_help *p_page_help = new page_help();
    qDebug() << "Constructor page_init";
    page_init *initPage = new page_init();
    qDebug() << "Constructor page_idle_products";
    page_idle_products *p_page_idle_products = new page_idle_products();
    qDebug() << "Constructor page_transactions";
    page_transactions *p_page_transactions = new page_transactions();

    qDebug() << "Constructor page_select_product";
    page_select_product *p_page_select_product = new page_select_product();
    qDebug() << "Constructor page_product";
    page_product *p_page_product = new page_product();
    qDebug() << "Constructor page_qr_payment";
    page_qr_payment *p_page_payment_qr = new page_qr_payment();
    qDebug() << "Constructor page_payment_tap_tcp";
    page_payment_tap_tcp *p_page_payment_tap_tcp = new page_payment_tap_tcp();
    qDebug() << "Constructor page_payment_tap_serial";
    page_payment_tap_serial *p_page_payment_tap_serial = new page_payment_tap_serial();
    qDebug() << "Constructor page_dispenser";
    page_dispenser *p_page_dispense = new page_dispenser();
    qDebug() << "Constructor page_error_wifi";
    page_error_wifi *p_page_wifi_error = new page_error_wifi();
    qDebug() << "Constructor page_end";
    page_end *p_page_end = new page_end();
    qDebug() << "Constructor page_product_overview";
    page_product_overview *p_page_product_overview = new page_product_overview();
    qDebug() << "Constructor page_sendFeedback";
    page_sendFeedback *p_page_sendFeedback = new page_sendFeedback();
    qDebug() << "Constructor page_maintenance";
    page_maintenance *p_page_maintenance = new page_maintenance();
    qDebug() << "Constructor page_maintenance_dispenser";
    page_maintenance_dispenser *p_page_maintenance_product = new page_maintenance_dispenser();
    qDebug() << "Constructor page_maintenance_general";
    page_maintenance_general *p_page_maintenance_general = new page_maintenance_general();
    qDebug() << "All Pages created.";

    DbManager db_config;
    machine thisMachine;
    product products[MAX_SLOT_COUNT];

    p_page_idle->setMachine(&thisMachine) :

                                            p_page_idle->g_database = &db_config;

    db_config.updateTableMachineWithText("software_version", UI_VERSION);
    thisMachine.setProducts(products);
    thisMachine.setDb(&db_config);
    thisMachine.initMachine(); // first setDb
    thisMachine.loadDynamicContent();

    qDebug() << "Check image paths.... (all paths resolved if nothing shows up).";

    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_IDLE_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_IDLE_PRODUCTS_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_END_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_TRANSACTIONS_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_SELECT_PRODUCT_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_PRODUCT_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_ORDER_OVERVIEW_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_DISPENSE_INSTRUCTIONS_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_DISPENSE_INSTRUCTIONS_MULTISPOUT_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_DISPENSE_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_QR_PAY_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_MAINTENANCE_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(ERROR_MESSAGE_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_TAP_PAY));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_TAP_PAY_SUCCESS));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_TAP_PAY_FAIL));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_AUTHORIZE_NOW));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_TAP_GENERIC));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_SEND_FEEDBACK_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_INIT_BACKGROUND_IMAGE_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(IMAGE_BUTTON_HELP));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(THANK_YOU_FOR_YOUR_FEEDBACK));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_ERROR_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(KEYBOARD_IMAGE_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(MACHINE_LOGO_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_DISPENSE_FILL_ANIMATION));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(ICON_TYPE_CONCENTRATE_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(ICON_TYPE_ALL_PURPOSE_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(ICON_TYPE_DISH_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(ICON_TYPE_HAND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(ICON_TYPE_LAUNDRY_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(ICON_TYPE_KOMBUCHA_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(ICON_TYPE_PROBIOTIC_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(ICON_TYPE_JUICE_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(ICON_TYPE_DEFAULT));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_HELP_BACKGROUND_GENERIC_WHITE));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_HELP_BACKGROUND_PATH));

    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_SELECT_PRODUCT_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_PRODUCT_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_ORDER_OVERVIEW_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_DISPENSE_INSTRUCTIONS_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_DISPENSE_INSTRUCTIONS_MULTISPOUT_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_DISPENSE_INSTRUCTIONS_SPOUT_INDICATOR_RIGHT));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_DISPENSE_INSTRUCTIONS_SPOUT_INDICATOR_DOWN));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_DISPENSE_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_QR_PAY_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_MAINTENANCE_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(ERROR_MESSAGE_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_TAP_PAY));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_TAP_PAY_SUCCESS));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_TAP_PAY_FAIL));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_AUTHORIZE_NOW));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_TAP_GENERIC));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_SEND_FEEDBACK_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_INIT_BACKGROUND_IMAGE_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(IMAGE_BUTTON_HELP));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(THANK_YOU_FOR_YOUR_FEEDBACK));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_ERROR_BACKGROUND_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(KEYBOARD_IMAGE_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(MACHINE_LOGO_PATH));
    df_util::warnIfPathDoesNotExist(p_page_idle->thisMachine.getTemplatePathFromName(PAGE_DISPENSE_FILL_ANIMATION));

    // Page pathing references to function calls.
    p_page_help->setPage(p_page_select_product, p_page_product, p_page_idle, p_page_payment_qr, p_page_transactions, p_page_maintenance, p_page_sendFeedback);
    p_page_transactions->setPage(p_page_idle);
    initPage->setPage(p_page_idle);
    p_page_maintenance_product->setPage(p_page_maintenance, p_page_idle, p_page_idle_products);

    p_page_maintenance_general->setPage(p_page_maintenance, p_page_idle, p_page_idle_products);
    p_page_maintenance->setPage(p_page_idle, p_page_maintenance_product, p_page_maintenance_general, p_page_select_product, p_page_product);
    p_page_idle->setPage(p_page_select_product, p_page_maintenance, p_page_maintenance_general, p_page_idle_products, p_page_wifi_error);
    p_page_idle_products->setPage(p_page_idle, p_page_select_product);
    p_page_select_product->setPage(p_page_product, p_page_idle_products, p_page_idle, p_page_maintenance, p_page_help);
    p_page_product->setPage(p_page_select_product, p_page_dispense, p_page_wifi_error, p_page_idle, p_page_payment_qr, p_page_payment_tap_serial, p_page_payment_tap_tcp, p_page_help, p_page_product_overview);
    p_page_payment_qr->setPage(p_page_product, p_page_wifi_error, p_page_dispense, p_page_idle, p_page_help);
    p_page_payment_tap_tcp->setPage(p_page_product, p_page_wifi_error, p_page_dispense, p_page_idle, p_page_help);
    p_page_payment_tap_serial->setPage(p_page_product, p_page_wifi_error, p_page_dispense, p_page_idle, p_page_help);

    p_page_dispense->setPage(p_page_payment_qr, p_page_payment_tap_serial, p_page_payment_tap_tcp, p_page_end, p_page_idle, p_page_sendFeedback);
    p_page_product_overview->setPage(p_page_select_product, p_page_dispense, p_page_wifi_error, p_page_idle, p_page_payment_qr, p_page_payment_tap_serial, p_page_payment_tap_tcp, p_page_help, p_page_product);
    p_page_sendFeedback->setPage(p_page_select_product, p_page_dispense, p_page_wifi_error, p_page_idle, p_page_payment_qr, p_page_help, p_page_product, p_page_end);
    p_page_end->setPage(p_page_dispense, p_page_idle, p_page_payment_qr, p_page_sendFeedback);
    p_page_wifi_error->setPage(p_page_payment_qr, p_page_end, p_page_idle);

    initPage->showFullScreen();

    // listen for fsm messages
    DfUiServer dfUiServer;
    dfUiServer.startServer();

    QObject::connect(&dfUiServer, &DfUiServer::controllerFinishedAck, p_page_end, &page_end::controllerFinishedTransaction);
    QObject::connect(&dfUiServer, &DfUiServer::printerStatus, p_page_maintenance_general, &page_maintenance_general::printerStatusFeedback);
    QObject::connect(&dfUiServer, &DfUiServer::printerStatus, p_page_idle, &page_idle::printerStatusFeedback);
    // QObject::connect(&dfUiServer, &DfUiServer::printerStatus, p_page_idle_products, &page_idle_products::printerStatusFeedback);
    QObject::connect(&dfUiServer, &DfUiServer::pleaseReset, p_page_dispense, &page_dispenser::resetDispenseTimeout);

    QObject::connect(&dfUiServer, &DfUiServer::signalUpdateVolume, p_page_dispense, &page_dispenser::fsmReceivedVolumeDispensed);
    QObject::connect(&dfUiServer, &DfUiServer::signalDispenseStatus, p_page_dispense, &page_dispenser::fsmReceiveDispenserStatus);
    QObject::connect(&dfUiServer, &DfUiServer::finalVolumeDispensed, p_page_end, &page_end::fsmReceiveFinalDispensedVolume);
    QObject::connect(&dfUiServer, &DfUiServer::signalDispenseRate, p_page_dispense, &page_dispenser::fsmReceiveDispenseRate);
    QObject::connect(&dfUiServer, &DfUiServer::targetHit, p_page_dispense, &page_dispenser::fsmReceiveTargetVolumeReached);
    QObject::connect(&dfUiServer, &DfUiServer::noFlowAbort, p_page_dispense, &page_dispenser::fsmReceiveNoFlowAbort);

    QObject::connect(&dfUiServer, &DfUiServer::signalUpdateVolume, p_page_maintenance_product, &page_maintenance_dispenser::fsmReceivedVolumeDispensed);
    QObject::connect(&dfUiServer, &DfUiServer::signalDispenseStatus, p_page_maintenance_product, &page_maintenance_dispenser::fsmReceiveDispenserStatus);
    QObject::connect(&dfUiServer, &DfUiServer::signalDispenseRate, p_page_maintenance_product, &page_maintenance_dispenser::fsmReceiveDispenseRate);
    QObject::connect(&dfUiServer, &DfUiServer::dispenseButtonPressedPosEdgeSignal, p_page_maintenance_product, &page_maintenance_dispenser::fsmReceiveDispenseButtonPressedPositiveEdge);
    QObject::connect(&dfUiServer, &DfUiServer::dispenseButtonPressedNegEdgeSignal, p_page_maintenance_product, &page_maintenance_dispenser::fsmReceiveDispenseButtonPressedNegativeEdge);
    QObject::connect(&dfUiServer, &DfUiServer::targetHit, p_page_maintenance_product, &page_maintenance_dispenser::fsmReceiveTargetVolumeReached);
    QObject::connect(&dfUiServer, &DfUiServer::noFlowAbort, p_page_maintenance_product, &page_maintenance_dispenser::fsmReceiveNoFlowAbort);
    QObject::connect(&dfUiServer, &DfUiServer::temperatureSignal, p_page_maintenance_product, &page_maintenance_dispenser::fsmReceiveTemperature);
    // QObject::connect(&dfUiServer, &DfUiServer::temperature2Signal, p_page_maintenance_product, &page_maintenance_dispenser::fsmReceiveTemperature2);

    QObject::connect(&dfUiServer, &DfUiServer::initReady, initPage, &page_init::initReadySlot);
    // QObject::connect(&dfUiServer, &DfUiServer::MM, p_page_idle, &page_idle::MMSlot);

    return mainApp.exec();
}
