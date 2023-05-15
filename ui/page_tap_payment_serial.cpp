//***************************************
//
// page_tap_payment_serial.cpp
// GUI class while machine is processing
// payment.
//
// Coordinates User input from payment select
// class then communcates results to page_dispenser.
//
// created: 05-04-2022
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "page_tap_payment_serial.h"
#include "ui_page_tap_payment_serial.h"

#include "page_product.h"
#include "page_dispenser.h"
#include "page_idle.h"
#include "payment/commands.h"
#include "payment/setup_Tap.h"

extern QString transactionLogging;
// CTOR

page_tap_payment_serial::page_tap_payment_serial(QWidget *parent) : QWidget(parent),
                                                      ui(new Ui::page_tap_payment_serial)
{
    // Fullscreen background setup
    ui->setupUi(this);
    qDebug() << "Payment page" << endl;
    // ui->previousPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->previousPage_Button->setStyleSheet(
        "QPushButton {"
        "font-family: 'Brevia';"
        "font-style: normal;"
        "font-weight: 75;"
        "font-size: 32px;"
        "line-height: 99px;"
        "letter-spacing: 1.5px;"
        "text-transform: lowercase;"
        "color: #003840;"
        "text-align: cCenter;"
        "qproperty-alignment: AlignCenter;"
        "border: none;"
        "}");
    ui->previousPage_Button->setText("<- Back");
    ui->mainPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->payment_bypass_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    ui->payment_bypass_Button->setEnabled(false);

    // Payment Declined
    declineTimer = new QTimer(this);
    connect(declineTimer, SIGNAL(timeout()), this, SLOT(declineTimer_start()));

    // Idle Payment reset
    idlePaymentTimer = new QTimer(this);
    connect(idlePaymentTimer, SIGNAL(timeout()), this, SLOT(idlePaymentTimeout()));

    ui->payment_bypass_Button->setEnabled(false);
    ui->title_Label->hide();

    // ui->order_total_amount->hide();
    DbManager db(DB_PATH);

}

void page_tap_payment_serial::stopPayTimers()
{

    if (checkPacketReceivedTimer != nullptr)
    {
        qDebug() << "cancel payment progress Timer" << endl;
        // stop_tap_action_thread=true;
        checkPacketReceivedTimer->stop();
    }
    if (checkCardTappedTimer != nullptr)
    {
        qDebug() << "cancel TAP progress Timer" << endl;
        checkCardTappedTimer->stop();
    }

    if (declineTimer != nullptr)
    {
        qDebug() << "cancel decline Timer" << endl;
        declineTimer->stop();
    }

    if (idlePaymentTimer != nullptr)
    {
        qDebug() << "cancel page_idle payment Timer" << endl;
        idlePaymentTimer->stop();
    }
}

/*
 * Page Tracking reference
 */
void page_tap_payment_serial::setPage(pageProduct *pageSizeSelect, page_error_wifi *pageWifiError, page_dispenser *page_dispenser, page_idle *pageIdle, page_help *pageHelp)
{
    tmpCounter = 0;
    this->p_pageProduct = pageSizeSelect;
    this->p_page_wifi_error = pageWifiError;
    this->p_page_dispense = page_dispenser;
    this->p_page_idle = pageIdle;
    this->p_page_help = pageHelp;
}

// DTOR
page_tap_payment_serial::~page_tap_payment_serial()
{
    delete ui;
}

/* ----- GUI ----- */

// Labels and button for tapping payment
void page_tap_payment_serial::displayPaymentPending(bool isVisible)
{
}

void page_tap_payment_serial::on_payment_bypass_Button_clicked()
{
    hideCurrentPageAndShowProvided(p_page_dispense);
}

void page_tap_payment_serial::updateTotals(string drinkDescription, string drinkAmount, string orderTotal)
{
}

void page_tap_payment_serial::resizeEvent(QResizeEvent *event)
{
}

void page_tap_payment_serial::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Tap Payment >>>>>>>>>";
    QWidget::showEvent(event);

}

void page_tap_payment_serial::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{

    resetPaymentPage();
    p_page_idle->pageTransition(this, pageToShow);
}

bool page_tap_payment_serial::setpaymentProcess(bool status)
{
    return (paymentProcessing = status);
}

void page_tap_payment_serial::setProgressLabel(QLabel *label, int dot)
{
}

// Local storge for now.  Will need to refactor logger to do a nightly push to AWS
void page_tap_payment_serial::storePaymentEvent(QSqlDatabase db, QString event)
{
}



void page_tap_payment_serial::declineTimer_start()
{
}

bool page_tap_payment_serial::exitConfirm()
{
    qDebug() << "In exit confirm";
    if (state_tap_payment == s_tap_payment_processing || state_tap_payment == s_tap_payment_done)
    {
        // ARE YOU SURE YOU WANT TO EXIT?
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::FramelessWindowHint); // do not show messagebox header with program name

        msgBox.setText("<p align=center><br><br>Cancel transaction and exit page?<br><br>It can take up to 30 seconds for dispensing to start after a payment is completed. <br></p>");
        msgBox.setStyleSheet("QMessageBox{min-width: 7000px; font-size: 24px; font-weight: bold; font-style: normal;  font-family: 'Montserrat';} QPushButton{font-size: 24px; min-width: 300px; min-height: 300px;}");

        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();
        bool success;
        switch (ret)
        {
        case QMessageBox::Yes:
        {
            // resetPaymentPage();
            // transactionLogging = "";
            return true;
        }
        break;
        case QMessageBox::No:
        {
            return false;
        }
        break;
        }
    }
    else
    {
        // exit, no questions asked.
        // resetPaymentPage();
        // transactionLogging = "";
        return true;
    }
}

// Navigation: Back to Drink Size Selection
void page_tap_payment_serial::on_previousPage_Button_clicked()
{
    qDebug() << "In previous page button" << endl;

}

void page_tap_payment_serial::on_mainPage_Button_clicked()
{
    if (exitConfirm())
    {
        hideCurrentPageAndShowProvided(p_page_help);
    }
}

void page_tap_payment_serial::idlePaymentTimeout()
{
    hideCurrentPageAndShowProvided(p_page_idle);
}
void page_tap_payment_serial::resetPaymentPage()
{
    ui->title_Label->hide();

    stopPayTimers();
    transactionLogging = "";
    response = true;
    qDebug() << "Cancelled";
}

/* ----- Payment ----- */

bool page_tap_payment_serial::tap_init()
{
    return true;
}
