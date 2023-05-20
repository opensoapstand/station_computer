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

#ifndef PAYSELECT_H
#define PAYSELECT_H

#include "df_util.h"
#include "page_dispenser.h"
#include "page_productOverview.h"

#include "page_error_wifi.h"
#include "page_help.h"

class page_select_product;
class page_qr_payment;
class page_idle;
class page_dispenser;
class page_error_wifi;
class page_help;
class pageProductOverview;

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
    QLabel *orderSizeBackgroundLabels[4];

    explicit pageProduct(QWidget *parent = nullptr);
    void setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_help *pageHelp, pageProductOverview *page_Overview);
    ~pageProduct();

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
    void on_pushButton_to_idle_clicked();
    void onSelectTimeoutTick();

    void on_pushButton_order_custom_clicked();

    void on_pushButton_order_medium_clicked();
    void loadProductBySize(int sizeIndex);
    void loadProdSpecs();

private:
    int product_sizes[4] = {SIZE_SMALL_INDEX, SIZE_MEDIUM_INDEX, SIZE_LARGE_INDEX, SIZE_CUSTOM_INDEX};
    bool stopSelectTimers();
    void reset_and_show_page_elements();
    void selectOnTick();
    

    std::string readBuffer;
    Ui::pageProduct *ui;
    page_select_product *p_page_select_product;
    page_qr_payment *paymentPage;
    page_idle *p_page_idle;
    page_dispenser *p_page_dispense;
    page_error_wifi *p_page_wifi_error;
    page_help *p_page_help;
    pageProductOverview *p_page_overview;



    QTimer *selectIdleTimer;
    int _selectIdleTimeoutSec;

    QResizeEvent *pageProductResize;
    QShowEvent *dispenseEvent;
    QShowEvent *wifiErrorEvent;
};

#endif // PAYSELECT_H
