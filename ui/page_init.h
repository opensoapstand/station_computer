//***************************************
//
// page_init.h
// GUI class while machine is initializing.
//
// Display Fullscreen DF branded Wallpaper
// Listen for User interaction to load
// Idle Page
//
// created: 16-06-2023
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef INIT_H
#define INIT_H

#include "df_util.h"
#include "page_idle.h"
#include "dfuicommthread.h"
#include "dbmanager.h"
#include "posm/mcommunication.h"
#include "posm/packetfromecr.h"
#include "posm/packetfromux410.h"
#include "posm/transactionPackets.h"
#include "posm/transactioninfo.h"
// #define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"
// #define DB_PATH_CLICKS "/release/db/sqlite/clicks.db"

class page_idle;

namespace Ui {
class page_init;
}

class page_init : public QWidget
{
    Q_OBJECT

public:
    explicit page_init(QWidget *parent = nullptr);
    void setPage(page_idle *pageIdle);
    void showEvent(QShowEvent *event);
    ~page_init();
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    void initReadySlot();


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
    bool sendToUX410PageInit();
    bool tap_serial_initiate();
    bool waitForUX410PageInit();
    void cancelPayment();
    bool getResponse(){return response;}
    bool tapSetupStarted =false;


    df_util* dfUtility;

    DfUiCommThread* dfComm;

private slots:
    void onInitTimeoutTick();
    void onRebootTimeoutTick();

private:
    Ui::page_init *ui;
    page_idle* p_page_idle;
    QTimer* initIdleTimer;
    int _initIdleTimeoutSec;
    QTimer* rebootTimer;
    int _rebootTimeoutSec;
    bool start_controller;

};

#endif // INIT_H
