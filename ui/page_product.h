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
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef PAYSELECT_H
#define PAYSELECT_H

#include "df_util.h"
#include <iostream>
#include <string>
#include <cmath>
#include <curl/curl.h>
#include <json.hpp>

#include "page_dispenser.h"
#include "page_productOverview.h"
#include "page_payment_tap_serial.h"
#include "page_payment_tap_tcp.h"
#include "page_qr_payment.h"
#include "page_select_product.h"
#include "page_productOverview.h"
#include "dispenser_slot.h"
#include "page_idle.h"
#include "page_error_wifi.h"
#include "page_help.h"

class statusbar;
class page_select_product;
class page_qr_payment;
class page_idle;
class page_dispenser;
class page_error_wifi;
class page_help;
class page_product_overview;
class page_payment_tap_serial;
class page_payment_tap_tcp;

namespace Ui
{
    class page_product;
}

class page_product : public QWidget
{
    Q_OBJECT

public:
    QPushButton *orderSizeButtons[4];
    QLabel *orderSizeLabelsPrice[4];
    QLabel *orderSizeLabelsVolume[4];
    QLabel *orderSizeBackgroundLabels[4];

    explicit page_product(QWidget *parent = nullptr);
    void setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_payment_tap_serial *page_payment_tap_serial, page_payment_tap_tcp *page_payment_tap_tcp, page_help *pageHelp, page_product_overview *page_Overview, statusbar *p_statusbar);
    ~page_product();

    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);

signals:
    void paymentTotal(string, string, string);

private slots:
    // **** Navigation ****
    void on_pushButton_previous_page_clicked();
    void on_pushButton_continue_clicked();
    void on_pushButton_back_clicked();
    // Set Drink Order
    void on_pushButton_order_small_clicked();
    void on_pushButton_order_big_clicked();
    void on_pushButton_to_help_clicked();
    void onSelectTimeoutTick();

    void on_pushButton_order_custom_clicked();

    void on_pushButton_order_medium_clicked();
    // void loadProdSpecs();

private:
    int product_sizes[4] = {SIZE_SMALL_INDEX, SIZE_MEDIUM_INDEX, SIZE_LARGE_INDEX, SIZE_CUSTOM_INDEX};
    bool stopSelectTimers();
    void reset_and_show_page_elements();
    void selectOnTick();
    int default_size;

    std::string readBuffer;
    Ui::page_product *ui;
    page_select_product *p_page_select_product;
    page_qr_payment *paymentPage;
    page_idle *p_page_idle;
    page_dispenser *p_page_dispense;
    page_error_wifi *p_page_wifi_error;
    page_help *p_page_help;
    page_product_overview *p_page_overview;
    page_payment_tap_tcp *p_page_payment_tap_tcp;
    page_payment_tap_serial *p_page_payment_tap_serial;
    statusbar *p_statusbar;

    QTimer *selectIdleTimer;
    int _selectIdleTimeoutSec;

    QResizeEvent *page_productResize;
    QShowEvent *dispenseEvent;
    QShowEvent *wifiErrorEvent;

    QVBoxLayout *statusbarLayout;
};

#endif // PAYSELECT_H
