//***************************************
//
// page_payment_tap_tcp.h
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

#ifndef page_tap_payment_H
#define page_tap_payment_H

#include "df_util.h"

#include "page_help.h"
#include "page_error_wifi.h"
#include "payment/commands.h"
#include "payment/setup_Tap.h"
#include <QProcess>

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
#include <QMovie>
#include <curl/curl.h>
#include <atomic>

extern std::atomic<bool> stop_tap_action_thread;
extern std::atomic<bool> stop_authorization_thread;

class statusbar;
class page_product;
class page_product_mixing;
class page_dispenser;
class page_idle;
class page_help;

namespace Ui
{
    class page_payment_tap_tcp;
}

typedef enum StateTapPayment
{
    s_tap_init,
    s_tap_payment_processing,
    s_tap_payment_done
} StateTapPayment;

using namespace std;

class page_payment_tap_tcp : public QWidget
{
    Q_OBJECT

public:
    // **** GUI Setup ****
    explicit page_payment_tap_tcp(QWidget *parent = nullptr);
    void setPage(page_product *p_page_product, page_product_mixing *p_page_product_mixing, page_error_wifi *pageWifiError, page_dispenser *page_dispenser, page_idle *pageIdle, page_help *pageHelp, statusbar *p_statusbar);
    ~page_payment_tap_tcp();

    void showEvent(QShowEvent *event);
    bool exitConfirm();

    // **** Control Functions ****
    void initiate_tap_setup();
    bool setpaymentProcess(bool status);
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    void authorized_transaction(std::map<std::string, std::string> responseObj);

    StateTapPayment state_tap_payment; 


private slots:
    // Navigation
    void on_pushButton_previous_page_clicked();
    void on_pushButton_payment_bypass_clicked();
    void on_pushButton_to_idle_clicked();

    // **** Payment ****
    void tapPaymentHandler();
    void check_packet_available();
    void check_card_tapped();
    void startPaymentProcess();

    void idlePaymentTimeout();
private:
     QMessageBox* msgBox;
    // **** GUI ****
    Ui::page_payment_tap_tcp *ui;
    page_product *p_page_product;
    page_product_mixing *p_page_product_mixing;
    page_dispenser *p_page_dispense;
    page_idle *p_page_idle;
    page_help *p_page_help;
    page_error_wifi *p_page_wifi_error;
    statusbar *p_statusbar;

    void stopPayTimers();

    // Payment Control
    bool paymentProcessing;

    QSqlDatabase db;

    QTimer *checkPacketReceivedTimer;
    QTimer *checkCardTappedTimer;

    QTimer *idlePaymentTimer;
    QTimer *inFlightTimer;

    std::string productSelectedPrice;

    void cancelPayment();

    bool response;
    bool tap_payment;
    int lastTransactionId;

    void resetPaymentPage();
    QString getPaymentMethod();
    QVBoxLayout *statusbarLayout;
};

#endif // page_tap_payment_H
