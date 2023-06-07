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
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef page_tap_payment_serial_H
#define page_tap_payment_serial_H

#include "df_util.h"
#include "drinkorder.h"
#include "page_help.h"

#include "posm/mcommunication.h"
#include "posm/packetfromecr.h"
#include "posm/packetfromux410.h"
#include "posm/transactionPackets.h"
#include "posm/transactioninfo.h"

#include "../library/qr/qrcodegen.hpp"
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <QPainter>
#include <QUuid>

#include <QMovie>
#include <curl/curl.h>

class pageProduct;
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
    void setPage(page_idle *pageIdle,pageProduct *pageSizeSelect, page_dispenser *page_dispenser);
    ~page_tap_payment_serial();
    void setProgressLabel(QLabel* label, int dot);
    // TODO: Figure out better Style Setup.
    void labelSetup(QLabel *label, int fontSize);

    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    bool exitConfirm();

    // **** Control Functions ****
    bool setpaymentProcess(bool status);

    // Database
    void storePaymentEvent(QSqlDatabase db, QString event);

    /* mpos */
    void stayAliveLogon();
    void batchClose();
    int getSelectedPriceSelect();
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



    QTimer *readTimer;
    // char * curl_data;
    // char * curl_data1;
    // char * curlOrderdata;
    StatePaymentSerial state_payment;

private slots:

    // Update Drink order totals section
    void updateTotals(string drinkDescription, string drinkAmount, string orderTotal);

    // Navigation
    void on_previousPage_Button_clicked();
    void on_payment_bypass_Button_clicked();
    void proceed_to_dispense();
     void on_mainPage_Button_clicked();
    //void on_payment_pass_Button_clicked();
    //void on_payment_cancel_Button_clicked();
    // For Debugging; will be removed.

    // **** Payment ****
    void displayPaymentPending(bool isVisible);

    void readTimer_loop();
    void progressStatusLabel();
    void declineTimer_start();
    void idlePaymentTimeout();


protected:

//    void paintEvent(QPaintEvent *p);

private:
    // **** GUI ****
    Ui::page_tap_payment_serial *ui;
    pageProduct* p_pageProduct;
    page_dispenser* p_page_dispense;
    page_idle* p_page_idle;
    page_help* helpPage;

    const QString TAP_READY_LABEL = "Ready for Tap";
    const QString TAP_PROCESSING_LABEL = "Processing";
    const QString TAP_DECLINED_LABEL = "Card Declined";
    const QString TAP_AGAIN_LABEL = "Try Again";
    const QString TAP_APPROVED_LABEL = "Payment Approved";
    const QString TAP_BLANK_LABEL = "";

    // Label Reference for order Details
    string drinkAmount;
    string drinkDescription;
    string orderTotal;

    // TODO: Put payment information into XML
    // Payment progress timer    
    bool approved = false;
    bool paymentConnected = false;
    bool isReadyForTap = false;

    void stopPayTimers();

    int progressDots = 1;
    int progressLoopCounter = 0;
    int declineCounter;

    // **** Payment ****
    // Payment Information
    string merchantName;
    string merchantAddress;
    string terminalID;

    // Payment Control
    bool paymentProcessing;

    bool isInitCancelled;
    bool isInitBatched;
    bool isInitLogin;
    bool isInitMerchant;
    bool isInitAddress;
    bool isInitTerminalID;

    QSqlDatabase db;


    QTimer *declineTimer;
    QTimer *paymentProgressTimer;
    QTimer *idlePaymentTimer;

    // Payment Communication
    // Moneris Packet communication reference
    mCommunication com;
    packetFromECR sendPacket;
    packetFromUX410 readPacket;
    transactionPacket paymentPacket;
    transactionInfo paymentPktInfo;

    // Payment Package Control
    bool purchaseEnable;
    int monerisConfig = 0;
    bool timerEnabled;
    std::vector<uint8_t> pktToSend;
    std::vector<uint8_t> pktResponded;
    std::string productSelectedPrice;
    bool sendToUX410();
    bool tap_init();
    bool waitForUX410();
    void cancelPayment();
    bool getResponse(){return response;}

    // **** Drink Order Reference ****
    // DrinkOrder paymentDrinkOrder;

    // Placeholder
    bool surveyBool;

    QString _paymentTimeLabel;
    int _pageTimeoutCounterSecondsLeft;
    QTimer* paymentEndTimer;

    QResizeEvent *pageProductResize;
    QShowEvent *dispenseEvent;

    bool response;
    // bool tap_payment;
    void resetPaymentPage();
    QString getPaymentMethod();


    int tmpCounter ;

   
    
};

#endif // page_tap_payment_serial_H
