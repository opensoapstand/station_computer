//***************************************
//
// page_help.h
// GUI class for user to see Drinkfill
// page_help and contact info
//
// created: 28-05-2021
// by: Paddy Riley
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef HELP_H
#define HELP_H

#include "df_util.h"
#include "page_select_product.h"
#include "page_product.h"
#include "pagepayment.h"
#include "page_idle.h"

class page_select_product;
class pagePayment;
class page_idle;
class pageProduct;

namespace Ui {
class page_help;
}

class page_help : public QWidget
{
    Q_OBJECT

public:
    explicit page_help(QWidget *parent = nullptr);
    void setPage(page_select_product *pageSelect, pageProduct* pageProduct, page_idle* pageIdle, pagePayment *pagePayment);
    ~page_help();
    QTimer* helpIdleTimer;

private slots:
    // **** Navigation ****
    void on_previousPage_Button_clicked();
    void on_previousPage_Button_2_clicked();
    void on_refreshButton_clicked();
    void onHelpTimeoutTick();

private:
    void showEvent(QShowEvent *event);
    //bool stopHelpTimers();
    //void helpOnTick();

    Ui::page_help *ui;
    page_select_product* p_page_select_product;
    pagePayment* paymentPage;
    page_idle* idlePage;
    pageProduct* selectPage;

    int _helpIdleTimeoutSec;

    QResizeEvent *helpResize;

};

#endif // HELP_H
