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

#ifndef FEEDBACK_H
#define FEEDBACK_H

#include "df_util.h"
#include "page_dispenser.h"
#include "page_error_wifi.h"
#include "page_sendFeedback.h"
#include "page_end.h"
#include "page_help.h"
#include "page_product.h"
#include "page_product_mixing.h"
#include <curl/curl.h>

class statusbar;
class keyboard;
class page_select_product;
class page_qr_payment;
class page_idle;
class page_dispenser;
class page_error_wifi;
class page_help;
class page_product;
class page_end;
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
    void setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_help *pageHelp, page_product *page_product, page_end *page_end, statusbar *p_statusbar, keyboard * keyboard);
    ~page_sendFeedback();

    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    void lineEditReturnButtonPressed();
    void textEditReturnButtonPressed();
    void textEditCancelButtonPressed();
    // void paintEvent(QPaintEvent *event);

signals:
    void paymentTotal(string, string, string);

private slots:

    void on_pushButton_to_idle_clicked();
    void onSelectTimeoutTick();

    void on_pushButton_send_clicked();
    void on_pushButton_previous_page_clicked();
    void on_pushButton_start_input_clicked();
    void on_pushButton_enter_email_clicked();
    void on_feedback_Text_Input_clicked();
    void keyboardButtonPressed(int);
    bool emailValid(QString email);
    void on_pushButton_help_page_clicked();

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
    page_product *p_page_product;
    statusbar *p_statusbar;
    keyboard *p_keyboard;

    QTimer *selectIdleTimer;
    int _selectIdleTimeoutSec;

    QResizeEvent *page_sendFeedbackResize;
    QShowEvent *dispenseEvent;
    QShowEvent *wifiErrorEvent;

    CURL *curl;
    CURLcode res;
    QByteArray curl_param_array;
    QString MachineSerialNumber;
    QVBoxLayout *statusbarLayout;
};

#endif // PAYSELECT_H
