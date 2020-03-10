//***************************************
//
// payment diplay window
// update scheme for to work with payment device
//
// created: 03-SEP-2019
// by: Jason Wang
//
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#ifndef PAYMENT_H
#define PAYMENT_H

#include "includefiles.h"

#include "posm/mcommunication.h"
#include "posm/packetfromecr.h"
#include "posm/packetfromux410.h"
#include "posm/transactionPackets.h"
#include "posm/transactioninfo.h"
#include "database_logger.h"
#include "beveragedata.h"

class MainWindow;
class DispenseWindow;
class countdownWindow;

namespace Ui {
class payment;
}

class payment : public QWidget
{
    Q_OBJECT

public:
    explicit payment(QWidget *parent = nullptr);
    ~payment();

    /* Setup */
    void pageSetup(QString optionTitle, QByteArray imgData, bool survey);
    void labelSetup(QLabel* label, int fontSize);
    void setPages(MainWindow* main, DispenseWindow* dispense);
    bool setpaymentProcess(bool status);
    void setProgressLabel(QLabel* label, int dot);
    void setProductPrice(QString price);

    /* database */
    void storeEvent(database_logger db, QString event);
    
    /* mpos */
    void stayAliveLogon();
    void batchClose();

    int getPriceSelect();

    void  sendCommand();

private:
    Ui::payment *ui;
    DispenseWindow* dispensePage;
    MainWindow * mainPage;

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

protected:
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *p);

private slots:
    void declineTimer_start();
    void on_goBackButton_clicked();
    void progressStatusLabel();
    void updatePageNumber();
    void on_payButton_clicked();
    void readTimer_loop();
    void goBack();
    void on_priceVolume1Button_clicked();
    void on_priceVolume2Button_clicked();
};

#endif // PAYMENT_H
