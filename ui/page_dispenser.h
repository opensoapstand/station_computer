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
// created: 05-04-2022
// by: Lode Ameije & Ash Singla
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
#include "payment/commands.h"
#include "payment/setup_Tap.h"
#include "posm/mcommunication.h"
#include "posm/packetfromecr.h"
#include "posm/packetfromux410.h"
#include "posm/transactionPackets.h"
#include "posm/transactioninfo.h"

class page_payment;
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
    void setPage(page_payment* page_payment, pagethankyou* pageThankYou, page_idle* pageIdle);
    ~page_dispenser();
    void showEvent(QShowEvent *event);
    void resetDispenseTimeout(void);
    void updateVolumeDisplayed(double dispensed, bool isFull );
    void volumeDispensedLabel(QLabel* label);
    void fsmReceiveTargetVolumeReached();
    void updateVolumeDispensedLabel(double dispensed);
    void fsmReceiveNoFlowAbort();
    QString getMostRecentDispensed();
    QString getPromoCodeUsed();

    void force_finish_dispensing();
    void startDispensing();

public slots:


private slots:
    // **** Navigation ****
    void fsmSendStartDispensing();
    void fsmSendPrice();
    void fsmSendPromo();

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
    page_payment* paymentPage;
    pagethankyou* thanksPage;
    page_idle* p_page_idle;

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
