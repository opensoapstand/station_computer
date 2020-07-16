//***************************************
//
// payselect.h
// GUI class for user to select size and
// payment for drink.
//
// Allows navigation to First product page
// payment page and idle page
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef PAYSELECT_H
#define PAYSELECT_H

#include <QWidget>

class productPage_1;
class payPage;
class idle;

namespace Ui {
class paySelect;
}

class paySelect : public QWidget
{
    Q_OBJECT

public:
    explicit paySelect(QWidget *parent = nullptr);
    void setPage(productPage_1 *pageSelect, payPage *pagePayment, idle* pageIdle);
    ~paySelect();

private slots:
    void on_previousPage_Button_clicked();

    void on_payPage_Button_clicked();

    void on_mainPage_Button_clicked();

private:
    Ui::paySelect *ui;

    productPage_1* firstProductPage;
    payPage* paymentPage;
    idle* idlePage;
};

#endif // PAYSELECT_H
