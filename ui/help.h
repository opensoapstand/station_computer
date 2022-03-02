//***************************************
//
// help.h
// GUI class for user to see Drinkfill
// help and contact info
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
#include "pageproductsoverview.h"
#include "payselect.h"
#include "pagepayment.h"
#include "idle.h"

class pageproductsoverview;
class pagePayment;
class idle;
class paySelect;

namespace Ui {
class help;
}

class help : public QWidget
{
    Q_OBJECT

public:
    explicit help(QWidget *parent = nullptr);
    void setPage(pageproductsoverview *pageSelect, paySelect* paySelect, idle* pageIdle, pagePayment *pagePayment);
    ~help();
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

    Ui::help *ui;
    pageproductsoverview* firstProductPage;
    pagePayment* paymentPage;
    idle* idlePage;
    paySelect* selectPage;

    int _helpIdleTimeoutSec;

    QResizeEvent *helpResize;

};

#endif // HELP_H
