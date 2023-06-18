//***************************************
//
// page_idle_products.h
// GUI class while machine is idle.
//
// Display Fullscreen DF branded Wallpaper
// Listen for User interaction to load
// Product Page1
//
// created: 01-05-2022
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado & Udbhav Kansal
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef IDLE_PRODUCTS_H
#define IDLE_PRODUCTS_H

#include "df_util.h"
#include "page_select_product.h"
#include "dfuicommthread.h"
#include "page_idle.h"
#include "dbmanager.h"
#include "page_maintenance.h"
#include "page_maintenance_general.h"
#include <QMediaPlayer>
#include <QGraphicsVideoItem>

// #define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"
// #define DB_PATH_CLICKS "/release/db/sqlite/clicks.db"
// #define DB_PATH_TEMPERATURE "/release/db/sqlite/temperature.db"

class page_maintenance;
class page_select_product;
class page_idle;
class page_maintenance_general;
class page_product;
class page_maintenance;
class page_help;

namespace Ui
{
    class page_idle_products;
}

class page_idle_products : public QWidget
{
    Q_OBJECT

public:
    explicit page_idle_products(QWidget *parent = nullptr);
    void setPage(page_idle *pageIdle, page_select_product *p_page_select_product);
    // function body
    ~page_idle_products();

    void select_product(int slot);
    void displayProducts();
    void addCompanyLogoToLabel(QLabel *label);
    void setBackgroundPictureFromTemplateToPage(QWidget *page, QString imageName);

    void printerStatusFeedback(bool isOnline, bool hasPaper);

    QLabel *labels_product_picture[4];
    QLabel *labels_selectProductOverlay[4];
    // QLabel* labels_product_name[4];
    QLabel *labels_product_icon[4];
    QLabel *labels_product_type[4];
    QLabel *labels_product_overlay_text[4];

private slots:
    // **** Navigation ****

    // void onProductPageTimeoutTick();
    // void on_p_page_maintenanceButton_pressed();
    void on_pushButton_to_select_product_page_clicked();

private:
    void showEvent(QShowEvent *event);
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    void hideAllLabelAndButtons();

    void resizeEvent(QResizeEvent *event);
    void checkReceiptPrinterStatus();

    Ui::page_idle_products *ui;
    // productPage_2 *selection_PageTwo;
    page_product *p_page_product;
    page_idle *p_page_idle;

    QResizeEvent *productResize;

    // int _productPageTimeoutSec;
    // QTimer *productPageEndTimer;

    page_select_product *p_pageSelectProduct;
    page_maintenance *p_page_maintenance;
    page_maintenance_general *p_page_maintenance_general;
    page_idle_products *p_page_idle_products;

    page_help *p_page_help;

    int maintenanceCounter;
    QTimer *backgroundChangeTimer;
};

#endif // page_idle_products_H