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
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef page_select_product_H
#define page_select_product_H

#include "df_util.h"
#include "page_maintenance.h"
#include "page_help.h"

class page_product;
class page_idle;
class page_maintenance;
class page_help;
class page_idle_products;
class statusbar;

namespace Ui {
class page_select_product;
}

class page_select_product : public QWidget
{
    Q_OBJECT

public:
    explicit page_select_product(QWidget *parent = nullptr);
    void setPage(page_product *p_page_product, page_idle_products *p_page_idle_products, page_idle *pageIdle, page_maintenance *pageMaintenance, page_help *pageHelp, statusbar *p_statusbar);
    ~page_select_product();
    
    void select_product(int slot);
    void displayProducts();
    

    QPushButton* pushButtons_product_select[4];
    QLabel* labels_product_picture[4];
    QLabel* labels_selectProductOverlay[4];
    QLabel* labels_product_name[4];
    QLabel* labels_product_icon[4];
    QLabel* labels_product_type[4];
    QLabel* labels_product_overlay_text[4];
    
private slots:
    // **** Navigation ****
    //void on_nextPageButton_clicked();
    // HACK: Could have better way for modular reference to buttons and drink orders...
    void on_pushButton_selection1_clicked();
    void on_pushButton_selection2_clicked();
    void on_pushButton_selection3_clicked();
    void on_pushButton_selection4_clicked();
    //void on_pushButton_selection5_clicked();
    //void on_pushButton_selection6_clicked();
    
    void onProductPageTimeoutTick();
    //void on_backButton_clicked();
    void on_pushButton_to_maintenance_pressed();
    void on_pushButton_to_idle_clicked();
    void on_pushButton_help_page_clicked();

private:
    void showEvent(QShowEvent *event);
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    void resizeEvent(QResizeEvent *event);
    Ui::page_select_product *ui;
    // productPage_2 *selection_PageTwo;
    page_product *p_page_product;
    page_idle* p_page_idle;
    page_maintenance* p_page_maintenance;
    page_help* p_page_help;
    statusbar* p_statusbar;

    QResizeEvent *productResize;
    int _productPageTimeoutSec;
    QTimer* productPageEndTimer;

    int maintenanceCounter;
    QVBoxLayout *statusbarLayout; 
};

#endif // page_select_product_H
