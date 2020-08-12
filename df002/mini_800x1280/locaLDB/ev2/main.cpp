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

#include <iostream>
#include <string>
#include <string.h>
#include <chrono>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <chrono>

#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

using namespace std;
//datamodel *g_SystemModel;
bool goHome;
int progressValue;
int pageNumber = 0;
bool active = false;

int option1Inventory;
int option2Inventory;
int option3Inventory;
int optionSelected;

QString machineID = "mini-3";

#define LOOP_TIMER 250
#define PAUSE_LIMIT 300

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    qputenv("QT_VIRTUALKEYBOARD_LAYOUT_PATH", "qrc:/layouts");
    //qputenv("QT_VIRTUALKEYBOARD_STYLE", "drinkfill");

    //dispensedRead.dispensedVolume=0;
    QApplication a(argc, argv);
    MainWindow selectionPage;
    DispenseWindow dispensingPage;
    countdownWindow countingDownPage;
    payment paymentPage;
    survey surveyPage;
    goHome = false;

    selectionPage.setPages(&paymentPage, &dispensingPage, &surveyPage, &countingDownPage);
    paymentPage.setPages(&selectionPage, &dispensingPage);
    dispensingPage.setPages(&selectionPage, &paymentPage,  &countingDownPage);
    countingDownPage.setPages(&selectionPage, &dispensingPage);
    surveyPage.setPages(&selectionPage);

    selectionPage.showFullScreen();

    return a.exec();
}
