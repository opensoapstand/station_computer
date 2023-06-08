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
#include "page_idle_products.h"
#include "dfuicommthread.h"
#include "dbmanager.h"
#include "page_maintenance.h"
#include "product.h"
#include "machine.h"
#include "page_maintenance_general.h"
#include <QMediaPlayer>
#include <QGraphicsVideoItem>

// #define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"
// #define DB_PATH_CLICKS "/release/db/sqlite/clicks.db"
// #define DB_PATH_TEMPERATURE "/release/db/sqlite/temperature.db"

class page_maintenance;
class page_select_product;
class page_maintenance_general;
class page_idle_products;
class page_error_wifi;

namespace Ui
{
    class page_idle;
}

class page_idle : public QWidget
{
    Q_OBJECT

public:
    explicit page_idle(QWidget *parent = nullptr);
    void setPage(page_select_product *p_page_select_product, page_maintenance *pageMaintenance, page_maintenance_general *pageMaintenanceGeneral, page_idle_products *p_page_idle_products, page_error_wifi *p_page_error_wifi);
    ~page_idle();
    void showEvent(QShowEvent *event);
    void addPictureToLabel(QLabel *label, QString picturePath);
    void addPictureToButton(QPushButton *button, QString picturePath);
    void addCompanyLogoToLabel(QLabel *label);
    // QString getTemplateFolder();
    void changeToIdleProductsIfSet();
    // void setTemplateFolder(QString rootPath, QString templateFolder);
    // QString getTemplatePathFromName(QString fileName);
    // QString getDefaultTemplatePathFromName(QString fileName);
    void setBackgroundPictureFromTemplateToPage(QWidget *page, QString imageName);
    void setBackgroundPictureToQWidget(QWidget *page, QString imageName);
    QString getCSS(QString cssName);
    void pageTransition(QWidget *pageToHide, QWidget *pageToShow);
    void loadDynamicContent();

    void setPromoCode(QString promoCode);
    QString getPromoCode();

    void setDiscountPercentage(double percentageFraction);
    double getDiscountPercentage();
    bool isPromoApplied();

    double getPriceCorrectedAfterDiscount(double price);

    void setSelectedProduct(uint8_t slot);
    product *getSelectedProduct();

    product products[SLOT_COUNT];
    product *selectedProduct;

    machine thisMachine;

    df_util *dfUtility;
    DfUiCommThread *dfComm;

    void printerStatusFeedback(bool isOnline, bool hasPaper);

    void MMSlot();
    bool m_transitioning = false;

    QLabel *video_label;
    QVideoWidget *videoWidget;
    QMediaPlayer *player;

    void addCssClassToObject(QWidget *element, QString classname, QString css_file_name);
    void setTemplateTextWithIdentifierToObject(QWidget *p_element, QString identifier);
    void setTemplateTextToObject(QWidget *p_element);
    void setTextToObject(QWidget *p_element, QString text);
    QString getTemplateTextByElementNameAndPage(QWidget *p_element);
    QString getTemplateTextByElementNameAndPageAndIdentifier(QWidget *p_element, QString identifier);
    QString getTemplateTextByPage(QWidget *page, QString identifier);
    QString getTemplateText(QString textName_to_find);
    void loadTextsFromTemplateCsv();
    void loadTextsFromDefaultCsv();

    void registerUserInteraction(QWidget *page);

    void loadTextsFromCsv(QString csv_path, std::map<QString, QString> *dictionary);

    QTimer *idlePageTypeSelectorTimer;
    int _idlePageTypeSelectorTimerTimeoutSec;

private slots:
    void on_pushButton_to_select_product_page_clicked();
    //  void on_pushButton_to_select_product_page_clicked();
    void onIdlePageTypeSelectorTimerTick();
    //    void on_savedBottles_label_clicked();

    void on_pushButton_test_clicked();

private:
    std::map<QString, QString> textNameToTextMap_template;
    std::map<QString, QString> textNameToTextMap_default;

    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    void checkReceiptPrinterStatus();
    QString m_templatePath;
    Ui::page_idle *ui;
    page_select_product *p_pageSelectProduct;
    page_maintenance *p_page_maintenance;
    page_maintenance_general *p_page_maintenance_general;
    page_idle_products *p_page_idle_products;
    page_error_wifi *p_page_error_wifi;
    bool p1, p2, p3, p4;
    QString idle_page_type;
    double m_discount_percentage_fraction = 0.0;
    QString m_promoCode;
};

#endif // IDLE_H