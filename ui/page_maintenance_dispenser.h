//***************************************
//
// page_maintenance_dispenser.h
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

#ifndef PAGE_MAINTENANCE_DISPENSER_H
#define PAGE_MAINTENANCE_DISPENSER_H

#include <includefiles.h>
#include "df_util.h"
#include "drinkorder.h"
#include "dfuicommthread.h"
#include "page_maintenance.h"
#include <curl/curl.h>
#include <ctime>

class page_maintenance;
class page_idle;

namespace Ui
{
    class page_maintenance_dispenser;
}

class page_maintenance_dispenser : public QWidget
{
    Q_OBJECT

public:
    explicit page_maintenance_dispenser(QWidget *parent = nullptr);
    void setPage(page_maintenance *pageMaintenance, page_idle *pageIdle);
    ~page_maintenance_dispenser();
    void resizeEvent(QResizeEvent *event);
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    void updateVolumeDisplayed(double dispensed, bool isFull);
    void fsmReceiveTargetVolumeReached();
    void fsmReceiveDispenseButtonPressed();
    void fsmReceiveNoFlowAbort();
    void setSoldOutButtonText();
    void dispense_test_end(bool sendStopToController);
    void dispense_test_start();
    void update_dispense_stats(double dispensed);
    DrinkOrder *selectedProductOrder;

private slots:
    void refreshLabels();
    void on_backButton_clicked();
    void on_image_clicked();
    void on_nameButton_clicked();
    void on_priceButton_s_clicked();
    void on_priceButton_l_clicked();
    void on_target_volumeButton_s_clicked();
    void on_target_volumeButton_l_clicked();
    void on_vol_per_tickButton_clicked();
    void on_refillButton_clicked();
    void on_soldOutButton_clicked();
    void on_fullButton_clicked();
    void onMaintainProductPageTimeoutTick();
    void onDispenseTimerTick();
    void on_remainingButton_clicked();
    void on_dispensedButton_clicked();
    void on_lastRefillButton_clicked();
    void on_temperatureButton_clicked();
    void on_pwmButton_clicked();
    void on_pumpButton_clicked();
    //    void on_testSmallButton_clicked();
    //    void on_testLargeButton_clicked();
    //    void on_pluButton_clicked();
    //    void on_numberEntry_buttonClicked();
    void on_button1_clicked();
    void on_button2_clicked();
    void on_button3_clicked();
    void on_button4_clicked();
    void on_button5_clicked();
    void on_button6_clicked();
    void on_button7_clicked();
    void on_button8_clicked();
    void on_button9_clicked();
    void on_button0_clicked();
    void on_buttonBack_clicked();
    void on_buttonPoint_clicked();
    void on_buttonDone_clicked();
    void on_buttonCancel_clicked();

    void pwmSliderMoved(int percentage);

    void on_autoDispenseLarge_clicked();

    void on_autoDispenseMedium_clicked();

    void on_autoDispenseSmallButton_clicked();

private:
    void setButtonPressCountLabel(bool init);
    void showEvent(QShowEvent *event);
    Ui::page_maintenance_dispenser *ui;
    page_maintenance *p_page_maintenance;
    page_idle *p_page_idle;

    bool pumping = false;

    int _maintainProductPageTimeoutSec;
    float dispenseTimeSecs;
    QTimer *maintainProductPageEndTimer;
    QTimer *dispenseTimer;

    QString text_entered;
    bool price_small;
    bool price_large;
    bool target_s;
    bool target_l;
    bool vol_per_tick;
    bool full;
    bool pwm;
    bool buffer;

    uint16_t button_press_count;

    QString units_selected_product;
    //    bool plu_s;
    //    bool plu_l;

    double volume_per_tick_buffer;

    void updateValues();
    void sendRestockToCloud();
    void autoDispenseStart(int size);
    void restockTransactionToFile(char *curl_params);
    time_t rawtime;
    struct tm *timeinfo;
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    QByteArray curl_param_array;
    char *curl_data;
};

#endif // PAGE_MAINTENANCE_DISPENSER_H
