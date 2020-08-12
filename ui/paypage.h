//***************************************
//
// paypage.h
// GUI class while machine is processing
// payment.
//
// Coordinates User input from payment select
// class then communcates results to dispensepage.
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef PAYPAGE_H
#define PAYPAGE_H

#include "df_util.h"
#include "drinkorder.h"

class paySelect;
class dispensePage;
class idle;

namespace Ui {
class payPage;
}

class payPage : public QWidget
{
    Q_OBJECT

public:
    explicit payPage(QWidget *parent = nullptr);
    void setPage(paySelect* pageSizeSelect, dispensePage* pageDispense, idle* pageIdle);
    ~payPage();

private slots:
    void updateTotals(string drinkDescription, string drinkAmount, string orderTotal);

    void on_previousPage_Button_clicked();
    void on_passPayment_Button_clicked();

    /*
     * Successful payment -> on_passPayment_Button_clicked()
     * Failed payment -> on_previous_Button_clicked()
     */
    void on_mainPage_Button_clicked();

private:
    Ui::payPage *ui;
    paySelect* paySelectPage;
    dispensePage* dispensingPage;
    idle* idlePage;

    DrinkOrder paymentDrinkOrder;

    string drinkAmount;
    string drinkDescription;
    string orderTotal;
};

#endif // PAYPAGE_H
