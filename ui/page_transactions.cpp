#include "page_transactions.h"
#include "ui_page_transactions.h"

page_transactions::page_transactions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_transactions)
{
    ui->setupUi(this);
}

page_transactions::~page_transactions()
{
    delete ui;
}
