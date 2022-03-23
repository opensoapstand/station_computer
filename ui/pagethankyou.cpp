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

// CTOR
pagethankyou::pagethankyou(QWidget *parent) : QWidget(parent),
                                              ui(new Ui::pagethankyou)
{
    ui->setupUi(this);
    QPixmap background(PAGE_THANK_YOU_BACKGROUND);
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

    // rinseTimer = new QTimer(this);
    // rinseTimer->setInterval(1000);
    // connect(rinseTimer, SIGNAL(timeout()), this, SLOT(onRinseTimerTick()));
}

/*
 * Page Tracking reference
 */
void pagethankyou::setPage(page_dispenser *page_dispenser, page_idle *pageIdle, pagePayment *pagePayment)
{
    this->idlePage = pageIdle;
    this->dispensingPage = page_dispenser;
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

    if (thankYouEndTimer == nullptr)
    {
        thankYouEndTimer = new QTimer(this);
        thankYouEndTimer->setInterval(1000);
        connect(thankYouEndTimer, SIGNAL(timeout()), this, SLOT(onThankyouTimeoutTick()));
    }

    // THIS WILL HAVE TO BE CHANGED SO THE SYSTEM CHECKS IF IT IS A DF / SS MACHINE

    // ui->rinse_label->hide();
    ui->mainPage_Button->setEnabled(true);
    is_controller_finished = false;
    is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = false;

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

    if (paymentMethod == "qr")
    {
        curler();
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

void pagethankyou::curler()
{
    QString order_id = this->paymentPage->getOID();
    double dispensed = this->dispensingPage->getTotalDispensed();
    //    qDebug() << "I'm in Thank You Page and the OID is: " << order_id << " and the total dispensed is: " << dispensed << endl;

    QString curl_param = "oid=" + order_id + "&dispensed_amount=" + QString::number(dispensed, 'f', 2);
    curl_param_array = curl_param.toLocal8Bit();
    curl_data = curl_param_array.data();

    curl = curl_easy_init();
    if (!curl)
    {
        //        qDebug() << "cURL failed to page_init" << endl;
        bufferCURL(curl_data);
    }
    else
    {
        //        qDebug() << "cURL page_init success" << endl;

        //        cout << "CURLING DATA: " << curl_param_array.data() << " is " << sizeof(curl_param_array.data()) << " bytes" << endl;

        curl_easy_setopt(curl, CURLOPT_URL, "https://soapstandportal.com/api/machine_data/updateOrder");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curl_param_array.data());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback2);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        //        qDebug() << "Curl Setup done" << endl;

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            //            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            bufferCURL(curl_data);
        }
        else
        {
            //            qDebug() << "CURL SUCCESS!" << endl;
            if (readBuffer == "true")
            {
                curl_easy_cleanup(curl);
                readBuffer = "";
            }
            else if (readBuffer == "false")
            {
                // TODO: Curl Buffer here but not sure of return state (currently false)
                curl_easy_cleanup(curl);
                readBuffer = "";
            }
            else
            {
                curl_easy_cleanup(curl);
                readBuffer = "";
            }
        }
    }
    is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER = true;
}

void pagethankyou::controllerFinishedTransaction()
{
    qDebug() << "Controller msg: All done for transaction.";
    is_controller_finished = true;
    thankYouEndTimer->start(1000);
    _thankYouTimeoutSec = 7;
}

void pagethankyou::bufferCURL(char *curl_params)
{
    char filetime[50];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(filetime, 50, "%F %T", timeinfo);
    std::string filelocation = "/home/df-admin/curlBuffer/";
    std::string filetype = "_fromQR.txt";
    std::string filename = filelocation + filetime + filetype;
    std::ofstream out;
    out.open(filename);
    if (!out.is_open())
    {
        //        std::cout << "Cannot open output file!";
    }
    else
    {
        out << curl_params;
        out.close();
    }
}

void pagethankyou::onThankyouTimeoutTick()
{
    if (--_thankYouTimeoutSec >= 0)
    {
    }
    else
    {

        exitPage();
    }
}

void pagethankyou::on_mainPage_Button_clicked()
{
    exitPage();
}

void pagethankyou::exitPage()
{

    if (is_controller_finished && is_payment_finished_SHOULD_HAPPEN_IN_CONTROLLER)
    {
        thankYouEndTimer->stop();
        idlePage->showFullScreen();
        this->hide();
    }
    else
    {

        ui->rinse_label->setText("<p align=center><br>Waiting for end<br>of transaction...</p>");
        ui->rinse_label->show();

        thankYouEndTimer->start(1000);
        _thankYouTimeoutSec = 7;
    }
}

// void pagethankyou::onRinseTimerTick()
// {

//     QMessageBox msgBox;
//     if (!rinse)
//     {
//         ui->rinse_label->show();
//         ui->rinse_label->setText("<p align=center>Water rinse coming in<br><br>" + QString::number(_rinseTimerTimeoutSec) + "</p>");
//         rinse = true;
//     }

//     if (--_rinseTimerTimeoutSec >= 1)
//     {
//         //        qDebug() << "rinseTimer: Tick Down: " << _rinseTimerTimeoutSec << endl;
//         ui->rinse_label->setText("<p align=center>Water rinse coming in<br><br>" + QString::number(_rinseTimerTimeoutSec) + "</p>");
//     }
//     else if (_rinseTimerTimeoutSec == 0)
//     {
//         ui->rinse_label->setText("<p align=center>Rinsing with water now</p>");
//     }
//     else if (_rinseTimerTimeoutSec == -1)
//     {
//         ui->rinse_label->setText("<p align=center>Rinsing with water now</p>");
//     }
//     else
//     {
//         //        qDebug() << "rinseTimer Done!" << _rinseTimerTimeoutSec << endl;
//         rinseTimer->stop();
//         ui->rinse_label->hide();
//         thankYouEndTimer->start(1000);
//         _thankYouTimeoutSec = 3;
//     }
// }
