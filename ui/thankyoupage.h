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

#include <QWidget>

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

private:
    Ui::thankYouPage *ui;

    dispensePage* dispensingPage;
    idle* idlePage;
};

#endif // THANKYOUPAGE_H
