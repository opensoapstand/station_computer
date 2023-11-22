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
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef wifiErrorPage_H
#define wifiErrorPage_H

#include "df_util.h"
#include "includefiles.h"
#include "page_idle.h"
#include "page_end.h"


class page_qr_payment;
class page_end;
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
    void setPage(page_qr_payment* page_qr_payment, page_end* page_end, page_idle* pageIdle);
    ~page_error_wifi();
    void showEvent(QShowEvent *event);
    void exit_page();
    // void fsmReceiveTargetVolumeReached();

public slots:


private slots:
    // **** Navigation ****
    void onTimeOutTick();
    void on_pushButton_mainPage_clicked();

private:
    // **** GUI *****
    Ui::page_error_wifi *ui;
    page_qr_payment* paymentPage;
    page_end* thanksPage;
    page_idle* p_page_idle;

    // XXX: Remove when interrupts and flowsensors work.

    // QString _dispenseTimeLabel;
    //int _dispenseTimeoutSec;
    // QTimer *dispenseEndTimer;

    int _goTop_page_idleTimeoutSec;
    QTimer *timeoutTimer;

    void stopDispenseTimer();


protected:

};

#endif // wifiErrorPage_H
