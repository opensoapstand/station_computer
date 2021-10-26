//***************************************
//
// help.h
// GUI class for user to see Drinkfill
// help and contact info
//
// created: 28-05-2021
// by: Paddy Riley
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef HELP_H
#define HELP_H

#include "df_util.h"
#include "productpage_1.h"
#include "payselect.h"
#include "paypage.h"
#include "idle.h"

class productPage_1;
class payPage;
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
    void setPage(productPage_1 *pageSelect, paySelect* paySelect, idle* pageIdle, payPage *pagePayment);
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
    productPage_1* firstProductPage;
    payPage* paymentPage;
    idle* idlePage;
    paySelect* selectPage;

    int _helpIdleTimeoutSec;

    QResizeEvent *helpResize;

};

#endif // HELP_H
