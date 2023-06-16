//***************************************
//
// page_maintenance.h
// GUI class while machine is processing
// payment.
//
// Coordinates User input from payment select
// class then communcates results to page_dispenser.
//
// created: 4-01-2021
// by: Paddy Riley
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef PAGE_MAINTENANCE_H
#define PAGE_MAINTENANCE_H

#include <includefiles.h>
#include "df_util.h"

#include "dfuicommthread.h"
#include "page_maintenance_dispenser.h"
#include "page_maintenance_general.h"
#include "page_idle.h"
#include "page_select_product.h"

class page_idle;
class page_maintenance_dispenser;
class page_maintenance_general;
class page_select_product;
class page_product;

namespace Ui {
class page_maintenance;
}

//using namespace std;

class page_maintenance : public QWidget
{
    Q_OBJECT

public:
    explicit page_maintenance(QWidget *parent = nullptr);
    void setPage(page_idle* pageIdle, page_maintenance_dispenser* p_pageMaintenanceDispenser, page_maintenance_general* p_pageMaintenanceGeneral, page_select_product *p_page_product, page_product* pagePaySelect);


    ~page_maintenance();
    int getSelection();
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);

private slots:
    void on_pushButton_to_previous_page_clicked();
    void on_pushButtons_product_clicked();
    void on_pushButtons_product_clicked();
    void on_pushButtons_product_clicked();
    void on_pushButtons_product_clicked();
//    void on_pushButtons_product_clicked();
//    void on_pushButtons_product_clicked();
//    void on_pushButtons_product_clicked();
//    void on_pushButtons_product_clicked();
//    void on_pushButtons_product_clicked();
    // void on_wifiButton_clicked();
    //void on_clean_button_clicked();
    //void on_restock_button_clicked();
    void onPage_maintenanceTimeoutTick();



    // void on_enable_empty_container_checkBox_stateChanged(int arg1);
    // void on_enable_pump_ramping_checkBox_stateChanged(int arg1);


    void on_pushButton_general_settings_clicked();

private:
    void showEvent(QShowEvent *event);
    Ui::page_maintenance *ui;
    page_idle* p_page_idle;
    page_maintenance_dispenser* p_page_maintenance_product;
    page_maintenance_general* p_page_maintenance_general;
    page_select_product* p_pageSelectProduct;
    page_product* p_page_product;

    // QResizeEvent *productSelection;
    QPushButton* pushButtons_products[4];
    QLabel* labels_product_status[4];
    int _page_maintenanceTimeoutSec;
    QTimer* page_maintenanceEndTimer;

};

#endif // PAGE_MAINTENANCE_H
