//***************************************
//
// dispensepage.cpp
// GUI class to dispense drink.
// Coordinates User input to payment class
// then communcates results to FSM IP_thread
//
// Allows navigation to paymentpage and
// thankyou page
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "dispensepage.h"
#include "ui_dispensepage.h"

#include "thankyoupage.h"

// CTOR
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

/*
 * Page Tracking reference to Payment page and completed payment
 */
void dispensePage::setPage(payPage *pagePayment, thankYouPage* pageThankYou)
{
    this->thanksPage = pageThankYou;
    this->paymentPage = pagePayment;
}

// DTOR
dispensePage::~dispensePage()
{
    delete ui;
}


/*
 * Page Tracking reference to Payment page and completed payment
 */
void dispensePage::on_finish_Button_clicked()
{
    // TODO: Link to FSM for Dispense
    this->hide();
    thanksPage->showFullScreen();
}
