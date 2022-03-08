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

void myMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    
// https://www.qtcentre.org/threads/19534-redirect-qDebug%28%29-to-file
	// QString txt;
	// switch (type) {
	// case QtDebugMsg:
	// 	txt = QString("Debug: %1").arg(msg);
	// 	break;
	// case QtWarningMsg:
	// 	txt = QString("Warning: %1").arg(msg);
	// break;
	// case QtCriticalMsg:
	// 	txt = QString("Critical: %1").arg(msg);
	// break;
	// case QtFatalMsg:
	// 	txt = QString("Fatal: %1").arg(msg);
	// 	abort();
	// }
    
	// QFile outFile("/home/df-admin/log/lodesnexttest.txt");

    // log_file = &outFile;

    // if (!log_file->open(QFile::WriteOnly | QFile::Text | QFile::Append))
    // {
    //     qInstallMessageHandler(nullptr);
    //     qDebug() << "Couldn't log to file!";
    // }



	// // outFile.open(QIODevice::WriteOnly | QIODevice::Append);
	// outFile.open(QIODevice::Append);
	// QTextStream ts(&outFile);
	// ts << txt << endl;

    // outFile.close();

    Q_UNUSED(type)
    Q_UNUSED(context)

    printf("%s\n", qPrintable(msg));
    fflush(stdout);

    
    QString line_out = "";

	// using namespace std::chrono;
    // uint64_t millis_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    // line_out.append(QString::number(millis_since_epoch));
    
    // line_out.append(QString::fromStdString(lode));

    QString time_stamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

   
    // string time_stamp = df_util::format_string(df_util::getTimeStamp(), "%Y-%m-%dT%H:%M:%S.%ms%Z");
    // line_out.append(QString::fromStdString(time_stamp));

    line_out.append(time_stamp);
	line_out.append(" : ");
    line_out.append(msg);


    QFile file("/home/df-admin/drinkfill/log/soapstand_log_ui_all.txt");

    log_file = &file;

    if (!log_file->open(QFile::WriteOnly | QFile::Text | QFile::Append))
    {
        qInstallMessageHandler(nullptr);
        qDebug() << "Couldn't log to file!";
    }else{
       
    }


    if (log_file)
    {
        // log_file->write(msg.toLatin1());
        log_file->write(line_out.toUtf8());
        log_file->write("\n");
        log_file->flush();
    }

}


// void myMessageHandler(QtMsgType type, const char *msg)
// {
    
// // https://www.qtcentre.org/threads/19534-redirect-qDebug%28%29-to-file
// 	QString txt;
// 	switch (type) {
// 	case QtDebugMsg:
// 		txt = QString("Debug: %1").arg(msg);
// 		break;
// 	case QtWarningMsg:
// 		txt = QString("Warning: %1").arg(msg);
// 	break;
// 	case QtCriticalMsg:
// 		txt = QString("Critical: %1").arg(msg);
// 	break;
// 	case QtFatalMsg:
// 		txt = QString("Fatal: %1").arg(msg);
// 		abort();
// 	}
// 	QFile outFile("log");
// 	outFile.open(QIODevice::WriteOnly | QIODevice::Append);
// 	QTextStream ts(&outFile);
// 	ts << txt << endl;
// }

// void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
// {
//     //https://stackoverflow.com/questions/4954140/how-to-redirect-qdebug-qwarning-qcritical-etc-output
//     static QMutex mutex;
//     QMutexLocker lock(&mutex);

//     LOGFILE_LOCATION = "testlode"
//     static QFile logFile(LOGFILE_LOCATION);
//     static bool logFileIsOpen = logFile.open(QIODevice::Append | QIODevice::Text);

//     std::cerr << qPrintable(qFormatLogMessage(type, context, message)) << std::endl;

//     if (logFileIsOpen) {
//         logFile.write(qFormatLogMessage(type, context, message).toUtf8() + '\n');
//         logFile.flush();
//     }
// }


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
    page_maintenance_dispenser* p_page_maintenance_product = new page_maintenance_dispenser();

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
    p_pageProduct->setPage(firstSelectPage, dispensingPage,wifiError, idlePage, paymentPage, helpPage);
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

