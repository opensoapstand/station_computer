//***************************************
//
// main
// critical launch funciton, sets up critical items and launches threads
//
// created: 15-JUL-2019
// by: Jason Wang
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************
#define UI_VERSION "0.1.3"
//moneris packet integration

#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

using namespace std;
int progressValue;
int pageNumber = 0;

int optionSelected;

QString machineID = "gold_unit";

#define LOOP_TIMER 250
#define PAUSE_LIMIT 300

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    qputenv("QT_VIRTUALKEYBOARD_LAYOUT_PATH", "qrc:/layouts");
    //qputenv("QT_VIRTUALKEYBOARD_STYLE", "drinkfill");

    QApplication a(argc, argv);
    MainWindow selectionPage;
    DispenseWindow dispensingPage;
    countdownWindow countingDownPage;
    payment paymentPage;
    survey surveyPage;

    selectionPage.setPages(&paymentPage, &dispensingPage, &surveyPage, &countingDownPage);
    paymentPage.setPages(&selectionPage, &dispensingPage);
    dispensingPage.setPages(&selectionPage, &paymentPage,  &countingDownPage);
    countingDownPage.setPages(&selectionPage, &dispensingPage);
    surveyPage.setPages(&selectionPage);

    selectionPage.showFullScreen();

    return a.exec();
}
