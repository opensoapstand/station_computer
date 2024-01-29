#ifndef PAGE_HOW_TO_H
#define PAGE_HOW_TO_H

#include "df_util.h"
#include "input_widget.h"
#include "page_help.h"
#include "page_select_product.h"
#include "page_product.h"
#include "page_product_mixing.h"
#include "page_transactions.h"
#include "page_qr_payment.h"
#include "page_idle.h"
#include "page_maintenance.h"
#include "dbmanager.h"
#include "page_sendFeedback.h"
#include "page_how_to.h"

class page_help;
class page_idle;
class page_transactions;
class page_maintenance;
class statusbar;
class keyboard;
class input_widget;
class page_how_to;

namespace Ui {
class page_how_to;
}

class page_how_to : public QWidget
{
    Q_OBJECT

public:
    explicit page_how_to(QWidget *parent = 0);
    ~page_how_to();
    void setPage(page_help *pageHelp, page_idle* pageIdle, page_transactions *pageTransactions, page_maintenance* pageMaintenance, page_sendFeedback *pageFeedback, statusbar *p_statusbar, keyboard * keyboard, input_widget * input_widget);
    QTimer* helpIdleTimer;
    void hideCurrentPageAndShowProvided(QWidget *pageToShow);

private slots:
    void on_pushButton_to_help_clicked();
    void on_pushButton_resetTimeout_clicked();
    void onHelpTimeoutTick();
    void doneButtonPressed();
    void on_pushButton_to_maintenance_clicked();
    void on_pushButton_to_feedback_clicked();


private:
    void showEvent(QShowEvent *event);
    Ui::page_how_to *ui;
    page_help *p_page_help;
    page_idle* p_page_idle;
    page_sendFeedback* p_page_feedback;
    page_transactions* p_page_transactions;
    page_maintenance* p_page_maintenance;
    page_how_to* p_page_howTo;
    statusbar *p_statusbar;
    keyboard *p_keyboard;
    input_widget *p_input_widget;
    int _helpIdleTimeoutSec;

    QVBoxLayout *statusbarLayout;

};

#endif // PAGE_HOW_TO_H
