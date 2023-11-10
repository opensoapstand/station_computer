//***************************************
//
// page_maintenance_general.h
// GUI class while machine is processing
// payment.
//
// Coordinates User input from payment select
// class then communcates results to page_dispenser.
//
// created: 16-06-2023
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef PAGE_MAINTENANCE_GENERAL_H
#define PAGE_MAINTENANCE_GENERAL_H

#include <includefiles.h>
#include "df_util.h"

#include "dfuicommthread.h"
#include "page_maintenance.h"
#include <curl/curl.h>
#include <ctime>

class statusbar;
class page_maintenance;
class page_idle;
class page_idle_products;

namespace Ui
{
    class page_maintenance_general;
}

class page_maintenance_general : public QWidget
{
    Q_OBJECT

public:
    explicit page_maintenance_general(QWidget *parent = nullptr);
    void setPage(page_maintenance *pageMaintenance, page_idle *pageIdle, page_idle_products *p_page_idle_products, statusbar *p_statusbar);    
    ~page_maintenance_general();
    void resizeEvent(QResizeEvent *event);
    void on_pushButton_to_previous_page_clicked();
    void updateProductLabelValues();
    void printerStatusFeedback(bool isOnline, bool hasPaper);
    void updateLabelValues();

    void on_pushButton_printer_check_status_clicked();
    void on_printer_test_print_button_clicked();
    void send_check_printer_status_command();
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);

private slots:

    void on_pushButton_back_clicked();

    void on_pushButton_minimize_clicked();

    void on_pushButton_reboot_clicked();

    void on_pushButton_shutdown_clicked();

    void on_checkBox_enable_pump_ramping_clicked(bool checked);
    void on_checkBox_enable_empty_container_clicked(bool checked);

    void on_test_lineEdit_textChanged(const QString &arg1);

    void button_connect_to_specifiic_wifi_network();
    void keyboardButtonPressed(int);
    // void on_wifiButton_clicked();
    // void onPage_maintenanceTimeoutTick();

    // void on_pushButton_printer_check_status_clicked();

    void on_pushButton_wifi_networks_clicked();

    void on_pushButton_rtunnel_restart_clicked();

    void on_pushButton_network_status_clicked();

    void on_pushButton_restart_UI_clicked();

    void on_pushButton_restart_electronics_clicked();

private:
    void showEvent(QShowEvent *event);
    Ui::page_maintenance_general *ui;
    page_maintenance *p_page_maintenance;
    page_idle *p_page_idle;
    statusbar *p_statusbar;

    int _maintenanceGeneralPageTimeoutSec;
    QTimer *maintenanceGeneralPageEndTimer;
    QVBoxLayout *statusbarLayout;

};

#endif // PAGE_MAINTENANCE_GENERAL_H
