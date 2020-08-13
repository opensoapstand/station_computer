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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "includefiles.h"
#include "beveragedata.h"

#include "payment.h"
#include "survey.h"
#include "dispensewindow.h"
#include "database_logger.h"
#include "ardcommunication.h"

#include <QSqlDatabase>

#define MAX_BEVS 9

struct nutritionInfo
{
    int calories;
    int fat;
    int carb;
    int fiber;
    int protein;
    int sodium;
};

struct products
{
    QString name;
    QByteArray imag;

    QString price_small;
    QString price_large;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setPages(payment *payPage, DispenseWindow *dispense, survey *surveypg, countdownWindow * pausePg);

    database_logger getDatabase();
    beverageData* getBeverageData(int idx);
    void setBeverageData(beverageData* bev, int idx);
    survey* getSurveyPage();

    QString getPrice_1();
    QString getPrice_2();

    QByteArray getardVolume();
    void sendardCommand(QByteArray command);
    bool checkard();
    bool reinitard();
    void clearArd();

    void loadButtonNameImg();

private slots:
    void soldOutButton(QPushButton* optionButton, QLabel* soldLabel, bool status);

    void updatePageNumber();
    void disable_oneSec();
    void labelSetup(QLabel* labelVolume, QLabel* labelPrice, QString productPrice);
    void buttonSetup(QPushButton* button, QByteArray imageData, int masksize);
    //void infoBoxSetup(QMessageBox* infoBox, QString textinfo, QString drink, nutritionInfo option);

    void comptuerStatCheck();

    void on_option1Button_clicked();
    void on_option3Button_clicked();
    void on_option2Button_clicked();
    void on_option4Button_clicked();
    void on_option5Button_clicked();
    void on_option6Button_clicked();
    void on_option7Button_clicked();
    void on_option8Button_clicked();
    void on_option9Button_clicked();

    void on_surveyButton_clicked();

private:
    /*Page initiation*/
    Ui::MainWindow *ui;
    DispenseWindow *dispensePage;
    countdownWindow * pausePage;
    survey *surveyPage;
    payment *paymentPage;

    /*Arduino communication [fsm]*/
    ardCommunication serial;

    /*products*/
    beverageData* currentBeverage[MAX_BEVS];
    products* currentProducts[MAX_BEVS];

    database_logger eventData;

    /*timer*/
    QTimer *timerUpdate;   //page update
    QTimer *disabletimer;  //disable button when page first loaded
    QTimer *frozeTimer;    //check update of current ui status

//    nutritionInfo option1Info;
//    nutritionInfo option2Info;
//    nutritionInfo option3Info;

    bool surveyFilled;

protected:
    void paintEvent(QPaintEvent *p);
    void showEvent(QShowEvent* event);
};

#endif // MAINWINDOW_H
