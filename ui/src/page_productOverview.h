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

#ifndef OVERVIEW_H
#define OVERVIEW_H

#include "df_util.h"
#include "page_dispenser.h"
#include "page_error_wifi.h"
#include "page_help.h"
#include "page_product.h"
#include "page_product_mixing.h"
#include "page_payment_tap_serial.h"
#include "page_email.h"

class statusbar;
class keyboard;
class page_select_product;
class page_qr_payment;
class page_offline_payment;
class page_payment_tap_tcp;
class page_payment_tap_serial;
class page_idle;
class page_dispenser;
class page_error_wifi;
class page_help;
class page_product;
class page_email;
class page_product_mixing;

// typedef enum UserRole
// {
//     user,
//     maintainer,
//     admin
// } UserRole;


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
    void setPage(page_select_product *pageSelect, page_product_mixing *p_page_product_mixing, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_offline_payment *page_offline_payment, page_payment_tap_serial *page_payment_tap_serial,page_payment_tap_tcp *page_payment_tap_tcp, page_help *pageHelp, page_product *page_product, page_email *page_email, statusbar * statusbar,keyboard * keyboard);
    ~page_product_overview();

    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);

    void cancelTimers();
    QString additivies_overview(QString product_additives_overview);
    void apply_promo_code(QString promocode);
    // ready to be killed off
    // void check_to_page_email();
    bool m_readyToSendCoupon; 
    void reset_and_show_page_elements();
    void enterButtonPressed();
signals:
    void paymentTotal(string, string, string);

private slots:
    // **** Navigation ****
    void on_pushButton_previous_page_clicked();
    void on_pushButton_continue(int);

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
    void selectOnTick();

    CURLcode res;
    std::string readBuffer;
    long http_code;
    Ui::page_product_overview *ui;
    page_select_product *p_page_select_product;
    page_qr_payment *p_page_payment_qr;
    page_offline_payment *p_page_offline_payment;
    page_payment_tap_tcp *p_page_payment_tap_tcp;
    page_payment_tap_serial *p_page_payment_tap_serial;
    page_idle *p_page_idle;
    page_dispenser *p_page_dispense;
    page_error_wifi *p_page_wifi_error;
    page_help *p_page_help;
    page_product *p_page_product;
    statusbar *p_statusbar;
    keyboard *p_keyboard;
    page_email* p_page_email;
    page_product_mixing *p_page_product_mixing;

    QTimer *selectIdleTimer;
    int _selectIdleTimeoutSec;

    QResizeEvent *page_product_overviewResize;
    QShowEvent *dispenseEvent;
    QShowEvent *wifiErrorEvent;
    QVBoxLayout *statusbarLayout;


};

#endif // PAYSELECT_H