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
#include "page_productFreeSample.h"
#include "page_payment_tap_serial.h"
#include "page_payment_tap_tcp.h"
#include "page_qr_payment.h"
#include "page_offline_payment.h"
#include "page_select_product.h"
#include "dispenser_slot.h"
#include "page_idle.h"
#include "page_error_wifi.h"
#include "page_help.h"
#include "page_product_menu.h"

class statusbar;
class page_select_product;
class page_qr_payment;
class page_offline_payment;
class page_idle;
class page_dispenser;
class page_error_wifi;
class page_help;
class page_product_overview;
class page_product_freeSample;
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
    void setPage(page_select_product *pageSelect, page_dispenser *page_dispenser, page_error_wifi *pageWifiError, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_offline_payment *page_offline_payment,page_payment_tap_serial *page_payment_tap_serial, page_payment_tap_tcp *page_payment_tap_tcp, page_help *pageHelp, page_product_overview *page_Overview, statusbar *p_statusbar, page_product_menu *page_product_menu,page_product_freeSample *p_page_freeSample);
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
    void on_pushButton_order_sample_clicked();

    void additiveMinusButtonsPressed(int index);
    void additivePlusButtonsPressed(int index);
    void on_pushButton_recommended_clicked();
    // void loadProdSpecs();

private:
    // button positions
    uint16_t orderSizeButtons_xywh_dynamic_ui_all_sizes_available[4][4] = {
        {495, 1322, 255, 158}, // S
        {775, 1322, 255, 158}, // M
        {495, 1505, 255, 158}, // L
        {775, 1505, 255, 158} // custom
    };

    uint16_t orderSizeButtons_xywh_dynamic_ui_small_available[4][4] = {
        {495, 1322, 255, 158}, // S
        {1, 1, 1, 1},          // M
        {1, 1, 1, 1},          // L
        {1, 1, 1, 1}           // custom
    };

    uint16_t orderSizeButtons_xywh_dynamic_ui_small_large_custom_available[4][4] = {
        {495, 1322, 255, 158}, // S
        {1, 1, 1, 1},
        {775, 1322, 255, 158}, // L
        {495, 1505, 255, 158} // custom
    };

    uint16_t orderSizeButtons_xywh_dynamic_ui_small_and_large_available[4][4] = {
        {495, 1322, 255, 158}, // S
        {1, 1, 1, 1},          // M
        {775, 1322, 255, 158}, // L
        {1, 1, 1, 1}           // custom
    };
    uint16_t orderSizeButtons_xywh_dynamic_ui_custom_available[4][4] = {
        {1, 1, 1, 1},
        {1, 1, 1, 1}, // M
        {1, 1, 1, 1},
        {495, 1322, 255, 158} // custom
        // {564, 1037, 424, 113} // custom
    };
    uint16_t orderSizeButtons_xywh_dynamic_ui_small_custom_available[4][4] = {
        {495, 1322, 255, 158}, // S
        {1, 1, 1, 1},         // M
        {1, 1, 1, 1},
        {775, 1322, 255, 158} // custom
        // {564, 1037, 424, 113} // custom
    };

    uint16_t orderSizeButtons_xywh_dynamic_ui_large_custom_available[4][4] = {
        {1, 1, 1, 1},         // S
        {1, 1, 1, 1},         // M
        {495, 1322, 255, 158}, // L
        {775, 1322, 255, 158} // custom
        // {564, 1037, 424, 113} // custom
    };
    
    uint16_t orderSizeVolumeLabels_xy_dynamic_ui_all_sizes_available[4][2] = {
        {546, 1365}, // S vol
        {826, 1365}, // M vol
        {546, 1548}, // L vol
        {813, 1535}  // custom col
    };
    uint16_t orderSizePriceLabels_xy_dynamic_ui_all_sizes_available[8][2] = {
        {546, 1401}, // S price
        {826, 1401}, // M price
        {546, 1584}, // L price
        {847, 1570}  // custom price
    };

    uint16_t orderSizeVolumeLabels_xy_dynamic_ui_small_large_custom_available[8][2] = {
        {546, 1365}, // S vol
        {1, 1},      // M vol
        {826, 1365}, // L vol
        {532, 1530}  // custom col
    };
    uint16_t orderSizePriceLabels_xy_dynamic_ui_small_large_custom_available[8][2] = {
        {546, 1401}, // S price
        {1, 1},      // M price
        {826, 1401}, // L price
        {566, 1565}  // custom price
    };

    uint16_t orderSizeVolumeLabels_xy_dynamic_ui_custom_available[8][2] = {
        {1, 1},     // S vol
        {1, 1},     // M vol
        {1, 1},     // L vol
        {532, 1347} // custom col
    };
    uint16_t orderSizePriceLabels_xy_dynamic_ui_custom_available[8][2] = {
        {1, 1},     // S price
        {1, 1},     // M price
        {1, 1},     // L price
        {566, 1382} // custom price
    };

    uint16_t orderSizeVolumeLabels_xy_dynamic_ui_small_custom_available[8][2] = {
        {546, 1365}, // S vol
        {1, 1},      // M vol
        {1, 1},      // L vol
        {812, 1347}  // custom col
    };

    uint16_t orderSizePriceLabels_xy_dynamic_ui_small_custom_available[8][2] = {
        {546, 1401}, // S price
        {1, 1},      // M price
        {1, 1},      // L price
        {846, 1382}  // custom price
    };

    uint16_t orderSizeVolumeLabels_xy_dynamic_ui_large_custom_available[8][2] = {
        {1, 1},      // S vol
        {1, 1},      // M vol
        {546, 1365}, // L vol
        {812, 1347}  // custom col
    };

    uint16_t orderSizePriceLabels_xy_dynamic_ui_large_custom_available[8][2] = {
        {1, 1},      // S price
        {1, 1},      // M price
        {546, 1401}, // L price
        {846, 1382}  // custom price
    };

    uint16_t orderSizeVolumeLabels_xy_dynamic_ui_small_available[8][2] = {
        {546, 1365}, // S vol
        {1, 1},      // M vol
        {1, 1},      // L vol
        {1, 1}       // custom col
    };
    uint16_t orderSizePriceLabels_xy_dynamic_ui_small_available[8][2] = {
        {546, 1401}, // S price
        {1, 1},      // M price
        {1, 1},      // L price
        {1, 1}       // custom price
    };

    uint16_t orderSizeVolumeLabels_xy_dynamic_ui_small_and_large_available[8][2] = {
        {546, 1365}, // S vol
        {1, 1},      // M vol
        {826, 1365}, // L vol
        {1, 1}       // custom col
    };
    uint16_t orderSizePriceLabels_xy_dynamic_ui_small_and_large_available[8][2] = {
        {546, 1401}, // S price
        {1, 1},      // M price
        {826, 1401}, // L price
        {1, 1}       // custom price
    };
    int product_sizes[5] = {SIZE_SMALL_INDEX, SIZE_MEDIUM_INDEX, SIZE_LARGE_INDEX, SIZE_CUSTOM_INDEX, SIZE_SAMPLE_INDEX};
    bool stopSelectTimers();
    void reset_and_show_page_elements();
    void selectOnTick();
    int default_size;

    std::string readBuffer;
    Ui::page_product_mixing *ui;
    page_product_menu *p_page_product_menu;
    page_select_product *p_page_select_product;
    page_qr_payment *paymentPage;
    page_offline_payment *paymentOfflinePage;
    page_idle *p_page_idle;
    page_dispenser *p_page_dispense;
    page_error_wifi *p_page_wifi_error;
    page_help *p_page_help;
    page_product_overview *p_page_overview;
    page_product_freeSample *p_page_product_freeSample;
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
