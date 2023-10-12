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
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef IDLE_H
#define IDLE_H

#include "page_select_product.h"
#include "page_idle_products.h"
#include "dfuicommthread.h"
#include "dbmanager.h"
#include "page_maintenance.h"
#include "product.h"
#include "machine.h"
#include "page_maintenance_general.h"
#include <QMediaPlayer>
#include <QGraphicsVideoItem>

class page_maintenance;
class page_select_product;
class page_maintenance_general;
class page_idle_products;
class page_error_wifi;

typedef enum StateFrozenScreenDetect{
    state_screen_check_not_initiated,
    state_screen_check_clicked_and_wait,
    state_screen_check_clicked_and_succes,
    state_screen_check_fail
}StateFrozenScreenDetect;

namespace Ui
{
    class page_idle;
}

class page_idle : public QWidget
{
    Q_OBJECT

public:
    void displayTemperature();
    explicit page_idle(QWidget *parent = nullptr);
    void setPage(page_select_product *p_page_select_product, page_maintenance *pageMaintenance, page_maintenance_general *pageMaintenanceGeneral, page_idle_products *p_page_idle_products, page_error_wifi *p_page_error_wifi);
    ~page_idle();
    void showEvent(QShowEvent *event);
    void changeToIdleProductsIfSet();
    void setMachine(machine* machine);


    void printerStatusFeedback(bool isOnline, bool hasPaper);

    // void MMSlot();
    machine* thisMachine;
    DfUiCommThread *dfComm;
    bool m_transitioning = false;

    QLabel *video_label;
    QVideoWidget *videoWidget;
    QMediaPlayer *player;

    QTimer *idlePageTypeSelectorTimer;
    int _idlePageTypeSelectorTimerTimeoutSec;
    QTimer *pollTemperatureTimer;
    int _pollTemperatureTimerTimeoutSec;
    QTimer *testForFrozenScreenTimer;
    int _testForFrozenScreenTimerTimeoutSec;

    void checkReceiptPrinterStatus();
    StateFrozenScreenDetect stateScreenCheck;
    void hideCurrentPageAndShowProvided(QWidget *pageToShow, bool createNewSessionId);

private slots:
    void on_pushButton_to_select_product_page_clicked();
    void onIdlePageTypeSelectorTimerTick();
    void onPollTemperatureTimerTick();
    void onTestForFrozenScreenTick();
    void on_pushButton_test_clicked();

private:
    Ui::page_idle *ui;
    page_select_product *p_pageSelectProduct;
    page_maintenance *p_page_maintenance;
    page_maintenance_general *p_page_maintenance_general;
    page_idle_products *p_page_idle_products;
    page_error_wifi *p_page_error_wifi;
    QString idle_page_type;
};

#endif // IDLE_H