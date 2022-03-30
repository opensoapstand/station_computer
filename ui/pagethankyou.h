//***************************************
//
// pagethankyou.h
// GUI class to show user dispense has been
// completed and route back to page_idle
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef PAGETHANKYOU_H
#define PAGETHANKYOU_H

#include "df_util.h"
#include "page_idle.h"
#include "page_dispenser.h"
#include <curl/curl.h>
#include <ctime>

class page_dispenser;
class page_idle;
class pagePayment;

namespace Ui {
class pagethankyou;
}

class pagethankyou : public QWidget
{
    Q_OBJECT

public:
    explicit pagethankyou(QWidget *parent = nullptr);
    void setPage(page_dispenser* page_dispenser, page_idle* pageIdle, pagePayment* pagePayment);
    ~pagethankyou();

    void controllerFinishedTransaction();
    void  exitPage();

    bool is_controller_finished;
    bool is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER;
    


private slots:
    void on_mainPage_Button_clicked();
    void onThankyouTimeoutTick();
    // void onRinseTimerTick();

private:
    void showEvent(QShowEvent *event);

    Ui::pagethankyou *ui;
    page_dispenser* p_page_dispense;
    page_idle* idlePage;
    pagePayment* paymentPage;

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
    void transactionToFile(char *curl_params);

    
    bool exitIsForceable; // avoid being stuck if internet fails.


};

#endif // PAGETHANKYOU_H
