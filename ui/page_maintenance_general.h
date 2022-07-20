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
#include "drinkorder.h"
#include "dfuicommthread.h"
#include "page_maintenance.h"
#include <curl/curl.h>
#include <ctime>

class page_maintenance;
class page_idle;

namespace Ui {
class page_maintenance_general;
}


class page_maintenance_general : public QWidget
{
    Q_OBJECT


public:
    explicit page_maintenance_general(QWidget *parent = nullptr);
    void setPage(page_maintenance* pageMaintenance, page_idle* pageIdle);
    ~page_maintenance_general();
    void resizeEvent(QResizeEvent *event);
    void on_backButton_clicked();
    void refreshLabels();
  

private slots:

private:
    void showEvent(QShowEvent *event);
    Ui::page_maintenance_general *ui;
    page_maintenance* p_page_maintenance;
    page_idle* p_page_idle;


    int _maintenanceGeneralPageTimeoutSec;
    QTimer* maintenanceGeneralPageEndTimer;
};

#endif // PAGE_MAINTENANCE_GENERAL_H
