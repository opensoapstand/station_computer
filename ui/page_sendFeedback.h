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

#ifndef FEEDBACK_H
#define FEEDBACK_H

#include "df_util.h"
#include "page_dispenser.h"
#include "page_error_wifi.h"
#include "page_sendFeedback.h"
#include "pagethankyou.h"
#include "page_help.h"
#include "page_product.h"
#include <curl/curl.h>

class page_select_product;
class page_qr_payment;
class page_idle;
class page_dispenser;
class page_error_wifi;
class page_help;
class pageProduct;
class pagethankyou;
namespace Ui
{
    class page_sendFeedback;
}

class page_sendFeedback : public QWidget
{
    Q_OBJECT

public:
    QPushButton *orderSizeButtons[4];
    QLabel *orderSizeLabelsPrice[4];
    QLabel *orderSizeLabelsVolume[4];
    QLabel *orderSizeBackgroundLabels[4];

    explicit page_sendFeedback(QWidget *parent = nullptr);
    void setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_help *pageHelp, pageProduct *page_product, pagethankyou *pagethankyou);
    ~page_sendFeedback();

    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    // void paintEvent(QPaintEvent *event);

signals:
    void paymentTotal(string, string, string);

private slots:
    // **** Navigation ****
//     void on_previousPage_Button_clicked();
    // void on_page_qr_payment_Button_clicked();
    // void on_promoCodeInput_clicked();

    // Set Drink Order
    void on_mainPage_Button_clicked();
    void onSelectTimeoutTick();

    void on_send_Button_clicked();

    // void keyboardButtonPressed(int);
    void on_previousPage_Button_clicked();
    void on_feedback_Input_Button_clicked();
    void on_feedback_Text_Input_clicked();
    void keyboardButtonPressed(int);

    

    void on_feedbackText_cursorPositionChanged(int arg1, int arg2);

private:
    int product_sizes[4] = {SIZE_SMALL_INDEX, SIZE_MEDIUM_INDEX, SIZE_LARGE_INDEX, SIZE_CUSTOM_INDEX};
    void reset_and_show_page_elements();
    void selectOnTick();
    void mainPage();

    std::string readBuffer;
    Ui::page_sendFeedback *ui;
    page_select_product *p_page_select_product;
    page_qr_payment *paymentPage;
    page_idle *p_page_idle;
    page_dispenser *p_page_dispense;
    page_error_wifi *p_page_wifi_error;
    page_help *p_page_help;
    pageProduct *p_page_product;
    DrinkOrder *selectedProductOrder;

    QTimer *selectIdleTimer;
    int _selectIdleTimeoutSec;

    QResizeEvent *page_sendFeedbackResize;
    QShowEvent *dispenseEvent;
    QShowEvent *wifiErrorEvent;

    CURL *curl;
    CURLcode res;
    QByteArray curl_param_array;
    QString MachineSerialNumber;
};

#endif // PAYSELECT_H
