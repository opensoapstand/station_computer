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

namespace Ui
{
    class pageProduct;
}

class pageProduct : public QWidget
{
    Q_OBJECT

public:
    QPushButton *orderSizeButtons[4];
    QLabel *orderSizeLabelsPrice[4];
    QLabel *orderSizeLabelsVolume[4];

    explicit pageProduct(QWidget *parent = nullptr);
    void setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, pagePayment *pagePayment, page_help *pageHelp);
    ~pageProduct();

    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    // void paintEvent(QPaintEvent *event);

    void cancelTimers();
    void updatePriceAfterPromo(double promoPercent);
    void couponHandler();
    void coupon_disable();
    void coupon_input_show();
    void coupon_input_hide();
    void coupon_input_reset();

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
    void keyboardButtonPressed(int);

    void on_orderCustom_Button_clicked();

    void on_orderMedium_Button_clicked();
    void loadOrderSize(int sizeIndex);
    void loadOrderSelectedSize();

private:
    bool stopSelectTimers();
    void reset_and_show_page_elements();
    void selectOnTick();
    void mainPage();

    std::string readBuffer;
    Ui::pageProduct *ui;
    page_select_product *p_page_select_product;
    pagePayment *paymentPage;
    page_idle *idlePage;
    page_dispenser *p_page_dispense;
    page_error_wifi *wifiError;
    page_help *helpPage;
    
    DrinkOrder *selectedProductOrder;

    QTimer *selectIdleTimer;
    int _selectIdleTimeoutSec;

    QResizeEvent *pageProductResize;
    QShowEvent *dispenseEvent;
    QShowEvent *wifiErrorEvent;
};

#endif // PAYSELECT_H
