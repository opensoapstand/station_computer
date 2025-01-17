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
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef PAGE_DISPENSER_H
#define PAGE_DISPENSER_H

#include "df_util.h"
#include "includefiles.h"
#include "page_idle.h"
#include "page_payment_tap_serial.h"
#include "dispenser_slot.h"
#include "page_sendFeedback.h"
#include "page_end.h"
#include "payment/commands.h"
#include "payment/setup_Tap.h"
#include "posm/mcommunication.h"
#include "posm/packetfromecr.h"
#include "posm/packetfromux410.h"
#include "posm/transactionPackets.h"


class page_qr_payment;
class page_offline_payment;
class page_payment_tap_tcp;
class page_payment_tap_serial;
class page_end;
class page_idle;
class statusbar;

namespace Ui {
class page_dispenser;
}

class page_dispenser : public QWidget
{
    Q_OBJECT

public:
    QMessageBox* msgBox_problems;
    QMessageBox* msgBox_abort;
    // QMessageBox* msgBox;

    // **** GUI ****
    explicit page_dispenser(QWidget *parent = nullptr);
    void setPage(page_qr_payment* page_qr_payment,page_offline_payment* page_offline_payment, page_payment_tap_serial* page_payment_tap_serial, page_payment_tap_tcp* page_payment_tap_tcp,  page_end* page_end, page_idle* pageIdle, page_sendFeedback* pageFeedback, statusbar *p_statusbar);
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
    // QString getCouponCodeUsed();
    // void startDispensing();
    void force_finish_dispensing();
    void processDispenserUpdate();





public slots:


private slots:
    // **** Navigation ****
    void fsmSendStartDispensing();
    // void fsmSendPrice();
    // void fsmSendPromo();
    void fsmSendStopDispensing();
    void onArrowAnimationStepTimerTick();

    void onDispenseIdleTick();

    void dispensing_end_admin();

    void on_pushButton_abort_clicked();
    void on_cancelButton_clicked();
    void on_pushButton_debug_Button_clicked();
    void on_pushButton_problems_clicked();

private:
    bool isDispensing = false;
    bool cancelPayment = false;
    ActivePaymentMethod paymentMethod;
    bool askForFeedbackAtEnd;
    // **** GUI *****
    Ui::page_dispenser *ui;
    page_qr_payment* paymentPage;
    page_offline_payment* paymentOfflinePage;
    page_payment_tap_tcp* p_page_payment_tap_tcp;
    page_payment_tap_serial* p_page_payment_tap_serial;
    page_end* thanksPage;
    page_sendFeedback* feedbackPage;
    page_idle* p_page_idle;
    statusbar *p_statusbar;

    uint16_t arrow_animation_step_counter;
    bool animationStepForwardElseBackward;

    dispenser_slot* selectedSlot;
    
    DispenseBehaviour m_previousRecentDispenseStatus;

    // XXX: Remove when interrupts and flowsensors work.

    QString _dispenseTimeLabel;
    //QTimer *dispenseEndTimer;
    int _dispenseIdleTimeoutSec;
    QTimer *dispenseIdleTimer;
    QTimer *arrowAnimationStepTimer;
    QVBoxLayout *statusbarLayout;

    // double volumeDispensed;
    // double targetVolume;

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
