//***************************************
//
// page_end.h
// GUI class to show user dispense has been
// completed and route back to page_idle
//
// created: 05-04-2022
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef PAGE_END_H
#define PAGE_END_H

#include "df_util.h"
#include "page_idle.h"
#include "page_dispenser.h"
#include "page_sendFeedback.h"
#include <curl/curl.h>
#include <ctime>

class page_dispenser;
class page_idle;
class page_qr_payment;
class page_sendFeedback;

namespace Ui
{
    class page_end;
}

class page_end : public QWidget
{
    Q_OBJECT

public:
    explicit page_end(QWidget *parent = nullptr);
    void setPage(page_dispenser* page_dispenser, page_idle* pageIdle, page_qr_payment* page_qr_payment, page_sendFeedback *page_sendFeedback);
    ~page_end();

    void controllerFinishedTransaction();
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    void finishHandler();

    bool is_controller_finished;
    bool is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER;

public slots:
private slots:
    void on_pushButton_to_idle_clicked();
    void onThankyouTimeoutTick();
    void on_pushButton_contact_clicked();
private:
    void showEvent(QShowEvent *event);

    Ui::page_end *ui;
    page_dispenser* p_page_dispense;
    page_idle* p_page_idle;
    page_qr_payment* paymentPage;
    page_sendFeedback* p_page_sendFeedback;

    QDialog *popup;

    int _thankYouTimeoutSec;
    QTimer *thankYouEndTimer;

    // QTimer *rinseTimer;
    // int _rinseTimerTimeoutSec;
    // bool rinse;

    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    QByteArray curl_param_array;
    char *curl_data;

    time_t rawtime;
    struct tm *timeinfo;

    void sendDispenseEndToCloud();
    void sendTapOrderToCloud();
    void transactionToFile(char *curl_params);


    bool exitIsForceable; // avoid being stuck if internet fails.

    bool is_in_state_thank_you;
};

#endif // PAGE_END_H
