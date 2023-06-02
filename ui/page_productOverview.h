//***************************************
//
// page_product.h
// GUI class for user to select size and
// payment for drink.
//
// Allows navigation to First product page
// payment page and page_idle page
//
// created: 05-04-2022
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef OVERVIEW_H
#define OVERVIEW_H

#include "df_util.h"
#include "page_dispenser.h"
#include "page_error_wifi.h"
#include "page_help.h"
#include "page_product.h"


class page_select_product;
class page_qr_payment;
class page_tap_payment;
class page_idle;
class page_dispenser;
class page_error_wifi;
class page_help;
class page_product;
namespace Ui
{
    class page_product_overview;
}

class page_product_overview : public QWidget
{
    Q_OBJECT

public:
    QPushButton *orderSizeButtons[4];
    QLabel *orderSizeLabelsPrice[4];
    QLabel *orderSizeLabelsVolume[4];
    QLabel *orderSizeBackgroundLabels[4];

    explicit page_product_overview(QWidget *parent = nullptr);
    void setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_qr_payment *page_qr_payment,page_tap_payment *page_tap_payment, page_help *pageHelp, page_product *page_product);
    ~page_product_overview();

    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);

    void cancelTimers();
    void coupon_disable();
    void coupon_input_show();
    void coupon_input_hide();
    void coupon_input_reset();

    bool areCouponsEnabled();
    void apply_promo_code();

signals:
    void paymentTotal(string, string, string);

private slots:
    // **** Navigation ****
    void on_pushButton_previous_page_clicked();
    void on_pushButton_continue_clicked();
   
    void on_lineEdit_promo_codeInput_clicked();
    void on_pushButton_select_product_page_clicked();
    void return_to_selectProductPage();
    void updatePriceLabel();
    void on_pushButton_to_help_clicked();
    void onSelectTimeoutTick();
    void keyboardButtonPressed(int);
private:
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    int product_sizes[4] = {SIZE_SMALL_INDEX, SIZE_MEDIUM_INDEX, SIZE_LARGE_INDEX, SIZE_CUSTOM_INDEX};
    bool stopSelectTimers();
    void reset_and_show_page_elements();
    void selectOnTick();

    std::string readBuffer;
    Ui::page_product_overview *ui;
    page_select_product *p_page_select_product;
    page_qr_payment *p_page_payment_qr;
    page_tap_payment *p_page_payment_tap;
    page_idle *p_page_idle;
    page_dispenser *p_page_dispense;
    page_error_wifi *p_page_wifi_error;
    page_help *p_page_help;
    page_product *p_page_product;

    QTimer *selectIdleTimer;
    int _selectIdleTimeoutSec;

    QResizeEvent *page_product_overviewResize;
    QShowEvent *dispenseEvent;
    QShowEvent *wifiErrorEvent;
};

#endif // PAYSELECT_H