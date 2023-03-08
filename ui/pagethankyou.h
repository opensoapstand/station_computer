//***************************************
//
// pagethankyou.h
// GUI class to show user dispense has been
// completed and route back to page_idle
//
// created: 05-04-2022
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef PAGETHANKYOU_H
#define PAGETHANKYOU_H

#include "df_util.h"
#include "page_idle.h"
#include "page_dispenser.h"
#include "page_sendFeedback.h"
#include <curl/curl.h>
#include <ctime>

class page_dispenser;
class page_idle;
class page_payment;
class page_sendFeedback;

namespace Ui {
class pagethankyou;
}

class pagethankyou : public QWidget
{
    Q_OBJECT

public:
    explicit pagethankyou(QWidget *parent = nullptr);
    void setPage(page_dispenser* page_dispenser, page_idle* pageIdle, page_payment* page_payment, page_sendFeedback *page_sendFeedback);
    ~pagethankyou();

    void controllerFinishedTransaction();
    void  exitPage();

    bool is_controller_finished;
    bool is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER;

public slots:
    // void showPopup();
    // void handleFirstOptionClick();
    // void handleSecondOptionClick();
    // void handleThirdOptionClick();
    // void handleFourthOptionClick();
    // void handleFifthOptionClick();
    
    // void handleFirstOptionRelease();
    // void handleSecondOptionRelease();
    // void handleThirdOptionRelease();
    // void handleFourthOptionRelease();
    // void handleFifthOptionRelease();
private slots:
    void on_mainPage_Button_clicked();
    void onThankyouTimeoutTick();
    void on_notifyUs_Button_clicked();
    // void onRinseTimerTick();

    // void on_option1_Button_clicked(bool checked);

    // void on_option1_Button_clicked();

private:
    void showEvent(QShowEvent *event);
   


    Ui::pagethankyou *ui;
    page_dispenser* p_page_dispense;
    page_idle* p_page_idle;
    page_payment* paymentPage;
    page_sendFeedback* p_page_sendFeedback;

    QDialog* popup;
    
    int _thankYouTimeoutSec;
    QTimer* thankYouEndTimer;


    QTimer* rinseTimer;
    int _rinseTimerTimeoutSec;
    bool rinse;

    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    QByteArray curl_param_array;
    char * curl_data;

    time_t rawtime;
    struct tm * timeinfo;

    void sendDispenseEndToCloud();
    void sendTapOrderToCloud();
    void transactionToFile(char *curl_params);

    
    bool exitIsForceable; // avoid being stuck if internet fails.

    bool is_in_state_thank_you;


};

#endif // PAGETHANKYOU_H
