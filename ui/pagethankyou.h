//***************************************
//
// pagethankyou.h
// GUI class to show user dispense has been
// completed and route back to idle
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
#include "idle.h"
#include "page_dispenser.h"
#include <curl/curl.h>
#include <ctime>

class page_dispenser;
class idle;
class pagePayment;

namespace Ui {
class pagethankyou;
}

class pagethankyou : public QWidget
{
    Q_OBJECT

public:
    explicit pagethankyou(QWidget *parent = nullptr);
    void setPage(page_dispenser* page_dispenser, idle* pageIdle, pagePayment* pagePayment);
    ~pagethankyou();

private slots:
    void on_mainPage_Button_clicked();
    void onThankyouTimeoutTick();
    void onRinseTimerTick();

private:
    void showEvent(QShowEvent *event);

    Ui::pagethankyou *ui;
    page_dispenser* dispensingPage;
    idle* idlePage;
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

    void curler();
    void bufferCURL(char *curl_params);

};

#endif // PAGETHANKYOU_H
