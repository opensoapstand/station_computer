//***************************************
//
// maintain_product.h
// GUI class while machine is processing
// payment.
//
// Coordinates User input from payment select
// class then communcates results to dispensepage.
//
// created: 4-01-2021
// by: Paddy Riley
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef MAINTAIN_PRODUCT_H
#define MAINTAIN_PRODUCT_H

#include <includefiles.h>
#include "df_util.h"
#include "drinkorder.h"
#include "dfuicommthread.h"
#include "maintenancePage.h"
#include <curl/curl.h>
#include <ctime>

class maintenancePage;
class idle;

namespace Ui {
class maintain_product;
}


class maintain_product : public QWidget
{
    Q_OBJECT


public:
    explicit maintain_product(QWidget *parent = nullptr);
    void setPage(maintenancePage* pageMaintenance, idle* pageIdle);
    ~maintain_product();
    void resizeEvent(QResizeEvent *event);
    void updateVolumeDisplayed(double dispensed);
    void targetHitDisplay();

private slots:
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

private:
    void showEvent(QShowEvent *event);
    Ui::maintain_product *ui;
    maintenancePage* maintenanceMode;
    idle* idlePage;

    void setValues(int option);
    bool pumping = false;

    int _maintainProductPageTimeoutSec;
    QTimer* maintainProductPageEndTimer;

    QString text_entered;
    bool price_s;
    bool price_l;
    bool target_s;
    bool target_l;
    bool vol_per_tick;
    bool full;
    bool pwm;
    bool buffer;
//    bool plu_s;
//    bool plu_l;

    double ticks;

    void updateValues();
    void curler();
    void bufferCURL(char *curl_params);
    time_t rawtime;
    struct tm * timeinfo;
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    QByteArray curl_param_array;
    char * curl_data;
};

#endif // MAINTAIN_PRODUCT_H
