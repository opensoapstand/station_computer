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
#include "page_payment_tap_tcp.h"


class statusbar;
class page_maintenance;
class page_idle;
class page_idle_products;
class keyboard;
class input_widget;

namespace Ui
{
    class page_maintenance_general;
}

class page_maintenance_general : public QWidget
{
    Q_OBJECT

public:
    explicit page_maintenance_general(QWidget *parent = nullptr);
    void setPage(page_maintenance *pageMaintenance, page_idle *pageIdle, page_idle_products *p_page_idle_products, statusbar *p_statusbar, keyboard * keyboard, input_widget * input_widget);    
    ~page_maintenance_general();
    void resizeEvent(QResizeEvent *event);
    void on_pushButton_to_previous_page_clicked();
    void updateProductLabelValues();
    void printerStatusFeedback(bool isOnline, bool hasPaper);
    void updateLabelValues();
    static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);
    bool ping_url(const std::string& url);
    void checkSoapstandPortal();
    // void on_pushButton_printer_check_status_clicked();
    void send_check_printer_status_command();
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);

private slots:
    void onPage_maintenance_general_TimeoutTick();

    void on_pushButton_back_clicked();

    void on_pushButton_minimize_clicked();

    void on_pushButton_reboot_clicked();

    void on_pushButton_shutdown_clicked();
    void on_pushButton_check_portal_clicked();
    
    void on_checkBox_enable_empty_container_clicked(bool checked);

    void on_test_lineEdit_textChanged(const QString &arg1);

    void button_connect_to_specifiic_wifi_network();
    void doneButtonPressed();
    void cancelButtonPressed();
    void keyboardButtonPressed(int);
    // void on_wifiButton_clicked();
    // void onPage_maintenanceTimeoutTick();

    void on_pushButton_printer_check_status_clicked();

    void on_pushButton_tap_check_status_clicked();
    void on_pushButton_tap_authorize_clicked();
    void on_pushButton_tap_cancel_clicked();
    void on_pushButton_wifi_networks_clicked();

    void on_pushButton_rtunnel_restart_clicked();

    void on_pushButton_network_status_clicked();

    void on_pushButton_restart_UI_clicked();

    void on_pushButton_restart_electronics_clicked();

    void on_checkBox_enable_pcb_3point3V_clicked(bool checked);

    void on_pushButton_printer_test_print_clicked();

private:
    void showEvent(QShowEvent *event);
    std::string readBuffer;
    Ui::page_maintenance_general *ui;
    page_maintenance *p_page_maintenance;
    page_idle *p_page_idle;
    statusbar *p_statusbar;
    keyboard *p_keyboard;
    input_widget *p_input_widget;
    int _maintenanceGeneralPageTimeoutSec;
    QTimer *maintenanceGeneralPageEndTimer;
    QVBoxLayout *statusbarLayout;

};

#endif // PAGE_MAINTENANCE_GENERAL_H
