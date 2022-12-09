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

// #define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"
// #define DB_PATH_CLICKS "/release/db/sqlite/clicks.db"
// #define DB_PATH_TEMPERATURE "/release/db/sqlite/temperature.db"

class page_maintenance;
class page_select_product;
class page_maintenance_general;

namespace Ui {
class page_idle;
}

class page_idle : public QWidget
{
    Q_OBJECT

public:
    explicit page_idle(QWidget *parent = nullptr);
    void setPage(page_select_product *p_pageProduct, page_maintenance *pageMaintenance, page_maintenance_general *pageMaintenanceGeneral);
    
    ~page_idle();
    void showEvent(QShowEvent *event);
    void addPictureToLabel(QLabel* label, QString picturePath);
    void addCompanyLogoToLabel(QLabel *label);
    QString getTemplateFolder();
    void setTemplateFolder(QString rootPath, QString templateFolder);
    QString getTemplatePathFromName(QString backgroundPictureName);
    QString getDefaultTemplatePathFromName(QString backgroundPictureName);
    void setBackgroundPictureFromTemplateToPage(QWidget* page, QString imageName );
    void pageTransition(QWidget* pageToHide, QWidget* pageToShow);

    DrinkOrder* currentProductOrder;
    df_util* dfUtility;
    

    DfUiCommThread* dfComm;

    // bool isSlotAvailable(int slot);
    // void setSlotAvailability(int slot, bool isEnabled);
    void printerStatusFeedback(bool isOnline, bool hasPaper);


    bool isEnough(int p);
    void MMSlot();
    bool m_transitioning = false;
    // bool slotIndexAvailable[4] = {true,true,true,true}; //;1,2,3,4

private slots:
    void on_toSelectProductPageButton_clicked();
//    void on_savedBottles_label_clicked();

    void on_testButton_clicked();

private:
    void checkReceiptPrinterStatus();
    QString m_templatePath;
    Ui::page_idle *ui;
    page_select_product* p_pageSelectProduct;
    page_maintenance* p_page_maintenance;
    page_maintenance_general* p_page_maintenance_general;
    bool p1, p2, p3, p4;

};

#endif // IDLE_H
