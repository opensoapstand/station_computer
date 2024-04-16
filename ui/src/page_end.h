//***************************************
//
// page_end.h
// GUI class to show user dispense has been
// completed and route back to page_idle
//
// created: 05-04-2022
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef PAGE_END_H
#define PAGE_END_H

#include "df_util.h"
#include "page_idle.h"
#include "page_dispenser.h"
#include "page_sendFeedback.h"
#include "dbmanager.h"
#include <curl/curl.h>
#include <ctime>

class statusbar;
class page_dispenser;
class page_idle;
class page_qr_payment;
class page_offline_payment;
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
    void setPage(page_dispenser* page_dispenser, page_idle* pageIdle, page_qr_payment* page_qr_payment,page_offline_payment* page_offline_payment, page_sendFeedback *page_sendFeedback, statusbar *p_statusbar);
    ~page_end();

    void controllerReceivedDispenseAftermath(QString status, QString start_time, QString end_time, double button_press_duration, double button_press_count, double volume_dispensed,QString volumeDispensedMixProduct);
    void controllerReceivedFinishedTransaction();
    
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    void updateDispensedVolumeLabel();
    void updateTransactionInDb(bool processed_by_backend,QString volume_dispensed_mix_product);

    bool is_controller_finished;
    bool is_dispense_aftermath_complete;

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
    page_offline_payment *paymentOfflinePage;
    page_sendFeedback* p_page_sendFeedback;
    statusbar *p_statusbar;
    DbManager* db;

    QDialog *popup;
    

    int _thankYouTimeoutSec;
    QTimer *thankYouEndTimer;
    QVBoxLayout *statusbarLayout;

    // QTimer *rinseTimer;
    // int _rinseTimerTimeoutSec;
    // bool rinse;

    CURLcode res;
    std::string readBuffer;
    long http_code;
    
    time_t rawtime;
    struct tm *timeinfo;

    void sendDispenseEndToCloud();
    void sendCompleteOrderToCloud(QString paymentMethod);
    void transactionToFile(QString curl_params);


    bool exitIsForceable; // avoid being stuck if internet fails.
    bool is_in_page_end;


};

#endif // PAGE_END_H
