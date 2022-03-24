//***************************************
//
// page_dispenser.h
// GUI class to dispense drink.
// Coordinates User input to payment class
// then communcates results to FSM IP_thread
//
// Allows navigation to paymentpage and
// thankyou page
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef PAGE_DISPENSER_H
#define PAGE_DISPENSER_H

#include "df_util.h"
#include "includefiles.h"
#include "page_idle.h"
#include "pagethankyou.h"

#include "posm/mcommunication.h"
#include "posm/packetfromecr.h"
#include "posm/packetfromux410.h"
#include "posm/transactionPackets.h"
#include "posm/transactioninfo.h"

class pagePayment;
class pagethankyou;
class page_idle;

namespace Ui {
class page_dispenser;
}

class page_dispenser : public QWidget
{
    Q_OBJECT

public:
    // **** GUI ****
    explicit page_dispenser(QWidget *parent = nullptr);
    void setPage(pagePayment* pagePayment, pagethankyou* pageThankYou, page_idle* pageIdle);
    ~page_dispenser();
    void showEvent(QShowEvent *event);
    void resetDispenseTimeout(void);
    void updateVolumeDisplayed(double dispensed, bool isFull );
    void volumeDispensedLabel(QLabel* label);
    void fsmReceiveTargetVolumeReached();
    QString getMostRecentDispensed();
    void force_finish_dispensing();
    void startDispensing();

public slots:


private slots:
    // **** Navigation ****
    void fsmSendStartDispensing();
    void fsmSendStopDispensing();
   // void onDispenseTick();
    void onDispenseIdleTick();
    // void onRinseTimerTick();
    void dispensing_end_admin();
    void on_abortButton_clicked();


    void on_cancelButton_clicked();

private:
    bool isDispensing;
    // **** GUI *****
    Ui::page_dispenser *ui;
    pagePayment* paymentPage;
    pagethankyou* thanksPage;
    page_idle* idlePage;

    DrinkOrder* selectedProductOrder;

    // XXX: Remove when interrupts and flowsensors work.

    QString _dispenseTimeLabel;
    //int _dispenseTimeoutSec;
    QTimer *dispenseEndTimer;

    int _dispenseIdleTimeoutSec;
    QTimer *dispenseIdleTimer;

    QTimer* dispenseNextPageTimer;

    QTimer* rinseTimer;
    int _rinseTimerTimeoutSec;
    bool rinse;

    double volumeDispensed;
    double targetVolume;

    void stopDispenseTimer();
    bool sendToUX410();
    bool waitForUX410();

    mCommunication com;
    packetFromECR sendPacket;
    packetFromUX410 readPacket;
    transactionPacket paymentPacket;
    std::vector<uint8_t> pktToSend;
    std::vector<uint8_t> pktResponded;

protected:

};

#endif // PAGE_DISPENSER_H
