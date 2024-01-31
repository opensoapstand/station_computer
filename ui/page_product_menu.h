//***************************************
//
// page_product_menu.h
// GUI class for user to browse up to
// 6 drinks.
//
// Allows navigation to First product page
// payment selection page
//
// created: 05-04-2022
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef page_product_menu_H
#define page_product_menu_H

#include "df_util.h"
#include "page_maintenance.h"
#include "page_help.h"

class page_product;
class page_product_mixing;
class page_idle;
class page_maintenance;
class page_help;
class page_idle_products;
class statusbar;

namespace Ui
{
    class page_product_menu;
}

class page_product_menu : public QWidget
{
    Q_OBJECT

public:
    explicit page_product_menu(QWidget *parent = nullptr);
    void setPage(page_product *p_page_product, page_product_mixing *p_page_product_mixing, page_idle_products *p_page_idle_products, page_idle *pageIdle, page_maintenance *pageMaintenance, page_help *pageHelp, statusbar *p_statusbar);
    ~page_product_menu();

    void select_submenu_option(int position);
    void displayProducts();

    QLabel *labels_base_product_bg[4];
    QPushButton *pushButtons_base_product[4];
    QLabel *labels_base_product_picture[4];
    QLabel *labels_base_product_name[4];

    QPushButton *pushButtons_dispense_product[6];
    QLabel *labels_dispense_product_picture[6];
    QLabel *labels_dispense_product_name[6];

    QPushButton *pushButtons_product_select[4];
    QLabel *labels_product_picture[4];
    QLabel *labels_selectProductOverlay[4];
    QLabel *labels_product_name[4];
    QLabel *labels_product_icon[4];
    QLabel *labels_product_type[4];
    QLabel *labels_product_overlay_text[4];

private slots:
    // **** Navigation ****
    // void on_nextPageButton_clicked();
    void on_pushButton_base_product_1_clicked();
    void on_pushButton_base_product_2_clicked();
    void on_pushButton_base_product_3_clicked();
    void on_pushButton_base_product_4_clicked();

    // HACK: Could have better way for modular reference to buttons and drink orders...
    // void on_pushButton_selection1_clicked();
    // void on_pushButton_selection2_clicked();
    // void on_pushButton_selection3_clicked();
    // void on_pushButton_selection4_clicked();
    // void on_pushButton_selection5_clicked();
    // void on_pushButton_selection6_clicked();

    void onProductPageTimeoutTick();
    // void on_backButton_clicked();
    //  void on_pushButton_to_maintenance_pressed();
    void on_pushButton_to_idle_clicked();
    void on_pushButton_help_page_clicked();

    void on_pushButton_dispense_product_1_clicked();

    void on_pushButton_dispense_product_2_clicked();

    void on_pushButton_dispense_product_3_clicked();

    void on_pushButton_dispense_product_4_clicked();

    void on_pushButton_dispense_product_5_clicked();

    void on_pushButton_dispense_product_6_clicked();

private:
    void showEvent(QShowEvent *event);
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);
    void resizeEvent(QResizeEvent *event);
    void select_base_product_in_menu(int base_product_index);
    void displayDispenseProductsMenu();
    Ui::page_product_menu *ui;
    // productPage_2 *selection_PageTwo;
    page_product *p_page_product;
    page_product_mixing *p_page_product_mixing;
    page_idle *p_page_idle;
    page_maintenance *p_page_maintenance;
    page_help *p_page_help;
    statusbar *p_statusbar;

    QResizeEvent *productResize;
    int _productPageTimeoutSec;
    QTimer *productPageEndTimer;

    int m_selectedBaseProductIndex; // 0->slot count-1.
    int maintenanceCounter;
    QVBoxLayout *statusbarLayout;
};

#endif // page_product_menu_H
