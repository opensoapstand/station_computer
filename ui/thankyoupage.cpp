//***************************************
//
// thankyoupage.cpp
// GUI class to show user dispense has been
// completed and route back to idle
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "thankyoupage.h"
#include "ui_thankyoupage.h"

#include "idle.h"
#include "dispensepage.h"

// CTOR
thankYouPage::thankYouPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::thankYouPage)
{
    ui->setupUi(this);
    QPixmap background(":/light/7_thank_you_page.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /*hacky transparent button*/
    ui->mainPage_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");
    ui->rinse_label->setText("<p align=center>Water rinse coming in<br>5</p>");
    ui->rinse_label->hide();

    thankYouEndTimer = new QTimer(this);
    thankYouEndTimer->setInterval(1000);
    connect(thankYouEndTimer, SIGNAL(timeout()), this, SLOT(onThankyouTimeoutTick()));

    rinseTimer = new QTimer(this);
    rinseTimer->setInterval(1000);
    connect(rinseTimer, SIGNAL(timeout()), this, SLOT(onRinseTimerTick()));

    //    QString counter = this->idlePage->dfUtility->get_local_db_max_transaction();
    //    ui->Counter->setStyleSheet("background-color : #F1F2F2; color: #CBA580");
    //    ui->Counter->setText(counter);
    //    ui->Counter->setText("0");
}

/*
 * Page Tracking reference
 */
void thankYouPage::setPage(dispensePage *pageDispense, idle *pageIdle, payPage *pagePayment)
{
    this->idlePage = pageIdle;
    this->dispensingPage = pageDispense;
    this->paymentPage = pagePayment;
}

// DTOR
thankYouPage::~thankYouPage()
{
    delete ui;
}

void thankYouPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    DbManager db(DB_PATH);

    if(thankYouEndTimer == nullptr){
        thankYouEndTimer = new QTimer(this);
        thankYouEndTimer->setInterval(1000);
        connect(thankYouEndTimer, SIGNAL(timeout()), this, SLOT(onThankyouTimeoutTick()));
    }


    // THIS WILL HAVE TO BE CHANGED SO THE SYSTEM CHECKS IF IT IS A DF / SS MACHINE

    if (db.getPaymentMethod(idlePage->userDrinkOrder->getOption()) == "tap"){
        rinse=false;
        rinseTimer->start(1000);
        _rinseTimerTimeoutSec = 5;
        ui->rinse_label->setText("<p align=center>Water rinse coming in<br><br>5</p>");
        ui->rinse_label->show();
        ui->mainPage_Button->setEnabled(false);
    }else{
        ui->rinse_label->hide();
        thankYouEndTimer->start(1000);
        _thankYouTimeoutSec = 5;
        ui->mainPage_Button->setEnabled(true);
    }

    thankYouEndTimer->start(1000);
    _thankYouTimeoutSec = 7;

    if (db.getPaymentMethod(idlePage->userDrinkOrder->getOption()) == "qr"){
        curler();
    }

    db.closeDB();
}

size_t WriteCallback2(char* contents, size_t size, size_t nmemb, void *userp){
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void thankYouPage::curler(){
    QString order_id = this->paymentPage->getOID();
    double dispensed = this->dispensingPage->getTotalDispensed();
//    qDebug() << "I'm in Thank You Page and the OID is: " << order_id << " and the total dispensed is: " << dispensed << endl;

    QString curl_param = "oid="+order_id+"&dispensed_amount="+QString::number(dispensed, 'f', 2);
    curl_param_array = curl_param.toLocal8Bit();
    curl_data = curl_param_array.data();

    curl = curl_easy_init();
    if (!curl){
//        qDebug() << "cURL failed to init" << endl;
        bufferCURL(curl_data);
    }else{
//        qDebug() << "cURL init success" << endl;

//        cout << "CURLING DATA: " << curl_param_array.data() << " is " << sizeof(curl_param_array.data()) << " bytes" << endl;

        curl_easy_setopt(curl, CURLOPT_URL, "http://Drinkfill-env.eba-qatmjpdr.us-east-2.elasticbeanstalk.com/api/machine_data/updateOrder");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curl_param_array.data());
       // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback2);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
//        qDebug() << "Curl Setup done" << endl;

        res = curl_easy_perform(curl);

        if (res != CURLE_OK){
//            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            bufferCURL(curl_data);
        }else{
//            qDebug() << "CURL SUCCESS!" << endl;
//            std::cout <<"Here's the output:\n" << readBuffer << endl;
            if (readBuffer == "true"){
                curl_easy_cleanup(curl);
                readBuffer = "";
            }else if (readBuffer == "false"){
                // TODO: Curl Buffer here but not sure of return state (currently false)
                curl_easy_cleanup(curl);
                readBuffer = "";
            }else{
                curl_easy_cleanup(curl);
                readBuffer = "";
            }

        }
    }
}

void thankYouPage::bufferCURL(char *curl_params){
    char filetime[50];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(filetime, 50, "%F %T", timeinfo);
//    std::cout << "Here I am in bufferCURL and I know the buffer is: " << curl_params << endl;
    std::string filelocation = "/home/df-admin/curlBuffer/";
    std::string filetype = "_fromQR.txt";
    std::string filename = filelocation+filetime+filetype;
//    std::cout << "filename is: " << filename << endl;
    std::ofstream out;
    out.open(filename);
    if (!out.is_open()){
//        std::cout << "Cannot open output file!";
    }else{
        out << curl_params;
        out.close();
    }
}

void thankYouPage::onThankyouTimeoutTick(){
    if(-- _thankYouTimeoutSec >= 0) {
//        qDebug() << "thanksPage: Tick Down: " << _thankYouTimeoutSec << endl;
    } else {
//        qDebug() << "thanksPage: Timer Done!" << _thankYouTimeoutSec << endl;

        thankYouEndTimer->stop();

        idlePage->showFullScreen();
        usleep(100);
        this->hide();
    }
}

void thankYouPage::on_mainPage_Button_clicked()
{

   thankYouEndTimer->stop();
   idlePage->showFullScreen();
   usleep(100);
   this->hide();
}

void thankYouPage::onRinseTimerTick(){

    QMessageBox msgBox;
    if (!rinse){
        ui->rinse_label->show();
        ui->rinse_label->setText("<p align=center>Water rinse coming in<br><br>"+ QString::number(_rinseTimerTimeoutSec) +"</p>");
        rinse=true;
    }

    if(-- _rinseTimerTimeoutSec >= 1) {
//        qDebug() << "rinseTimer: Tick Down: " << _rinseTimerTimeoutSec << endl;
        ui->rinse_label->setText("<p align=center>Water rinse coming in<br><br>"+ QString::number(_rinseTimerTimeoutSec) +"</p>");
    }else if(_rinseTimerTimeoutSec == 0) {
        ui->rinse_label->setText("<p align=center>Rinsing with water now</p>");
    }
    else if(_rinseTimerTimeoutSec == -1) {
        ui->rinse_label->setText("<p align=center>Rinsing with water now</p>");
    }
    else{
//        qDebug() << "rinseTimer Done!" << _rinseTimerTimeoutSec << endl;
        rinseTimer->stop();
        ui->rinse_label->hide();
        thankYouEndTimer->start(1000);
        _thankYouTimeoutSec = 3;
    }
}
