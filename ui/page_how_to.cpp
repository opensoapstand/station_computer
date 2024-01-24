#include "page_how_to.h"
#include "ui_page_how_to.h"
#include "page_help.h"
#include "input_widget.h"
#include "df_util.h"
#include "page_select_product.h"
#include "page_product.h"
#include "page_product_mixing.h"
#include "page_qr_payment.h"
#include "page_idle.h"

page_how_to::page_how_to(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_how_to)
{
    ui->setupUi(this);

    statusbarLayout = new QVBoxLayout(this);
}

page_how_to::~page_how_to()
{
    delete ui;
}

void page_how_to::setPage(page_select_product *pageSelect, page_product *page_product, page_idle *pageIdle, page_qr_payment *page_qr_payment, page_transactions *pageTransactions, page_maintenance *pageMaintenance, page_sendFeedback *pageFeedback, statusbar *p_statusbar, keyboard *keyboard, input_widget *input_widget)
{
    this->p_page_idle = pageIdle;
    this->p_page_feedback = pageFeedback;
    this->p_page_payment = page_qr_payment;
    this->p_page_product = page_product;
    this->p_page_select_product = pageSelect;
    this->p_page_transactions = pageTransactions;
    this->p_page_maintenance = pageMaintenance;
    this->p_statusbar = p_statusbar;
    this->p_keyboard = keyboard;
    this->p_input_widget = input_widget;
}

void page_how_to::showEvent(QShowEvent *event)
{
    p_page_idle->thisMachine->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    QString styleSheet = p_page_idle->thisMachine->getCSS(PAGE_HOWTO_CSS);
    p_page_idle->thisMachine->setBackgroundPictureFromTemplateToPage(this, PAGE_HOWTO_BACKGROUND_PATH);

}
