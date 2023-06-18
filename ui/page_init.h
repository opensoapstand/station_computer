//***************************************
//
// page_init.h
// GUI class while machine is initializing.
//
// Display Fullscreen DF branded Wallpaper
// Listen for User interaction to load
// Idle Page
//
// created: 16-06-2023
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef INIT_H
#define INIT_H

#include "df_util.h"
#include "page_idle.h"
#include "page_tap_payment.h"
#include "page_tap_payment_serial.h"
#include "dfuicommthread.h"
#include "dbmanager.h"

// #define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"
// #define DB_PATH_CLICKS "/release/db/sqlite/clicks.db"

class page_idle;

namespace Ui {
class page_init;
}

class page_init : public QWidget
{
    Q_OBJECT

public:
    explicit page_init(QWidget *parent = nullptr);
    void setPage(page_idle *pageIdle);
    void showEvent(QShowEvent *event);
    ~page_init();
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    void initReadySlot();

    df_util* dfUtility;

    DfUiCommThread* dfComm;

private slots:
    void onInitTimeoutTick();
    void onRebootTimeoutTick();

private:
    Ui::page_init *ui;
    page_idle* p_page_idle;
    QTimer* initIdleTimer;
    int _initIdleTimeoutSec;
    QTimer* rebootTimer;
    int _rebootTimeoutSec;
    bool start_controller;

};

#endif // INIT_H
