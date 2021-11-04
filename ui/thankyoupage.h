//***************************************
//
// thankyoupage.h
// GUI class to show user dispense has been
// completed and route back to idle
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef THANKYOUPAGE_H
#define THANKYOUPAGE_H

#include "df_util.h"
#include <curl/curl.h>
#include <ctime>

class dispensePage;
class idle;
class payPage;

namespace Ui {
class thankYouPage;
}

class thankYouPage : public QWidget
{
    Q_OBJECT

public:
    explicit thankYouPage(QWidget *parent = nullptr);
    void setPage(dispensePage* pageDispense, idle* pageIdle, payPage* pagePayment);
    ~thankYouPage();

private slots:
    void on_mainPage_Button_clicked();
    void onThankyouTimeoutTick();
    void onRinseTimerTick();

private:
    void showEvent(QShowEvent *event);

    Ui::thankYouPage *ui;
    dispensePage* dispensingPage;
    idle* idlePage;
    payPage* paymentPage;

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

#endif // THANKYOUPAGE_H
