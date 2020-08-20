//***************************************
//
// productpage_1.h
// GUI class for user to browse up to
// 6 drinks.
//
// Allows navigation to First product page
// payment selection page
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef PRODUCTPAGE_1_H
#define PRODUCTPAGE_1_H

#include "df_util.h"

class productPage_2;
class paySelect;
class idle;

namespace Ui {
class productPage_1;
}

class productPage_1 : public QWidget
{
    Q_OBJECT

public:
    explicit productPage_1(QWidget *parent = nullptr);
    void setPage(productPage_2 *pageTwoProducts, paySelect *pageSizeSelect, idle* pageIdle);
    ~productPage_1();

private slots:
    // **** Navigation ****
    void on_nextPageButton_clicked();
    // HACK: Could have better way for modular reference to buttons and drink orders...
    void on_selection1_Button_clicked();
    void on_selection2_Button_clicked();
    void on_selection3_Button_clicked();
    void on_selection4_Button_clicked();
    void on_selection5_Button_clicked();
    void on_selection6_Button_clicked();

private:
    Ui::productPage_1 *ui;
    productPage_2 *selection_PageTwo;
    paySelect *paymentSelectPage;
    idle* idlePage;
};

#endif // PRODUCTPAGE_1_H
