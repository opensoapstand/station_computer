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
#include "product.h"
#include "page_sendFeedback.h"
#include "page_end.h"
#include "payment/commands.h"
#include "payment/setup_Tap.h"
#include "posm/mcommunication.h"
#include "posm/packetfromecr.h"
#include "posm/packetfromux410.h"
#include "posm/transactionPackets.h"

class page_qr_payment;
class page_tap_payment;
class page_end;
class page_idle;

namespace Ui {
class page_dispenser;
}

class page_dispenser : public QWidget
{
    Q_OBJECT

public:

    QMessageBox* msgBox_problems;
    QMessageBox* msgBox;

    // **** GUI ****
    explicit page_dispenser(QWidget *parent = nullptr);
    void setPage(page_qr_payment* page_qr_payment, page_tap_payment* page_tap_payment, page_end* page_end, page_idle* pageIdle, page_sendFeedback* pageFeedback);
    ~page_dispenser();
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    void showEvent(QShowEvent *event);

    void resetDispenseTimeout(void);

    void updateVolumeDisplayed(double dispensed, bool isFull );
    void label_volume_dispensed_ml(QLabel* label);
    void updatelabel_volume_dispensed_ml(double dispensed);

    void fsmReceivedVolumeDispensed(double dispensed, bool isFull );
    void fsmReceiveDispenseRate(double flowrate);
    void fsmReceiveDispenserStatus(QString status);
    void fsmReceiveTargetVolumeReached();
    void fsmReceiveNoFlowAbort();
    
    // QString getMostRecentDispensed();
    QString getPromoCodeUsed();
    // void startDispensing();
    void force_finish_dispensing();
    QString getStartDispensingCommand();


public slots:


private slots:
    // **** Navigation ****
    void fsmSendStartDispensing();
    // void fsmSendPrice();
    // void fsmSendPromo();
    void fsmSendStopDispensing();

    void onDispenseIdleTick();

    void dispensing_end_admin();

    void on_pushButton_abort_clicked();
    void on_cancelButton_clicked();
    void on_pushButton_debug_Button_clicked();
    void on_pushButton_problems_clicked();

    void on_pushButton_report_clicked();

private:
    bool isDispensing;
    bool askForFeedbackAtEnd;
    // **** GUI *****
    Ui::page_dispenser *ui;
    page_qr_payment* paymentPage;
    page_tap_payment* paymentTapPage;
    page_end* thanksPage;
    page_sendFeedback* feedbackPage;
    page_idle* p_page_idle;

    product* selectedProduct;

    // XXX: Remove when interrupts and flowsensors work.

    QString _dispenseTimeLabel;
    QTimer *dispenseEndTimer;
    int _dispenseIdleTimeoutSec;
    QTimer *dispenseIdleTimer;

    // double volumeDispensed;
    // double targetVolume;


    mCommunication com;
    packetFromECR sendPacket;
    transactionPacket paymentPacket;
    std::vector<uint8_t> pktToSend;
    std::vector<uint8_t> pktResponded;

protected:

};

#endif // PAGE_DISPENSER_H
