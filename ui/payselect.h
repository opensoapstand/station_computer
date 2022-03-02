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
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef PAYSELECT_H
#define PAYSELECT_H

#include "df_util.h"
#include "page_dispenser.h"
#include "wifiErrorPage.h"
#include "help.h"

class pageproductsoverview;
class pagePayment;
class idle;
class page_dispenser;
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
    void setPage(pageproductsoverview *pageSelect, page_dispenser* page_dispenser,wifiErrorPage* pageWifiError,  idle* pageIdle, pagePayment *pagePayment, help* pageHelp);
    ~paySelect();

    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);

    void cancelTimers();
    void updatePriceAfterPromo(double promoPercent);



signals:
    void paymentTotal(string, string, string);

private slots:
    // **** Navigation ****
    void on_previousPage_Button_clicked();
    void on_pagePayment_Button_clicked();
    void on_applyPromo_Button_clicked();
    void on_promoCodeInput_clicked();


    // Set Drink Order
    void on_orderSmall_Button_clicked();
    void on_orderBig_Button_clicked();
    void on_mainPage_Button_clicked();
    void onSelectTimeoutTick();
    void buttonWasClicked(int);


private:
    bool stopSelectTimers();
    void selectOnTick();
    void mainPage();

    std::string readBuffer;
    Ui::paySelect *ui;
    pageproductsoverview* firstProductPage;
    pagePayment* paymentPage;
    idle* idlePage;
    page_dispenser* dispensingPage;
    wifiErrorPage* wifiError;
    help* helpPage;

    QTimer* selectIdleTimer;
    int _selectIdleTimeoutSec;

    QResizeEvent *paySelectResize;
    QShowEvent *dispenseEvent;
    QShowEvent *wifiErrorEvent;

};

#endif // PAYSELECT_H
