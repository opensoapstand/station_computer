//***************************************
//
// DispenseWindow
// for dispensing beverages updates user states
//
// created: 15-JUL-2019
// by: Jason Wang
//
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#ifndef dispenseWindow_H
#define dispenseWindow_H

#include "includefiles.h"
#include "payment.h"
#include "countdownwindow.h"
//#include "packetcommunications.h"

class MainWindow;

namespace Ui {
class DispenseWindow;
}

class DispenseWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DispenseWindow(QWidget *parent = nullptr);
    ~DispenseWindow();

    void setPages(MainWindow *main, payment *pay, countdownWindow * pausePage);
    void setProgressBarDisplay(bool status);
    void showpage();
    void updateInventory(database_logger db);
    int getPriceSelected();

signals:
    void dispenseProgress(int);
    void valueChanged(int);

private slots:
    void on_pushButton_clicked();
    void updatePageNumber();
    void refreshDispense();
    void updateVolume();

private:
    Ui::DispenseWindow *ui;
    countdownWindow * pausePage;
    MainWindow * mainPage;
    payment *paymentPage;

    QTimer *timerUpdate;
    QTimer *timerUpdate2;
    QTimer *ardTimer;

protected:
    void paintEvent(QPaintEvent *p);

    void showEvent(QShowEvent* event);
};

#endif // dispenseWindow_H
