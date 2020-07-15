#include "productpage_1.h"
#include "ui_productpage_1.h"

#include "productpage_2.h"
#include "payselect.h"

productPage_1::productPage_1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::productPage_1)
{
    ui->setupUi(this);
    QPixmap background(":/light/2_drink_page_1.jpg");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /*hacky transparent button*/
    ui->nextPageButton->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->selection1_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

}

void productPage_1::setPage(productPage_2 *pageTwoProducts, paySelect *pageSizeSelect)
{
    this->selection_PageTwo = pageTwoProducts;
    this->paymentSelectPage = pageSizeSelect;
}

productPage_1::~productPage_1()
{
    delete ui;
}

void productPage_1::on_nextPageButton_clicked()
{
    selection_PageTwo->showFullScreen();
    this->hide();
}

void productPage_1::on_selection1_Button_clicked()
{
    paymentSelectPage->showFullScreen();
    this->hide();
}
