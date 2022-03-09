//***************************************
//
// page_product.h
// GUI class for user to select size and
// payment for drink.
//
// Allows navigation to First product page
// payment page and page_idle page
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
#include "page_error_wifi.h"
#include "page_help.h"

class page_select_product;
class pagePayment;
class page_idle;
class page_dispenser;
class page_error_wifi;
class page_help;

namespace Ui {
class pageProduct;
}

class pageProduct : public QWidget
{
    Q_OBJECT

public:
    explicit pageProduct(QWidget *parent = nullptr);
    void setPage(page_select_product *pageSelect, page_dispenser* page_dispenser,page_error_wifi* pageWifiError,  page_idle* pageIdle, pagePayment *pagePayment, page_help* pageHelp);
    ~pageProduct();

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

    void set_product_label(int checkOption, char drinkSize);

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
    Ui::pageProduct *ui;
    page_select_product* firstProductPage;
    pagePayment* paymentPage;
    page_idle* idlePage;
    page_dispenser* dispensingPage;
    page_error_wifi* wifiError;
    page_help* helpPage;

    QTimer* selectIdleTimer;
    int _selectIdleTimeoutSec;

    QResizeEvent *pageProductResize;
    QShowEvent *dispenseEvent;
    QShowEvent *wifiErrorEvent;

};

#endif // PAYSELECT_H
