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

#ifndef COUNTDOWNWINDOW_H
#define COUNTDOWNWINDOW_H

#include "includefiles.h"
//#include "packetcommunications.h"
#include "database_logger.h"

class MainWindow;
class DispenseWindow;

namespace Ui {
class countdownWindow;
}

class countdownWindow : public QWidget
{
    Q_OBJECT

public:
    explicit countdownWindow(QWidget *parent = nullptr);
    ~countdownWindow();

    void setPages(MainWindow * mainPage, DispenseWindow * dispensePage);

    void setProgressBarDisplay(bool status);

    void FilledPageTitle();

    void notFilledPageTitle();

    void updateInventory(database_logger db);



private slots:
    void on_pushButton_clicked();

    void countDown();

    void waitDispense();

    void updateVolume();


    void on_softButton_pressed();

    void on_softButton_released();

private:
    Ui::countdownWindow *ui;

    MainWindow * mainPage;
    DispenseWindow * dispensePage;

    QTimer *timer;
    QTimer *timer2;
    QTimer *ardTimer;
    int tenSec = 10;

protected:
    void paintEvent(QPaintEvent *p);
    void showEvent(QShowEvent* event);
};

#endif // COUNTDOWNWINDOW_H
