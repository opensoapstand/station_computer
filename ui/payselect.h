//***************************************
//
// payselect.h
// GUI class for user to select size and
// payment for drink.
//
// Allows navigation to First product page
// payment page and idle page
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef PAYSELECT_H
#define PAYSELECT_H

#include "df_util.h"
#include "dispensepage.h"
#include "wifiErrorPage.h"
#include "help.h"

class productPage_1;
class payPage;
class idle;
class dispensePage;
class wifiErrorPage;
class help;

namespace Ui {
class paySelect;
}

class paySelect : public QWidget
{
    Q_OBJECT

public:
    explicit paySelect(QWidget *parent = nullptr);
    void setPage(productPage_1 *pageSelect, dispensePage* pageDispense,wifiErrorPage* pageWifiError,  idle* pageIdle, payPage *pagePayment, help* pageHelp);
    ~paySelect();

    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);

    void cancelTimers();



signals:
    void paymentTotal(string, string, string);

private slots:
    // **** Navigation ****
    void on_previousPage_Button_clicked();
    void on_payPage_Button_clicked();


    // Set Drink Order
    void on_orderSmall_Button_clicked();
    void on_orderBig_Button_clicked();
    void on_mainPage_Button_clicked();
    void onSelectTimeoutTick();

private:
    bool stopSelectTimers();
    void selectOnTick();
    void mainPage();

    Ui::paySelect *ui;
    productPage_1* firstProductPage;
    payPage* paymentPage;
    idle* idlePage;
    dispensePage* dispensingPage;
    wifiErrorPage* wifiError;
    help* helpPage;

    QTimer* selectIdleTimer;
    int _selectIdleTimeoutSec;

    QResizeEvent *paySelectResize;
    QShowEvent *dispenseEvent;
    QShowEvent *wifiErrorEvent;

};

#endif // PAYSELECT_H
