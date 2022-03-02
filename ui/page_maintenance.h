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
#include "drinkorder.h"
#include "idle.h"
#include "dfuicommthread.h"
#include "page_maintenance_dispenser.h"
#include "pageproductsoverview.h"

class idle;
class page_maintenance_dispenser;
class pageproductsoverview;
class paySelect;

namespace Ui {
class page_maintenance;
}

//using namespace std;

class page_maintenance : public QWidget
{
    Q_OBJECT

public:
    explicit page_maintenance(QWidget *parent = nullptr);
    void setPage(idle* pageIdle, page_maintenance_dispenser* pageMaintain, pageproductsoverview *pageProduct, paySelect* pagePaySelect);
    ~page_maintenance();
    int getSelection();


private slots:
    void on_backButton_clicked();
    void on_product1_button_clicked();
    void on_product2_button_clicked();
    void on_product3_button_clicked();
    void on_product4_button_clicked();
//    void on_product5_button_clicked();
//    void on_product6_button_clicked();
//    void on_product7_button_clicked();
//    void on_product8_button_clicked();
//    void on_product9_button_clicked();
    void on_wifiButton_clicked();
    //void on_clean_button_clicked();
    //void on_restock_button_clicked();
    void onPage_maintenanceTimeoutTick();
    void btn_clicked();
//    void on_buttonGroup_buttonClicked();
    void buttonWasClicked(int);

private:
    void showEvent(QShowEvent *event);
    Ui::page_maintenance *ui;
    idle* idlePage;
    page_maintenance_dispenser* maintainPage;
    pageproductsoverview* selection_PageOne;
    paySelect* paySelectPage;

    QResizeEvent *productSelection;

    int _page_maintenanceTimeoutSec;
    QTimer* page_maintenanceEndTimer;

};

#endif // PAGE_MAINTENANCE_H
