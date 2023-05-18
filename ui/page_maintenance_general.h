//***************************************
//
// page_maintenance_general.h
// GUI class while machine is processing
// payment.
//
// Coordinates User input from payment select
// class then communcates results to page_dispenser.
//
// created: 4-01-2021
// by: Paddy Riley
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef PAGE_MAINTENANCE_GENERAL_H
#define PAGE_MAINTENANCE_GENERAL_H

#include <includefiles.h>
#include "df_util.h"

#include "dfuicommthread.h"
#include "page_maintenance.h"
#include <curl/curl.h>
#include <ctime>

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
    void setPage(page_maintenance *pageMaintenance, page_idle *pageIdle, page_idle_products *p_page_idle_products);    
    ~page_maintenance_general();
    void resizeEvent(QResizeEvent *event);
    void on_backButton_clicked();
    void refreshLabels();
    void printerStatusFeedback(bool isOnline, bool hasPaper);

    void on_printer_check_status_clicked();
    void on_printer_test_button_clicked();
    void send_check_printer_status_command();
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);

private slots:

    void on_back_Button_clicked();

    void on_minimize_Button_clicked();

    void on_reboot_Button_clicked();

    void on_shutdown_Button_clicked();

    void on_enable_pump_ramping_checkBox_clicked(bool checked);
    void on_enable_empty_container_checkBox_clicked(bool checked);

    void on_test_lineEdit_textChanged(const QString &arg1);

    void btn_clicked();
    void keyboardButtonPressed(int);
    void on_wifiButton_clicked();
    // void onPage_maintenanceTimeoutTick();

    void on_printer_check_status_button_clicked();

    void on_printer_test_print_button_clicked();

    void on_rtunnel_restart_Button_clicked();

    void on_network_status_Button_clicked();

    void on_restart_UI_Button_clicked();

    void on_restart_electronics_Button_clicked();

private:
    void showEvent(QShowEvent *event);
    Ui::page_maintenance_general *ui;
    page_maintenance *p_page_maintenance;
    page_idle *p_page_idle;

    int _maintenanceGeneralPageTimeoutSec;
    QTimer *maintenanceGeneralPageEndTimer;
};

#endif // PAGE_MAINTENANCE_GENERAL_H
