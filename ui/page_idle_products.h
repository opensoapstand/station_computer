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
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado & Udbhav Kansal
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef IDLE_PRODUCTS_H
#define IDLE_PRODUCTS_H

#include "df_util.h"
// #include "page_select_product.h"
// #include "dfuicommthread.h"
#include "page_idle.h"
// #include "dbmanager.h"
// #include "page_maintenance.h"
// #include "page_maintenance_general.h"
#include <QMediaPlayer>
#include <QGraphicsVideoItem>

// #define CONFIG_DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"
// #define USAGE_DB_PATH_CLICKS "/release/db/sqlite/clicks.db"
// #define USAGE_DB_PATH_TEMPERATURE "/release/db/sqlite/temperature.db"

// class page_maintenance;
class page_select_product;
class page_idle;
// class page_maintenance_general;
// class page_product;
// class page_maintenance;
// class page_help;

namespace Ui
{
    class page_idle_products;
}

class page_idle_products : public QWidget
{
    Q_OBJECT

public:
    explicit page_idle_products(QWidget *parent = nullptr);
    void setPage(page_idle *pageIdle, page_select_product *page_select_product);

    // function body
    ~page_idle_products();

    // void select_product(int slot);
    void displayProducts();
    void addCustomerLogoToLabel(QLabel *label);
    void setBackgroundPictureFromTemplateToPage(QWidget *page, QString imageName);
    int setStepTimerFromFileName(QString fileName, int defaultTimeMillis);

    void printerStatusFeedback(bool isOnline, bool hasPaper);

    QLabel *labels_product_picture[4];
    QLabel *labels_selectProductOverlay[4];
    // QLabel* labels_product_name[4];
    QLabel *labels_product_icon[4];
    QLabel *labels_product_type[4];
    QLabel *labels_product_overlay_text[4];

private slots:
    // **** Navigation ****

    void on_pushButton_to_select_product_page_clicked();
    // void onBackgroundChangeTimerTimeout();
    void onBackgroundChangeTimerTick();

private:
    void showEvent(QShowEvent *event);
    void hideCurrentPageAndShowProvided(QWidget *pageToShow, bool createNewSessionId);
    
    void hideAllLabelAndButtons();
    void showAllLabelsAndButtons();
    void changeBackground();
    void displayPrinterStatus();

    void resizeEvent(QResizeEvent *event);

    QResizeEvent *productResize;
    Ui::page_idle_products *ui;
    page_idle *p_page_idle;
    page_select_product *p_page_select_product;
    page_idle_products *p_page_idle_products;

    int _backgroundChangeTimeLeftTenthsOfSec;
    int active_background_index;
    int maintenanceCounter;
    QTimer *backgroundChangeTimer;

    // bool m_printer_isOnline = true;
    // bool m_printer_hasPaper = true;
    // bool m_printer_status_visible;
};

#endif // page_idle_products_H