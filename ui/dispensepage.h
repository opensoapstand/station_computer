//***************************************
//
// dispensepage.h
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
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef DISPENSEPAGE_H
#define DISPENSEPAGE_H

#include "df_util.h"
#include "dfuiserver.h"

class payPage;
class thankYouPage;
class idle;
class DfUiServer;

namespace Ui {
class dispensePage;
}

class dispensePage : public QWidget
{
    Q_OBJECT

public:
    // **** GUI ****
    explicit dispensePage(QWidget *parent = nullptr);
    void setPage(payPage* pagePayment, thankYouPage* pageThankYou, idle* pageIdle);
    ~dispensePage();
    void showEvent(QShowEvent *event);
    //void resetTimerSlot();
    bool isDispense = false;

private slots:
    // **** Navigation ****
    void on_finish_Button_clicked();
    void onDispenseTick();
    void onDispenseIdleTick();

private:
    // **** GUI *****
    Ui::dispensePage *ui;
    payPage* paymentPage;
    thankYouPage* thanksPage;
    idle* idlePage;
    DfUiServer* dfuiserver;

    // XXX: Remove when interrupts and flowsensors work.

    QString _dispenseTimeLabel;
    int _dispenseTimeoutSec;
    QTimer *dispenseEndTimer;

    int _dispenseIdleTimeoutSec;
    QTimer *dispenseIdleTimer;

    QTimer* dispenseNextPageTimer;

    void stopDispenseTimer();
    void resetTimerSlot();

protected:

};

#endif // DISPENSEPAGE_H
