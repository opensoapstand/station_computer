//***************************************
//
// page_idle.h
// GUI class while machine is idle.
//
// Display Fullscreen DF branded Wallpaper
// Listen for User interaction to load
// Product Page1
//
// created: 05-04-2022
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef IDLE_H
#define IDLE_H

#include "df_util.h"
#include "page_select_product.h"
#include "dfuicommthread.h"
#include "dbmanager.h"
#include "page_maintenance.h"
#include "page_maintenance_general.h"
#include "page_idle_products.h"
#include "product.h"
#include <QMediaPlayer>
#include <QGraphicsVideoItem>

// #define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"
// #define DB_PATH_CLICKS "/release/db/sqlite/clicks.db"
// #define DB_PATH_TEMPERATURE "/release/db/sqlite/temperature.db"

class page_maintenance;
class page_select_product;
class page_maintenance_general;
class page_idle_products;

namespace Ui
{
    class page_idle;
}

class page_idle : public QWidget
{
    Q_OBJECT

public:
    explicit page_idle(QWidget *parent = nullptr);
    void setPage(page_select_product *p_pageProduct, page_maintenance *pageMaintenance, page_maintenance_general *pageMaintenanceGeneral, page_idle_products *p_page_idle_products);
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    ~page_idle();
    void setSelectedProduct(uint8_t slot);
    void showEvent(QShowEvent *event);
    void addPictureToLabel(QLabel *label, QString picturePath);
    void addCompanyLogoToLabel(QLabel *label);
    QString getTemplateFolder();
    void setTemplateFolder(QString rootPath, QString templateFolder);
    QString getTemplatePathFromName(QString fileName);
    QString getDefaultTemplatePathFromName(QString fileName);
    void setBackgroundPictureFromTemplateToPage(QWidget *page, QString imageName);
    QString getCSS(QString cssName);
    void pageTransition(QWidget *pageToHide, QWidget *pageToShow);

    DrinkOrder *currentProductOrder;
    // Product [PRODUCTS_COUNT] products;
    // Product* selectedProduct;

    df_util *dfUtility;
   //for products.cpp
   // product products[SLOT_COUNT]; // declare products as a member variable
    // product* getSelectedProduct();
    

    DfUiCommThread *dfComm;

    // bool isSlotAvailable(int slot);
    // void setSlotAvailability(int slot, bool isEnabled);
    void printerStatusFeedback(bool isOnline, bool hasPaper);

    bool isEnough(int p);
    // void MMSlot();
    bool m_transitioning = false;
    // bool slotIndexAvailable[4] = {true,true,true,true}; //;1,2,3,4

    QLabel *video_label;
    QVideoWidget *videoWidget;
    QMediaPlayer *player;

private slots:
    void on_toSelectProductPageButton_clicked();
    //    void on_savedBottles_label_clicked();

    void on_testButton_clicked();

private:
    void checkReceiptPrinterStatus();
    QString m_templatePath;
    QString idle_page_type;
    // for products.cpp
    // product* selectedProduct;

    Ui::page_idle *ui;
    page_select_product *p_pageSelectProduct;
    page_maintenance *p_page_maintenance;
    page_maintenance_general *p_page_maintenance_general;
    page_idle_products *p_page_idle_products;

    bool p1, p2, p3, p4;
};

#endif // IDLE_H
