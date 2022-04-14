//***************************************
//
// page_select_product.h
// GUI class for user to browse up to
// 6 drinks.
//
// Allows navigation to First product page
// payment selection page
//
// created: 05-04-2022
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef page_select_product_H
#define page_select_product_H

#include "df_util.h"
#include "page_maintenance.h"
#include "page_help.h"

class pageProduct;
class page_idle;
class page_maintenance;
class page_help;

namespace Ui {
class page_select_product;
}

class page_select_product : public QWidget
{
    Q_OBJECT

public:
    explicit page_select_product(QWidget *parent = nullptr);
    void setPage(pageProduct *pageSizeSelect, page_idle* pageIdle, page_maintenance *pageMaintenance, page_help *pageHelp);
    ~page_select_product();
    void cancelTimers();
    void select_product(int slot);
    void displayProducts();
    

    QPushButton* selectProductButtons[4];
    QLabel* selectProductPhotoLabels[4];
    QLabel* selectProductOverlayLabels[4];
    QLabel* selectProductNameLabels[4];
    QLabel* selectProductIconLabels[4];
    QLabel* selectProductTypeLabels[4];
    QLabel* selectProductPhotoLabelsText[4];
    
private slots:
    // **** Navigation ****
    //void on_nextPageButton_clicked();
    // HACK: Could have better way for modular reference to buttons and drink orders...
    void on_selection1_Button_clicked();
    void on_selection2_Button_clicked();
    void on_selection3_Button_clicked();
    void on_selection4_Button_clicked();
    //void on_selection5_Button_clicked();
    //void on_selection6_Button_clicked();
    void mainPage();
    void onProductPageTimeoutTick();
    //void on_backButton_clicked();
    void on_p_page_maintenanceButton_pressed();
    void on_mainPage_Button_clicked();
    void on_helpPage_Button_clicked();

private:
    void showEvent(QShowEvent *event);
    void resizeEvent(QResizeEvent *event);
    Ui::page_select_product *ui;
    // productPage_2 *selection_PageTwo;
    pageProduct *p_page_product;
    page_idle* p_page_idle;

    QResizeEvent *productResize;

    int _productPageTimeoutSec;
    QTimer* productPageEndTimer;

    page_maintenance* p_page_maintenance;

    page_help* helpPage;

    int maintenanceCounter;
};

#endif // page_select_product_H
