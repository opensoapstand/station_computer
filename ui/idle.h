//***************************************
//
// idle.h
// GUI class while machine is idle.
//
// Display Fullscreen DF branded Wallpaper
// Listen for User interaction to load
// Product Page1
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef IDLE_H
#define IDLE_H

#include "df_util.h"
#include "productpage_1.h"
#include "dfuicommthread.h"
#include "dbmanager.h"
#include "maintenancePage.h"

#define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"

class maintenancePage;

namespace Ui {
class idle;
}

class idle : public QWidget
{
    Q_OBJECT

public:
    explicit idle(QWidget *parent = nullptr);
    void setPage(productPage_1 *pageProduct, maintenancePage *pageMaintenance);
    ~idle();

    DrinkOrder* userDrinkOrder;
    df_util* dfUtility;

    DfUiCommThread* dfComm;

    bool isEnough(int p);

private slots:
    void on_nextPageButton_clicked();
    void on_maintenanceModeButton_clicked();

private:
    Ui::idle *ui;
    productPage_1* selection_PageOne;
    maintenancePage* maintenanceMode;
    bool p1, p2, p3, p4;

};

#endif // IDLE_H
