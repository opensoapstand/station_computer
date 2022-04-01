//***************************************
//
// pagethankyou.cpp
// GUI class to show user dispense has been
// completed and route back to page_idle
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "pagethankyou.h"
#include "ui_pagethankyou.h"

// static QPointer<QFile> log_file = nullptr;

// CTOR
pagethankyou::pagethankyou(QWidget *parent) : QWidget(parent),
                                              ui(new Ui::pagethankyou)
{
    ui->setupUi(this);
    QPixmap background(PAGE_THANK_YOU_BACKGROUND_PATH);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /*hacky transparent button*/
    ui->mainPage_Button->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    ui->rinse_label->setText("<p align=center>Water rinse coming in<br>5</p>");
    ui->rinse_label->hide();

    thankYouEndTimer = new QTimer(this);
    thankYouEndTimer->setInterval(1000);
    connect(thankYouEndTimer, SIGNAL(timeout()), this, SLOT(onThankyouTimeoutTick()));
}

/*
 * Page Tracking reference
 */
void pagethankyou::setPage(page_dispenser *page_dispenser, page_idle *pageIdle, pagePayment *pagePayment)
{
    this->idlePage = pageIdle;
    this->p_page_dispense = page_dispenser;
    this->paymentPage = pagePayment;
}

// DTOR
pagethankyou::~pagethankyou()
{
    delete ui;
}

void pagethankyou::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    qDebug() << "ahoyy24";
    DbManager db(DB_PATH);
    QString paymentMethod = db.getPaymentMethod(idlePage->currentProductOrder->getSelectedSlot());
    db.closeDB();

    thankYouEndTimer = new QTimer(this);
    thankYouEndTimer->setInterval(1000);
    connect(thankYouEndTimer, SIGNAL(timeout()), this, SLOT(onThankyouTimeoutTick()));

    // THIS WILL HAVE TO BE CHANGED SO THE SYSTEM CHECKS IF IT IS A DF / SS MACHINE

    // ui->rinse_label->hide();
    ui->mainPage_Button->setEnabled(true);
    is_controller_finished = false;
    is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = false;
    exitIsForceable = false;

    // if (paymentMethod == "tap"){
    //     rinse=false;
    //     rinseTimer->start(1000);
    //     _rinseTimerTimeoutSec = 5;
    //     ui->rinse_label->setText("<p align=center>Water rinse coming in<br><br>5</p>");
    //     ui->rinse_label->show();
    //     ui->mainPage_Button->setEnabled(false);

    // }else{
    //     ui->rinse_label->hide();
    //     thankYouEndTimer->start(1000);
    //     _thankYouTimeoutSec = 5;
    //     ui->mainPage_Button->setEnabled(true);
    // }
    
    if (paymentMethod == "qr" || paymentMethod == "tap")
    {
        sendDispenseEndToCloud();
    }
    else
    {
        is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
    }

    ui->rinse_label->hide();
    // ui->rinse_label->show();
}

size_t WriteCallback2(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void pagethankyou::sendDispenseEndToCloud()
{
    QString order_id = this->paymentPage->getOID();
    QString dispensed_correct_units = this->p_page_dispense->getMostRecentDispensed();

    qDebug() << "Send data at finish of order : " << order_id << ". Total dispensed: " << this->p_page_dispense->getMostRecentDispensed() << "corrected units send to soapstandportal: " << dispensed_correct_units;

    QString curl_param = "oid=" + order_id + "&dispensed_amount=" + dispensed_correct_units;
    curl_param_array = curl_param.toLocal8Bit();
    curl_data = curl_param_array.data();

    curl = curl_easy_init();
    if (!curl)
    {
        qDebug() << "pagethankyou: cURL failed to init. parameters:" + curl_param;
        transactionToFile(curl_data);
        is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
        return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/updateOrder");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curl_param_array.data());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback2);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);
    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        qDebug() << "ERROR: Transaction NOT sent to cloud. cURL fail. Error code: " + QString::number(res);
       

        transactionToFile(curl_data);
        is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
    }
    else
    {
        QString feedback = QString::fromUtf8(readBuffer.c_str());
        qDebug() << "Transaction sent to cloud. cURL success. Server feedback readbuffer: " << feedback;

        // readbuffer is a string. "true" or "false"
        if (readBuffer == "true" || "Order Completed")
        {
            // return data
            is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
        }
       
    }
    curl_easy_cleanup(curl);
    readBuffer = "";
}

void pagethankyou::controllerFinishedTransaction()
{
    qDebug() << "Controller msg: All done for transaction.";
    is_controller_finished = true;
    thankYouEndTimer->start(1000);
    _thankYouTimeoutSec = 7;
}

void pagethankyou::transactionToFile(char *curl_params)
{
    QString data_out = curl_params;
    idlePage->dfUtility->write_to_file_timestamped(TRANSACTION_DISPENSE_END_OFFINE_PATH, data_out);
}

void pagethankyou::onThankyouTimeoutTick()
{
    if (--_thankYouTimeoutSec >= 0)
    {
    }
    else
    {
        exitPage();
        exitIsForceable = true;
    }
}

void pagethankyou::on_mainPage_Button_clicked()
{
    exitPage();
}

void pagethankyou::exitPage()
{

    if ((is_controller_finished && is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER) || exitIsForceable)
    {
        thankYouEndTimer->stop();
        idlePage->showFullScreen();
        this->hide();

        if (exitIsForceable)
        {
            qDebug() << "ERROR?!:Forced exit. controller ok?: " << is_controller_finished << " is payment finished?:" << is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER;
        }
    }
    else
    {

        ui->rinse_label->setText("<p align=center><br>Waiting for end<br>of transaction...</p>");
        ui->rinse_label->show();

        thankYouEndTimer->start(1000);
        _thankYouTimeoutSec = 7;
    }
}
