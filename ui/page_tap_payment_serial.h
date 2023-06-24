//***************************************
//
// page_tap_payment_serial.h
// GUI class while machine is processing
// payment.
//
// Coordinates User input from payment select
// class then communcates results to page_dispenser.
//
// created: 05-04-2022
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef page_tap_payment_serial_H
#define page_tap_payment_serial_H

#include "df_util.h"

#include "page_help.h"

#include "posm/mcommunication.h"
#include "posm/packetfromecr.h"
#include "posm/packetfromux410.h"
#include "posm/transactionPackets.h"
#include "posm/transactioninfo.h"

#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>

class page_product;
class page_dispenser;
class page_idle;
class page_help;

namespace Ui {
class page_tap_payment_serial;
}

typedef enum StatePaymentSerial{
    s_serial_init,
    s_serial_payment_processing,
    s_serial_payment_done
}StatePaymentSerial;

using namespace std;

class page_tap_payment_serial : public QWidget
{
    Q_OBJECT

public:
    // **** GUI Setup ****
    explicit page_tap_payment_serial(QWidget *parent = nullptr);
    void setPage(page_product *p_page_product, page_error_wifi *pageWifiError, page_dispenser *page_dispenser, page_idle *pageIdle, page_help *pageHelp);
    ~page_tap_payment_serial();

    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    bool exitConfirm();

    bool setpaymentProcess(bool status);

    /* mpos */
    void stayAliveLogon();
    void batchClose();

    void sendCommand();

    string getTerminalID(){
        return terminalID;
    }

    string getMerchantName(){
        return merchantName;
    }

    string getMerchantAddress() {
        return merchantAddress;
    }
    void tap_serial_initiate();

    QTimer *readTimer;
    StatePaymentSerial state_payment;

private slots:

    // Navigation
    void on_pushButton_previous_page_clicked();
    void on_pushButton_payment_bypass_clicked();
    void on_pushButton_to_idle_clicked();

    void readTimer_loop();
    void idlePaymentTimeout();

private:
    // **** GUI ****
    Ui::page_tap_payment_serial *ui;
    page_product *p_page_product;
    page_dispenser *p_page_dispense;
    page_idle *p_page_idle;
    page_help *p_page_help;
    page_error_wifi *p_page_wifi_error;
   
    bool approved = false;

    void stopPayTimers();

    QSqlDatabase db;

 // Tap Payment Control
    string merchantName;
    string merchantAddress;
    string terminalID;
    mCommunication com;
    packetFromECR sendPacket;
    packetFromUX410 readPacket;
    transactionPacket paymentPacket;
    transactionInfo paymentPktInfo;
    bool paymentProcessing;
    
    bool isInitCancelled;
    bool isInitBatched;
    bool isInitLogin;
    bool isInitMerchant;
    bool isInitAddress;
    bool isInitTerminalID;
    // Payment Package Control
    bool purchaseEnable;
    bool response;
    bool paymentConnected = false;
    bool isReadyForTap = false;
    int monerisConfig = 0;
    bool timerEnabled;
    std::vector<uint8_t> pktToSend;
    std::vector<uint8_t> pktResponded;
    std::string productSelectedPrice;
    bool sendToUX410();
    bool waitForUX410();
    void cancelPayment();
    bool getResponse(){return response;}
    bool tapSetupStarted =false;

    QTimer* paymentEndTimer;
    QTimer* idlePaymentTimer;
    void resetPaymentPage();
    int tmpCounter ;
};


#endif // page_tap_payment_serial_H
