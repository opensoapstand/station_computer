//***************************************
//
// thankyoupage.h
// GUI class to show user dispense has been
// completed and route back to idle
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef THANKYOUPAGE_H
#define THANKYOUPAGE_H

#include "df_util.h"

class dispensePage;
class idle;

namespace Ui {
class thankYouPage;
}

class thankYouPage : public QWidget
{
    Q_OBJECT

public:
    explicit thankYouPage(QWidget *parent = nullptr);
    void setPage(dispensePage* pageDispense, idle* pageIdle);
    ~thankYouPage();

private slots:
    void on_mainPage_Button_clicked();
    void onThankyouTimeoutTick();
    void onRinseTimerTick();

private:
    void showEvent(QShowEvent *event);

    Ui::thankYouPage *ui;
    dispensePage* dispensingPage;
    idle* idlePage;

    int _thankYouTimeoutSec;
    QTimer* thankYouEndTimer;


    QTimer* rinseTimer;
    int _rinseTimerTimeoutSec;
    bool rinse;
};

#endif // THANKYOUPAGE_H
