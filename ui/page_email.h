//***************************************
//
// page_email.h
// GUI class for user to see Drinkfill
// page_help and contact info
//
// created: 16-06-2023
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef PAGE_EMAIL_H
#define PAGE_EMAIL_H

#include "df_util.h"
#include "page_dispenser.h"
#include "page_idle.h"
#include "dbmanager.h"
#include "page_help.h"

class page_dispenser;
class page_idle;
class page_help;
class page_product_overview;

namespace Ui {
class page_email;
}

class page_email : public QWidget
{
    Q_OBJECT

public:
    explicit page_email(QWidget *parent = nullptr);

    void setPage(page_dispenser *page_dispenser, page_idle* pageIdle, page_help *p_page_help, page_product_overview *p_page_product_overview);
    ~page_email();
    QTimer* helpIdleTimer;

    void hideCurrentPageAndShowProvided(QWidget *pageToShow);

private slots:
    // **** Navigation ****
    // void on_pushButton_previous_page_clicked();
    void on_pushButton_to_product_overview_clicked();
    void on_pushButton_resetTimeout_clicked();
    void onHelpTimeoutTick();
    void on_pushButton_continue_clicked();
    void on_pushButton_to_help_clicked();
    // void on_pushButton_to_transactions_clicked();

    void keyboardButtonPressed(int);

    // void on_pushButton_to_maintenance_clicked();

    // void on_pushButton_to_feedback_clicked();
    bool emailValid(QString email);
private:
    void showEvent(QShowEvent *event);

    Ui::page_email *ui;
    page_dispenser* p_page_dispenser;
    page_help* p_page_help;
    page_idle* p_page_idle;
    page_product_overview* p_page_product_overview;

    int _helpIdleTimeoutSec;

    QResizeEvent *helpResize;
    QString maintenance_pwd;
    QString help_text_html;
    bool m_hasStartedTyping=false;
};

#endif // PAGE_EMAIL_H
