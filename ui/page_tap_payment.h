//***************************************
//
// page_tap_payment.h
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

#ifndef page_tap_payment_H
#define page_tap_payment_H

#include "df_util.h"
#include "drinkorder.h"
#include "page_help.h"
#include "page_error_wifi.h"
#include "payment/commands.h"
#include "payment/setup_Tap.h"

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
#include <QPainter>
#include <QUuid>
#include <QMovie>
#include <curl/curl.h>
#include<atomic>

extern std::atomic<bool> stop_tap_action_thread;
extern std::atomic<bool> stop_authorization_thread;

class pageProduct;
class page_dispenser;
class page_idle;
class page_help;

namespace Ui {
class page_tap_payment;
}

typedef enum StateTapPayment{
    s_tap_init,
    s_tap_payment_processing,
    s_tap_payment_done
}StateTapPayment;

using namespace std;
using namespace qrcodegen;

class page_tap_payment : public QWidget
{
    Q_OBJECT

public:
    // **** GUI Setup ****
    explicit page_tap_payment(QWidget *parent = nullptr);
    void setPage(pageProduct* pageSizeSelect,page_error_wifi *pageWifiError, page_dispenser* page_dispenser, page_idle* pageIdle, page_help *pageHelp);
    ~page_tap_payment();
    void setProgressLabel(QLabel* label, int dot);
    // TODO: Figure out better Style Setup.
    void labelSetup(QLabel *label, int fontSize);

    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    bool exitConfirm();

    // **** Control Functions ****
    bool setpaymentProcess(bool status);
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    void authorized_transaction(std::map<std::string, std::string> responseObj);

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

    QString getOID(){
        return orderId;
    }
    QTimer *readTimer;
    // char * curl_data;
    // char * curl_data1;
    // char * curlOrderdata;
    StateTapPayment state_tap_payment;

// signals:
//     void cardTapped();
private slots:

    // Update Drink order totals section
    void updateTotals(string drinkDescription, string drinkAmount, string orderTotal);

    // Navigation
    void on_pushButton_previous_page_clicked();
    void on_payment_bypass_Button_clicked();
    void on_mainPage_Button_clicked();

    //void on_payment_pass_Button_clicked();
    //void on_payment_cancel_Button_clicked();
    // For Debugging; will be removed.

    // **** Payment ****
    void displayPaymentPending(bool isVisible);

    // void readTimer_loop();
    void tapPaymentHandler();
    void declineTimer_start();
    void check_packet_available();
    void check_card_tapped();
    void startPaymentProcess();
    void restartTapPayment();

    void idlePaymentTimeout();



protected:

//    void paintEvent(QPaintEvent *p);

private:
    // **** GUI ****
    Ui::page_tap_payment *ui;
    pageProduct* p_pageProduct;
    page_dispenser* p_page_dispense;
    page_idle* p_page_idle;
    page_help* p_page_help;
    page_error_wifi *p_page_wifi_error;


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
    QTimer *checkPacketReceivedTimer;
    QTimer *checkCardTappedTimer;

    QTimer *idlePaymentTimer;
    QTimer *inFlightTimer;

    // Payment Communication
    // Moneris Packet communication reference
    // mCommunication com;
    // packetFromECR sendPacket;
    // packetFromUX410 readPacket;
    // transactionPacket paymentPacket;
    // transactionInfo paymentPktInfo;

    // Payment Package Control
    bool purchaseEnable;
    int monerisConfig = 0;
    bool timerEnabled;
    std::vector<uint8_t> pktToSend;
    std::vector<uint8_t> pktResponded;
    std::string productSelectedPrice;
    
    bool tap_init();
    void cancelPayment();
    bool getResponse(){return response;}

    
    // **** Drink Order Reference ****
    // DrinkOrder paymentDrinkOrder;

    // Placeholder
    bool surveyBool;

    QString _paymentTimeLabel;
    int _pageTimeoutCounterSecondsLeft;
    QTimer* paymentEndTimer;

    int _qrProcessedPeriodicalCheckSec;
    QTimer* qrPeriodicalCheckTimer;

    QResizeEvent *pageProductResize;
    QShowEvent *dispenseEvent;

    bool response;
    bool tap_payment;
    int lastTransactionId;
    void testQRgen();

    void printQr(const QrCode &qr);
    std::string toSvgString(const QrCode &qr, int border);
    void paintQR(QPainter &painter, const QSize sz, const QString &data, QColor fg);
    void resetPaymentPage();
    QString getPaymentMethod();



    // QString order_id;
    QString orderId="";

    CURL *curl;
    CURLcode res;
    QByteArray curl_param_array;
   
    CURLcode res1;
    CURL *curl1;
    QByteArray curl_order_parameters;

    //size_t WriteCallback(char* contents, size_t size, size_t nmemb, void *userp);
    std::string readBuffer;


    int tmpCounter ;

    void isQrProcessedCheckOnline();
    void setupQrOrder();
    bool createOrderIdAndSendToBackend();
    
};

#endif // page_tap_payment_H
