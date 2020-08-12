//***************************************
//
// countdownWindow
// User pause window
//
// created: 15-JUL-2019
// by: Jason Wang
//
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#include "countdownwindow.h"
#include "ui_countdownwindow.h"
#include "dispensewindow.h"
#include "mainwindow.h"

//extern packet toSend;
extern int pageNumber;
extern int progressValue;

extern QString machineID;
extern int optionSelected;

extern QByteArray DispenseVolume;
extern int tempCount;
extern int tempDispenseVolume;
int hideDispense;

countdownWindow::countdownWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::countdownWindow)
{
    ui->setupUi(this);

    ui->fillLabel->setAttribute(Qt::WA_TranslucentBackground);
    ui->pourLabel->setAttribute(Qt::WA_TranslucentBackground);
    ui->maskLabel->setAttribute(Qt::WA_TranslucentBackground);
    ui->bottleLabel->setAttribute(Qt::WA_TranslucentBackground);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(countDown()));
    timer->setInterval(1000);

    timer2 = new QTimer(this);
    connect(timer2, SIGNAL(timeout()), this, SLOT(waitDispense()));
    timer2->setInterval(10);

    ardTimer = new QTimer(this);
    connect(ardTimer, SIGNAL(timeout()), this, SLOT(updateVolume()));
    ardTimer->setInterval(5);

    ui->progressBar->setStyleSheet("QProgressBar{background-color: transparent;"
                                   "text-align : center;"
                                   "border: 0px;}"

                                   "QProgressBar::chunk{background-color: #ADD8E6;}");
}

countdownWindow::~countdownWindow()
{
    //delete ui;
}

void countdownWindow::setPages(MainWindow *mainPage, DispenseWindow *dispensePage){
    this->mainPage = mainPage;
    this->dispensePage = dispensePage;
}

void countdownWindow::on_pushButton_clicked()
{
    pageNumber = 3;
    mainPage->sendardCommand("E");
    ui->pushButton->setEnabled(false);

    updateInventory(mainPage->getDatabase());
}

void countdownWindow::countDown()
{
    tenSec--;
    ui->timeLabel->setStyleSheet("QLabel {color : white; font: 50pt; }");
    ui->timeLabel->setAlignment(Qt::AlignCenter);
    ui->timeLabel->setText(QString::number(tenSec) + "s");

    if(tenSec == 19)
        ui->pushButton->setEnabled(true);

    if(tenSec == 0)
    {
        tenSec = 20; //reset countdown
        timer->stop();

        pageNumber = 3;
        ui->pushButton->setEnabled(false);
        updateInventory(mainPage->getDatabase());
    }
}

void countdownWindow::waitDispense()
{
    //this->showFullScreen();
    if (pageNumber==1)
    {
        //hide();
        dispensePage->showFullScreen();
        tenSec = 20;
        timer2->stop();
        ardTimer->stop();
        timer->stop();
    }
    else if (pageNumber==3)
    {
        //hide();
        progressValue = 0;
        DispenseVolume.clear();
        mainPage->sendardCommand("E");
        mainPage->showFullScreen();
        timer2->stop();
        ardTimer->stop();
        timer->stop();
        tenSec = 20;
    }

    timer2->start();

}

void countdownWindow::paintEvent(QPaintEvent *p)
{
 //   QPainter paint(this);
 //   paint.drawPixmap(0, 0, QPixmap(":/assets/V5/progressbarmask.png").scaled(size()));

//    QPixmap pixmap(":/assets/V6/up_arrow.png");
//    QMatrix rotateMatrix;
//    rotateMatrix.rotate(90);
//    pixmap = pixmap.transformed(rotateMatrix);
//    ui->fillLabelArrow->setPixmap(pixmap);
//    ui->fillLabelArrow->hide();

    QWidget::paintEvent(p);
}

void countdownWindow::showEvent(QShowEvent *event)
{
    tempDispenseVolume = progressValue;

    timer->start();
    timer2->start();
    tenSec = 20;
    ui->timeLabel->setStyleSheet("QLabel {color : white; font: 50pt; }");
    ui->timeLabel->setAlignment(Qt::AlignCenter);
    ui->timeLabel->setText("20s");

    if(dispensePage->getPriceSelected() == 1)
        ui->progressBar->setRange(0,375);
    else if(dispensePage->getPriceSelected() == 2)
        ui->progressBar->setRange(0,500);

    ui->progressBar->setValue(progressValue);

    hideDispense = 0;

    ardTimer->start();
    QWidget::showEvent(event);


}

void countdownWindow::setProgressBarDisplay(bool status)
{
    if (status == true)
    {
        ui->progressBar->hide(); //if beer is chosen, hide the progress bar
    }
    else {
        ui->progressBar->show(); //other options still shows progress bar
    }
}

void countdownWindow::FilledPageTitle()
{
    QFont newFont("Arial", 70);
    ui->pourLabel->setFont(newFont);
    ui->pourLabel->setText("<font color = white>"
                           "All \n Done"
                           "</font>");
    ui->pourLabel->setAlignment(Qt::AlignCenter);
}

void countdownWindow::notFilledPageTitle()
{
    QFont newFont("Arial", 70);
    ui->pourLabel->setFont(newFont);
    ui->pourLabel->setText("<font color = white>"
                           "Still There?"
                           "</font>");
    ui->pourLabel->setAlignment(Qt::AlignCenter);
}

void countdownWindow::updateInventory(database_logger db)
{
    beverageData* bev = mainPage->getBeverageData(optionSelected);
    bev->setDispenseVolume(progressValue);
    bev->updateInventoryVolume();
    mainPage->setBeverageData(bev, optionSelected);

    if(db.checkDataBaseConnection())
        db.reconnectDatabase();

    db.event_log(machineID, QDateTime::currentDateTime(), optionSelected, "Inventory Update", bev->getInventoryVolume());
    //cout << progressValue << endl;
}

void countdownWindow::updateVolume()
{
    ardTimer->stop();

    if (hideDispense == 0)
    {
        dispensePage->hide();
        hideDispense = 1;
    }


    DispenseVolume = mainPage->getardVolume();

    //--------------Page Change----------------------//
    if(tempDispenseVolume < DispenseVolume.toInt()){ //if reading is not the same means botton being pressed
        pageNumber = 1;
        progressValue = DispenseVolume.toInt();
        return;
    }
    //-----------------------------------------------//
    qDebug() << progressValue;
    ardTimer->start();
}



