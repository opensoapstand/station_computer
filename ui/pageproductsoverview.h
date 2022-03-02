//***************************************
//
// pageproductsoverview.h
// GUI class for user to browse up to
// 6 drinks.
//
// Allows navigation to First product page
// payment selection page
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef PAGEPRODUCTSOVERVIEW_H
#define PAGEPRODUCTSOVERVIEW_H

#include "df_util.h"
#include "page_maintenance.h"
#include "help.h"

class pageProduct;
class idle;
class page_maintenance;
class help;

namespace Ui {
class pageproductsoverview;
}

class pageproductsoverview : public QWidget
{
    Q_OBJECT

public:
    explicit pageproductsoverview(QWidget *parent = nullptr);
    void setPage(pageProduct *pageSizeSelect, idle* pageIdle, page_maintenance *pageMaintenance, help *pageHelp);
    ~pageproductsoverview();
    void cancelTimers();

private slots:
    // **** Navigation ****
    //void on_nextPageButton_clicked();
    // HACK: Could have better way for modular reference to buttons and drink orders...
    void on_selection1_Button_clicked();
    void on_selection2_Button_clicked();
    void on_selection3_Button_clicked();
    void on_selection4_Button_clicked();
    //void on_selection5_Button_clicked();
    //void on_selection6_Button_clicked();
    void mainPage();
    void onProductPageTimeoutTick();
    //void on_backButton_clicked();
    void on_p_page_maintenanceButton_pressed();
    void on_mainPage_Button_clicked();

private:
    void showEvent(QShowEvent *event);
    Ui::pageproductsoverview *ui;
    // productPage_2 *selection_PageTwo;
    pageProduct *paymentSelectPage;
    idle* idlePage;

    QResizeEvent *productResize;

    int _productPageTimeoutSec;
    QTimer* productPageEndTimer;

    page_maintenance* p_page_maintenance;

    help* helpPage;

    int maintenanceCounter;
};

#endif // PAGEPRODUCTSOVERVIEW_H
