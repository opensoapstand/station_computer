#include "payselect.h"
#include "ui_payselect.h"

#include "paypage.h"
#include "productpage_1.h"
#include "idle.h"

paySelect::paySelect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::paySelect)
{
    ui->setupUi(this);
    QPixmap background(":/light/4_pay_select_page.jpg");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /*hacky transparent button*/
    ui->previousPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->payPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->mainPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

}

void paySelect::setPage(productPage_1 *pageSelect, payPage *pagePayment, idle* pageIdle)
{
    this->firstProductPage = pageSelect;
    this->paymentPage = pagePayment;
    this->idlePage = pageIdle;
}

paySelect::~paySelect()
{
    delete ui;
}

void paySelect::on_previousPage_Button_clicked()
{
    firstProductPage->showFullScreen();
    this->hide();
}

void paySelect::on_payPage_Button_clicked()
{
    paymentPage->showFullScreen();
    this->hide();
}

void paySelect::on_mainPage_Button_clicked()
{
    idlePage->showFullScreen();
    this->hide();
}
