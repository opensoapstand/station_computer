//***************************************
//
// page_product_mixing.h
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

#ifndef PAYSELECTMIX_H
#define PAYSELECTMIX_H

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
#include "dispenser_slot.h"
#include "page_idle.h"
#include "page_error_wifi.h"
#include "page_help.h"
#include "page_product_menu.h"

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
class page_product_menu;

namespace Ui
{
    class page_product_mixing;
}

class page_product_mixing : public QWidget
{
    Q_OBJECT

public:
    QLabel *additiveBackgroundRows[5];
    QLabel *additiveTitles[5];
    QLabel *additiveMinusButtonBackgrounds[5];
    QPushButton *additiveMinusButtons[5];
    QLabel *additivePlusButtonBackgrounds[5];
    QPushButton *additivePlusButtons[5];
    QLabel *additivePercentageLabels[5];

    QPushButton *orderSizeButtons[4];
    QLabel *orderSizeLabelsPrice[4];
    QLabel *orderSizeLabelsVolume[4];
    QLabel *orderSizeBackgroundLabels[4];

    QVector<QPushButton*> minusButtonCollection;
    QVector<QPushButton*> plusButtonCollection;

    explicit page_product_mixing(QWidget *parent = nullptr);
    void setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_payment_tap_serial *page_payment_tap_serial, page_payment_tap_tcp *page_payment_tap_tcp, page_help *pageHelp, page_product_overview *page_Overview, statusbar *p_statusbar, page_product_menu *page_product_menu);
    ~page_product_mixing();
    size_t WriteCallback_coupon(char *contents, size_t size, size_t nmemb, void *userp);

    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    void hideCurrentPageAndShowProductMenu();
    double convertAdditivePRatioToPercentage(double additivePRatio);
    bool isAdditiveEnabled(int index);
signals:
    void paymentTotal(string, string, string);

private slots:
    // **** Navigation ****
    // void on_pushButton_previous_page_clicked();
    void on_pushButton_continue_clicked();
    void on_pushButton_back_clicked();
    // Set Drink Order
    void on_pushButton_order_small_clicked();
    void on_pushButton_order_big_clicked();
    void on_pushButton_to_help_clicked();
    void onSelectTimeoutTick();
    void on_pushButton_order_custom_clicked();

    void on_pushButton_order_medium_clicked();

    void additiveMinusButtonsPressed(int index);
    void additivePlusButtonsPressed(int index);
    // void loadProdSpecs();

private:
    // button positions
    uint16_t orderSizeButtons_xywh_dynamic_ui_all_sizes_available[4][4] = {
        {560, 990, 135, 100}, // S
        {706, 990, 135, 100}, // M
        {852, 990, 135, 100}, // L
        {560, 1100, 430, 115} // custom
    };

    uint16_t orderSizeButtons_xywh_dynamic_ui_small_available[4][4] = {
        {564, 1088, 209, 126}, // S
        {1, 1, 1, 1},          // M
        {1, 1, 1, 1},          // L
        {1, 1, 1, 1}           // custom
    };

    uint16_t orderSizeButtons_xywh_dynamic_ui_small_large_custom_available[4][4] = {
        {564, 990, 209, 100}, // S
        {1, 1, 1, 1},
        {790, 990, 198, 100}, // L
        {564, 1100, 424, 113} // custom
    };

    uint16_t orderSizeButtons_xywh_dynamic_ui_small_and_large_available[4][4] = {
        {567, 1024, 198, 126}, // S
        {1, 1, 1, 1},          // M
        {788, 1024, 198, 126}, // L
        {1, 1, 1, 1}           // custom
    };
    uint16_t orderSizeButtons_xywh_dynamic_ui_custom_available[4][4] = {
        {1, 1, 1, 1},
        {1, 1, 1, 1}, // M
        {1, 1, 1, 1},
        {564, 1037, 424, 113} // custom
        // {564, 1037, 424, 113} // custom
    };
    uint16_t orderSizeButtons_xywh_dynamic_ui_small_custom_available[4][4] = {
        {560, 990, 430, 100}, // S
        {1, 1, 1, 1},         // M
        {1, 1, 1, 1},
        {560, 1100, 430, 115} // custom
        // {564, 1037, 424, 113} // custom
    };

    uint16_t orderSizeButtons_xywh_dynamic_ui_large_custom_available[4][4] = {
        {1, 1, 1, 1},         // S
        {1, 1, 1, 1},         // M
        {560, 990, 430, 100}, // L
        {560, 1100, 430, 115} // custom
        // {564, 1037, 424, 113} // custom
    };

    // labels of volume and price are different (and annoying)
    uint16_t orderSizeButtons_xywh_static_product_page[4][4] = {
        {564, 1088, 209, 126},
        {1, 1, 1, 1},
        {790, 1087, 198, 126},
        {1, 1, 1, 1}};

    uint16_t orderSizeVolumeLabels_xy_dynamic_ui_all_sizes_available[4][2] = {
        {560, 1000}, // S vol
        {710, 1000}, // M vol
        {860, 1000}, // L vol
        {570, 1110}  // custom col
    };
    uint16_t orderSizePriceLabels_xy_dynamic_ui_all_sizes_available[8][2] = {
        {560, 1040}, // S price
        {710, 1040}, // M price
        {860, 1040}, // L price
        {560, 1160}  // custom price
    };

    uint16_t orderSizeVolumeLabels_xy_dynamic_ui_small_large_custom_available[8][2] = {
        {605, 1000}, // S vol
        {1, 1},      // M vol
        {825, 1000}, // L vol
        {570, 1110}  // custom col
    };
    uint16_t orderSizePriceLabels_xy_dynamic_ui_small_large_custom_available[8][2] = {
        {605, 1040}, // S price
        {1, 1},      // M price
        {825, 1040}, // L price
        {560, 1160}  // custom price
    };

    uint16_t orderSizeVolumeLabels_xy_dynamic_ui_custom_available[8][2] = {
        {1, 1},     // S vol
        {1, 1},     // M vol
        {1, 1},     // L vol
        {570, 1047} // custom col
    };
    uint16_t orderSizePriceLabels_xy_dynamic_ui_custom_available[8][2] = {
        {1, 1},     // S price
        {1, 1},     // M price
        {1, 1},     // L price
        {560, 1097} // custom price
    };

    uint16_t orderSizeVolumeLabels_xy_dynamic_ui_small_custom_available[8][2] = {
        {710, 1000}, // S vol
        {1, 1},      // M vol
        {1, 1},      // L vol
        {570, 1110}  // custom col
    };

    uint16_t orderSizePriceLabels_xy_dynamic_ui_small_custom_available[8][2] = {
        {710, 1040}, // S price
        {1, 1},      // M price
        {1, 1},      // L price
        {560, 1160}  // custom price
    };

    uint16_t orderSizeVolumeLabels_xy_dynamic_ui_large_custom_available[8][2] = {
        {1, 1},      // S vol
        {1, 1},      // M vol
        {710, 1000}, // L vol
        {570, 1110}  // custom col
    };

    uint16_t orderSizePriceLabels_xy_dynamic_ui_large_custom_available[8][2] = {
        {1, 1},      // S price
        {1, 1},      // M price
        {710, 1040}, // L price
        {560, 1160}  // custom price
    };

    uint16_t orderSizeVolumeLabels_xy_dynamic_ui_small_available[8][2] = {
        {605, 1150}, // S vol
        {1, 1},      // M vol
        {1, 1},      // L vol
        {1, 1}       // custom col
    };
    uint16_t orderSizePriceLabels_xy_dynamic_ui_small_available[8][2] = {
        {605, 1110}, // S price
        {1, 1},      // M price
        {1, 1},      // L price
        {1, 1}       // custom price
    };

    uint16_t orderSizeVolumeLabels_xy_dynamic_ui_small_and_large_available[8][2] = {
        {605, 1050}, // S vol
        {1, 1},      // M vol
        {825, 1050}, // L vol
        {1, 1}       // custom col
    };
    uint16_t orderSizePriceLabels_xy_dynamic_ui_small_and_large_available[8][2] = {
        {605, 1090}, // S price
        {1, 1},      // M price
        {825, 1090}, // L price
        {1, 1}       // custom price
    };
    int product_sizes[4] = {SIZE_SMALL_INDEX, SIZE_MEDIUM_INDEX, SIZE_LARGE_INDEX, SIZE_CUSTOM_INDEX};
    bool stopSelectTimers();
    void reset_and_show_page_elements();
    void selectOnTick();
    int default_size;

    std::string readBuffer;
    Ui::page_product_mixing *ui;
    page_product_menu *p_page_product_menu;
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

#endif // PAYSELECTMIX_H
