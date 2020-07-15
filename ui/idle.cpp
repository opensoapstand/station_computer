#include "idle.h"
#include "ui_idle.h"

idle::idle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::idle)
{
    ui->setupUi(this);
    QPixmap background(":/light/1_welcome_panel.jpg");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /*!!!!need to find a way to make the button transparent (hacky by using a transparent image)*/
    //ui->nextPageButton->setAttribute(Qt::WA_TranslucentBackground);
    ui->nextPageButton->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    //setStyleSheet("QPushButton{background: transparent;}");


}

void idle::setPage(productPage_1 *pageProduct)
{
    this->selection_PageOne = pageProduct;
}

idle::~idle()
{
    delete ui;
}

void idle::on_nextPageButton_clicked()
{
    selection_PageOne->showFullScreen();
    this->hide();
}
