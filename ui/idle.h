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
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef IDLE_H
#define IDLE_H

#include "df_util.h"
#include "pageproductsoverview.h"
#include "dfuicommthread.h"
#include "dbmanager.h"
#include "page_maintenance.h"

#define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"
#define DB_PATH_CLICKS "/release/db/sqlite/clicks.db"
#define DB_PATH_TEMPERATURE "/release/db/sqlite/temperature.db"

class page_maintenance;
class pageproductsoverview;

namespace Ui {
class idle;
}

class idle : public QWidget
{
    Q_OBJECT

public:
    explicit idle(QWidget *parent = nullptr);
    void setPage(pageproductsoverview *p_pageProduct, page_maintenance *pageMaintenance);
    ~idle();
    void showEvent(QShowEvent *event);

    DrinkOrder* userDrinkOrder;
    df_util* dfUtility;

    DfUiCommThread* dfComm;


    bool isEnough(int p);
    void MMSlot();

private slots:
    void on_nextPageButton_clicked();
//    void on_savedBottles_label_clicked();

private:
    Ui::idle *ui;
    pageproductsoverview* selection_PageOne;
    page_maintenance* p_page_maintenance;
    bool p1, p2, p3, p4;


};

#endif // IDLE_H
