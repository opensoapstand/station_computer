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
    void updateVolumeDisplayed(int dispensed);
    void targetHitDisplay();

private slots:
    void on_backButton_clicked();
    void on_image_clicked();
    void on_nameButton_clicked();
    void on_priceButton_clicked();
    void on_target_volumeButton_clicked();
    void on_vol_per_tickButton_clicked();

private:
    Ui::maintain_product *ui;
    maintenancePage* maintenanceMode;
    idle* idlePage;

    void setValues(int option);
    bool pumping = false;
};

#endif // MAINTAIN_PRODUCT_H
