//***************************************
//
// page_maintenance_dispenser.h
// GUI class while machine is processing
// payment.
//
// Coordinates User input from payment select
// class then communcates results to page_dispenser.
//
// created: 16-06-2023
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef PAGE_MAINTENANCE_DISPENSER_H
#define PAGE_MAINTENANCE_DISPENSER_H

#include <includefiles.h>
#include "df_util.h"

#include "dfuicommthread.h"
#include "page_maintenance.h"
#include "page_idle_products.h"
#include <curl/curl.h>
#include <ctime>

class page_maintenance;
class page_idle;
class page_idle_products;

namespace Ui
{
    class page_maintenance_dispenser;
}

class page_maintenance_dispenser : public QWidget
{
    Q_OBJECT

public:
    explicit page_maintenance_dispenser(QWidget *parent = nullptr);
    void setPage(page_maintenance *pageMaintenance, page_idle *pageIdle, page_idle_products *p_page_idle_products);
    ~page_maintenance_dispenser();
    void resizeEvent(QResizeEvent *event);
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    void fsmReceivedVolumeDispensed(double dispensed, bool isFull);
    void fsmReceiveTargetVolumeReached();
    void fsmReceiveDispenseButtonPressedPositiveEdge();
    void fsmReceiveDispenseButtonPressedNegativeEdge();
    void fsmReceiveDispenseRate(double flowrate);
    void fsmReceiveDispenserStatus(QString status);
    void fsmReceiveNoFlowAbort();
    void dispense_test_end(bool sendStopToController);
    void dispense_test_start();
    void update_volume_received_dispense_stats(double dispensed);
    void reset_all_dispense_stats();

private slots:
    void updateProductLabelValues(bool reloadFromDb);
    void on_pushButton_to_previous_page_clicked();
    void on_image_clicked();
    void on_pushButton_price_small_clicked();
    void on_pushButton_price_medium_clicked();
    void on_pushButton_price_large_clicked();
    void on_pushButton_price_custom_clicked();
    void on_pushButton_target_volume_small_clicked();
    void on_pushButton_target_volume_medium_clicked();
    void on_pushButton_target_volume_large_clicked();
    void on_pushButton_target_volume_custom_clicked();
    void on_pushButton_plu_small_clicked();
    void on_pushButton_plu_medium_clicked();
    void on_pushButton_plu_large_clicked();
    void on_pushButton_plu_custom_clicked();
    void on_pushButton_volume_per_tick_clicked();
    void on_pushButton_restock_clicked();
    void on_pushButton_set_status_clicked();
    void on_pushButton_set_restock_volume_clicked();
    void onMaintainProductPageTimeoutTick();
    void onDispenseTimerTick();
    void on_pushButton_set_volume_remaining_clicked();
    void on_pushButton_setting_temperature_clicked();
    void on_pushButton_setting_speed_pwm_clicked();
    void on_pushButton_enable_pump_clicked();
    void on_buttonBack_clicked();
    void on_buttonPeriod_clicked();
    void on_pushButton_done_clicked();
    void on_pushButton_cancel_clicked();

    void on_pushButton_auto_dispense_large_clicked();
    void on_pushButton_auto_dispense_medium_clicked();
    void on_pushButton_auto_dispense_small_clicked();

    void update_changes_to_portal();
    void buttonGroup_edit_product_Pressed(int buttonId);
    void buttonGroup_keypad_Pressed(int buttonId);
    

    void on_pushButton_clear_problem_clicked();

    void on_checkBox_enable_small_clicked();

    void on_checkBox_enable_medium_clicked();

    void on_checkBox_enable_large_clicked();

    void on_checkBox_enable_custom_clicked();

private:
    void setStatusTextLabel(QLabel* label, QString statusText, bool displayRawStatus);
    void setButtonPressCountLabel(bool init);
    void setButtonPressCountLabel2(bool init);

    void showEvent(QShowEvent *event);
    Ui::page_maintenance_dispenser *ui;
    page_maintenance *p_page_maintenance;
    page_idle *p_page_idle;

    bool pump_enabled = false;

    int _maintainProductPageTimeoutSec;
    float dispenserEnabledSecs;
    float dispenserPumpingSecs;
    QTimer *maintainProductPageEndTimer;
    QTimer *dispenseTimer;

    bool isDispenseButtonPressed;
    uint16_t button_press_count;

    QString units_selected_product;

    void sendRestockToCloud();
    void autoDispenseStart(int size);
    void restockTransactionToFile(char *curl_params);
    time_t rawtime;
    struct tm *timeinfo;
    CURL *curl;
    CURLcode res;
    CURL *curl2;
    CURLcode res2;
    std::string readBuffer;
    QByteArray curl_param_array;
    QByteArray curl_param_array2;
    char *curl_data;

    QString activeEditField = "";
};

#endif // PAGE_MAINTENANCE_DISPENSER_H
