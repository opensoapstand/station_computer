//***************************************
//
// DispenseWindow
// for dispensing beverages updates user states
//
// created: 15-JUL-2019
// by: Jason Wang
//
// Version number: ev2.2.1
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************


#include "dispensewindow.h"
#include "ui_dispenseWindow.h"
#include "mainwindow.h"
#include "payment.h"
#include "countdownwindow.h"

extern int pageNumber;
extern bool goHome;
extern int progressValue;
QByteArray DispenseVolume;

//inventroy database update
extern QString machineID;
extern int optionSelected;

extern bool enterPage;
int tempDispenseVolume;
int tempCount;

DispenseWindow::DispenseWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DispenseWindow)
{
    ui->setupUi(this);

    ui->fillLabel->setAttribute(Qt::WA_TranslucentBackground);
    ui->pourLabel->setAttribute(Qt::WA_TranslucentBackground);
    ui->bottleLabel->setAttribute(Qt::WA_TranslucentBackground);
    ui->maskLabel->setAttribute(Qt::WA_TranslucentBackground);


    ui->progressBar->setStyleSheet("QProgressBar{background-color: transparent;"

                                   "text-align : center;"
                                   "border: 0px;}"

                                   "QProgressBar::chunk{background-color: #ADD8E6;}");

    timerUpdate = new QTimer(this);
    connect(timerUpdate, SIGNAL(timeout()), this, SLOT(updatePageNumber()));
    timerUpdate->setInterval(10);

    timerUpdate2 = new QTimer(this);  // use for button delay
    connect(timerUpdate2, SIGNAL(timeout()), this, SLOT(refreshDispense()));
    timerUpdate2->setInterval(1000);

    ardTimer = new QTimer(this);
    connect(ardTimer, SIGNAL(timeout()), this, SLOT(updateVolume()));
    ardTimer->setInterval(5);
    // qDebug() << "DispenseWindow::DispenseWindow";
}

DispenseWindow::~DispenseWindow()
{
    delete ui;
}

void DispenseWindow::setPages(MainWindow *main, payment *pay, countdownWindow * pausePage){
    this->mainPage = main;
    this->pausePage = pausePage;
    this->paymentPage = pay;
}

void DispenseWindow::updatePageNumber()
{
    timerUpdate->stop();

    if(enterPage){
        usleep(20);
        paymentPage->hide();
        pausePage->hide();
        ui->pushButton->setEnabled(true);
        enterPage = false;
    }

    if (pageNumber == 2)
    {
        //-------------------label changes-----------------------//
        if(paymentPage->getPriceSelect() == 1)
        {
            if(progressValue >= 40){
                pausePage->FilledPageTitle();
            }

            else {
                pausePage->notFilledPageTitle();
            }
        }

        else if(paymentPage->getPriceSelect() == 2 )
        {
            if(progressValue >= 375){
                pausePage->FilledPageTitle();
            }

            else {
                pausePage->notFilledPageTitle();
            }
        }
        //-------------------------------------------------------//
//        timerUpdate->stop();
        pausePage->showFullScreen();
        ardTimer->stop();

        return;

    } else if(pageNumber == 3){

        mainPage->sendardCommand("E");
        mainPage->sendardCommand("Z");
        ui->progressBar->setValue(0); // reset the value of the progress bar
//        timerUpdate->stop();
        ardTimer->stop();

        progressValue = 0;
        DispenseVolume.clear();
        mainPage->showFullScreen();
        return;
    } else {
        ui->progressBar->setValue(progressValue);
        timerUpdate->start();
        return;
    }


}

void DispenseWindow::refreshDispense()
{
    timerUpdate2->stop();
    timerUpdate->start();
    ui->pushButton->setEnabled(true);
}

void DispenseWindow::setProgressBarDisplay(bool status)
{
    if (status == true)
    {
        ui->progressBar->hide();    //if beer is chosen, hide the progress bar
        pausePage->setProgressBarDisplay(true);
    }
    else {
        ui->progressBar->show();    //other options still shows progress bar
        pausePage->setProgressBarDisplay(false);
    }
}

void DispenseWindow::on_pushButton_clicked()
{
    pageNumber = 3;

    mainPage->sendardCommand("E");
    updateInventory(mainPage->getDatabase());

    ui->pushButton->setEnabled(false);
    ui->progressBar->setValue(0);

}

void DispenseWindow::paintEvent(QPaintEvent *p)
{
    QPainter paint(this);
    paint.drawPixmap(0, 0, QPixmap(":/assets/V5/progressbarmask.png").scaled(size()));

    QPixmap pixmap(":/assets/V6/side_arrow.png");
    QMatrix rotateMatrix;

    if(optionSelected == 1)
        rotateMatrix.rotate(130);
    else if (optionSelected == 2)
        rotateMatrix.rotate(70);

    pixmap = pixmap.transformed(rotateMatrix);
    ui->arrowLabel->setPixmap(pixmap);
    //ui->fillLabelArrow->hide();

    QWidget::paintEvent(p);
}

void DispenseWindow::showEvent(QShowEvent *event)
{
    timerUpdate->stop();
    timerUpdate2->start();


    if(paymentPage->getPriceSelect() == 1)
        ui->progressBar->setRange(0,40);
    else if(paymentPage->getPriceSelect() == 2)
        ui->progressBar->setRange(0,375);

    ui->progressBar->setValue(progressValue);

    enterPage = true;

    tempDispenseVolume = progressValue; //store the value from previous reading
    tempCount = 0; //reset temp count
    ardTimer->start();
    QWidget::showEvent(event);
}

void DispenseWindow::showpage(){
    if (pageNumber == 2)
    {
        pausePage->showFullScreen();
    }
}


void DispenseWindow::updateInventory(database_logger db)
{
    beverageData* bev = mainPage->getBeverageData(optionSelected);
    bev->setDispenseVolume(progressValue);
    bev->updateInventoryVolume();
    mainPage->setBeverageData(bev, optionSelected);

    if(db.checkDataBaseConnection())
        db.reconnectDatabase();

    db.event_log(machineID, QDateTime::currentDateTime(), optionSelected, "Inventory Update", bev->getInventoryVolume());

    //cout << bev->getInventoryVolume() << endl;
}

int DispenseWindow::getPriceSelected()
{
    return paymentPage->getPriceSelect();
}

void DispenseWindow::updateVolume()
{
    ardTimer->stop();
    DispenseVolume = mainPage->getardVolume();

    tempDispenseVolume = progressValue;
    //store the value from previous reading
    if (DispenseVolume.toInt()>= progressValue){
        progressValue = DispenseVolume.toInt(); //new reading from ard
    }
    //progressValue += 1;

    if(tempDispenseVolume == progressValue)
    {
        tempCount++;
    }
    else
    {
        tempCount = 0; //reset temp counting because value has changed
    }

    //--------------Page Change----------------------//
    if(tempCount >= 500){ //if reading is the same more than ten times, jump to next page
        pageNumber = 2;
    }
    else if(progressValue >= 40 && paymentPage->getPriceSelect() == 1) //if the volume exceed
    {
        mainPage->sendardCommand("E");
        mainPage->sendardCommand("Z");
        tempCount = 0; //reset temp counting
        pageNumber = 3;
//        progressValue = 0;
//        DispenseVolume.clear();
        updateInventory(mainPage->getDatabase());

        qDebug() << "reset: " << progressValue;
        return;
    }
    else if(progressValue >= 375 && paymentPage->getPriceSelect() == 2) //if the volume exceed
    {
        mainPage->sendardCommand("E");
        mainPage->sendardCommand("Z");
        tempCount = 0; //reset temp counting
        pageNumber = 3;
//        progressValue = 0;
//        DispenseVolume.clear();
        updateInventory(mainPage->getDatabase());
        qDebug() << "reset: "<< progressValue;
        return;
    }
    //-----------------------------------------------//
    ardTimer->start();
    qDebug() << progressValue;
}


