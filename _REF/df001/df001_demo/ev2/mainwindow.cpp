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

#include <QVBoxLayout>

extern int pageNumber;
extern int progressValue;
extern int optionSelected;

extern bool enterPage;
extern QString machineID;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPixmap logo(":/assets/V6/ev2_ui_assets_v6-08.png");
    logo = logo.scaled(ui->logoLabel->size(), Qt::IgnoreAspectRatio);
    ui->logoLabel->setPixmap(logo);

    //==============================================================//
    for(int i = 0; i < MAX_BEVS; i++)
    {
        currentBeverage[i] = new beverageData(i+1, eventData.getInventory(i+1, 15000, machineID));
        currentProducts[i] = new products();
    }
    //==============================================================//
    loadButtonNameImg();
    //==============================================================//
    buttonSetup(ui->option1Button, currentProducts[1-1]->imag, 300);
    buttonSetup(ui->option2Button, currentProducts[2-1]->imag, 300);
    buttonSetup(ui->option3Button, currentProducts[3-1]->imag, 300);
    buttonSetup(ui->option4Button, currentProducts[4-1]->imag, 300);
    buttonSetup(ui->option5Button, currentProducts[5-1]->imag, 300);
    buttonSetup(ui->option6Button, currentProducts[6-1]->imag, 300);
    buttonSetup(ui->option7Button, currentProducts[7-1]->imag, 300);
    buttonSetup(ui->option8Button, currentProducts[8-1]->imag, 300);
    buttonSetup(ui->option9Button, currentProducts[9-1]->imag, 300);
    //===============================================================//
    labelSetup(ui->option1Label, ui->option1ALabel, currentProducts[1-1]->price_small);
    labelSetup(ui->option2Label, ui->option2ALabel, currentProducts[2-1]->price_small);
    labelSetup(ui->option3Label, ui->option3ALabel, currentProducts[3-1]->price_small);
    labelSetup(ui->option4Label, ui->option4ALabel, currentProducts[4-1]->price_small);
    labelSetup(ui->option5Label, ui->option5ALabel, currentProducts[5-1]->price_small);
    labelSetup(ui->option6Label, ui->option6ALabel, currentProducts[6-1]->price_small);
    labelSetup(ui->option7Label, ui->option7ALabel, currentProducts[7-1]->price_small);
    labelSetup(ui->option8Label, ui->option8ALabel, currentProducts[8-1]->price_small);
    labelSetup(ui->option9Label, ui->option9ALabel, currentProducts[9-1]->price_small);
    //===============================================================//
    ui->option1ALabel->show();
    ui->option2ALabel->show();
    ui->option3ALabel->show();
    ui->option4ALabel->show();
    ui->option5ALabel->show();
    ui->option6ALabel->show();
    ui->option7ALabel->show();
    ui->option8ALabel->show();
    ui->option9ALabel->show();
    //===============================================================//
    ui->option1InfoButton->hide();
    ui->option2InfoButton->hide();
    ui->option3InfoButton->hide();
    ui->option4InfoButton->hide();
    ui->option5InfoButton->hide();
    ui->option6InfoButton->hide();
    ui->option7InfoButton->hide();
    ui->option8InfoButton->hide();
    ui->option9InfoButton->hide();
    //===============================================================//

    /*Scrolling area*/
    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setBackgroundRole(QPalette::NoRole);
    ui->scrollArea->setStyleSheet("background-color:transparent;");
    //========================================================================//
    QVBoxLayout *const layout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    layout->addWidget(ui->option1);
    layout->addWidget(ui->option2);
    layout->addWidget(ui->option3);
    layout->addWidget(ui->option4);
//    layout->addWidget(ui->option5);
//    layout->addWidget(ui->option6);
//    layout->addWidget(ui->option7);
//    layout->addWidget(ui->option8);
//    layout->addWidget(ui->option9);
    //=========================================================================//
    ui->surveyButton->setStyleSheet("border-image:url(:/assets/V6/ev2_ui_assets_v6-17.png); color: white;");
    QFont buttonFont("Arial", 20, QFont::Bold);
    ui->surveyButton->setFont(buttonFont);
    ui->surveyButton->hide();

    //====================================================================//
    disabletimer = new QTimer(this);
    connect(disabletimer, SIGNAL(timeout()), this, SLOT(disable_oneSec()));
    disabletimer->setInterval(400);

    timerUpdate = new QTimer(this);
    connect(timerUpdate, SIGNAL(timeout()), this, SLOT(updatePageNumber()));
    timerUpdate->setInterval(10);

    frozeTimer = new QTimer(this);
    connect(frozeTimer, SIGNAL(timeout()), this, SLOT(comptuerStatCheck()));
    frozeTimer->setInterval(3000000);
    //====================================================================//

    serial.init();
    sendardCommand("E"); //make sure the arduino is back in the proper state


    eventData.event_log(machineID, QDateTime::currentDateTime(), 0, "UI Starts", 0);
}

MainWindow::~MainWindow()
{
    //delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    sendardCommand("Z"); //make sure fsm reset
    enterPage = true;
    disabletimer->start();
    frozeTimer->start();
    pageNumber = 0;
    progressValue= 0;
    ui->option1Button->setEnabled(false);
    ui->option2Button->setEnabled(false);
    ui->option3Button->setEnabled(false);
    ui->option4Button->setEnabled(false);
    ui->option5Button->setEnabled(false);
    ui->option6Button->setEnabled(false);
    ui->option7Button->setEnabled(false);
    ui->option8Button->setEnabled(false);
    ui->option9Button->setEnabled(false);

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


void MainWindow::updatePageNumber()
{
    timerUpdate->stop();
    this->showFullScreen();
    if (enterPage){
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



beverageData* MainWindow::getBeverageData(int idx)
{
    return currentBeverage[idx-1];
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

QString MainWindow::getPrice_1()
{
    return currentProducts[optionSelected-1]->price_small;
}

QString MainWindow::getPrice_2()
{
    return currentProducts[optionSelected-1]->price_large;
}

//=========================== Inventory =========================================//
void MainWindow::setBeverageData(beverageData *bev, int idx)
{
    currentBeverage[idx-1]->updateBeverage(bev);
}
//============================Set    up==========================================//
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

void MainWindow::soldOutButton(QPushButton* optionButton, QLabel* soldLabel, bool status)
{

    if (status == true){
        optionButton->setEnabled(false);
        soldLabel->setAttribute(Qt::WA_TranslucentBackground);
        soldLabel->setStyleSheet("QLabel{color: #ffffff; font: 20pt;}");
        soldLabel->setAlignment(Qt::AlignLeft);
        soldLabel->setWordWrap(true);
        soldLabel->show();
    }
    else{
        optionButton->setEnabled(true);
        soldLabel->hide();
    }
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
    //label->setText("FREE \n" + volume);

//    ui->option1ALabel->setStyleSheet("QLabel{color: #ffffff; font: 20pt;}");
//    ui->option1ALabel->setAlignment(Qt::AlignCenter);
//    ui->option1ALabel->setText("12 oz (355mL)");

    labelVolume->setStyleSheet("QLabel{color: #ffffff; font: 40pt;}");
    labelVolume->setAlignment(Qt::AlignCenter);
    labelVolume->setText("12 oz (355mL)");


}

void MainWindow::setPages(payment *payPage, DispenseWindow * dispense, survey *surveypg, countdownWindow * pausePg){
    this->dispensePage = dispense;
    this->paymentPage = payPage;
    this->surveyPage = surveypg;
    this->pausePage = pausePg;
}

void MainWindow::paintEvent(QPaintEvent *p)
{
    QPainter paint(this);
    paint.drawPixmap(0, 0, QPixmap(":/assets/V6/ev2_ui_assets_v6-07.png").scaled(size()));

    QWidget::paintEvent(p);
}

//===============================================================================//

//============================      Timer       =================================//
void MainWindow::disable_oneSec()
{
    timerUpdate->start();
    disabletimer->stop();
    sendardCommand("Z");

    //-------------------------------------------------------------------//
    if (currentBeverage[0]->getInventoryVolume() <= 500)
    {
        soldOutButton(ui->option1Button, ui->option1SoldOutLabel, true);
    }
    else {
        soldOutButton(ui->option1Button, ui->option1SoldOutLabel, false);
    }
    //-------------------------------------------------------------------//
    if (currentBeverage[1]->getInventoryVolume() <= 500){
        soldOutButton(ui->option2Button, ui->option2SoldOutLabel, true);
    }
    else {
        soldOutButton(ui->option2Button, ui->option2SoldOutLabel, false);
    }
    //-------------------------------------------------------------------//
    if (currentBeverage[2]->getInventoryVolume() <= 500){
        soldOutButton(ui->option3Button, ui->option3SoldOutLabel, true);
    }
    else {
        soldOutButton(ui->option3Button, ui->option3SoldOutLabel, false);
    }
    //-------------------------------------------------------------------//
    if (currentBeverage[3]->getInventoryVolume() <= 500){
        soldOutButton(ui->option4Button, ui->option4SoldOutLabel, true);
    }
    else {
        soldOutButton(ui->option4Button, ui->option4SoldOutLabel, false);
    }
    //-------------------------------------------------------------------//
    if (currentBeverage[4]->getInventoryVolume() <= 500){
        soldOutButton(ui->option5Button, ui->option5SoldOutLabel, true);
    }
    else {
        soldOutButton(ui->option5Button, ui->option5SoldOutLabel, false);
    }
    //-------------------------------------------------------------------//
    if (currentBeverage[5]->getInventoryVolume() <= 500){
        soldOutButton(ui->option6Button, ui->option6SoldOutLabel, true);
    }
    else {
        soldOutButton(ui->option6Button, ui->option6SoldOutLabel, false);
    }
    //-------------------------------------------------------------------//
    if (currentBeverage[6]->getInventoryVolume() <= 500){
        soldOutButton(ui->option7Button, ui->option7SoldOutLabel, true);
    }
    else {
        soldOutButton(ui->option7Button, ui->option7SoldOutLabel, false);
    }
    //-------------------------------------------------------------------//
    if (currentBeverage[7]->getInventoryVolume() <= 500){
        soldOutButton(ui->option8Button, ui->option8SoldOutLabel, true);
    }
    else {
        soldOutButton(ui->option8Button, ui->option8SoldOutLabel, false);
    }
    //-------------------------------------------------------------------//
    if (currentBeverage[8]->getInventoryVolume() <= 500){
        soldOutButton(ui->option9Button, ui->option9SoldOutLabel, true);
    }
    else {
        soldOutButton(ui->option9Button, ui->option9SoldOutLabel, false);
    }
}
//===============================================================================//


//============================FSM Communication =================================//
bool MainWindow::checkard()
{
    return serial.checkConnected();
}

bool MainWindow::reinitard()
{
    return serial.init();
}

QByteArray MainWindow::getardVolume()
{
    return serial.readPacket(1);
}

void MainWindow::clearArd()
{
    serial.clearVolume();
}

void MainWindow::sendardCommand(QByteArray command)
{
    serial.writePacket(command);
}
//===============================================================================//

//============================Button Click=======================================//
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

void MainWindow::on_option2Button_clicked()
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

void MainWindow::on_option3Button_clicked()
{
    frozeTimer->stop();

    if(!eventData.checkDataBaseConnection())
    {
        eventData.reconnectDatabase();
    }
    eventData.event_log(machineID, QDateTime::currentDateTime(), optionSelected, " Selected", eventData.getInventory(optionSelected, 15000, machineID));

    pageNumber = 4;
    optionSelected = 3;
}

void MainWindow::on_option4Button_clicked()
{
    frozeTimer->stop();

    if(!eventData.checkDataBaseConnection())
    {
        eventData.reconnectDatabase();
    }
    eventData.event_log(machineID, QDateTime::currentDateTime(), optionSelected, " Selected", eventData.getInventory(optionSelected, 15000, machineID));

    pageNumber = 4;
    optionSelected = 4;
}

void MainWindow::on_option5Button_clicked()
{
    frozeTimer->stop();

    if(!eventData.checkDataBaseConnection())
    {
        eventData.reconnectDatabase();
    }
    eventData.event_log(machineID, QDateTime::currentDateTime(), optionSelected, " Selected", eventData.getInventory(optionSelected, 15000, machineID));

    pageNumber = 4;
    optionSelected = 5;
}

void MainWindow::on_option6Button_clicked()
{
    frozeTimer->stop();

    if(!eventData.checkDataBaseConnection())
    {
        eventData.reconnectDatabase();
    }
    eventData.event_log(machineID, QDateTime::currentDateTime(), optionSelected, " Selected", eventData.getInventory(optionSelected, 15000, machineID));

    pageNumber = 4;
    optionSelected = 6;
}


void MainWindow::on_option7Button_clicked()
{
    frozeTimer->stop();

    if(!eventData.checkDataBaseConnection())
    {
        eventData.reconnectDatabase();
    }
    eventData.event_log(machineID, QDateTime::currentDateTime(), optionSelected, " Selected", eventData.getInventory(optionSelected, 15000, machineID));

    pageNumber = 4;
    optionSelected = 7;
}

void MainWindow::on_option8Button_clicked()
{
    frozeTimer->stop();

    if(!eventData.checkDataBaseConnection())
    {
        eventData.reconnectDatabase();
    }
    eventData.event_log(machineID, QDateTime::currentDateTime(), optionSelected, " Selected", eventData.getInventory(optionSelected, 15000, machineID));

    pageNumber = 4;
    optionSelected = 9;
}

void MainWindow::on_option9Button_clicked()
{
    frozeTimer->stop();

    if(!eventData.checkDataBaseConnection())
    {
        eventData.reconnectDatabase();
    }
    eventData.event_log(machineID, QDateTime::currentDateTime(), optionSelected, " Selected", eventData.getInventory(optionSelected, 15000, machineID));

    pageNumber = 4;
    optionSelected = 9;
}

void MainWindow::on_surveyButton_clicked()
{

}
//==============================================================================//

//=========================Status Update========================================//
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
//=============================================================================//


