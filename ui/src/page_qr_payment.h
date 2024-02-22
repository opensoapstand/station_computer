//***************************************
//
// page_qr_payment.h
// GUI class while machine is processing
// payment.
//
// Coordinates User input from payment select
// class then communcates results to page_dispenser.
//
// created: 05-04-2022
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef page_qr_payment_H
#define page_qr_payment_H

#include "df_util.h"

#include "page_help.h"
#include "page_error_wifi.h"

#include "../library/qr/qrcodegen.hpp"
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <QCryptographicHash>
#include <openssl/evp.h>

#include <QPainter>
#include <QUuid>
#include <QMovie>
#include <curl/curl.h>
#include<atomic>

class statusbar;
class page_product;
class page_dispenser;
class page_idle;
class page_help;
class page_product_mixing;

namespace Ui {
class page_qr_payment;
}

typedef enum StatePayment{
    s_init,
    s_payment_processing,
    s_payment_done
}StatePayment;

using namespace std;
using namespace qrcodegen;

class page_qr_payment : public QWidget
{
    Q_OBJECT

public:
    // **** GUI Setup ****
    explicit page_qr_payment(QWidget *parent = nullptr);
    void setPage(page_product* p_page_product,page_error_wifi *pageWifiError, page_dispenser* page_dispenser, page_idle* pageIdle, page_help *pageHelp, statusbar *p_statusbar, page_product_mixing* p_page_product_mixing);
    ~page_qr_payment();

    void showEvent(QShowEvent *event);
    bool exitConfirm();

    QString getOID(){
        return orderId;
    }

    StatePayment state_payment;
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);

private slots:

    void on_pushButton_previous_page_clicked();
    void proceed_to_dispense();
    void onTimeoutTick();
    void idlePaymentTimeout();
    void on_pushButton_refresh_clicked();
    void qrProcessedPeriodicalCheck();
    void showErrorTimerPage();
    QString generateCode(QString str);

private:
    QMessageBox* msgBox;
    Ui::page_qr_payment *ui;
    page_product* p_page_product;
    page_dispenser* p_page_dispense;
    page_idle* p_page_idle;
    page_help* p_page_help;
    page_error_wifi *p_page_wifi_error;
    statusbar *p_statusbar;
    page_product_mixing *p_page_product_mixing;

    QString _paymentTimeLabel;
    int _pageTimeoutCounterSecondsLeft;
    QTimer* paymentEndTimer;

    int _qrProcessedPeriodicalCheckSec;
    QTimer* qrPeriodicalCheckTimer;
    QTimer* showErrorTimer;

    void printQr(const QrCode &qr);
    std::string toSvgString(const QrCode &qr, int border);
    void paintQR(QPainter &painter, const QSize sz, const QString &data, QColor fg);
    void resetPaymentPage();

    QString orderId="";

    CURLcode res;
    std::string readBuffer;
    long http_code;
    QVBoxLayout *statusbarLayout;

    void isQrProcessedCheckOnline();
    void setupQrOrder();
    bool createOrderIdAndSendToBackend();
    
};

#endif // page_qr_payment_H