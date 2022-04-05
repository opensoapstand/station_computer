//***************************************
//
// page_error_wifi.h
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

#ifndef wifiErrorPage_H
#define wifiErrorPage_H

#include "df_util.h"
#include "includefiles.h"
#include "page_idle.h"
#include "pagethankyou.h"


class pagePayment;
class pagethankyou;
class page_idle;

namespace Ui {
class page_error_wifi;
}

class page_error_wifi : public QWidget
{
    Q_OBJECT

public:
    // **** GUI ****
    explicit page_error_wifi(QWidget *parent = nullptr);
    void setPage(pagePayment* pagePayment, pagethankyou* pageThankYou, page_idle* pageIdle);
    ~page_error_wifi();
    void showEvent(QShowEvent *event);
    // void fsmReceiveTargetVolumeReached();

public slots:


private slots:
    // **** Navigation ****
    void on_wifi_ack_Button_clicked();
    void onTimeOutTick();

private:
    // **** GUI *****
    Ui::page_error_wifi *ui;
    pagePayment* paymentPage;
    pagethankyou* thanksPage;
    page_idle* idlePage;

    // XXX: Remove when interrupts and flowsensors work.

    // QString _dispenseTimeLabel;
    //int _dispenseTimeoutSec;
    // QTimer *dispenseEndTimer;

    int _goToIdlePageTimeoutSec;
    QTimer *timeoutTimer;

    void stopDispenseTimer();


protected:

};

#endif // wifiErrorPage_H
