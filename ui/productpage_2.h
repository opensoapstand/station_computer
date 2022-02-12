//***************************************
//
// productpage_2.h
// TODO: Compress to single Product page class
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef PRODUCTPAGE_2_H
#define PRODUCTPAGE_2_H

#include "df_util.h"
#include "maintenancePage.h"

class productPage_1;
class paySelect;
class idle;
class maintenancePage;

namespace Ui {
class productPage_2;
}

class productPage_2 : public QWidget
{
    Q_OBJECT

public:
    explicit productPage_2(QWidget *parent = nullptr);
    void setPage(productPage_1 *pageOneProducts, paySelect *pageSizeSelect, idle* pageIdle, maintenancePage *pageMaintenance);
    ~productPage_2();

private slots:
    void on_previousPage_Button_clicked();
    void on_selection7_Button_clicked();
    void on_selection8_Button_clicked();
    void on_selection9_Button_clicked();

private:
    Ui::productPage_2 *ui;

    productPage_1* firstProductPage;
    paySelect* paySelectPage;
    idle* idlePage;
    maintenancePage* maintenanceMode;
};

#endif // PRODUCTPAGE_2_H
