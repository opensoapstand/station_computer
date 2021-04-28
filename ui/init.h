//***************************************
//
// init.h
// GUI class while machine is initializing.
//
// Display Fullscreen DF branded Wallpaper
// Listen for User interaction to load
// Idle Page
//
// created: 16-04-2021
// by: Paddy Riley
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef INIT_H
#define INIT_H

#include "df_util.h"
#include "idle.h"
#include "dfuicommthread.h"
#include "dbmanager.h"

#define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"
#define DB_PATH_CLICKS "/release/db/sqlite/clicks.db"

class idle;

namespace Ui {
class init;
}

class init : public QWidget
{
    Q_OBJECT

public:
    explicit init(QWidget *parent = nullptr);
    void setPage(idle *pageIdle);
    void showEvent(QShowEvent *event);
    ~init();
    void initReadySlot(void);

    DrinkOrder* userDrinkOrder;
    df_util* dfUtility;

    DfUiCommThread* dfComm;

private slots:
    void onInitTimeoutTick();
    void onRebootTimeoutTick();

private:
    Ui::init *ui;
    idle* idlePage;
    QTimer* initIdleTimer;
    int _initIdleTimeoutSec;
    QTimer* rebootTimer;
    int _rebootTimeoutSec;

};

#endif // INIT_H
