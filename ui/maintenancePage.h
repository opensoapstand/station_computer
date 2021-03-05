//***************************************
//
// maintenancePage.h
// GUI class while machine is processing
// payment.
//
// Coordinates User input from payment select
// class then communcates results to dispensepage.
//
// created: 4-01-2021
// by: Paddy Riley
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef MAINTENANCEPAGE_H
#define MAINTENANCEPAGE_H

#include <includefiles.h>
#include "df_util.h"
#include "drinkorder.h"
#include "idle.h"
#include "dfuicommthread.h"
#include "maintain_product.h"
#include "productpage_1.h"

class idle;
class maintain_product;
class productPage_1;

namespace Ui {
class maintenancePage;
}

//using namespace std;

class maintenancePage : public QWidget
{
    Q_OBJECT

public:

    explicit maintenancePage(QWidget *parent = nullptr);
    void setPage(idle* pageIdle, maintain_product* pageMaintain, productPage_1 *pageProduct);
    ~maintenancePage();

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
    //void on_wifi_button_clicked();
    //void on_clean_button_clicked();
    //void on_restock_button_clicked();
    void onMaintenancePageTimeoutTick();

private:
    void showEvent(QShowEvent *event);
    Ui::maintenancePage *ui;
    idle* idlePage;
    maintain_product* maintainPage;
    productPage_1* selection_PageOne;

    QResizeEvent *productSelection;

    int _maintenancePageTimeoutSec;
    QTimer* maintenancePageEndTimer;
};

#endif // MAINTENANCEPAGE_H
