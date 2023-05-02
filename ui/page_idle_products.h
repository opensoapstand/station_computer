//***************************************
//
// page_idle_products.h
// GUI class while machine is idle.
//
// Display Fullscreen DF branded Wallpaper
// Listen for User interaction to load
// Product Page1
//
// created: 01-05-2022
// by: Lode Ameije & Ash Singla & Udbhav Kansal
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef IDLE_PRODUCTS_H
#define IDLE_PRODUCTS_H

#include "df_util.h"
#include "page_select_product.h"
#include "dfuicommthread.h"
#include "page_idle.h"
#include "dbmanager.h"
#include "page_maintenance.h"
#include "page_maintenance_general.h"
#include "ui_page_idle.h"
#include <QMediaPlayer>
#include <QGraphicsVideoItem>

// #define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"
// #define DB_PATH_CLICKS "/release/db/sqlite/clicks.db"
// #define DB_PATH_TEMPERATURE "/release/db/sqlite/temperature.db"

class page_maintenance;
class page_select_product;
class page_idle;
class page_maintenance_general;

namespace Ui
{
    class page_idle_products;
}

class page_idle_products : public QWidget
{
    Q_OBJECT

public:
    explicit page_idle_products(QWidget *parent = nullptr);
    void setPage(page_select_product *p_pageProduct, page_idle *p_page_idle, page_maintenance *pageMaintenance, page_maintenance_general *pageMaintenanceGeneral);

    ~page_idle_products();
    void showEvent(QShowEvent *event);
   
};

#endif // IDLE_PRODUCTS_H
