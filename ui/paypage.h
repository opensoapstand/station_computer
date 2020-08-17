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

#include "../library/posm/mcommunication.h"
#include "../library/posm/packetfromecr.h"
#include "../library/posm/packetfromux410.h"
#include "../library/posm/transactionPackets.h"
#include "../library/posm/transactioninfo.h"

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
    explicit payPage(QWidget *parent = nullptr);
    void setPage(paySelect* pageSizeSelect, dispensePage* pageDispense, idle* pageIdle);
    ~payPage();

    bool setpaymentProcess(bool status);
    void setProgressLabel(QLabel* label, int dot);
    void setProductPrice(QString price);

private slots:
    void updateTotals(string drinkDescription, string drinkAmount, string orderTotal);

    void on_previousPage_Button_clicked();
    void on_passPayment_Button_clicked();

    void declineTimer_start();
    void on_goBackButton_clicked();
    void progressStatusLabel();
    void updatePageNumber();
    void on_payButton_clicked();
    void readTimer_loop();
    void goBack();
    void on_priceVolume1Button_clicked();
    void on_priceVolume2Button_clicked();

    /*
     * Successful payment -> on_passPayment_Button_clicked()
     * Failed payment -> on_previous_Button_clicked()
     */
    void on_mainPage_Button_clicked();

protected:
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *p);

private:
    Ui::payPage *ui;
    paySelect* paySelectPage;
    dispensePage* dispensingPage;
    idle* idlePage;

    bool paymentProcessing;

    /*payment send & responding packet for moneris device*/
    mCommunication com;
    packetFromECR sendPacket;
    packetFromUX410 readPacket;
    transactionPacket paymentPacket;
    transactionInfo paymentPktInfo;

    /*timer*/
    QTimer *readTimer;
    QTimer *declineTimer;
    QTimer* timer;
    QTimer* pageUpdateTimer;
    QTimer* goBackTimer;

    /*prgress timer*/
    int progressDots = 1;
    int counter = 0;
    int declineCounter;

    std::vector<uint8_t> pktToSend;
    std::vector<uint8_t> pktResponded;

    bool purchaseEnable;
    int monerisConfig = 0;
    bool timerEnabled;
    std::string productSelectedPrice;
    bool sendToUX410();
    bool paymentInit();
    bool waitForUX410();

    bool surveyBool;
    int priceSelect;

    DrinkOrder paymentDrinkOrder;

    string drinkAmount;
    string drinkDescription;
    string orderTotal;
};

#endif // PAYPAGE_H
