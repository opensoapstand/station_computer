//***************************************
//
// wifiErrorPage.h
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

#ifndef wifiErrorPage_H
#define wifiErrorPage_H

#include "df_util.h"
#include "includefiles.h"
#include "idle.h"
#include "thankyoupage.h"


class payPage;
class thankYouPage;
class idle;

namespace Ui {
class wifiErrorPage;
}

class wifiErrorPage : public QWidget
{
    Q_OBJECT

public:
    // **** GUI ****
    explicit wifiErrorPage(QWidget *parent = nullptr);
    void setPage(payPage* pagePayment, thankYouPage* pageThankYou, idle* pageIdle);
    ~wifiErrorPage();
    void showEvent(QShowEvent *event);
    void targetHitDisplay();

public slots:


private slots:
    // **** Navigation ****
    void on_finish_Button_clicked();
    void onDispenseIdleTick();



private:
    // **** GUI *****
    Ui::wifiErrorPage *ui;
    payPage* paymentPage;
    thankYouPage* thanksPage;
    idle* idlePage;

    // XXX: Remove when interrupts and flowsensors work.

    QString _dispenseTimeLabel;
    //int _dispenseTimeoutSec;
    QTimer *dispenseEndTimer;

    int _dispenseIdleTimeoutSec;
    QTimer *dispenseIdleTimer;

    QTimer* dispenseNextPageTimer;



    void stopDispenseTimer();


protected:

};

#endif // wifiErrorPage_H
