//***************************************
//
// mainwindow
// mainUI working window for product selection
//
// created: 15-JUL-2019
// by: Jason Wang
//
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <QString>

extern int pageNumber;
extern bool Active;
extern int progressValue;
extern int optionSelected;

bool enterPage;
extern QString machineID;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPixmap logo(":/assets/V6/ev2_ui_assets_v6-08.png");
    logo = logo.scaled(ui->logoLabel->size(), Qt::IgnoreAspectRatio);
    ui->logoLabel->setPixmap(logo);

    for(int i = 0; i < MAX_BEVS; i++)
    {
        currentBeverage[i] = new beverageData(i+1, eventData.getInventory(i+1, 15000, machineID));
        currentProducts[i] = new products();
    }

    loadButtonNameImg();

    labelSetup(ui->option1Label, ui->option1ALabel, currentProducts[1-1]->price_large);
    labelSetup(ui->option3Label, ui->option3ALabel, currentProducts[2-1]->price_large);

    buttonSetup(ui->option1Button, currentProducts[1-1]->imag, 400);
    buttonSetup(ui->option3Button, currentProducts[2-1]->imag, 400);

    /*survey page setup*/
    ui->surveyButton->setStyleSheet("border-image:url(:/assets/V6/ev2_ui_assets_v6-17.png); color: white;");
    QFont buttonFont("Arial", 20, QFont::Bold);
    ui->surveyButton->setFont(buttonFont);
    ui->surveyButton->hide();

    disabletimer = new QTimer(this);
    connect(disabletimer, SIGNAL(timeout()), this, SLOT(disable_oneSec()));
    disabletimer->setInterval(500);

    timerUpdate = new QTimer(this);
    connect(timerUpdate, SIGNAL(timeout()), this, SLOT(updatePageNumber()));
    timerUpdate->setInterval(10);

    frozeTimer = new QTimer(this);
    connect(frozeTimer, SIGNAL(timeout()), this, SLOT(comptuerStatCheck()));
    //frozeTimer->setInterval(3600000);
    frozeTimer->setInterval(600000);

    currentBeverage[0] = new beverageData(1, eventData.getInventory(1, 15000, machineID));
    currentBeverage[1] = new beverageData(2, eventData.getInventory(2, 15000, machineID));

    ui->option1ALabel->show();
    ui->option3ALabel->show();

    serial.init();
    sendardCommand("E");
    eventData.event_log(machineID, QDateTime::currentDateTime(), 0, "UI Starts", 0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setPages(payment *payPage, DispenseWindow * dispense, survey *surveypg, countdownWindow * pausePg){
    this->dispensePage = dispense;
    this->paymentPage = payPage;
    this->surveyPage = surveypg;
    this->pausePage = pausePg;
}

void MainWindow::on_option1Button_clicked()
{
    frozeTimer->stop();

    if(!eventData.checkDataBaseConnection())
    {
        eventData.reconnectDatabase();
    }
    eventData.event_log(machineID, QDateTime::currentDateTime(), optionSelected, " Selected", eventData.getInventory(optionSelected, 15000, machineID));

    pageNumber = 4;
    optionSelected = 1;
}

void MainWindow::on_option3Button_clicked()
{
    frozeTimer->stop();

    if(!eventData.checkDataBaseConnection())
    {
        eventData.reconnectDatabase();
    }
    eventData.event_log(machineID, QDateTime::currentDateTime(), optionSelected, " Selected", eventData.getInventory(optionSelected, 15000, machineID));

    pageNumber = 4;
    optionSelected = 2;
}

void MainWindow::paintEvent(QPaintEvent *p)
{
    QPainter paint(this);
    paint.drawPixmap(0, 0, QPixmap(":/assets/V6/ev2_ui_assets_v6-07.png").scaled(size()));

    QWidget::paintEvent(p);
}

void MainWindow::showEvent(QShowEvent *event)
{
    sendardCommand("Z");
    enterPage = true;
    disabletimer->start();
    frozeTimer->start();

    pageNumber = 0;
    progressValue= 0;
    ui->option1Button->setEnabled(false);
    ui->option3Button->setEnabled(false);

//    surveyFilled = surveyPage->getSurveyFilled();
//    if(surveyFilled)
//    {
//        //ui->surveyButton->setEnabled(false);
//        //ui->surveyButton->setText("$1 off applied");
//        ui->surveyButton->setText("Take a survey :)");
//    }
//    else
//    {
//        ui->surveyButton->setEnabled(true);
//        //ui->surveyButton->setText("Take a survey, and save a dollar!");
//        ui->surveyButton->setText("Take a survey :)");
//    }

    QWidget::showEvent(event);
}

void MainWindow::soldOutButton(QPushButton* optionButton, QLabel* soldLabel, bool status)
{
    if (status == true){
        optionButton->setEnabled(false);
        soldLabel->show();
    }
    else{
        optionButton->setEnabled(true);
        soldLabel->hide();
    }
}

void MainWindow::updatePageNumber()
{
    timerUpdate->stop();
    if (enterPage){
        usleep(20);
        paymentPage->hide();
        dispensePage->hide();
        pausePage->hide();
        surveyPage->hide();
        enterPage = false;
    }

    if (pageNumber == 4)
    {
        paymentPage->pageSetup(currentProducts[optionSelected-1]->name, currentProducts[optionSelected-1]->imag, surveyFilled);
        paymentPage->showFullScreen();

        return;
    }
    timerUpdate->start();
}

void MainWindow::disable_oneSec()
{
    timerUpdate->start();
    disabletimer->stop();
    sendardCommand("Z");
    if (currentBeverage[0]->getInventoryVolume() <= 500)
    {
        soldOutButton(ui->option1Button, ui->option1SoldOutLabel, true);
    }
    else {
        soldOutButton(ui->option1Button, ui->option1SoldOutLabel, false);
    }

    if (currentBeverage[1]->getInventoryVolume() <= 500){
        soldOutButton(ui->option3Button, ui->option3SoldOutLabel, true);
    }
    else {
        soldOutButton(ui->option3Button, ui->option3SoldOutLabel, false);
    }


}

void MainWindow::labelSetup(QLabel* labelVolume, QLabel* labelPrice, QString productPrice)
{
    labelPrice->setAttribute(Qt::WA_TranslucentBackground);
    labelPrice->setStyleSheet("QLabel{color: #ffffff; font: 30pt;}");
    labelPrice->setAlignment(Qt::AlignCenter);
    labelPrice->setWordWrap(true);

    if (productPrice == "Not S")
    {
        labelPrice->setText(QString("$ 3.00"));
    }
    else{
        labelPrice->setText(QString("$ ") + QString(productPrice));
    }

    labelVolume->setStyleSheet("QLabel{color: #ffffff; font: 30pt;}");
    labelVolume->setAlignment(Qt::AlignCenter);
    labelVolume->setText("12 oz (355mL)");
}

void MainWindow::buttonSetup(QPushButton *button, QByteArray imageData, int masksize)
{
    if ("Not Set" != imageData) //info button does not have its own image asset
                                //from database
    {
        QPixmap image;
        image.loadFromData(imageData);

        QIcon ButtonIcon(image);
        button->setIcon(ButtonIcon);
        button->setIconSize(QSize(masksize,masksize));
    }

    QRect rect(0,0,masksize-2,masksize-2);
    QRegion region(rect, QRegion::Ellipse);
    button->setMask(region); //masking the region to be circle for the button
}

beverageData* MainWindow::getBeverageData(int idx)
{
    return currentBeverage[idx-1];
}

void MainWindow::setBeverageData(beverageData *bev, int idx)
{
    currentBeverage[idx-1]->updateBeverage(bev);
}

survey* MainWindow::getSurveyPage()
{
    return surveyPage;
}

database_logger MainWindow::getDatabase()
{
    if(eventData.checkDataBaseConnection())
    {
        eventData.reconnectDatabase();
    }
    return eventData;
}

void MainWindow::on_surveyButton_clicked()
{
    //surveyPage->showFullScreen();
}

void MainWindow::comptuerStatCheck()
{
    frozeTimer->stop();
    if(eventData.checkDataBaseConnection())
    {
        eventData.reconnectDatabase();
    }
    eventData.event_log(machineID, QDateTime::currentDateTime(), 0, "Not Froze", 0);

    paymentPage->stayAliveLogon();

    QDateTime xxx;
    xxx = xxx.currentDateTime();
    QString hour = xxx.time().toString("H:mm:ss");
    std::cout << hour.toStdString();
    if (hour[0] == '0' && hour[1] == '4')
    {
        paymentPage->batchClose();
    }


    frozeTimer->start();
}

QString MainWindow::getPrice_1()
{
    return currentProducts[optionSelected-1]->price_small;
}

QString MainWindow::getPrice_2()
{
    return currentProducts[optionSelected-1]->price_large;
}

QByteArray MainWindow::getardVolume()
{
    return serial.readPacket(1);
}

void MainWindow::sendardCommand(QByteArray command)
{
    serial.writePacket(command);
}

bool MainWindow::checkard()
{
    return serial.checkConnected();
}

bool MainWindow::reinitard()
{
    return serial.init();
}

void MainWindow::clearArd()
{
    serial.clearVolume();
}

void MainWindow::loadButtonNameImg()
{
    for(int i = 1; i <= MAX_BEVS; i++)
    {
        currentProducts[i-1]->name = eventData.getProductName(i, machineID);
        currentProducts[i-1]->imag = eventData.getImgArray(i, machineID);
        currentProducts[i-1]->price_small = eventData.getProductPrice(i, 1,machineID);
        currentProducts[i-1]->price_large = eventData.getProductPrice(i, 2, machineID);
    }
}
