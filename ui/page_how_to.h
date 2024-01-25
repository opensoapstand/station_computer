#ifndef PAGE_HOW_TO_H
#define PAGE_HOW_TO_H

#include "df_util.h"
#include "input_widget.h"
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

class page_select_product;
class page_qr_payment;
class page_idle;
class page_product;
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
    void setPage(page_select_product *pageSelect, page_product* page_product, page_idle* pageIdle, page_qr_payment *page_qr_payment, page_transactions *pageTransactions, page_maintenance* pageMaintenance, page_sendFeedback *pageFeedback, statusbar *p_statusbar, keyboard * keyboard, input_widget * input_widget);


private:
    void showEvent(QShowEvent *event);

    Ui::page_how_to *ui;
    int number_of_steps;
    page_select_product* p_page_select_product;
    page_qr_payment* p_page_payment;
    page_idle* p_page_idle;
    page_sendFeedback* p_page_feedback;
    page_product* p_page_product;
    page_transactions* p_page_transactions;
    page_maintenance* p_page_maintenance;
    page_how_to* p_page_howTo;
    statusbar *p_statusbar;
    keyboard *p_keyboard;
    input_widget *p_input_widget;

    QVBoxLayout *statusbarLayout;

};

#endif // PAGE_HOW_TO_H
