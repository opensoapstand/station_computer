#include "dispensepage.h"
#include "ui_dispensepage.h"

#include "thankyoupage.h"

dispensePage::dispensePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dispensePage)
{
    ui->setupUi(this);
    QPixmap background(":/light/6_dispense_page.jpg");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /*hacky transparent button*/
    ui->finish_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
}

void dispensePage::setPage(payPage *pagePayment, thankYouPage* pageThankYou)
{
    this->thanksPage = pageThankYou;
    this->paymentPage = pagePayment;
}

dispensePage::~dispensePage()
{
    delete ui;
}

void dispensePage::on_finish_Button_clicked()
{
    this->hide();
    thanksPage->showFullScreen();
}
