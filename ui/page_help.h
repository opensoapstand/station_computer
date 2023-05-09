//***************************************
//
// page_help.h
// GUI class for user to see Drinkfill
// page_help and contact info
//
// created: 28-05-2021
// by: Paddy Riley
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef HELP_H
#define HELP_H

#include "df_util.h"
#include "page_select_product.h"
#include "page_product.h"
#include "page_transactions.h"
#include "page_qr_payment.h"
#include "page_idle.h"
#include "page_maintenance.h"
#include "dbmanager.h"
#include "page_sendFeedback.h"

class page_select_product;
class page_qr_payment;
class page_idle;
class pageProduct;
class page_transactions;
class page_maintenance;

namespace Ui {
class page_help;
}

class page_help : public QWidget
{
    Q_OBJECT

public:
    explicit page_help(QWidget *parent = nullptr);
    void setPage(page_select_product *pageSelect, pageProduct* pageProduct, page_idle* pageIdle, page_qr_payment *page_qr_payment, page_transactions *pageTransactions, page_maintenance* pageMaintenance, page_sendFeedback *pageFeedback);
    ~page_help();
    QTimer* helpIdleTimer;

    void hideCurrentPageAndShowProvided(QWidget *pageToShow);

private slots:
    // **** Navigation ****
    // void on_previousPage_Button_clicked();
    void on_previousPage_Button_2_clicked();
    void on_refreshButton_clicked();
    void onHelpTimeoutTick();
    

    void on_transactions_Button_clicked();

    void keyboardButtonPressed(int);

    void on_maintenance_page_Button_clicked();

    void on_feedback_Button_clicked();

private:
    void showEvent(QShowEvent *event);

    Ui::page_help *ui;
    page_select_product* p_page_select_product;
    page_qr_payment* paymentPage;
    page_idle* p_page_idle;
    page_sendFeedback* p_page_feedback;
    pageProduct* selectPage;
    page_transactions* p_page_transactions;
    page_maintenance* p_page_maintenance;

    int _helpIdleTimeoutSec;

    QResizeEvent *helpResize;
    QString maintenance_pwd;
    QString help_text_html;
};

#endif // HELP_H
