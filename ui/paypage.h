//***************************************
//
// paypage.h
// GUI class while machine is processing
// payment.
//
// Coordinates User input from payment select
// class then communcates results to dispensepage.
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef PAYPAGE_H
#define PAYPAGE_H

#include "df_util.h"
#include "drinkorder.h"

#include <QTimer>

#include "posm/mcommunication.h"
#include "posm/packetfromecr.h"
#include "posm/packetfromux410.h"
#include "posm/transactionPackets.h"
#include "posm/transactioninfo.h"

class paySelect;
class dispensePage;
class idle;

namespace Ui {
class payPage;
}

class payPage : public QWidget
{
    Q_OBJECT

public:
    // **** GUI Setup ****
    explicit payPage(QWidget *parent = nullptr);
    void setPage(paySelect* pageSizeSelect, dispensePage* pageDispense, idle* pageIdle);
    ~payPage();
    void setProgressLabel(QLabel* label, int dot);
    // TODO: Figure out better Style Setup.
    void labelSetup(QLabel *label, int fontSize);

    // **** Control Functions ****
    bool setpaymentProcess(bool status);

    // Database
    void storePaymentEvent(QSqlDatabase db, QString event);

    /* mpos */
    void stayAliveLogon();
    void batchClose();
    int getPriceSelect();
    void sendCommand();

private slots:

    // Update Drink order totals section
    void updateTotals(string drinkDescription, string drinkAmount, string orderTotal);

    // Navigation
    void on_previousPage_Button_clicked();
    void on_mainPage_Button_clicked();
    // For Debugging; will be removed.
    void on_passPayment_Button_clicked();

    // **** Payment ****
    void displayPaymentPending(bool isVisible);

    void readTimer_loop();
    void progressStatusLabel();

    void cancelPayment();
    void declineTimer_start();

protected:
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *p);

private:
    // **** GUI ****
    Ui::payPage *ui;
    paySelect* paySelectPage;
    dispensePage* dispensingPage;
    idle* idlePage;

    const QString TAP_DECLINED= "Card Declined";
    const QString TAP_AGAIN = "Try Again";

    // Label Reference for order Details
    string drinkAmount;
    string drinkDescription;
    string orderTotal;

    // TODO: Put payment information into XML
    // Payment progress timer    
    bool approved = false;
    bool paymentConnected = false;

    int progressDots = 1;
    int counter = 0;
    int declineCounter;

    // **** Payment ****
    // Payment Information
//    std::string merchantName;
//    std::string merchantAddress;
//    std::string terminalID;

    // Payment Control
    bool paymentProcessing;
    QSqlDatabase db;

    QTimer *readTimer;
    QTimer *declineTimer;
    QTimer* timer;
    QTimer* pageUpdateTimer;
    QTimer* goBackTimer;

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
    bool paymentInit();
    bool waitForUX410();

    // **** Drink Order Reference ****
    DrinkOrder paymentDrinkOrder;

    // Placeholder
    bool surveyBool;
};

#endif // PAYPAGE_H
