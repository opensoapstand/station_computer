//***************************************
//
// productpage_1.h
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

#ifndef PRODUCTPAGE_1_H
#define PRODUCTPAGE_1_H

#include "df_util.h"
#include "maintenancePage.h"
#include "help.h"

class paySelect;
class idle;
class maintenancePage;
class help;

namespace Ui {
class productPage_1;
}

class productPage_1 : public QWidget
{
    Q_OBJECT

public:
    explicit productPage_1(QWidget *parent = nullptr);
    void setPage(paySelect *pageSizeSelect, idle* pageIdle, maintenancePage *pageMaintenance, help *pageHelp);
    ~productPage_1();
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
    void on_maintenanceModeButton_pressed();
    void on_mainPage_Button_clicked();

private:
    void showEvent(QShowEvent *event);
    Ui::productPage_1 *ui;
    // productPage_2 *selection_PageTwo;
    paySelect *paymentSelectPage;
    idle* idlePage;

    QResizeEvent *productResize;

    int _productPageTimeoutSec;
    QTimer* productPageEndTimer;

    maintenancePage* maintenanceMode;

    help* helpPage;

    int maintenanceCounter;
};

#endif // PRODUCTPAGE_1_H
